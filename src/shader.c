#include "shader.h"

static void _createVertexShader(Shader *this, const GLchar *shaderSource)
{
    this->vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(this->vertexShader, 1, &shaderSource, NULL);
    free((void *)shaderSource);
}

static void _createFragmentShader(Shader *this, const GLchar *shaderSource)
{
    this->fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(this->fragmentShader, 1, &shaderSource, NULL);
    free((void *)shaderSource);
}

static void _compileShader(Shader *this, GLuint shader)
{

    glCompileShader(shader);
    // check for shader compile errors
    glGetShaderiv(shader, GL_COMPILE_STATUS, &this->success);
    if (!this->success)
    {
        glGetShaderInfoLog(shader, 512, NULL, this->infoLog);
        printf("ERROR::SHADER::COMPILATION_FAILED\n%s\n", this->infoLog);
    }
}

static void _linkShaders(Shader *this)
{
    // link shaders
    this->ID = glCreateProgram();
    glAttachShader(this->ID, this->vertexShader);
    glAttachShader(this->ID, this->fragmentShader);
    glLinkProgram(this->ID);
    // check for linking errors
    glGetProgramiv(this->ID, GL_LINK_STATUS, &this->success);
    if (!this->success)
    {
        glGetProgramInfoLog(this->ID, 512, NULL, this->infoLog);
        printf("ERROR::SHADER::PROGRAM::LINKING_FAILED\n%s\n", this->infoLog);
    }
    glDeleteShader(this->vertexShader);
    glDeleteShader(this->fragmentShader);
}

static void _destructShader(Shader *this)
{
    glDeleteProgram(this->ID);
}

static void _loadShader(Shader *shader, const GLchar *vertexShaderFile, const GLchar *fragmentShaderFile)
{
    const GLchar *vertexShaderSource = readFile(vertexShaderFile);
    const GLchar *fragmentShaderSource = readFile(fragmentShaderFile);
    shader->createVertexShader(shader, vertexShaderSource);
    shader->compileShader(shader, shader->vertexShader);
    shader->createFragmentShader(shader, fragmentShaderSource);
    shader->compileShader(shader, shader->fragmentShader);
    shader->linkShaders(shader);
}

static void _use(Shader *this)
{
    glUseProgram(this->ID);
}
// utility uniform functions
// ------------------------------------------------------------------------
static void _setBool(Shader *this, const GLchar *name, GLint value)
{
    this->use(this);
    glUniform1i(glGetUniformLocation(this->ID, name), value);
}
// ------------------------------------------------------------------------
static void _setInt(Shader *this, const GLchar *name, GLint value)
{
    this->use(this);
    glUniform1i(glGetUniformLocation(this->ID, name), value);
}
// ------------------------------------------------------------------------
static void _setFloat(Shader *this, const GLchar *name, GLfloat value)
{
    this->use(this);
    glUniform1f(glGetUniformLocation(this->ID, name), value);
}

static void _setMat4(Shader *this, const GLchar *name, mat4 value)
{
    this->use(this);
    glUniformMatrix4fv(glGetUniformLocation(this->ID, name), 1, GL_FALSE, value[0]);
}

static void _setVec3f(Shader *this, const GLchar *name, GLfloat v0, GLfloat v1, GLfloat v2)
{
    this->use(this);
    glUniform3f(glGetUniformLocation(this->ID, name), v0, v1, v2);
}

static void _setVec3fv(Shader *this, const GLchar *name, vec3 value)
{
    this->use(this);
    glUniform3fv(glGetUniformLocation(this->ID, name), 1, value);
}

Shader initShader(const GLchar *vertexShaderFile, const GLchar *fragmentShaderFile)
{
    Shader shader = {0};
    shader.createVertexShader = _createVertexShader;
    shader.createFragmentShader = _createFragmentShader;
    shader.compileShader = _compileShader;
    shader.linkShaders = _linkShaders;
    shader.destructShader = _destructShader;
    shader.use = _use;
    shader.setBool = _setBool;
    shader.setInt = _setInt;
    shader.setFloat = _setFloat;
    shader.setMat4 = _setMat4;
    shader.setVec3f = _setVec3f;
    shader.setVec3fv = _setVec3fv;

    _loadShader(&shader, vertexShaderFile, fragmentShaderFile);

    return shader;
}