//
// Created by nvt on 8/21/16.
//

#include "Font.h"

#include "core/Predef.h"

#define STB_TRUETYPE_IMPLEMENTATION

#include <stb/stb_truetype.h>
#include <graphics/Image.h>
#include <graphics/TextureBlock.h>
#include <msdfgen/msdfgen.h>


Font::Font(const unsigned char *data, TextureBlock *textureBlock, FontConfig config) :
        data(data),
        info(new stbtt_fontinfo()),
        renderBuffer(new unsigned char[renderBufferWidth * renderBufferHeight]),
        tmpImgBuffer(new unsigned char[renderBufferWidth * renderBufferHeight * 4]),
        textureBlock(textureBlock),
		config(config) {
    load();
}

Font::~Font() {
    delete[] renderBuffer;
    delete info;
}


void Font::load() {
    stbtt_InitFont(info, data, 0);
    stbtt_GetFontVMetrics(info, &ascent, &descent, &lineGap);
    scale = stbtt_ScaleForPixelHeight(info, config.pixelSize);

}

Font::GlyphData &Font::glyphFor(int codepoint) {
    static std::mutex glyphLoadMutex;

    std::lock_guard<std::mutex> guard(glyphLoadMutex);
    return glyphData.getOrElseUpdate(codepoint, [this, codepoint]() {
        GlyphData data;
        stbtt_GetCodepointHMetrics(info, codepoint, &data.rawAdvanceWidth, &data.rawLeftSideBearing);
		data.advanceWidth = normalizeEm(data.rawAdvanceWidth);
		data.leftSideBearing = normalizeEm(data.rawLeftSideBearing);
        glm::ivec2 gbMax;
        stbtt_GetCodepointBox(info, codepoint, &data.rawGlyphBox.x,&data.rawGlyphBox.y,&gbMax.x, &gbMax.y);
        data.rawGlyphBox.width = gbMax.x - data.rawGlyphBox.x;
        data.rawGlyphBox.height = gbMax.y - data.rawGlyphBox.y;

		data.glyphBox.x = normalizeEm(data.rawGlyphBox.x);
		data.glyphBox.y = normalizeEm(data.rawGlyphBox.y);
		data.glyphBox.width = normalizeEm(data.rawGlyphBox.width);
		data.glyphBox.height = normalizeEm(data.rawGlyphBox.height);

        if (codepoint == int(' ') || codepoint == int('\n') || codepoint == int('\t')) {
            auto blankImg = Image::ofDimensions(1,1);
            data.textureCell = textureBlock->getOrElseUpdateCell(blankImg);
        } else {
            if (!config.useMSDF) {
                memset(renderBuffer, 0, (size_t) (renderBufferWidth * renderBufferHeight));

                glm::ivec2 v0, v1;
                stbtt_GetCodepointBitmapBox(info, codepoint, scale, scale, &v0.x, &v0.y, &v1.x, &v1.y);
                stbtt_MakeCodepointBitmapSubpixel(info, renderBuffer, renderBufferWidth, renderBufferHeight, renderBufferWidth, scale,
                                          scale, 0.0f, 0.0f, codepoint);

                auto dim = v1 - v0;
                Image tmpImg(tmpImgBuffer, dim.x, dim.y, none<std::string>(), false);

                for (int y = 0; y < dim.y; ++y) {
                    unsigned char *srcRow = renderBuffer + renderBufferWidth * y;
                    for (int x = 0; x < dim.x; ++x) {
                        unsigned char alpha = srcRow[x];
                        Color &col = tmpImg.pixelColor(x, tmpImg.height - y - 1);
                        col.r = 255;
                        col.g = 255;
                        col.b = 255;
                        col.a = alpha;
                    }
                }

                int lsb = int(data.rawLeftSideBearing * scale);
                Noto::info("Char {}, w: {}, h: {}, v0.x: {}, v0.y: {}, v1.x: {}, v1.y: {}, lsb: {}", (char)codepoint, dim.x,dim.y,v0.x,v0.y,v1.x,v1.y,lsb);

                data.textureCell = textureBlock->getOrElseUpdateCell(tmpImg);
            } else {
                Noto::info("Generating msdf for codepoint : {}", (char)codepoint);
                auto tmpImg = renderSignedDistanceFieldFor(codepoint);
                data.textureCell = textureBlock->getOrElseUpdateCell(tmpImg, { 2 });
            }
        }

        return data;
    });
}

FontPtr Font::fromFile(const Arx::File& file, TextureBlock *textureBlock) {
    std::vector<char> data;
    if (!file.exists()) {
        throw fmt("Could not read font from file, did not exist: {}",file);
    }
    readAllBytes(file.path.raw(), data);
    char * newData = new char[data.size()];
    memcpy(newData, data.data(), data.size());

	FontConfig config(0.75f);
    Arx::File confFile = file.withExtension(".axm");
	if (confFile.exists()) {
		auto node = AxmParser::parse(confFile);
		readFromConf(config, *node);
	}

    return std::shared_ptr<Font>(new Font((const unsigned char *) newData, textureBlock, config));
}

ImagePtr Font::generateSignedDistanceFieldFor(int codepoint) {
    auto tmpImg = renderSignedDistanceFieldFor(codepoint);
    ImagePtr ret = Image::ofDimensions(tmpImg.width,tmpImg.height);
    for (int x = 0; x < tmpImg.width; ++x) {
        for (int y = 0; y < tmpImg.height; ++y) {
            ret->pixelColor(x,y) = tmpImg.pixelColor(x,y);
        }
    }
    return ret;
}

Image Font::renderSignedDistanceFieldFor(int codepoint) {
    stbtt_vertex *vertices;
    int numVertices = stbtt_GetCodepointShape(info, codepoint, &vertices);

    glm::ivec2 min(1000000,100000);
    glm::ivec2 max(-1000000,-100000);

    msdfgen::Point2 cursor;
    msdfgen::Shape shape;
    std::vector<msdfgen::Contour> contours;
    for (int i = 0; i < numVertices; ++i) {
        int x = vertices[i].x;
        int y = vertices[i].y;
        min = glm::ivec2(std::min(min.x,x), std::min(min.y,y));
        max = glm::ivec2(std::max(max.x,x), std::max(max.y,y));
        if (vertices[i].type == STBTT_vmove) {
            cursor.x = x;
            cursor.y = y;
            contours.push_back(msdfgen::Contour());
        } else {
            msdfgen::Point2 newPoint(x, y);
            if (vertices[i].type == STBTT_vline) {
                contours.back().addEdge(msdfgen::EdgeHolder(cursor, newPoint));
            } else {
                msdfgen::Point2 controlPoint(vertices[i].cx, vertices[i].cy);
                contours.back().addEdge(msdfgen::EdgeHolder(cursor, controlPoint, newPoint));
            }
            cursor = newPoint;
        }

    }
    for (msdfgen::Contour contour : contours) {
        shape.addContour(contour);
    }

    shape.normalize();

    glm::ivec2 dim = max - min;
    int w = int(config.pixelSize * (float(dim.x) / float(dim.y)));
    int h = config.pixelSize;
    if (w > h) {
        w = config.pixelSize;
        h = int(config.pixelSize * (float(dim.y) / float(dim.x)));
    }
    int border = 2;
    Image tmpImg(tmpImgBuffer, w+border*2, h+border*2, none<std::string>(), false);

    int maxPixDim = std::max(w,h);
    int maxGlyphDim = std::max(dim.x,dim.y);
    float scale = float(maxPixDim) / float(maxGlyphDim);
    glm::ivec2 offset = -min;
    offset += int((border / float(maxPixDim)) * maxGlyphDim);

    msdfgen::edgeColoringSimple(shape, config.angleThreshold, (unsigned long long int) config.seed);
    msdfgen::Bitmap<msdfgen::FloatRGB> msdf(w+border*2,h+border*2);
    msdfgen::generateMSDF(msdf,shape,std::max(dim.x,dim.y)*config.rangeScale,msdfgen::Vector2(scale,scale),msdfgen::Vector2(offset.x,offset.y));

    for (int x = 0; x < w+border*2; ++x) {
        for (int y = 0; y < h+border*2; ++y) {
			auto& v = msdf(x,y);
			tmpImg.pixelColor(x,y).setF(v.r,v.g,v.b,1.0f);
        }
    }

    return tmpImg;
}

float Font::normalizeEm(int em) {
	return em / float(ascent - descent);
}

FontConfig::FontConfig(float rangeScale) : rangeScale(rangeScale) {}

#include "Font.h_generated.cpp" // Include Generated