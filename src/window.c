#include "../headers/window.h"

#define CTRL(n) (n & 0x1F)

#define CTRL_Q CTRL('q')
#define CTRL_S CTRL('s')

#define KEY(k) k

#define ARROW_DOWN 258
#define ARROW_UP 259
#define ENTER 10

#define REGULAR_COLOR_PAIR 1
#define CURSOR_COLOR_PAIR 2
#define MENU_COLOR_PAIR 3

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
  start_color();
  init_pair(REGULAR_COLOR_PAIR, COLOR_WHITE, COLOR_BLACK);
  init_pair(CURSOR_COLOR_PAIR, COLOR_BLACK, COLOR_WHITE);
  init_pair(MENU_COLOR_PAIR, COLOR_WHITE, COLOR_BLUE);
  refresh();

  return w;
}

void mWindowSave(MWindow *w) {
  
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


void mWindowShutdown(MWindow *w) {
  //TODO(maxim) ask if wasn't saved befor last junge
}

void mWindowRunMainMenu(MWindow *w) {
  clear();
  attron(COLOR_PAIR(MENU_COLOR_PAIR));
  mvprintw(0, 0, "%-*s", COLS - 1, "MAIN MENU");
  attroff(COLOR_PAIR(MENU_COLOR_PAIR));
  u32 cursor_pos = 1;
  i32 ch = 0;
  
  while(true) {
    for(i32 i = 1; i < MAIN_MENU_LEN; i++) {
      if (i == cursor_pos) {
	attron(COLOR_PAIR(CURSOR_COLOR_PAIR));
	mvprintw(i, 0, "> %s", main_menu[i - 1]);
	attroff(COLOR_PAIR(CURSOR_COLOR_PAIR));
      } else {
	attron(COLOR_PAIR(REGULAR_COLOR_PAIR));
	mvprintw(i, 0, "  %s", main_menu[i - 1]);
	attroff(COLOR_PAIR(REGULAR_COLOR_PAIR));
      }
    }
    ch = getch();
    switch(ch) {
    case CTRL_Q: {
      mWindowShutdown(w);
    } break;
    case CTRL_S: {
      mWindowSave(w);
    } break;
    case ARROW_DOWN:
    case KEY('j'): {
      if (cursor_pos == MAIN_MENU_LEN) {
	continue;
      } else {
	cursor_pos++;
      }
    } break;
    case ARROW_UP:
    case KEY('k'): {
      if (cursor_pos == 1) {
	continue;
      } else  {
	cursor_pos--;
      }
    } break;
    case ENTER: {
      switch(cursor_pos) {
      case 1: 
      }
    } break;
    }
    
  }
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
