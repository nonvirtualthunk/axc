//
// Created by Sam Bock on 8/12/16.
//

#ifndef TEST2_IMAGE_H
#define TEST2_IMAGE_H


#include <string>
#include <Optional.h>
#include <glm/vec4.hpp>
#include "Color.h"

class Image;

typedef std::shared_ptr<Image> ImagePtr;

class Image {
public:
    unsigned char * data = nullptr;
    int width;
    int height;
    int components;
    int uid;
    unsigned int name;
    const Optional<std::string> loadedFrom;
    bool ownsData;


    Image(unsigned char *data, int width, int height, const Optional<std::string> &loadedFrom, bool ownsData);
    Image(int width, int height);
    Image(const std::string& loadFrom);
    ~Image();
    Image(const Image& img);

    inline const unsigned char * row(int y) const {
        return data + width * y * components;
    }
    inline unsigned char * row(int y) {
        return data + width * y * components;
    }
    inline const unsigned char * pixel(int x, int y) const {
        return row(y) + x * components;
    }
    inline unsigned char * pixel(int x, int y) {
        return row(y) + x * components;
    }


    inline const glm::vec4 pixelV4(int x, int y) {
        const unsigned char * p = pixel(x,y);
        return glm::vec4(p[0] / 255.0f, p[1] / 255.0f, p[2] / 255.0f, p[3] / 255.0f);
    }
    inline Color& pixelColor(int x, int y) {
        unsigned char * p = pixel(x,y);
        Color * cp = reinterpret_cast<Color *>(p);
        return *cp;
    }
    inline const Color& pixelColor(int x, int y) const {
        const unsigned char * p = pixel(x,y);
        const Color * cp = reinterpret_cast<const Color *>(p);
        return *cp;
    }

    void copySection(const unsigned char *from, int startX, int startY, int numPixels);

    bool valid() const {
        return data != nullptr;
    }
    bool isSentinel() const {
        return data == nullptr;
    }

    void writeToFile(const char * path);

    static ImagePtr load(const std::string& loadFrom);
    static ImagePtr ofDimensions(int w,int h);

    static ImagePtr sentinel();
protected:
    void assignUID();
    size_t hash() const;

public:
    bool operator==(const Image &rhs) const;
    bool operator!=(const Image &rhs) const;
};

#endif //TEST2_IMAGE_H
