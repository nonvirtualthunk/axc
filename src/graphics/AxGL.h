//
// Created by Sam Bock on 8/12/16.
//

#ifndef TEST2_AXGL_H
#define TEST2_AXGL_H

#include <OpenGL/OpenGL.h>

class AxGL {
public:
    struct Viewport {
        int x,y,w,h;
    };
public:
    static GLuint boundArrayBuffer;
    static GLuint boundIndexBuffer;
    static GLuint boundTextures[10];
    static int activeTextureSlot;
    static GLuint boundVertexArrayObject;
    static bool vertexAttribEnabled[20];
    static GLuint activeShader;
    static Viewport activeViewport;

    static void checkError();
    static void setViewport(int x,int y,int w,int h);
};

#endif //TEST2_AXGL_H
