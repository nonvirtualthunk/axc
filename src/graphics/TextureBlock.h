//
// Created by nvt on 8/13/16.
//

#ifndef TEST2_TEXTUREBLOCK_H
#define TEST2_TEXTUREBLOCK_H


#include <glm/vec2.hpp>
#include <math/Shapes.h>
#include <containers/ArxMap.h>
#include <mutex>
#include <glad/glad.h>
#include "Image.h"

class Texture {
protected:
    int committedRevision = 0;
public:
    typedef std::shared_ptr<Texture> ptrType;
    std::shared_ptr<Image> image;

    int revision = 1;

    float floatTexture = false;

    // OpenGL data
    GLuint name = 0;
    GLenum minFilter = GL_NEAREST;
    GLenum magFilter = GL_NEAREST;
    GLenum wrap = GL_CLAMP_TO_EDGE;
    bool mipmap = true;
    Optional<int> mipmapLevels;
    GLenum internalFormat = GL_RGBA;
    GLenum targetFormat = GL_RGBA;

    Texture(const std::shared_ptr<Image> &image);

    void bind(int n = 0);
    void commit();
    void commit(glm::ivec2 min, glm::ivec2 max);


    static void bind(int n, GLuint name);
    static ptrType load(const std::string fromFile);
    static ptrType fromImage(std::shared_ptr<Image> image);
};

class TextureBlock : public Texture {
protected:
    struct Cell {
        Rect<int> location;
        bool rotated;
        glm::vec2 texCoords[4];
    };

    std::mutex mapLock;
public:
    Arx::Map<int,Cell> imageLocations;

    TextureBlock(const std::shared_ptr<Image> &image);
};


#endif //TEST2_TEXTUREBLOCK_H
