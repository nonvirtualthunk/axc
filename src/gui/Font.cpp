//
// Created by nvt on 8/21/16.
//

#include "Font.h"

#include "core/Predef.h"

#define STB_TRUETYPE_IMPLEMENTATION

#include <stb/stb_truetype.h>
#include <graphics/Image.h>
#include <graphics/TextureBlock.h>


Font::Font(const unsigned char *data, TextureBlock *textureBlock) :
        data(data),
        info(new stbtt_fontinfo()),
        renderBuffer(new unsigned char[renderBufferWidth * renderBufferHeight]),
        tmpImgBuffer(new unsigned char[renderBufferWidth * renderBufferHeight * 4]),
        textureBlock(textureBlock) {
    load();
}

Font::~Font() {
    delete[] renderBuffer;
    delete info;
}


void Font::load() {
    stbtt_InitFont(info, data, 0);
    stbtt_GetFontVMetrics(info, &ascent, &descent, &lineGap);
    // TODO: this is...rather specific to one individual font
    scale = stbtt_ScaleForPixelHeight(info,20);

}

Font::GlyphData &Font::glyphFor(int codepoint) {
    return glyphData.getOrElseUpdate(codepoint, [this, codepoint]() {
        GlyphData data;
        stbtt_GetCodepointHMetrics(info, codepoint, &data.advanceWidth, &data.leftSideBearing);
        glm::ivec2 v0, v1;
        stbtt_GetCodepointBitmapBox(info, codepoint, scale, scale, &v0.x, &v0.y, &v1.x, &v1.y);
        stbtt_MakeCodepointBitmap(info, renderBuffer, renderBufferWidth, renderBufferHeight, renderBufferWidth, scale,
                                  scale, codepoint);

//        stbtt_vertex * vertices;
//        int numVertices = stbtt_GetCodepointShape(info, codepoint, &vertices);
//
//        glm::ivec2 rv0, rv1;
//        stbtt_GetCodepointBox(info, codepoint, &rv0.x,&rv0.y,&rv1.x, &rv1.y);

        auto dim = v1 - v0;
        Image tmpImg(tmpImgBuffer,dim.x,dim.y,none<std::string>());

        for (int y = 0; y < dim.y; ++y) {
            unsigned char *srcRow = renderBuffer + renderBufferWidth * y + v0.x;
            for (int x = 0; x < dim.x; ++x) {
                unsigned char alpha = srcRow[x];
                Color &col = tmpImg.pixelColor(x, y);
                col.r = 0;
                col.g = 0;
                col.b = 0;
                col.a = alpha;
            }
        }

        const auto& cell = textureBlock->getOrElseUpdateCell(tmpImg);
        for (int i = 0; i < 4; ++i) {
            data.texCoords[i] = cell.texCoords[i];
        }

        return data;
    });
}

FontPtr Font::fromFile(const char *path, TextureBlock *textureBlock) {
    std::vector<char> data;
    readAllBytes(path, data);
    return std::shared_ptr<Font>(new Font((const unsigned char *)(data.data()),textureBlock));
}
