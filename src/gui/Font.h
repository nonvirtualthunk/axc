//
// Created by nvt on 8/21/16.
//

#ifndef TEST2_FONT_H
#define TEST2_FONT_H


#include <containers/ArxMap.h>
#include <glm/vec2.hpp>
#include <graphics/Image.h>

class stbtt_fontinfo;
class TextureBlock;

class Font;

typedef std::shared_ptr<Font> FontPtr;

class Font {
public:
    struct GlyphData {
        int advanceWidth = 0;
        int leftSideBearing = 0;

        glm::vec2 texCoords[4];
    };
protected:
    stbtt_fontinfo* info = nullptr;
    const unsigned char * data = nullptr;
    int ascent = 0;
    int descent = 0;
    int lineGap = 0;

    Arx::Map<int,GlyphData> glyphData;

    float scale = 1.0f;
    const int renderBufferWidth = 256;
    const int renderBufferHeight = 256;
    unsigned char * renderBuffer;
    unsigned char * tmpImgBuffer;

    TextureBlock * textureBlock;
public:
    void load();

    Font(const unsigned char * data, TextureBlock *);

    GlyphData& glyphFor(int codepoint);

    virtual ~Font();

    static FontPtr fromFile(const char * path, TextureBlock * textureBlock);
};


#endif //TEST2_FONT_H
