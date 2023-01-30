#ifndef APP_H
#define APP_H

#include <GLFW/glfw3.h>

typedef struct Program Program;

struct Program {
    GLFWwindow *window;
    int (*cleanup)(struct Program *self);
};

Program *_program__new(unsigned int w, unsigned int h, char *title);

#ifndef USE_LONG_NAMES_PROGRAM
#define new_program(...) _program__new(__VA_ARGS__)
#endif

#endif