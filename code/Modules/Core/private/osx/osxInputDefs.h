#pragma once
//------------------------------------------------------------------------------
/**
    @brief Cocoa input-related defines (taken from and compatible with GLFW)
*/
#define ORYOL_OSXBRIDGE_WHEEL_DELTA 120

#define ORYOL_OSXBRIDGE_MOD_SHIFT   0x0001
#define ORYOL_OSXBRIDGE_MOD_CONTROL 0x0002
#define ORYOL_OSXBRIDGE_MOD_ALT     0x0004
#define ORYOL_OSXBRIDGE_MOD_SUPER   0x0008

#define ORYOL_OSXBRIDGE_MOUSE_BUTTON_1         0
#define ORYOL_OSXBRIDGE_MOUSE_BUTTON_2         1
#define ORYOL_OSXBRIDGE_MOUSE_BUTTON_3         2
#define ORYOL_OSXBRIDGE_MOUSE_BUTTON_4         3
#define ORYOL_OSXBRIDGE_MOUSE_BUTTON_5         4
#define ORYOL_OSXBRIDGE_MOUSE_BUTTON_6         5
#define ORYOL_OSXBRIDGE_MOUSE_BUTTON_7         6
#define ORYOL_OSXBRIDGE_MOUSE_BUTTON_8         7
#define ORYOL_OSXBRIDGE_MOUSE_BUTTON_LAST      ORYOL_OSXBRIDGE_MOUSE_BUTTON_8
#define ORYOL_OSXBRIDGE_MOUSE_BUTTON_LEFT      ORYOL_OSXBRIDGE_MOUSE_BUTTON_1
#define ORYOL_OSXBRIDGE_MOUSE_BUTTON_RIGHT     ORYOL_OSXBRIDGE_MOUSE_BUTTON_2
#define ORYOL_OSXBRIDGE_MOUSE_BUTTON_MIDDLE    ORYOL_OSXBRIDGE_MOUSE_BUTTON_3

#define ORYOL_OSXBRIDGE_RELEASE                0
#define ORYOL_OSXBRIDGE_PRESS                  1
#define ORYOL_OSXBRIDGE_REPEAT                 2

#define ORYOL_OSXBRIDGE_CURSOR                 0x00033001
#define ORYOL_OSXBRIDGE_STICKY_KEYS            0x00033002
#define ORYOL_OSXBRIDGE_STICKY_MOUSE_BUTTONS   0x00033003

#define ORYOL_OSXBRIDGE_CURSOR_NORMAL          0x00034001
#define ORYOL_OSXBRIDGE_CURSOR_HIDDEN          0x00034002
#define ORYOL_OSXBRIDGE_CURSOR_DISABLED        0x00034003

#define ORYOL_OSXBRIDGE_KEY_INVALID            -2
#define ORYOL_OSXBRIDGE_KEY_UNKNOWN            -1
#define ORYOL_OSXBRIDGE_KEY_SPACE              32
#define ORYOL_OSXBRIDGE_KEY_APOSTROPHE         39  /* ' */
#define ORYOL_OSXBRIDGE_KEY_COMMA              44  /* , */
#define ORYOL_OSXBRIDGE_KEY_MINUS              45  /* - */
#define ORYOL_OSXBRIDGE_KEY_PERIOD             46  /* . */
#define ORYOL_OSXBRIDGE_KEY_SLASH              47  /* / */
#define ORYOL_OSXBRIDGE_KEY_0                  48
#define ORYOL_OSXBRIDGE_KEY_1                  49
#define ORYOL_OSXBRIDGE_KEY_2                  50
#define ORYOL_OSXBRIDGE_KEY_3                  51
#define ORYOL_OSXBRIDGE_KEY_4                  52
#define ORYOL_OSXBRIDGE_KEY_5                  53
#define ORYOL_OSXBRIDGE_KEY_6                  54
#define ORYOL_OSXBRIDGE_KEY_7                  55
#define ORYOL_OSXBRIDGE_KEY_8                  56
#define ORYOL_OSXBRIDGE_KEY_9                  57
#define ORYOL_OSXBRIDGE_KEY_SEMICOLON          59  /* ; */
#define ORYOL_OSXBRIDGE_KEY_EQUAL              61  /* = */
#define ORYOL_OSXBRIDGE_KEY_A                  65
#define ORYOL_OSXBRIDGE_KEY_B                  66
#define ORYOL_OSXBRIDGE_KEY_C                  67
#define ORYOL_OSXBRIDGE_KEY_D                  68
#define ORYOL_OSXBRIDGE_KEY_E                  69
#define ORYOL_OSXBRIDGE_KEY_F                  70
#define ORYOL_OSXBRIDGE_KEY_G                  71
#define ORYOL_OSXBRIDGE_KEY_H                  72
#define ORYOL_OSXBRIDGE_KEY_I                  73
#define ORYOL_OSXBRIDGE_KEY_J                  74
#define ORYOL_OSXBRIDGE_KEY_K                  75
#define ORYOL_OSXBRIDGE_KEY_L                  76
#define ORYOL_OSXBRIDGE_KEY_M                  77
#define ORYOL_OSXBRIDGE_KEY_N                  78
#define ORYOL_OSXBRIDGE_KEY_O                  79
#define ORYOL_OSXBRIDGE_KEY_P                  80
#define ORYOL_OSXBRIDGE_KEY_Q                  81
#define ORYOL_OSXBRIDGE_KEY_R                  82
#define ORYOL_OSXBRIDGE_KEY_S                  83
#define ORYOL_OSXBRIDGE_KEY_T                  84
#define ORYOL_OSXBRIDGE_KEY_U                  85
#define ORYOL_OSXBRIDGE_KEY_V                  86
#define ORYOL_OSXBRIDGE_KEY_W                  87
#define ORYOL_OSXBRIDGE_KEY_X                  88
#define ORYOL_OSXBRIDGE_KEY_Y                  89
#define ORYOL_OSXBRIDGE_KEY_Z                  90
#define ORYOL_OSXBRIDGE_KEY_LEFT_BRACKET       91  /* [ */
#define ORYOL_OSXBRIDGE_KEY_BACKSLASH          92  /* \ */
#define ORYOL_OSXBRIDGE_KEY_RIGHT_BRACKET      93  /* ] */
#define ORYOL_OSXBRIDGE_KEY_GRAVE_ACCENT       96  /* ` */
#define ORYOL_OSXBRIDGE_KEY_WORLD_1            161 /* non-US #1 */
#define ORYOL_OSXBRIDGE_KEY_WORLD_2            162 /* non-US #2 */
#define ORYOL_OSXBRIDGE_KEY_ESCAPE             256
#define ORYOL_OSXBRIDGE_KEY_ENTER              257
#define ORYOL_OSXBRIDGE_KEY_TAB                258
#define ORYOL_OSXBRIDGE_KEY_BACKSPACE          259
#define ORYOL_OSXBRIDGE_KEY_INSERT             260
#define ORYOL_OSXBRIDGE_KEY_DELETE             261
#define ORYOL_OSXBRIDGE_KEY_RIGHT              262
#define ORYOL_OSXBRIDGE_KEY_LEFT               263
#define ORYOL_OSXBRIDGE_KEY_DOWN               264
#define ORYOL_OSXBRIDGE_KEY_UP                 265
#define ORYOL_OSXBRIDGE_KEY_PAGE_UP            266
#define ORYOL_OSXBRIDGE_KEY_PAGE_DOWN          267
#define ORYOL_OSXBRIDGE_KEY_HOME               268
#define ORYOL_OSXBRIDGE_KEY_END                269
#define ORYOL_OSXBRIDGE_KEY_CAPS_LOCK          280
#define ORYOL_OSXBRIDGE_KEY_SCROLL_LOCK        281
#define ORYOL_OSXBRIDGE_KEY_NUM_LOCK           282
#define ORYOL_OSXBRIDGE_KEY_PRINT_SCREEN       283
#define ORYOL_OSXBRIDGE_KEY_PAUSE              284
#define ORYOL_OSXBRIDGE_KEY_F1                 290
#define ORYOL_OSXBRIDGE_KEY_F2                 291
#define ORYOL_OSXBRIDGE_KEY_F3                 292
#define ORYOL_OSXBRIDGE_KEY_F4                 293
#define ORYOL_OSXBRIDGE_KEY_F5                 294
#define ORYOL_OSXBRIDGE_KEY_F6                 295
#define ORYOL_OSXBRIDGE_KEY_F7                 296
#define ORYOL_OSXBRIDGE_KEY_F8                 297
#define ORYOL_OSXBRIDGE_KEY_F9                 298
#define ORYOL_OSXBRIDGE_KEY_F10                299
#define ORYOL_OSXBRIDGE_KEY_F11                300
#define ORYOL_OSXBRIDGE_KEY_F12                301
#define ORYOL_OSXBRIDGE_KEY_F13                302
#define ORYOL_OSXBRIDGE_KEY_F14                303
#define ORYOL_OSXBRIDGE_KEY_F15                304
#define ORYOL_OSXBRIDGE_KEY_F16                305
#define ORYOL_OSXBRIDGE_KEY_F17                306
#define ORYOL_OSXBRIDGE_KEY_F18                307
#define ORYOL_OSXBRIDGE_KEY_F19                308
#define ORYOL_OSXBRIDGE_KEY_F20                309
#define ORYOL_OSXBRIDGE_KEY_F21                310
#define ORYOL_OSXBRIDGE_KEY_F22                311
#define ORYOL_OSXBRIDGE_KEY_F23                312
#define ORYOL_OSXBRIDGE_KEY_F24                313
#define ORYOL_OSXBRIDGE_KEY_F25                314
#define ORYOL_OSXBRIDGE_KEY_KP_0               320
#define ORYOL_OSXBRIDGE_KEY_KP_1               321
#define ORYOL_OSXBRIDGE_KEY_KP_2               322
#define ORYOL_OSXBRIDGE_KEY_KP_3               323
#define ORYOL_OSXBRIDGE_KEY_KP_4               324
#define ORYOL_OSXBRIDGE_KEY_KP_5               325
#define ORYOL_OSXBRIDGE_KEY_KP_6               326
#define ORYOL_OSXBRIDGE_KEY_KP_7               327
#define ORYOL_OSXBRIDGE_KEY_KP_8               328
#define ORYOL_OSXBRIDGE_KEY_KP_9               329
#define ORYOL_OSXBRIDGE_KEY_KP_DECIMAL         330
#define ORYOL_OSXBRIDGE_KEY_KP_DIVIDE          331
#define ORYOL_OSXBRIDGE_KEY_KP_MULTIPLY        332
#define ORYOL_OSXBRIDGE_KEY_KP_SUBTRACT        333
#define ORYOL_OSXBRIDGE_KEY_KP_ADD             334
#define ORYOL_OSXBRIDGE_KEY_KP_ENTER           335
#define ORYOL_OSXBRIDGE_KEY_KP_EQUAL           336
#define ORYOL_OSXBRIDGE_KEY_LEFT_SHIFT         340
#define ORYOL_OSXBRIDGE_KEY_LEFT_CONTROL       341
#define ORYOL_OSXBRIDGE_KEY_LEFT_ALT           342
#define ORYOL_OSXBRIDGE_KEY_LEFT_SUPER         343
#define ORYOL_OSXBRIDGE_KEY_RIGHT_SHIFT        344
#define ORYOL_OSXBRIDGE_KEY_RIGHT_CONTROL      345
#define ORYOL_OSXBRIDGE_KEY_RIGHT_ALT          346
#define ORYOL_OSXBRIDGE_KEY_RIGHT_SUPER        347
#define ORYOL_OSXBRIDGE_KEY_MENU               348
#define ORYOL_OSXBRIDGE_KEY_LAST               ORYOL_OSXBRIDGE_KEY_MENU


