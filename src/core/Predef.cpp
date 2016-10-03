//
// Created by nvt on 8/4/16.
//

#include <core/Predef.h>
#include <chrono>
#include <Noto.h>
#include <fstream>
#include <GLFW/glfw3.h>

double elapsedSeconds() {
    return glfwGetTime();
}

long long int elapsedMillis() {
//    return chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count();
    return (long long int) (glfwGetTime() * 1000);
}

long long int elapsedNanos() {
//    return chrono::duration_cast<chrono::nanoseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count();
    return (long long int) (glfwGetTime() * 1e9);
}


long long int epochMillisSystem() {
    Noto::warn("Doesn't work?");
    return chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();
}

const char *getTmpDir() {
    return "/tmp";
}

const char *pathify(const char *path) {
    return path;
}

void readAllBytes(char const *rawPath, std::vector<char> &out) {
    const char * filename = pathify(rawPath);
    ifstream ifs(filename, ios::binary|ios::ate);
    ifstream::pos_type pos = ifs.tellg();
    int ipos = pos;

    out.resize(ipos);

    ifs.seekg(0, ios::beg);
    ifs.read(out.data(), pos);
}

atomic_int &IdCounter::getIdCounter() {
    static atomic_int idCounter(1);
    return idCounter;
}