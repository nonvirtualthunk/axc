//
// Created by Sam Bock on 8/12/16.
//


#include <catch.hpp>
#include <graphics/Image.h>

TEST_CASE("ImageLoading","[graphics][image]") {
    auto img = Image::load("resources/defaultium.png");

    REQUIRE(img->valid());
    REQUIRE(img->width == 256);
    REQUIRE(img->height == 256);
    REQUIRE(img->data != nullptr);
    REQUIRE(img->pixel(0,0)[0] == 255);
}