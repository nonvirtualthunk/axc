//
// Created by nvt on 8/13/16.
//

#ifndef TEST2_RESOURCEMANAGER_H
#define TEST2_RESOURCEMANAGER_H

#include <containers/ArxMap.h>

class Font;
class Shader;
class Image;
class TextureBlock;

class ResourceManager {
protected:
    static ResourceManager& inst();

    Arx::Map<std::string,std::shared_ptr<Shader>> shaders;
    Arx::Map<std::string,std::shared_ptr<Image>> images;


public:
    static std::shared_ptr<Shader> shader(const std::string& path);
    static std::shared_ptr<Image> image(const std::string& path);
    static std::shared_ptr<Font> font(const std::string& path, TextureBlock * tb);

    static std::string adjustedPath(const std::string& path);
};


#endif //TEST2_RESOURCEMANAGER_H
