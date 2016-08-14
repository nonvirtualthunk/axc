//
// Created by nvt on 8/13/16.
//

#include <fstream>
#include "ResourceManager.h"

#include "core/Predef.h"


Arx::String readAll(const char * path) {
    std::ifstream vStream;
    vStream.open(path);

    std::string line;
    Arx::String all;

    while (std::getline(vStream, line, '\n')) {
        all.append(line.c_str());
        all.append('\n');
    }
    return all;
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
