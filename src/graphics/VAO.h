//
// Created by Sam Bock on 8/12/16.
//

#ifndef TEST2_VAO_H
#define TEST2_VAO_H

#include <OpenGL/OpenGL.h>

class VAO {
public:
    static void bind(GLuint name);
    static void unbind() {
        VAO::bind(0);
    }
};

#endif //TEST2_VAO_H
