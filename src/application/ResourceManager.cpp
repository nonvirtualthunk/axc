//
// Created by nvt on 8/13/16.
//

#include <fstream>
#include "ResourceManager.h"

#include "core/Predef.h"

#include <graphics/Shader.h>
#include <graphics/Image.h>
#include <gui/Font.h>
#include <conf/AxmParser.h>
#include <core/ArxFile.h>
#include <core/Parsing.h>

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

std::shared_ptr<Shader> ResourceManager::shader(const Arx::String &path) {
    return inst().shaders.getOrElseUpdate(path,[&](){
        auto ret = std::make_shared<Shader>();

        auto vpath = ResourceManager::adjustedPath(path + ".vertex");
        Arx::String vsrc = readAll(vpath.raw());
        auto fpath = ResourceManager::adjustedPath(path + ".fragment");
        Arx::String fsrc = readAll(fpath.raw());

        ret->load(vsrc.raw(),fsrc.raw());
        return ret;
    });
}

std::shared_ptr<Image> ResourceManager::image(const Arx::String &path) {
    return inst().images.getOrElseUpdate(path,[&](){
        return Image::load(ResourceManager::adjustedPath(path));
    });
}

Arx::String ResourceManager::adjustedPath(const Arx::String &path) {
    return rootResourceDirectory().relativeFile(path).path;
}

std::shared_ptr<Font> ResourceManager::font(const Arx::String &path, TextureBlock * tb) {
    auto apath = adjustedPath(path);
    Arx::File file(apath);
    return Font::fromFile(file,tb);
}

Arx::String ResourceManager::loadTextFile(const Arx::File &file) {
    return readAll(file.path.raw());
}

const AxmNode& ResourceManager::conf(const Arx::String &path) {
    AxmParser parser;

    ParsingContext ctxt;
    auto apath = adjustedPath(path);
    ctxt.rootDirectory = rootResourceDirectory(); // the base directory for resources in general
    ctxt.referenceDirectory = Arx::File(apath).parent(); // the directory containing the target file
            auto node = parser.parse(ctxt, readAll(apath.raw()));

    inst().confNodes.put(apath, node);
    return *node;
}

Arx::File ResourceManager::rootResourceDirectory() {
    return Arx::File("resources/");
}
