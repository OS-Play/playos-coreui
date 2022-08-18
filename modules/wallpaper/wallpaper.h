#ifndef __PLAYOS_WALLPAPER_H__
#define __PLAYOS_WALLPAPER_H__

#include <glm/glm.hpp>
#include <vector>

namespace playos {

class Wallpaper {
public:
    enum ScaleMode {
        Fit,
        Fill,
        Center,
        Tile
    };

public:
    Wallpaper(int width, int height);
    ~Wallpaper();

    void draw();

    void setImage(void *image, int width, int height);
    void setBgColor(float r, float g, float b, float a);
    void setScaleMode(enum ScaleMode mode);

private:
    glm::mat4 calProjection(int width, int height, int iw, int ih, ScaleMode mode);

private:
    unsigned int VBO, VAO, EBO;
    unsigned int texture1;
    int s_program;
    int s_proj_id;
    glm::mat4 projection;

    int m_width;
    int m_height;

    int image_width;
    int image_height;
    enum ScaleMode mode;
    std::vector<float> bgColor;
};

}

#endif
