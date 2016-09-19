//
// Created by nvt on 8/21/16.
//

#include <catch.hpp>
#include <graphics/TextureBlock.h>
#include <gui/Font.h>
#include <application/ResourceManager.h>
#include <core/Metrics.h>

TEST_CASE("FontCreation","[gui][font][slow]") {
    TextureBlock tb(128,128);

    TIMER(FontLoadTestTime);

    FontPtr font = ResourceManager::font("fonts/Goethe.ttf",&tb);

    auto& glyph = font->glyphFor('A');

    REQUIRE(glyph.rawAdvanceWidth > 0);
//    REQUIRE(glyph.leftSideBearing != 0);
    REQUIRE(glyph.textureCell.texCoords[0].x > 0.0f);

    PRINT_TIMER(FontLoadTestTime);

    tb.image->writeToFile("/tmp/baked.png");
}

TEST_CASE("MSDFGeneration","[gui][font][slow]") {
    TextureBlock tb(128,128);

    TIMER(FontMSDFGenerationTime);

    FontPtr font = ResourceManager::font("fonts/Goethe.ttf",&tb);

    ImagePtr img = font->generateSignedDistanceFieldFor('A');
    img->writeToFile("/tmp/msdf.png");

    PRINT_TIMER(FontMSDFGenerationTime);
}