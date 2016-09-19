//
// Created by Sam Bock on 8/12/16.
//

#include <core/Predef.h>
#include <Noto.h>
#include "Image.h"

#define STBI_ONLY_PNG
#define STB_IMAGE_IMPLEMENTATION

#include "stb/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <stb/stb_image_write.h>

Image::Image(unsigned char * data, int width, int height, const Optional<std::string> &loadedFrom, bool ownsData) :
        data(data),
        width(width),
        height(height),
        loadedFrom(loadedFrom),
        components(4),
        ownsData(ownsData) {
    assignUID();
}

Image::Image(int width, int height) :
        width(width),
        height(height),
        components(4),
        data(new unsigned char[width * height * 4]),
        loadedFrom(none<std::string>()),
        ownsData(true) {
    memset(data, 0, (size_t) (width * height * 4));
    assignUID();
}

Image::Image(const std::string &loadFrom) : loadedFrom(some(loadFrom)), components(4), ownsData(true) {
    int comp;
    stbi_set_flip_vertically_on_load(true);
    data = stbi_load(loadFrom.c_str(),&width,&height,&comp,components);
    if (data == nullptr) {
        Noto::warn("Failed to load image from {}", loadFrom);
    }
    assignUID();
}

Image::Image(const Image &img) :
    width(img.width),
    height(img.height),
    components(img.components),
    loadedFrom(img.loadedFrom),
    ownsData(img.ownsData)
{
    if (ownsData) {
        data = new unsigned char[width * height * components];
        memcpy(data, img.data, (size_t) (width * height * components));
    } else {
        data = img.data;
    }
}


Image::~Image() {
    if (ownsData && data != nullptr) {
        delete[] data;
    }
}


ImagePtr Image::load(const std::string &loadFrom) {
    return std::make_shared<Image>(loadFrom);
}

ImagePtr Image::ofDimensions(int width, int height) {
    return std::make_shared<Image>(width, height);
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

std::shared_ptr<Image> Image::sentinel() {
    static std::shared_ptr<Image> tmp(new Image(nullptr,0,0,none<std::string>(), false));
    return tmp;
}

void Image::copySection(const unsigned char *from, int startX, int startY, int numPixels) {
    if (startX + numPixels >= width) {
        Noto::error("Attempted to copy more than a single row of data into image");
    } else {
        unsigned char * ptr = data + width * startY * components + startX * components;
        memcpy(ptr, from, (size_t) (numPixels * components));
    }
}

void Image::writeToFile(const char *path) {
    unsigned char * endRow = row(height-1);
    stbi_write_png(path,width,height,components,endRow,-width * components);
}

