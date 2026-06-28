#include "../headers/window.h"

str main_menu[] = {
  "Create Library",
  "Delete Library",
  "Select Library",
  "Exit"
};
#define MAIN_MENU_LEN 4

str libs_menu[] = {
  "Add pair",
  "Delete pair",
  "Change pair",
  "Start learning",
  "Back to menu"
};
#define LIBS_MENU_LEN 5


MWindow *mWindowInit() {
  MWindow *w = make(MWindow);
  w->x = w->y = 0;
  w->libraries = null;
  w->cur = null;
  w->libraries = dbGetLibrariesList();
  w->user_input = null;
  
  initscr();
  raw();
  noecho();
  keypad(stdscr, true);
  refresh();

  return w;
}

void mWindowDestroy(MWindow *w) {
  endwin();
  if (w->libraries != null) {
    for(i32 i = 0; i < DA_LEN(w->libraries); i++) {
      DEALLOC(w->libraries[i]);
    }
    daDestroy(w->libraries);
  }
  if (w->cur != null) {
    mLibraryDestroy(w->cur);
  }
  DEALLOC(w);
}

void mWindowRunMainMenu(MWindow *w) {
  
}

void mWindowRunLibsMenu(MWindow *w) {}

void mWindowGetUserInput(MWindow *w, str title) {
  WINDOW *input = newwin(3, COLS / 1.5, LINES - (LINES / 8), COLS / 8);
  box(input, 0, 0);
  mvwprintw(input, 0, (COLS / 1.5) / 2, "%s", title);
  wrefresh(input);
  wmove(input, 1, 1);
  i32 ch;
  byte in[32] = {0};
  i32 len = 0;
  i32 x = 1;
  while((ch = wgetch(input)) != '\n') {
    if (len == 31) {
      continue;
    }
    if (ch == 127) {
      len--;
      in[len] = 0;
      x--;
      mvwaddch(input, 1, x, ' ');
      wmove(input, 1, x);
      wrefresh(input);
      continue;
    }
    mvwaddch(input, 1, x, ch);
    x++;
    in[len++] = ch;
    wrefresh(input);
  }

  wborder(input, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
  wrefresh(input);
  delwin(input);
  if (w->user_input != null) {
    DEALLOC(w->user_input);
  }
  w->user_input = strCopy(in);
}
