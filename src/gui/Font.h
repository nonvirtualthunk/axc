//
// Created by nvt on 8/21/16.
//

#ifndef TEST2_FONT_H
#define TEST2_FONT_H


#include <containers/ArxMap.h>
#include <glm/vec2.hpp>
#include <graphics/Image.h>
#include <conf/AxmParser.h>
#include <graphics/TextureBlock.h>

class stbtt_fontinfo;
class TextureBlock;

class Font;

typedef std::shared_ptr<Font> FontPtr;

struct RichFloat {
    float& fvar;

    RichFloat(float &fvar) : fvar(fvar) {}

    void operator=(const AxmNode& n) {
        if (n.nonEmpty()) {
            fvar = n.asFloat();
        }
    }
};

// generateMeta
struct FontConfig {
    // beginFields
    float rangeScale;
    int pixelSize = 20;
    bool useMSDF = false;
    int seed = 0L;
    float angleThreshold = 3.0f;
    // endFields

	FontConfig(float rangeScale);
};

class Font {
public:
    struct GlyphData {
        int rawAdvanceWidth = 0;
		float advanceWidth = 0.0f;
        int rawLeftSideBearing = 0;
		float leftSideBearing = 0.0f;

        // Position/dimensions/coords in the texture block
        TextureBlock::Cell textureCell;
        // Position/dimensions in abstract glyph space
        Rect<int> rawGlyphBox;
		Rect<float> glyphBox;
    };

public:
    int ascent = 0;
    int descent = 0;
    int lineGap = 0;
	FontConfig config;

    float scale = 1.0f;
    TextureBlock * textureBlock;
protected:
    stbtt_fontinfo* info = nullptr;
    const unsigned char * data = nullptr;
    Arx::Map<int,GlyphData> glyphData;

    const int renderBufferWidth = 256;
    const int renderBufferHeight = 256;
    unsigned char * renderBuffer;
    unsigned char * tmpImgBuffer;
public:
    void load();

	float normalizeEm(int em);

    Font(const unsigned char * data, TextureBlock * textureBlock, FontConfig config);

    GlyphData& glyphFor(int codepoint);

    virtual ~Font();

    ImagePtr generateSignedDistanceFieldFor(int codepoint);
    // Renders an MSDF for the given codepoint into the render buffer, returns the size
    Image renderSignedDistanceFieldFor(int codepoint);

    static FontPtr fromFile(const Arx::File& file, TextureBlock * textureBlock);
};


#include "Font.h_generated.h" // Include Generated
#endif //TEST2_FONT_H
