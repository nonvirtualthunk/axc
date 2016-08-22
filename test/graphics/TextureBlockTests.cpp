//
// Created by Sam Bock on 8/17/16.
//

#include <catch.hpp>
#include <graphics/TextureBlock.h>

TEST_CASE("TextureBlockPacking","[graphics][texture]") {
    TextureBlock tb(512,512);

    ImagePtr img1 = Image::ofDimensions(32,32);
    for (int x = 0; x < 32; ++x) {
        for (int y = 0; y < 32; ++y) {
            img1->pixelColor(x,y) = Color(x,y,0,255);
        }
    }

    const auto& cell = tb.getOrElseUpdateCell(img1);
    REQUIRE(cell.location.width == img1->width);
    REQUIRE(cell.location.height == img1->height);
    REQUIRE(cell.location.x > 0);
    REQUIRE(cell.location.y > 0);

    for (int x = 0; x < 32; ++x) {
        for (int y = 0; y < 32; ++y) {
            REQUIRE(tb.image->pixelColor(cell.location.x + x,cell.location.y + y) == img1->pixelColor(x,y));
            REQUIRE(img1->pixel(x,y)[0] == x);
        }
    }

    ImagePtr img2 = Image::ofDimensions(48,48);

    const auto& cell2 = tb.getOrElseUpdateCell(img2);
    REQUIRE(cell2.location.width == img2->width);
    REQUIRE(cell2.location.height == img2->height);
    REQUIRE(cell2.location.x > 0);
    REQUIRE(cell2.location.y > 0);

    REQUIRE((cell2.location.x != cell.location.x || cell2.location.y != cell.location.y));
}