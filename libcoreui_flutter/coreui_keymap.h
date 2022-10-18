#ifndef __PLAYOS_COREUI_KEYMAP_H__
#define __PLAYOS_COREUI_KEYMAP_H__

#ifdef __cplusplus
extern "C" {
#endif

uint64_t coreui_getPhysicalKeyboardKey(int key);
uint64_t coreui_getLogicalKeyboardKey(int key);

#ifdef __cplusplus
}
#endif

#endif
