#ifndef __PLAYOS_LAYER_SHELL_CONFIG_H__
#define __PLAYOS_LAYER_SHELL_CONFIG_H__

#include <stdint.h>


struct layer_shell_config {
    uint32_t width;
    uint32_t height;

    int layer;
    int anchor;
    int32_t top;
    int32_t right;
    int32_t bottom;
    int32_t left;
    int32_t zone;
};

#endif
