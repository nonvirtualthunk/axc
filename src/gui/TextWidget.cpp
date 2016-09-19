//
// Created by Sam Bock on 8/22/16.
//

#include <application/ResourceManager.h>
#include "TextWidget.h"
#include "Font.h"

TextWidget::TextWidget(Widget *parent, const Arx::String &str) :
        Widget(parent),
        text(Seq({str})) {
}

struct TextRenderState {
	glm::ivec2 cursor;
	int charIndex = 0;
    int strIndex = 0;
	const Arx::Sequence<Arx::String>& string;
	int writeableWidth;
	FontPtr font;
	float normAsc;
	float normDesc;
	float normLineGap;
	int fontSize;

	void advance() {
		charIndex += 1;
        if (charIndex >= string[strIndex].size()) {
            strIndex += 1;
            charIndex = 0;
        }
	}
	bool hasNext() const {
		return strIndex < string.size() && charIndex < string[strIndex].size();
	}
	char curChar() const {
		return string[strIndex][charIndex];
	}

	void moveCursorToNextLine() {
		cursor.x = 0;
		cursor.y += ((normAsc - normDesc) + normLineGap) * fontSize;
	}

	float effAdvanceWidth(int codepoint) {
		return font->glyphFor(codepoint).advanceWidth * fontSize;
	}

    TextRenderState(const Arx::Sequence<Arx::String> &string) : string(string) {}
};

GlyphLayout TextLayout::layoutGlyph(TextRenderState &state) {
	char c = state.curChar();

	const auto &glyph = state.font->glyphFor(c);
	GlyphLayout ret;
	ret.location.x = state.cursor.x + (glyph.leftSideBearing * state.fontSize);
	ret.location.y = state.cursor.y + (-glyph.glyphBox.maxY() * state.fontSize);
	ret.location.width = glyph.glyphBox.width * state.fontSize;
	ret.location.height = glyph.glyphBox.height * state.fontSize;
	ret.codepoint = c;

	state.cursor.x += glyph.advanceWidth * state.fontSize;

	return ret;
}

Arx::Sequence<GlyphLayout> TextLayout::layoutWord(TextRenderState &state) {
	bool resetOccurred = false;
	int startingCharacterIndex = state.charIndex;
	Arx::Sequence<GlyphLayout> ret;
	while (state.hasNext()) {
		char c = state.curChar();
		if (c == ' ' || c == '\n' || c == '\t') {
			return ret;
		} else {
			ret.add(layoutGlyph(state));
			if (ret.last().location.maxX() > state.writeableWidth) {
				// TODO: This is a rough approximation. Basically, if this word by itself is too big
				// to fit on a line, we break it up, otherwise we move it entirely onto the next line
				int wordWidth = int(ret.last().location.maxX() - ret.head().location.minX());
				if (wordWidth > state.writeableWidth - state.fontSize) {
					// If at least the last character can fit on a line, then move down and re-layout glyph
					if (ret.last().location.width < state.writeableWidth) {
						ret.pop();
						state.moveCursorToNextLine();
					} else {
						// warn and skip if the character itself doesn't fit properly
						Noto::warn("Laying out word and encountered character {}, which is wider than entire width {}",c, state.writeableWidth);
						state.advance();
					}
				} else {
					if (resetOccurred) {
						Noto::error("Moved entire word down a line twice, this not happen");
						state.advance();
					} else {
						// The word alone can fit on a line, so move down and restart the layout
						ret.clear();
						state.moveCursorToNextLine();
						state.charIndex = startingCharacterIndex;
						resetOccurred = true;
					}
				}
			} else {
				// everything fits and is ok, keep advancing
				state.advance();
			}
		}
	}
	return ret;
}

Arx::Sequence<GlyphLayout> TextLayout::layoutString(TextRenderState &state) {
	Arx::Sequence<GlyphLayout> ret;
	while (state.hasNext()) {
		char c = state.curChar();
		if (c == ' ') {
			float w = state.effAdvanceWidth(' ');
			float h = state.normAsc - state.normDesc;
			GlyphLayout g(Rect<float>(state.cursor.x,state.cursor.y,w,h),c,false);
			ret.add(g);
			state.cursor.x += w;
			state.advance();
		} else if (c == '\n') {
			GlyphLayout g(Rect<float>(state.cursor.x,state.cursor.y,0,0),c,false);
			ret.add(g);
			state.moveCursorToNextLine();
			state.advance();
		} else if (c == '\t') {
			// TODO: this isn't how tabbing works
			float w = state.effAdvanceWidth(' ') * 4;
			float h = state.normAsc - state.normDesc;
			GlyphLayout g(Rect<float>(state.cursor.x,state.cursor.y,w,h),c,false);
			ret.add(g);
			state.cursor.x += w;
			state.advance();
		} else {
			ret.addAll(layoutWord(state));
		}
	}
	return ret;
}

Arx::Sequence<GlyphLayout>
TextLayout::layoutText(const Arx::Sequence<Arx::String> &text, std::shared_ptr<Font> font, int fontSize, int regionWidth) {
	TextRenderState state(text);

	state.font = font;
	state.fontSize = fontSize;
	state.normDesc = state.font->normalizeEm(state.font->descent);
	state.normAsc = state.font->normalizeEm(state.font->ascent);
	state.normLineGap = state.font->normalizeEm(state.font->lineGap);

	state.writeableWidth = regionWidth;
	// move down by the ascent, to be at baseline
	state.cursor = glm::ivec2(0,state.normAsc * fontSize);

	return layoutString(state);
}

void TextWidget::drawPre(WidgetDrawContext &context) {
	auto font = ResourceManager::font(fontName, context.fontTextureBlock);

	Arx::Sequence<GlyphLayout> glyphLayout = TextLayout::layoutText(text.resolve(),font,fontSize,width());

	for (const GlyphLayout& glyph : glyphLayout) {
		if (glyph.render) {
            const auto& texCoordRect = font->glyphFor(glyph.codepoint).textureCell.texCoordRect;
            drawQuad(context, glyph.location.x, glyph.location.y, glyph.location.width, glyph.location.height,
                     fontColor, texCoordRect, 0, true, true);
        }
	}
}

void TextWidget::setText(const Arx::String &str) {
    text = Seq({str});
}
