//
// Created by Sam Bock on 8/12/16.
//

#include <core/Predef.h>
#include "Image.h"

#define STBI_ONLY_PNG
#define STB_IMAGE_IMPLEMENTATION

#include "stb/stb_image.h"

Image::Image(const unsigned char *data, int width, int height, const std::string &loadedFrom) :
        data(data),
        width(width),
        height(height),
        loadedFrom(some(loadedFrom)),
        components(4) {
    assignUID();
}

Image::Image(int width, int height) :
        width(width),
        height(height),
        components(4),
        data(new unsigned char[width * height * 4]),
        loadedFrom(none<std::string>()) {
    assignUID();
}

Image::Image(const std::string &loadFrom) : loadedFrom(some(loadFrom)), components(4) {
    int comp;
    data = stbi_load(loadFrom.c_str(),&width,&height,&comp,components);
    assignUID();
}

std::shared_ptr<Image> Image::load(const std::string &loadFrom) {
    return std::make_shared<Image>(loadFrom);
}


void Image::assignUID() {
    uid = getTaggedIdCounter<Image>().fetch_add(1);
}


size_t Image::hash() const {
    return (size_t) (uid * 31);
}

bool Image::operator==(const Image &rhs) const {
    return uid == rhs.uid;
}

bool Image::operator!=(const Image &rhs) const {
    return !(rhs == *this);
}
