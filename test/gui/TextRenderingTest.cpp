//
// Created by Sam Bock on 9/15/16.
//


#include <catch.hpp>
#include <graphics/TextureBlock.h>
#include <gui/Font.h>
#include <application/ResourceManager.h>
#include <gui/TextWidget.h>

using namespace Arx;

TEST_CASE("TextLayout","[gui][font][text]") {
	TextureBlock tb(1024,1024);
	FontPtr font = ResourceManager::font("fonts/Eurostile.ttf",&tb);

    SECTION("All can fit on one line") {
        String text = "This is a test";
        int fontSize = 20;
        Sequence<GlyphLayout> layout = TextLayout::layoutText(text,font,fontSize,10000);

        // one glyph layout for every character in the text
        REQUIRE(layout.size() == text.size());
        // all the glyphs should be on the same line
        bool sameLine = layout.forall([&](const GlyphLayout& g) {
            return std::abs(g.location.y - layout.head().location.y) < fontSize/2;
        });

        REQUIRE(sameLine);
    }

    SECTION("All but last word can fit on line") {
        String text = "This is a test";
        int fontSize = 20;
        Sequence<GlyphLayout> layout = TextLayout::layoutText(text,font,fontSize,90);

        REQUIRE(layout.size() == text.size());
        // all the glyphs except the last word should be on the same line
        bool sameLine = layout.forall([&](const GlyphLayout& g) {
            return std::abs(g.location.y - layout.head().location.y) < fontSize/2;
        });
        REQUIRE(!sameLine);

        bool sameLineStart = layout.take(Arx::String("This is a").size()).forall([&](const GlyphLayout& g) {
            return std::abs(g.location.y - layout.head().location.y) < fontSize/2;
        });
        REQUIRE(sameLineStart);

        bool sameLineEnd = layout.takeRight(Arx::String("test").size()).forall([&](const GlyphLayout& g) {
            return std::abs(g.location.y - layout.head().location.y) < fontSize/2;
        });
        REQUIRE(!sameLineEnd);
    }

    SECTION("Long word cannot fit on the line at all") {
        String text = "Thisisatestthatistoolongtofitonaline";
        int fontSize = 20;
        Sequence<GlyphLayout> layout = TextLayout::layoutText(text,font,fontSize,90);

        REQUIRE(layout.size() == text.size());
        // all the glyphs except the last word should be on the same line
        bool sameLine = layout.forall([&](const GlyphLayout& g) {
            return std::abs(g.location.y - layout.head().location.y) < fontSize/2;
        });
        REQUIRE(!sameLine);
    }
}