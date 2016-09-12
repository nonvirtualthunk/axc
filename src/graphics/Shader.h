//
// Created by nvt on 8/13/16.
//

#ifndef TEST2_SHADER_H
#define TEST2_SHADER_H


#include <OpenGL/OpenGL.h>
#include <containers/ArxMap.h>
#include <core/ArxString.h>
#include <glm/mat4x4.hpp>
#include <glm/ext.hpp>
#include <glad/glad.h>
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


    GLint uniformLocation(const std::string& uniformName) {
        return uniformLocations.getOrElseUpdate(uniformName,[&](){
            return glGetUniformLocation(name, uniformName.c_str());
        });
    }

    void setUniform(const std::string& uniformName, const int& newValue) {
        glUniform1i(uniformLocation(uniformName),newValue);
    }

    void setUniform(const std::string& uniformName, const glm::mat4x4& newValue) {
        glUniformMatrix4fv(uniformLocation(uniformName),1,GL_FALSE,glm::value_ptr(newValue));
    }

    static void bind(GLuint shaderName);
    static bool isGood(GLuint name, GLenum kind);
};

typedef std::shared_ptr<Shader> ShaderPtr;

#endif //TEST2_SHADER_H
