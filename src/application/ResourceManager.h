//
// Created by nvt on 8/13/16.
//

#ifndef TEST2_RESOURCEMANAGER_H
#define TEST2_RESOURCEMANAGER_H

#include <containers/ArxMap.h>
#include <core/ArxString.h>

class Font;
class Shader;
class Image;
class TextureBlock;
class AxmNode;
namespace Arx {
    class File;
    class String;
}

class ResourceManager {
protected:
    static ResourceManager& inst();

    Arx::Map<std::string,std::shared_ptr<Shader>> shaders;
    Arx::Map<std::string,std::shared_ptr<Image>> images;
    Arx::Map<std::string,std::shared_ptr<AxmNode>> confNodes;


public:
    static std::shared_ptr<Shader> shader(const Arx::String& path);
    static std::shared_ptr<Image> image(const Arx::String& path);
    static std::shared_ptr<Font> font(const Arx::String& path, TextureBlock * tb);
    static const AxmNode& conf(const Arx::String& path);

    static Arx::String adjustedPath(const Arx::String& path);
    static Arx::File rootResourceDirectory();

    static Arx::String loadTextFile(const Arx::File& file);
};


#endif //TEST2_RESOURCEMANAGER_H
