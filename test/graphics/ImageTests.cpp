//
// Created by Sam Bock on 8/12/16.
//


#include <catch.hpp>
#include <graphics/Image.h>
#include <core/ArxString.h>
#include <core/Predef.h>
#include <glm/vec2.hpp>

TEST_CASE("ImageLoading","[graphics][image]") {
    auto img = Image::load("resources/defaultium.png");

    REQUIRE(img->valid());
    REQUIRE(img->width == 256);
    REQUIRE(img->height == 256);
    REQUIRE(img->data != nullptr);
    REQUIRE(img->pixel(0,0)[0] == 255);
}

TEST_CASE("ImageWriteRead","[graphics][image]") {
    auto img = Image::ofDimensions(64,32);

    for (int y = 0; y < 32; ++y) {
        for (int x = 0; x < 32; ++x) {
            img->pixelColor(x,y) = Color(x,y,0,255);
        }
    }

    auto path = Arx::String(getTmpDir()) + "/test_image_64_32.png";
    img->writeToFile(path);

    auto img2 = Image::load(path);

    glm::ivec2 differentPixel(-1,-1);
    for (int y = 0; y < 32; ++y) {
        for (int x = 0; x < 32; ++x) {
            if (img2->pixelColor(x,y) != Color(x,y,0,255)) {
                differentPixel = glm::ivec2(x,y);
            }
        }
    }

    REQUIRE(differentPixel == glm::ivec2(-1,-1));
}