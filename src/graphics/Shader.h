//
// Created by nvt on 8/13/16.
//

#ifndef TEST2_SHADER_H
#define TEST2_SHADER_H


#include <OpenGL/OpenGL.h>
#include <containers/ArxMap.h>
#include <core/ArxString.h>
#include <glm/mat4x4.hpp>
#include <Noto.h>


class SubShader {
public:
    GLuint name = 0;
    const GLenum shaderType;
    Arx::String source;
    Arx::Map<std::string, int> attribIndices;

    void load(const char * source);
    bool commit();
public:
    SubShader(GLenum shaderType);
};

class Shader {
public:
    GLuint name = 0;
    SubShader vertexShader;
    SubShader fragmentShader;
    Arx::Map<std::string, GLint> uniformLocations;

    Shader();

    void load(const char *vertexSource, const char *fragmentSource);
    void bind();
    bool commit();


    GLint uniformLocation(const std::string& uniformName);

    bool hasUniform(const std::string& uniformName);

    void setUniform(const std::string& uniformName, const int& newValue);

    void setUniform(const std::string& uniformName, const glm::mat4x4& newValue);

    static void bind(GLuint shaderName, Shader * shaderObject);
    static bool isGood(GLuint name, GLenum kind);
};

typedef std::shared_ptr<Shader> ShaderPtr;

#endif //TEST2_SHADER_H
