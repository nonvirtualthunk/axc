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
#include <rbp/MaxRectsBinPack.h>

class Texture {
protected:
    // OpenGL data
    GLuint name = 0;
    int committedRevision = 0;
public:
    typedef std::shared_ptr<Texture> ptrType;
    std::shared_ptr<Image> image;

    std::atomic_int revision;

    float floatTexture = false;

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
    std::mutex mapLock;

    rbp::MaxRectsBinPack binPack;
public:
    struct Cell {
        Rect<int> location = Rect<int>();
        bool rotated = false;
        glm::vec2 texCoords[4];
        Rect<float> texCoordRect;

        Cell();
    };
    int border = 1;
public:

    Arx::Map<int,Cell> imageCells;

    TextureBlock(const ImagePtr &image);
    TextureBlock(int w,int h);

    const Cell& getOrElseUpdateCell(const ImagePtr& newImage);
    const Cell& getOrElseUpdateCell(const Image& newImage);
};


#endif //TEST2_TEXTUREBLOCK_H
