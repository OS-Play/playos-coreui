#include "wallpaper.h"

#include <stdio.h>

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

static float vertices[] = {
    // positions            // texture coords
    1.0f,  1.0f, 0.0f,     1.0f, 1.0f, // top right
    1.0f,  0.0f, 0.0f,     1.0f, 0.0f, // bottom right
    0.0f,  0.0f, 0.0f,     0.0f, 0.0f, // bottom left
    0.0f,  1.0f, 0.0f,     0.0f, 1.0f  // top left 
};
static unsigned int indices[] = {
    0, 1, 3, // first triangle
    1, 2, 3  // second triangle
};

static const char *s_vshader =
"uniform mat4 proj;\n"
"attribute vec3 pos;\n"
"attribute vec2 texcoord;\n"
"varying vec2 v_texcoord;\n"
"\n"
"void main() {\n"
"   gl_Position = proj * vec4(pos, 1.0);\n"
"   v_texcoord = texcoord;\n"
"}\n";

static const char *s_fshader =
"precision mediump float;\n"
"varying vec2 v_texcoord;\n"
"uniform sampler2D tex;\n"
"\n"
"void main() {\n"
"   gl_FragColor = texture2D(tex, v_texcoord);\n"
"}\n";


static unsigned int VBO, VAO, EBO;
static unsigned int texture1;
static int s_program;
static int s_proj_id;

static void checkCompileErrors(unsigned int shader, const char *type)
{
    int success;
    char infoLog[1024];
    if (strcmp(type, "PROGRAM") == 0) {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            printf("ERROR::SHADER_COMPILATION_ERROR of type: %s\n %s\n -- --------------------------------------------------- -- ",
                type, infoLog);
        }
    } else {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            printf("ERROR::PROGRAM_LINKING_ERROR of type: %s\n %s\n -- --------------------------------------------------- -- ",
                type, infoLog);
        }
    }
}

static int compileShader(const char *shader, int type)
{
    int shaderId = glCreateShader(type);
    glShaderSource(shaderId, 1, &shader, NULL);
    glCompileShader(shaderId);

    return shaderId;
}

static int compileProgram(const char *vs, const char *fs)
{
    int vShader = compileShader(vs, GL_VERTEX_SHADER);
    checkCompileErrors(vShader, "SHADER");
    int fShader = compileShader(fs, GL_FRAGMENT_SHADER);
    checkCompileErrors(fShader, "SHADER");

    int id = glCreateProgram();
    glAttachShader(id, vShader);
    glAttachShader(id, fShader);
    glLinkProgram(id);
    checkCompileErrors(id, "PROGRAM");
    // delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vShader);
    glDeleteShader(fShader);

    return id;
}

namespace playos {

Wallpaper::Wallpaper(int width, int height):
        m_width(width), m_height(height), mode(Fit),
        bgColor(4)
{
    glGenVertexArraysOES(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArrayOES(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    s_program = compileProgram(s_vshader, s_fshader);
    glUseProgram(s_program);
    s_proj_id = glGetUniformLocation(s_program, "proj");

    // position attribute
    int attrId = glGetAttribLocation(s_program, "pos");
    glVertexAttribPointer(attrId, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(attrId);

    // texture coord attribute
    attrId = glGetAttribLocation(s_program, "texcoord");
    glVertexAttribPointer(attrId, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(attrId);

    // texture 1
    // ---------
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
     // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glUniform1i(glGetUniformLocation(s_program, "tex"), 0);
}

Wallpaper::~Wallpaper()
{
    glDeleteVertexArraysOES(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    glDeleteProgram(s_program);
    glDeleteTextures(1, &texture1);
}

void Wallpaper::draw()
{
    glClearColor(bgColor[0], bgColor[1], bgColor[2], bgColor[3]);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(s_program);
    // bind textures on corresponding texture units
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture1);

    glUniformMatrix4fv(s_proj_id, 1, GL_FALSE, glm::value_ptr(projection));

    // render container
    glBindVertexArrayOES(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void Wallpaper::setImage(void *image, int width, int height)
{
    image_width = width;
    image_height = height;

    glBindTexture(GL_TEXTURE_2D, texture1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);

    projection = calProjection(m_width, m_height, width, height, mode);
}

void Wallpaper::setBgColor(float r, float g, float b, float a)
{
    bgColor[0] = r;
    bgColor[1] = g;
    bgColor[2] = b;
    bgColor[3] = a;
}

void Wallpaper::setScaleMode(enum ScaleMode mode)
{
    this->mode = mode;
    projection = calProjection(m_width, m_height, image_width, image_height, mode);
}

glm::mat4 Wallpaper::calProjection(int width, int height, int iw, int ih, ScaleMode mode)
{
    glm::mat4 model         = glm::mat4(1.0f);
    glm::mat4 view          = glm::mat4(1.0f);
    glm::mat4 projection    = glm::mat4(1.0f);

    glm::vec2 position = {0.0f, 0.0f};
    glm::vec2 size = {iw,  ih};

    int scaleW = width;
    int scaleH = height;
    switch (mode) {
    case Fit: {
        float s = std::min(width*1.0/iw, height*1.0/ih);
        scaleW = iw * s;
        scaleH = ih * s;

        position.x = (width - scaleW) / 2;
        position.y = (height - scaleH) / 2;
    }
        break;
    case Fill:
        scaleW = width;
        scaleH = height;
        break;
    case Center:
        if (width < iw || height < ih) {
            float s = std::min(width*1.0/iw, height*1.0/ih);
            s *= 0.9;
            scaleW = iw * s;
            scaleH = ih * s;
        } else {
            scaleW = iw;
            scaleH = ih;
        }

        position.x = (width - scaleW) / 2;
        position.y = (height - scaleH) / 2;

        break;
    case Tile:
        if (iw < width && ih < height) {

        } else {
            scaleW = width;
            scaleH = height;
        }
        break;
    }
    model = glm::translate(model, glm::vec3(position, 0.0f));
    model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f));
    model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f));
    model = glm::scale(model, glm::vec3(scaleW, scaleH, 1.0f));

    // view  = glm::translate(view, glm::vec3(0.0f, 0.0f, 1.0f));
    // projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
    projection = glm::ortho(0.0f, (float)width, 0.0f, (float)height, -1.0f, 1.0f);

    return projection * model;
}

}
