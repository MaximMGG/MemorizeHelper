#ifndef MEMORIZE_WINDOW_H
#define MEMORIZE_WINDOW_H
#include "library.h"
#include <cstdext/core.h>
#include <ncurses.h>

typedef struct {
  str *libraries;
  MLibrary *cur;
  str user_input;
  bool saved;
} MWindow;

MWindow *mWindowInit();
void     mWindowDestroy           (MWindow *w);
void     mWindowRunMainMenu       (MWindow *w);
void     mWindowRunLibsMenu       (MWindow *w);
void     mWindowRunLibContentMenu (MWindow *w);
void     mWindowGetUserInput      (MWindow *w, str title);
str      mWindowAskQuestion       (MWindow *w, str question);
bool     mWindowAskYesNoQuestion  (MWindow *w, str question);
void     mWindowDrawErrorMessage  (MWindow *w, str err_mesage);
void     mWindowDrawTempMessage   (str message);


#endif //MEMORIZE_WINDOW_H
