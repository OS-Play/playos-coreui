#include "gles.h"
#include "libwallpaper.h"
#include "private.h"

#include <EGL/egl.h>

#include <stddef.h>
#include <stdio.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#ifdef __cplusplus
extern "C" {
#endif

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
"uniform float alpha;\n"
"\n"
"void main() {\n"
"   gl_FragColor = texture2D(tex, v_texcoord) * 1.0f;\n"
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
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            printf("ERROR::SHADER_COMPILATION_ERROR of type: %s\n %s\n -- --------------------------------------------------- -- ",
                type, infoLog);
        }
    } else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
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

void gles_init()
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
}

void gles_set_texture(const void *data, int width, int height)
{
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
}

void gles_draw(struct pos_wallpaper *ctx)
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // bind textures on corresponding texture units
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture1);

    glUniformMatrix4fv(s_proj_id, 1, GL_FALSE, (const float *)ctx->projection);

    // render container
    glUseProgram(s_program);
    glBindVertexArrayOES(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void gles_destroy()
{
    glDeleteVertexArraysOES(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    glDeleteProgram(s_program);
    glDeleteTextures(1, &texture1);
}

void gles_get_projection(int width, int height, int iw, int ih, enum pos_scale_mode mode, float _projection[16])
{
    glm::mat4 model         = glm::mat4(1.0f);
    glm::mat4 view          = glm::mat4(1.0f);
    glm::mat4 projection    = glm::mat4(1.0f);

    glm::vec2 position = {0.0f, 0.0f};
    glm::vec2 size = {iw,  ih};

    int scaleW = width;
    int scaleH = height;
    switch (mode) {
    case Fit:
        model = glm::scale(model, glm::vec3(size, 1.0f));
        break;
    case Fill:
        scaleW = width;
        scaleH = height;
        break;
    case Center:
        if (width < iw || height < ih) {

        } else {

        }

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

    auto value = projection * model;

    memcpy(_projection, glm::value_ptr(value), sizeof(float) * 16);
}

#ifdef __cplusplus
}
#endif
