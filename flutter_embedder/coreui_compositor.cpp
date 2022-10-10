#include "coreui_compositor.h"

#include <stdio.h>
#include <memory>
#include <string.h>
#include <thread>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "coreui/window.h"

namespace playos {
namespace flutter {


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
static int s_program;
static int s_proj_id;


static bool compositor_create_backing_store_callback(const FlutterBackingStoreConfig* config,
            FlutterBackingStore* backing_store_out, void* user_data);

static bool compositor_collect_backing_store_callback(
        const FlutterBackingStore* renderer, void* user_data);

static bool compositor_present_layers_callback(const FlutterLayer** layers,
            size_t layers_count, void* user_data);

static void checkCompileErrors(unsigned int shader, const char *type)
{
    int success;
    char infoLog[1024];
    if (strcmp(type, "SHADER") == 0) {
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

glm::mat4 gles_get_projection(int width, int height, int x, int y, int iw, int ih)
{
    glm::mat4 model         = glm::mat4(1.0f);
    glm::mat4 view          = glm::mat4(1.0f);
    glm::mat4 projection    = glm::mat4(1.0f);

    glm::vec2 position = {x, y};
    glm::vec2 size = {iw,  ih};

    int scaleW = width;
    int scaleH = height;

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

Compositor::Compositor(std::shared_ptr<Window> window): m_window(window)
{
    window->makeCurrent();
    if (!gladLoadGLES2Loader((GLADloadproc)eglGetProcAddress)) {
        printf("Failed to load gles library\n");
    }

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
    glUniform1i(glGetUniformLocation(s_program, "tex"), 0);

    // position attribute
    int attrId = glGetAttribLocation(s_program, "pos");
    glVertexAttribPointer(attrId, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(attrId);

    // texture coord attribute
    attrId = glGetAttribLocation(s_program, "texcoord");
    glVertexAttribPointer(attrId, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(attrId);

    window->clearCurrent();
}

Compositor::~Compositor()
{
    m_window->makeCurrent();

    glDeleteVertexArraysOES(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    glDeleteProgram(s_program);

    m_window->clearCurrent();
}

bool Compositor::createBackingStore(const FlutterBackingStoreConfig* config,
        FlutterBackingStore* backing_store_out)
{
    backing_store_out->type = kFlutterBackingStoreTypeOpenGL;
    backing_store_out->open_gl.type = kFlutterOpenGLTargetTypeTexture;
    FlutterOpenGLTexture &texture = backing_store_out->open_gl.texture;

    glGenTextures(1, &texture.name);
    glBindTexture(GL_TEXTURE_2D, texture.name);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, config->size.width, config->size.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


    texture.target = GL_TEXTURE_2D;
    texture.format = GL_BGRA8_EXT; // other value may not work
    texture.width = config->size.width;
    texture.height = config->size.height;
    texture.destruction_callback = [](void* p) {};

    return true;
}

bool Compositor::collectBackingStore(const FlutterBackingStore* renderer)
{
    glDeleteTextures(1, &renderer->open_gl.texture.name);

    return true;
}

bool Compositor::presentLayers(const FlutterLayer** layers, size_t layers_count)
{
    glViewport(0, 0, m_window->width(), m_window->height());

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(s_program);

    for (int i = 0; i < layers_count; ++i) {
        const FlutterLayer* layer = layers[i];

        if (layer->type == kFlutterLayerContentTypeBackingStore) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, layer->backing_store->open_gl.texture.name);
        } else if (layer->type == kFlutterLayerContentTypePlatformView) {
            // printf(">>> kFlutterLayerContentTypePlatformView\n");
            continue;
        }

        glUniformMatrix4fv(s_proj_id, 1, GL_FALSE,
                glm::value_ptr(gles_get_projection(m_window->width(), m_window->height(), layer->offset.x, layer->offset.y, layer->size.width, layer->size.height)));
        glBindVertexArrayOES(VAO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }

    m_window->swapBuffer();

    return true;
}

FlutterCompositor Compositor::getFlutterCompositor()
{
    FlutterCompositor compositor = {};
    compositor.struct_size = sizeof(FlutterCompositor);
    compositor.user_data = this;
    compositor.create_backing_store_callback = compositor_create_backing_store_callback;
    compositor.collect_backing_store_callback = compositor_collect_backing_store_callback;
    compositor.present_layers_callback = compositor_present_layers_callback;

    return compositor;
}

bool compositor_create_backing_store_callback(const FlutterBackingStoreConfig* config,
            FlutterBackingStore* backing_store_out, void* user_data)
{
    playos::flutter::Compositor *compositor = (playos::flutter::Compositor *)user_data;

    return compositor->createBackingStore(config, backing_store_out);
}

bool compositor_collect_backing_store_callback(const FlutterBackingStore* renderer, void* user_data)
{
    playos::flutter::Compositor *compositor = (playos::flutter::Compositor *)user_data;

    return compositor->collectBackingStore(renderer);
}

bool compositor_present_layers_callback(const FlutterLayer** layers,
            size_t layers_count, void* user_data)
{
    playos::flutter::Compositor *compositor = (playos::flutter::Compositor *)user_data;

    return compositor->presentLayers(layers, layers_count);
}

}
}
