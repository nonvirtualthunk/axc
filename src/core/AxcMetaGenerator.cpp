//
// Created by Sam Bock on 9/7/16.
//

#include <iostream>
#include <core/Introspection.h>
#include <fstream>
#include <Noto.h>
#include "Format.h"

int main(int argc, char **argv) {
    StructParser parser;
    Arx::File backupDir("/tmp");
    long ms = chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count();
    for (int i = 1; i < argc; ++i) {
        Noto::info("Preprocessing file: {}",argv[i]);

        // Create a backup
        Arx::File srcFile(argv[i]);
        Arx::String filename = srcFile.fileName();
        Arx::String backupFilename = Format::format("{}_{}.backup",filename,ms);
        Arx::File backupFile = backupDir.relativeFile(filename);
        Noto::info("\tWriting backup to: {}",backupFilename);

        std::ifstream  src(srcFile.path.raw(), std::ios::binary);
        std::ofstream  dst(backupFile.path.raw(),   std::ios::binary);
        dst << src.rdbuf();

        parser.amendHeaderFile(Arx::File(Arx::String(argv[i])));
    }
}