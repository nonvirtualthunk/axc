//
// Created by Sam Bock on 8/12/16.
//

#ifndef TEST2_AXGL_H
#define TEST2_AXGL_H

#include <OpenGL/OpenGL.h>
#include <Optional.h>

class Shader;

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
    static Optional<Shader*> activeShaderObject;
    static Viewport activeViewport;

    static void checkError();
    static void setViewport(int x,int y,int w,int h);
    static float aspectRatio();
};

#endif //TEST2_AXGL_H
