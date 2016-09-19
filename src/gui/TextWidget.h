//
// Created by Sam Bock on 8/22/16.
//

#ifndef TEST2_TEXTWIDGET_H
#define TEST2_TEXTWIDGET_H


#include <core/ArxString.h>
#include <core/Moddable.h>
#include "Widget.h"

class Font;
class TextRenderState;

class TextWidget : public Widget {
public:
    Moddable<Arx::Sequence<Arx::String>> text;
	Arx::String fontName = "Eurostile";
	int fontSize = 20;
	Color fontColor = Color(255,255,255,255);

    void setText(const Arx::String& str);

public:
	TextWidget(Widget *parent, const Arx::String &text);

protected:
	virtual void drawPre(WidgetDrawContext &context) override;
};

struct GlyphLayout {
	Rect<float> location;
	int codepoint;
	bool render = true;

	GlyphLayout(const Rect<float> &location, int codepoint, bool render) : location(location), codepoint(codepoint),
																		   render(render) {}
	GlyphLayout() {}
};

class TextLayout {
protected:
	static GlyphLayout layoutGlyph(TextRenderState &state);
	static Arx::Sequence<GlyphLayout> layoutWord(TextRenderState &state);
	static Arx::Sequence<GlyphLayout> layoutString(TextRenderState &state);
public:
	static Arx::Sequence<GlyphLayout> layoutText(const Arx::Sequence<Arx::String>& text, std::shared_ptr<Font> font, int fontSize, int regionWidth);
};


#endif //TEST2_TEXTWIDGET_H
