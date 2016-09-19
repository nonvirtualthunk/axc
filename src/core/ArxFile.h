//
// Created by Sam Bock on 9/3/16.
//

#ifndef TEST2_ARXFILE_H
#define TEST2_ARXFILE_H


#include "ArxString.h"

namespace Arx {
    class File {
    public:
        String path;

    public:
        File(const Arx::String &path);
        File(const File& file);
        File& operator=(const File& other);


        File parent() const;
        File relativeFile(const String &childPath) const;
        File withExtension(const String& extension) const;
        String fileName() const;
        String extension() const;

        Sequence<File> children();

        void write(const String& data) const;
        String readAllText() const;

        bool exists() const;
        Arx::String toString() const;
    };
}

#endif //TEST2_ARXFILE_H
