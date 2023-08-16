#include "ebo.h"

static void _bindEBO(EBO *this)
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ID);
}

static void _unbindEBO()
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

static void _storeIndexes(EBO *this, void *data, GLsizeiptr data_size)
{
    this->bind(this);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, data_size, data, GL_STATIC_DRAW);
}


static void _destroy(EBO *this)
{
    glDeleteBuffers(1, &this->ID);
}

EBO initEBO()
{
    EBO ebo = {0};
    glGenBuffers(1, &ebo.ID);
    return ebo;
}