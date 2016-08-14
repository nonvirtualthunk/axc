//
// Created by Sam Bock on 8/12/16.
//

#ifndef TEST2_IMAGE_H
#define TEST2_IMAGE_H


#include <string>
#include <Optional.h>

class Image {
public:
    const unsigned char * data;
    int width;
    int height;
    int components;
    int uid;
    unsigned int name;
    const Optional<std::string> loadedFrom;


    Image(const unsigned char *data, int width, int height, const std::string &loadedFrom);
    Image(int width, int height);
    Image(const std::string& loadFrom);


    inline const unsigned char * row(int y) {
        return data + width * y * components;
    }
    inline const unsigned char * pixel(int x, int y) {
        return row(y) + x * components;
    }

    bool valid() const {
        return data != nullptr;
    }

    static std::shared_ptr<Image> load(const std::string& loadFrom);

protected:
    void assignUID();
    size_t hash() const;

public:
    bool operator==(const Image &rhs) const;
    bool operator!=(const Image &rhs) const;
};


#endif //TEST2_IMAGE_H
