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
    GLuint boundBefore = AxGL::boundTextures[0];
    Texture::bind(0, name);

    if (!floatTexture) {
        AxGL::checkError();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
        AxGL::checkError();

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

Texture::Texture(const shared_ptr<Image> &image) : image(image), revision(1) {}

Texture::ptrType Texture::fromImage(std::shared_ptr<Image> image) {
    return std::make_shared<Texture>(image);
}

Texture::ptrType Texture::load(const std::string fromFile) {
    return std::make_shared<Texture>(Image::load(fromFile));
}

TextureBlock::TextureBlock(const ImagePtr &image) :
        Texture(image),
        binPack(image->width, image->height) {

}

TextureBlock::TextureBlock(int w, int h) :
        TextureBlock(Image::ofDimensions(w,h)) {

}

const TextureBlock::Cell &TextureBlock::getOrElseUpdateCell(const ImagePtr &newImage) {
    return getOrElseUpdateCell(*newImage);

}

const TextureBlock::Cell &TextureBlock::getOrElseUpdateCell(const Image &newImage) {
    std::lock_guard<std::mutex> guard(mapLock);
    return imageCells.getOrElseUpdate(newImage.uid, [&]() {
        int targetWidth = newImage.width + border * 2;
        int targetHeight = newImage.height + border * 2;
        rbp::Rect res = binPack.Insert(targetWidth, targetHeight, rbp::MaxRectsBinPack::RectBestAreaFit);

        Cell newCell;
        if (res.width != targetWidth) {
            newCell.rotated = true;
        }
        newCell.location = Rect<int>(res.x + border, res.y + border, res.width - border * 2, res.height - border * 2);
        newCell.texCoordRect = Rect<float>(newCell.location.x / float(image->width),
                                           newCell.location.y / float(image->height),
                                           newCell.location.width / float(image->width),
                                           newCell.location.height / float(image->height));
        int indexOffset = newCell.rotated ? 1 : 0;
        newCell.texCoords[(0 + indexOffset) % 4] = newCell.texCoordRect.xy();
        newCell.texCoords[(1 + indexOffset) % 4] = newCell.texCoordRect.xy() + glm::vec2(newCell.texCoordRect.width,0.0f);
        newCell.texCoords[(2 + indexOffset) % 4] = newCell.texCoordRect.xy() + glm::vec2(newCell.texCoordRect.width,newCell.texCoordRect.height);
        newCell.texCoords[(3 + indexOffset) % 4] = newCell.texCoordRect.xy() + glm::vec2(0.0f,newCell.texCoordRect.height);

        if (!newCell.rotated) {
            for (int y = 0; y < newImage.height; ++y) {
                image->copySection(newImage.row(y), newCell.location.x, newCell.location.y + y, newImage.width);
                for (int x = 0; x < border; ++x) {
                    image->pixelColor(newCell.location.x+x-border,newCell.location.y+y) = newImage.pixelColor(0,y);
                    image->pixelColor(newCell.location.x+newCell.location.width+x,newCell.location.y+y) = newImage.pixelColor(newImage.width-1,y);
                }
            }

            for (int x = -border; x < newCell.location.width+border; ++x) {
                int ax = std::min(std::max(x,0),newImage.width-1);
                for (int y = 0; y < border; ++y) {
                    image->pixelColor(newCell.location.x + x, newCell.location.y - y - 1) = newImage.pixelColor(ax, 0);
                    image->pixelColor(newCell.location.x + x, newCell.location.y + newCell.location.height + y) = newImage.pixelColor(ax, newImage.height-1);
                }
            }
        } else {
            Noto::error("We're not currently supporting rotation of images in texture blocks");
        }

        revision.fetch_add(1);

        return newCell;
    });
}

TextureBlock::Cell::Cell() {}
