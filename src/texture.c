#include "texture.h"

static void _destructTexture(Texture *this)
{
    this->image->destructImage(this->image);
}

static void _bindTexture(Texture *this, GLenum texture)
{
    glActiveTexture(texture);
    glBindTexture(GL_TEXTURE_2D, this->ID);
}

static void _loadTexture(Texture *texture)
{
    glGenTextures(1, &texture->ID);
    glBindTexture(GL_TEXTURE_2D, texture->ID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    if (texture->image->nrChannels == 4)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture->image->width, texture->image->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture->image->data);
    else
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture->image->width, texture->image->height, 0, GL_RGB, GL_UNSIGNED_BYTE, texture->image->data);
    glGenerateMipmap(GL_TEXTURE_2D);
    texture->image->destructImage(texture->image);
}

Texture initTexture(Image *image)
{
    Texture texture = {0};
    texture.image = image;
    texture.bindTexture = _bindTexture;
    texture.destructTexture = _destructTexture;
    _loadTexture(&texture);
    return texture;
}
