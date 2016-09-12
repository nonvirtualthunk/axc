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
    return glyphData.getOrElseUpdate(codepoint, [this, codepoint]() {
        GlyphData data;
        stbtt_GetCodepointHMetrics(info, codepoint, &data.advanceWidth, &data.leftSideBearing);
        glm::ivec2 gbMax;
        stbtt_GetCodepointBox(info, codepoint, &data.glyphBox.x,&data.glyphBox.y,&gbMax.x, &gbMax.y);
        data.glyphBox.width = gbMax.x - data.glyphBox.x;
        data.glyphBox.height = gbMax.y - data.glyphBox.y;

        if (codepoint == int(' ')) {
            auto blankImg = Image::ofDimensions(1,1);
            data.textureCell = textureBlock->getOrElseUpdateCell(blankImg);
        } else {
            if (!config.useMSDF) {
                memset(renderBuffer, 0, renderBufferWidth * renderBufferHeight);

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

                Arx::String path("/tmp/baked_");
                path.append(char(codepoint));
                path.append(".png");
                tmpImg.writeToFile(path.raw());

                int lsb = int(data.leftSideBearing * scale);
                Noto::info("Char {}, w: {}, h: {}, v0.x: {}, v0.y: {}, v1.x: {}, v1.y: {}, lsb: {}", (char)codepoint, dim.x,dim.y,v0.x,v0.y,v1.x,v1.y,lsb);

                data.textureCell = textureBlock->getOrElseUpdateCell(tmpImg);
            } else {
                auto tmpImg = renderSignedDistanceFieldFor(codepoint);
                data.textureCell = textureBlock->getOrElseUpdateCell(tmpImg);
                data.textureCell.location.x+=2;
                data.textureCell.location.y+=2;
                data.textureCell.location.width-=4;
                data.textureCell.location.height-=4;

                data.textureCell.texCoordRect = Rect<float>(data.textureCell.location.x / float(textureBlock->width()),
                                                   data.textureCell.location.y / float(textureBlock->height()),
                                                   data.textureCell.location.width / float(textureBlock->width()),
                                                   data.textureCell.location.height / float(textureBlock->height()));

                int indexOffset = 0;
                data.textureCell.texCoords[(0 + indexOffset) % 4] = data.textureCell.texCoordRect.xy();
                data.textureCell.texCoords[(1 + indexOffset) % 4] = data.textureCell.texCoordRect.xy() + glm::vec2(data.textureCell.texCoordRect.width,0.0f);
                data.textureCell.texCoords[(2 + indexOffset) % 4] = data.textureCell.texCoordRect.xy() + glm::vec2(data.textureCell.texCoordRect.width,data.textureCell.texCoordRect.height);
                data.textureCell.texCoords[(3 + indexOffset) % 4] = data.textureCell.texCoordRect.xy() + glm::vec2(0.0f,data.textureCell.texCoordRect.height);
            }
        }

        return data;
    });
}

FontPtr Font::fromFile(const Arx::File& file, TextureBlock *textureBlock) {
    std::vector<char> data;
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
//            if (contours.size() != 0) {
//                auto& last = contours.back().edges.back();
//                auto& first = contours.back().edges.front();
//                contours.back().addEdge()
//            }
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
    float rawScale = float(maxPixDim) / float(maxGlyphDim);
    float scale = float(std::max(w,h)) / float(std::max(dim.x,dim.y));
    glm::ivec2 offset = -min;
    offset += int((border / float(maxPixDim)) * maxGlyphDim);

    msdfgen::edgeColoringSimple(shape, config.angleThreshold, (unsigned long long int) config.seed);
    msdfgen::Bitmap<msdfgen::FloatRGB> msdf(w+border*2,h+border*2);
    msdfgen::generateMSDF(msdf,shape,std::max(dim.x,dim.y)*config.rangeScale,msdfgen::Vector2(scale,scale),msdfgen::Vector2(offset.x,offset.y));

    for (int x = 0; x < w+border*2; ++x) {
        for (int y = 0; y < h+border*2; ++y) {
//            if (x < border || x >= w-border || y < border || y >= h-border) {
//                tmpImg.pixelColor(x,y).setF(1.0f,1.0f,1.0f,1.0f);
//            } else {
                auto& v = msdf(x,y);
                tmpImg.pixelColor(x,y).setF(v.r,v.g,v.b,1.0f);
//            }
        }
    }

    tmpImg.writeToFile(fmt("/tmp/msdf_{}.png",(char)codepoint).raw());

    return tmpImg;
}

FontConfig::FontConfig(float rangeScale) : rangeScale(rangeScale) {}

#include "Font.h_generated.cpp" // Include Generated
