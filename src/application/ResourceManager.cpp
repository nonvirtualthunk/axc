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
        Arx::String vsrc = readAll(vpath.path.raw());
        auto fpath = ResourceManager::adjustedPath(path + ".fragment");
        Arx::String fsrc = readAll(fpath.path.raw());

        ret->load(vsrc.raw(),fsrc.raw());
        return ret;
    });
}

std::shared_ptr<Image> ResourceManager::image(const Arx::String &path) {
    File apath = ResourceManager::adjustedPath(path);
    return inst().images.getOrElseUpdate(apath.path,[&](){
        return Image::load(apath.path);
    });
}

Arx::File ResourceManager::adjustedPath(const Arx::String &path) {
    return rootResourceDirectory().relativeFile(path);
}

std::shared_ptr<Font> ResourceManager::font(const Arx::String &path, TextureBlock * tb) {
    Arx::File tmp(path);
    Arx::String extensionedPath = tmp.withExtension(".ttf").path;

    auto apath = adjustedPath(path);
    if (!apath.exists()) {
        apath = adjustedPath(fmt("fonts/{}",extensionedPath));
    }

    PathAndTexture ident(apath.path,tb);
    return ResourceManager::inst().fonts.getOrElseUpdate(ident,[&](){ return Font::fromFile(apath,tb); });

}

Arx::String ResourceManager::loadTextFile(const Arx::File &file) {
    return readAll(file.path.raw());
}

const AxmNode& ResourceManager::conf(const Arx::String &path) {
    AxmParser parser;

    ParsingContext ctxt;
    auto apath = adjustedPath(path);
    ctxt.rootDirectory = rootResourceDirectory(); // the base directory for resources in general
    ctxt.referenceDirectory = apath.parent(); // the directory containing the target file
    auto node = parser.parse(ctxt, readAll(apath.path.raw()));

    inst().confNodes.put(apath.path, node);
    return *node;
}

Arx::File ResourceManager::rootResourceDirectory() {
    return Arx::File("resources/");
}
