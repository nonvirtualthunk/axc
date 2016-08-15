//
// Created by Sam Bock on 8/12/16.
//


#include <OpenGL/OpenGL.h>
#include <glad/glad.h>
#include <string>
#include "AxGL.h"
#include "Noto.h"

GLuint AxGL::boundArrayBuffer = 0;
GLuint AxGL::boundIndexBuffer = 0;
GLuint AxGL::boundTextures[10] = {0,0,0,0,0,0,0,0,0,0};
GLuint AxGL::boundVertexArrayObject = 0;
bool AxGL::vertexAttribEnabled[20] = {false,false,false,false,false,false,false,false,false,false,
                                      false,false,false,false,false,false,false,false,false,false};
int AxGL::activeTextureSlot = 0;
GLuint AxGL::activeShader = 0;
AxGL::Viewport AxGL::activeViewport;

void AxGL::checkError() {
    GLenum err = GL_NO_ERROR;
    while((err = glGetError()) != GL_NO_ERROR)
    {
        std::string errStr = "Unknown";
        if (err == GL_INVALID_ENUM) {
            errStr = "Invalid Enum";
        } else if (err == GL_INVALID_VALUE) {
            errStr = "Invalid Value";
        } else if (err == GL_INVALID_OPERATION) {
            errStr = "Invalid Operation";
        }
        Noto::warn("Error encountered: {}", errStr);
    }
}

void AxGL::setViewport(int x, int y, int w, int h) {
    auto& v = AxGL::activeViewport;
    if (x != v.x || y != v.y || w != v.w || h != v.h) {
        glViewport(x,y,w,h);
        v.x = x;
        v.y = y;
        v.w = w;
        v.h = h;
    }
}

float AxGL::aspectRatio() {
    return (float)AxGL::activeViewport.w / (float)AxGL::activeViewport.h;
}
