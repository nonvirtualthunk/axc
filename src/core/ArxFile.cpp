//
// Created by Sam Bock on 9/3/16.
//

#include <fstream>
#include "ArxFile.h"
#include "Predef.h"
#include <Noto.h>
#include <boost/filesystem.hpp>


namespace Arx {
    String removeTrailingSlash(const String &str) {
        return str.dropRightWhile(fnc(c == '/'));
    }

    String removeLeadingSlash(const String &str) {
        return str.dropWhile(fnc(c == '/'));
    }

    File::File(const Arx::String &path) : path(removeTrailingSlash(path)) {

    }

    File File::parent() const {
        auto finalSection = path.takeRightWhile(fnc(c != '/'));
        if (finalSection == "..") {
            return File(path + "/..");
        } else if (finalSection.isEmpty()) {
            return File("..");
        } else {
            return File(path.dropRight(finalSection.size() + 1));
        }
    }

    File File::relativeFile(const String &childPathRaw) const {
        auto childPath = removeLeadingSlash(removeTrailingSlash(childPathRaw));
        return File(path + "/" + childPath);
    }

    bool File::exists() const {
        std::ifstream stream(path);
        return stream.good();
    }

    File::File(const File &file) : path(file.path) {
    }

    File &File::operator=(const File &other) {
        path = other.path;
        return *this;
    }

    Arx::String File::toString() const {
        return path;
    }

    File File::withExtension(const String &extensionRaw) const {
        String extension = extensionRaw.startsWith(".") ? extensionRaw : String(".") + extensionRaw;
        String finalSection = extension.takeRightWhile(fnc(c != '/'));
        if (finalSection == "." || finalSection == "..") {
            Noto::error("Cannot change extension to {} on file with path {}", extension, path);
            return *this;
        } else {
            return Arx::File(path.dropRightWhile(fnc(c != '.')).dropRight(1) + extension);
        }
    }

    void File::write(const String &data) const {
        std::ofstream stream(path.raw());
        stream << data.raw();
    }

    String File::fileName() const {
        return path.takeRightWhile(fnc(c != '/'));
    }

    Sequence<File> File::children() {
        Sequence<File> ret;
        if (exists()) {
            boost::filesystem::directory_iterator end;
            for (boost::filesystem::directory_iterator iter(boost::filesystem::path(path.raw()));
                 iter != end;
                 ++iter) {
                const char * childPathName = (*iter).path().string().c_str();
                ret.add(Arx::File(childPathName));
            }
        }
        return ret;
    }

    String File::readAllText() const {
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
}
