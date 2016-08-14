//
// Created by Sam Bock on 8/12/16.
//
#include <OpenGL/OpenGL.h>
#include <glad/glad.h>
#include "AxGL.h"
#include "VAO.h"


void VAO::bind(GLuint name) {
    if (AxGL::boundVertexArrayObject != name) {
        glBindVertexArray(name);
        AxGL::boundVertexArrayObject = name;
    }
}