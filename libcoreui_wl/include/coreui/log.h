#ifndef __PLAYOS_LOG_H__
#define __PLAYOS_LOG_H__

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#define LOG_DEBUG(TAG, fmt, ...) printf("[" TAG "] " fmt, __VA_ARGS__)
#define LOG_ERROR(TAG, fmt, ...) printf("[" TAG "] " fmt, __VA_ARGS__)
#define LOG_WARNING(TAG, fmt, ...) printf("[" TAG "] " fmt, __VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif
