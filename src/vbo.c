#include "vbo.h"

static void _bindVBO(VBO *this)
{
    glBindBuffer(GL_ARRAY_BUFFER, this->ID);
}

static void _unbindVBO()
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

static void _bufferVBO(VBO *this, void* data, GLsizeiptr dataSize, bool dynamic)
{
    this->bind(this);
    glBufferData(GL_ARRAY_BUFFER, dataSize, data, dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
}
static void _destroyVBO(VBO *this)
{
    glDeleteBuffers(1, &this->ID);
}

VBO initVBO()
{
    VBO vbo = {0};
    vbo.bind = _bindVBO;
    vbo.unbind = _unbindVBO;
    vbo.buffer = _bufferVBO;
    vbo.destroy = _destroyVBO;
    glGenBuffers(1, &vbo.ID);
    return vbo;
}