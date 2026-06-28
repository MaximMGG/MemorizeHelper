#ifndef MEMORIZE_WINDOW_H
#define MEMORIZE_WINDOW_H
#include "library.h"
#include <cstdext/core.h>
#include <ncurses.h>

typedef struct {
  str *libraries;
  MLibrary *cur;
  u32 x, y;
  str user_input;
} MWindow;

MWindow *mWindowInit();
void mWindowDestroy(MWindow *w);
void mWindowRunMainMenu(MWindow *w);
void mWindowRunLibsMenu(MWindow *w);
void mWindowGetUserInput(MWindow *w, str title);


#endif //MEMORIZE_WINDOW_H
