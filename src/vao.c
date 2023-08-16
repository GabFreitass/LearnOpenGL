#include "vao.h"

static void _bindVAO(VAO *this)
{
    glBindVertexArray(this->ID);
}

static void _unbindVAO()
{
    glBindVertexArray(0);
}

static void _configVertex(VAO *this, GLuint location, GLint size, GLenum type, GLuint stride, GLuint offset)
{
    this->bind(this);
    glVertexAttribPointer(location, size, type, GL_FALSE, stride * sizeof(type), (void *)(offset * sizeof(type)));
    glEnableVertexAttribArray(location);
}

static void _destroyVAO(VAO *this)
{
    glDeleteVertexArrays(1, &this->ID);
}

VAO initVAO()
{
    VAO vao = {0};
    glGenVertexArrays(1, &vao.ID);
    vao.bind = _bindVAO;
    vao.unbind = _unbindVAO;
    vao.configVertex = _configVertex;
    vao.destroy = _destroyVAO;
    return vao;
}