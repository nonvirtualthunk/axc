//
// Created by nvt on 8/13/16.
//

#include <Noto.h>
#include "TextureBlock.h"

#include "core/Predef.h"
#include "AxGL.h"
#include <glad/glad.h>

void Texture::bind(int n) {
    if (name == 0 || committedRevision < revision) {
        commit();
    }
    glBindTexture(GL_TEXTURE_2D, name);
}

void Texture::commit() {
    commit(glm::ivec2(0, 0), glm::ivec2(image->width, image->height));
}

void Texture::commit(glm::ivec2 min, glm::ivec2 max) {
    bool first = name == 0;
    if (name == 0) {
        glGenTextures(1, &name);
    }
    int boundBefore = AxGL::boundTextures[0];
    Texture::bind(0, name);

    if (!floatTexture) {
        glEnable(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);

        if (mipmap) {
            int maxLevels = (int) (log(std::min(image->width, image->height)) / log(2));
            int numLevels = mipmapLevels.orElse(maxLevels);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, std::max(numLevels - 1, 0));
        }

        if (first) {
            glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, image->width, image->height,
                         0, targetFormat, GL_UNSIGNED_BYTE, image->data);
        } else {
            GLsizei w = max.x - min.x;
            GLsizei h = max.y - min.y;
            glTexSubImage2D(GL_TEXTURE_2D, 0, (GLsizei) min.x, (GLsizei) min.y, w, h,
                            targetFormat, GL_UNSIGNED_BYTE, image->data);
        }

        if (mipmap) {
            glGenerateMipmap(GL_TEXTURE_2D);
        }
    } else {
        Noto::error("Float textures are not yet supported");
    }

    Texture::bind(0, boundBefore);
    committedRevision = revision;
}

void Texture::bind(int n, GLuint name) {
    if (AxGL::activeTextureSlot != n) {
        glActiveTexture((GLenum) (GL_TEXTURE0 + n));
        AxGL::activeTextureSlot = n;
    }

    if (AxGL::boundTextures[n] != name) {
        glBindTexture(GL_TEXTURE_2D, name);
        AxGL::boundTextures[n] = name;
    }
}

Texture::Texture(const shared_ptr<Image> &image) : image(image) {}

Texture::ptrType Texture::fromImage(std::shared_ptr<Image> image) {
    return std::make_shared<Texture>(image);
}

Texture::ptrType Texture::load(const std::string fromFile) {
    return std::make_shared<Texture>(Image::load(fromFile));
}

TextureBlock::TextureBlock(const shared_ptr<Image> &image) :
        Texture(image)
{

}
