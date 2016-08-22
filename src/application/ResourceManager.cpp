//
// Created by nvt on 8/13/16.
//

#include <fstream>
#include "ResourceManager.h"

#include "core/Predef.h"

#include <graphics/Shader.h>
#include <graphics/Image.h>
#include <gui/Font.h>


Arx::String readAll(const char * path) {
    std::ifstream vStream;
    vStream.open(path);
    if (vStream.good()) {
        std::string line;
        Arx::String all;

        while (std::getline(vStream, line, '\n')) {
            all.append(line.c_str());
            all.append('\n');
        }
        return all;
    } else {
        Noto::error("Could not read non existent file: {}", path);
        throw "Could not read non existent file";
    }
}


ResourceManager &ResourceManager::inst() {
    static ResourceManager manager;
    return manager;
}

std::shared_ptr<Shader> ResourceManager::shader(const std::string &path) {
    return inst().shaders.getOrElseUpdate(path,[&](){
        auto ret = std::make_shared<Shader>();

        auto vpath = ResourceManager::adjustedPath(path + ".vertex");
        Arx::String vsrc = readAll(vpath.c_str());
        auto fpath = ResourceManager::adjustedPath(path + ".fragment");
        Arx::String fsrc = readAll(fpath.c_str());

        ret->load(vsrc,fsrc);
        return ret;
    });
}

std::shared_ptr<Image> ResourceManager::image(const std::string &path) {
    return inst().images.getOrElseUpdate(path,[&](){
        return Image::load(ResourceManager::adjustedPath(path));
    });
}

std::string ResourceManager::adjustedPath(const std::string &path) {
    return "resources/" + path;
}

std::shared_ptr<Font> ResourceManager::font(const std::string &path, TextureBlock * tb) {
    auto apath = adjustedPath(path);
    return Font::fromFile(apath.c_str(),tb);
}
