#ifndef MEMORIZE_WINDOW_H
#define MEMORIZE_WINDOW_H
#include "library.h"
#include <cstdext/core.h>
#include <ncurses.h>
#include <panel.h>

void     mWindowInit();
void     mWindowDestroy           ();
void     mWindowRunMainMenu       ();
void     mWindowRunLibsMenu       ();
void     mWindowRunLibContentMenu ();
void     mWindowGetUserInput      (str title);
str      mWindowAskQuestion       (str question);
bool     mWindowAskYesNoQuestion  (str question);
void     mWindowDrawErrorMessage  (str err_mesage, ...);
void     mWindowDrawTempMessage   (str message, ...);


#endif //MEMORIZE_WINDOW_H
