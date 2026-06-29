#include <stdio.h>
#include "../headers/window.h"

int main() {
  MWindow *w = mWindowInit();
  mWindowRunMainMenu(w);
  return 0;
}
