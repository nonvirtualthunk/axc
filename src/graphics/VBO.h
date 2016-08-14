//
// Created by Sam Bock on 8/11/16.
//

#ifndef TEST2_VBO_H
#define TEST2_VBO_H

#include <cstdint>
#include <algorithm>
#include <OpenGL/OpenGL.h>
#include <glad/glad.h>
#include <Noto.h>
#include <core/Predef.h>
#include "AxGL.h"
#include "VAO.h"

struct Attribute {
    int n;
    GLenum dataType;
    bool normalize;
    bool rawInteger = false;

    Attribute(int n, GLenum dataType, bool normalize = true) : n(n), dataType(dataType), normalize(normalize) {}

    int byteSize() const {
        int base = 0;
        if (dataType == GL_FLOAT || dataType == GL_UNSIGNED_INT || dataType == GL_INT) {
            base = 4;
        } else if (dataType == GL_SHORT || dataType == GL_UNSIGNED_SHORT || dataType == GL_HALF_FLOAT) {
            base = 2;
        } else if (dataType == GL_BYTE || dataType == GL_UNSIGNED_BYTE) {
            base = 1;
        }
        return base * n;
    }
};

template<typename T>
class GrowableArray {
    T* _raw = new T[1];
    int capacity = 1;
    int _size = 0;
public:
    int size() const {
        return _size;
    }

    void ensureSize(int s) {
        if (s > capacity) {
            int oldCapacity = capacity;
            while (s > capacity) {
                capacity <<= 1;
            }
            T * new_raw = new T[capacity];
            std::memcpy(new_raw, _raw, sizeof(T) * oldCapacity);
            delete[] _raw;
            _raw = new_raw;
        }
    }

    T& operator()(int i) {
        return _raw[i];
    }
    const T& operator()(int i) const {
        return _raw[i];
    }

    T* pointerNFromBack(int n) const {
        return _raw+_size-n-1;
    }
    T* pointerToIndex(int n) const {
        return _raw+n;
    }
    T* raw() const {
        return _raw;
    }

    void push_back(const T& t) {
        ensureSize(_size+1);
        _raw[_size] = t;
        _size++;
    }

    void clear() {
        if (capacity > 1) {
            T * old_raw = raw();
            _size = 0;
            capacity = 1;
            _raw = new T[1];
            delete[] old_raw;
        }
    }

public:
    virtual ~GrowableArray() {
        delete[] _raw;
    }
};


template<typename T>
class VBODataContainer {
public:
    GrowableArray<T> data;
    GLuint name = 0;
    const int byteStride = sizeof(T);
    int numSolidifiedElements = 0;

    void clear() {
        data.clear();
    }
};


class VBOs {
public:
    enum VBOState {
        Dirty,
        Updating,
        Updated,
        Solidifying,
        Clean
    };
};

template<typename VT, typename IT = uint16_t>
class VBO {


protected:
    VBODataContainer<VT> vertices;
    VBODataContainer<IT> indices;
    std::atomic<VBOs::VBOState> state;
    GLuint vao = 0;

public:
    VBO() {
        state.store(VBOs::Dirty);
    }

    void drawElements(GLuint usage = GL_DYNAMIC_DRAW) {
        AxGL::checkError();
        solidifyIfNecessary(usage);
        AxGL::checkError();
        VAO::bind(vao);
        AxGL::checkError();
        GLenum indexType;
        if (std::is_same<IT, uint16_t>::value) {
            indexType = GL_UNSIGNED_SHORT;
        } else if (std::is_same<IT, uint32_t>::value) {
            indexType = GL_UNSIGNED_INT;
        } else {
            Noto::error("Cannot use a VBO with a index type other than [uint16_t, uint32_t]");
            return;
        }
        AxGL::checkError();
        glDrawElements(GL_TRIANGLES,indices.numSolidifiedElements,indexType,0);
        AxGL::checkError();
    }

    void solidify(GLuint usage = GL_DYNAMIC_DRAW) {
        if (vao == 0) {
            glGenVertexArrays(1, &vao);
        }
        VAO::bind(vao);
        subSolidify(vertices, GL_ARRAY_BUFFER, usage);
        subSolidify(indices, GL_ELEMENT_ARRAY_BUFFER, usage);

        if (vertices.data.size() > 0) {
            setupGLPointers();
        }
        state.store(VBOs::Clean);
    }

    void solidifyIfNecessary(GLuint usage = GL_DYNAMIC_DRAW) {
        VBOs::VBOState expected = VBOs::Updated;
        if (state.compare_exchange_strong(expected, VBOs::Solidifying)) {
            solidify(usage);
        }
    }

    void clear() {
        vertices.clear();
        indices.clear();
    }

    bool changeState (VBOs::VBOState from, VBOs::VBOState to) {
        return state.compare_exchange_strong(from,to);
    }

protected:
    Arx::Set<GLint> normalizable = Set({GL_BYTE, GL_UNSIGNED_BYTE, GL_SHORT, GL_UNSIGNED_SHORT, GL_INT, GL_UNSIGNED_INT});

    void setupGLPointers() {
        if (AxGL::boundArrayBuffer != 0) {
            int offset = vertices.byteStride;
            for (int i = VT::attributes.size()-1; i >= 0; --i) {
                AxGL::checkError();
                toggleVertexAttrib(i,true);
                const Attribute& attr = VT::attributes(i);
                offset -= attr.byteSize();
                bool normalize = attr.normalize && normalizable.contains(attr.dataType);
                const void * offsetPtr = reinterpret_cast<void *>(offset);
                if (!attr.rawInteger) {
                    glVertexAttribPointer(i, attr.n, attr.dataType, (GLboolean) normalize, vertices.byteStride, offsetPtr);
                    AxGL::checkError();
                } else {
                    glVertexAttribIPointer(i, attr.n, attr.dataType, vertices.byteStride, offsetPtr);
                }
            }
        } else {
            Noto::error("Attribute vbos must be bound to be used");
        }
    }

    template<typename T>
    void subSolidify(VBODataContainer<T>& container, GLenum bufferType, GLuint usage) {
        if (state.load() == VBOs::Updating) { Noto::error("Attempting to solidify vbo mid-update"); }
        else {
            int solidifyingSize = container.data.size();
            if (solidifyingSize > 0) {
                if (container.name == 0) {
                    glGenBuffers(1,&container.name);
                }
                bind(bufferType,container.name);
                glBufferData(bufferType, container.byteStride * solidifyingSize, container.data.raw(), usage);
            }
            container.numSolidifiedElements = solidifyingSize;
        }
    }

    static void toggleVertexAttrib(int i, bool enable) {
        if (enable) {
            glEnableVertexAttribArray(i);
        } else {
            glDisableVertexAttribArray(i);
        }
        AxGL::vertexAttribEnabled[i] = enable;
    }

    static void bind(GLenum bufferType, GLuint name) {
        if (bufferType == GL_ARRAY_BUFFER) {
            if (AxGL::boundArrayBuffer != name) {
                glBindBuffer(bufferType, name);
                AxGL::boundArrayBuffer = name;
            }
        } else {
            if (AxGL::boundIndexBuffer != name) {
                glBindBuffer(bufferType, name);
                AxGL::boundIndexBuffer = name;
            }
        }
    }

public:
    // Add n new vertices to the back of the vertices and return the index of the first created vertex
    int addV(int n) {
        int ret = vertices.data.size();
        for (int i = 0; i < n;++i) {
            vertices.data.push_back(VT());
        }
        return ret;
    }

    int addI(int n) {
        int ret = indices.data.size();
        for (int i = 0; i < n;++i) {
            indices.data.push_back((IT)0);
        }
        return ret;
    }

    // Create 4 new vertices and 6 new indices pointing to them, then returns a pointer to the first of the vertices.
    // The pointer is only guaranteed to be valid until the next call that adds vertices.
    VT* addQuad() {
        int vi = addV(4);
        int ii = addI(6);

        VT* vp = vertices.data.pointerToIndex(vi);
        IT* ip = indices.data.pointerToIndex(ii);
        ip[0] = vi + 0;
        ip[1] = vi + 1;
        ip[2] = vi + 2;

        ip[3] = vi + 2;
        ip[4] = vi + 3;
        ip[5] = vi + 0;

        return vp;
    }
};


#endif //TEST2_VBO_H
