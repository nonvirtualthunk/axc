//
// Created by nvt on 8/13/16.
//

#include <glad/glad.h>
#include <sstream>
#include <bstrlib/bstrwrap.h>
#include <core/ArxString.h>
#include <Noto.h>
#include "Shader.h"
#include <glm/ext.hpp>
#include <glad/glad.h>
#include <Optional.h>

#include "core/Predef.h"
#include "AxGL.h"

void Shader::load(const char *vertexSource, const char *fragmentSource) {
    vertexShader.load(vertexSource);
    fragmentShader.load(fragmentSource);
}

bool SubShader::commit() {
    name = glCreateShader(shaderType);

    const GLchar * pointers[] = {source.raw()};
    int lengths[] = {source.size()};
    glShaderSource(name, (GLsizei) 1, pointers, lengths);
    glCompileShader(name);

    if (!Shader::isGood(name, GL_COMPILE_STATUS)) {
        Noto::warn("Non-compilable shader code was:\n{}",source.raw());
        return false;
    }
    return true;
}


void SubShader::load(const char *source_raw) {
    if (shaderType == GL_VERTEX_SHADER) {
        std::stringstream ss(source_raw);
        std::string line;

        int count = 0;
        while (std::getline(ss, line, '\n')) {
            Arx::String str(line);

            if (str.startsWith("in ")) {
                auto initialSection = str
                        .takeWhile([](char c) { return c != '/' && c != ';'; })
                        .trimmed()
                        .split(' ');
                attribIndices.put(initialSection[2], count++);

            }
        }
    }

    source = Arx::String(source_raw);
}


void Shader::bind() {
    if (name == 0) {
        if (!commit()) {
            Noto::error("Could not commit shader, error encountered ^");
            return;
        }
    }

    Shader::bind(name, this);
}

void Shader::bind(GLuint name, Shader * shaderObject) {
    if (AxGL::activeShader != name) {
        glUseProgram(name);
        AxGL::activeShader = name;
        AxGL::activeShaderObject = Optional<Shader*>::some(shaderObject);
    }
}

bool Shader::commit() {
    if (name == 0) {
        name = glCreateProgram();
    }
    if (!vertexShader.commit()) { return false; }
    if (!fragmentShader.commit()) { return false; }

    glAttachShader(name, vertexShader.name);
    glAttachShader(name, fragmentShader.name);

    vertexShader.attribIndices.foreach([&](Arx::String key, int index){
        glBindAttribLocation(name, (GLuint) index, key.raw());
        Noto::info("Binding attribute {} -> {}", key.raw(), index);
    });

    glLinkProgram(name);
    glValidateProgram(name);

    return Shader::isGood(name, GL_LINK_STATUS);
}


SubShader::SubShader(GLenum shaderType) : shaderType(shaderType) {}

bool Shader::isGood(GLuint name, GLenum kind) {
    GLint result;
    if (kind == GL_COMPILE_STATUS) {
        glGetShaderiv(name, kind, &result);
    } else if (kind == GL_LINK_STATUS) {
        glGetProgramiv(name, kind, &result);
    } else {
        Noto::error("isGood check in shader given invalid kind {}", kind);
        return false;
    }

    if (result != GL_TRUE) {
        int maxChars = 5000;
        int numChars = 0;
        GLchar resultStr[maxChars];
        if (kind == GL_COMPILE_STATUS) {
            glGetShaderInfoLog(name, maxChars, &numChars, resultStr);
        } else if (kind == GL_LINK_STATUS) {
            glGetProgramInfoLog(name, maxChars, &numChars, resultStr);
        }

        std::string operation = kind == GL_COMPILE_STATUS ? "compile" : "link";
        Noto::warn("Shader {} was not good:\n{}", operation, std::string(resultStr));
    }
    return result == GL_TRUE;
}

Shader::Shader() : vertexShader(GL_VERTEX_SHADER), fragmentShader(GL_FRAGMENT_SHADER) {}

GLint Shader::uniformLocation(const std::string &uniformName) {
    return uniformLocations.getOrElseUpdate(uniformName,[&](){
        return glGetUniformLocation(name, uniformName.c_str());
    });
}

bool Shader::hasUniform(const std::string &uniformName) {
    return uniformLocations.contains(uniformName);
}

void Shader::setUniform(const std::string &uniformName, const int &newValue) {
    glUniform1i(uniformLocation(uniformName),newValue);
}

void Shader::setUniform(const std::string &uniformName, const glm::mat4x4 &newValue) {
    glUniformMatrix4fv(uniformLocation(uniformName),1,GL_FALSE,glm::value_ptr(newValue));
}
