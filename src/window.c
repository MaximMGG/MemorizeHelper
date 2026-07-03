#include "../headers/window.h"
#include <pthread.h>
#include <unistd.h>


#define TEMP_WINDOW_WIDTH 40
#define TEMP_WINDOW_HEIGHT 8
WINDOW *temp_window;
PANEL *temp_panel;
pthread_mutex_t temp_mutex;


#define CTRL(n) (n & 0x1F)

#define CTRL_Q CTRL('q')
#define CTRL_S CTRL('s')

#define KEY(k) k

#define ARROW_DOWN 258
#define ARROW_UP   259
#define ENTER      10

#define REGULAR_COLOR_PAIR 1
#define CURSOR_COLOR_PAIR  2
#define MENU_COLOR_PAIR    3
#define ERROR_COLOR_PAIR   4

str main_menu[] = {
  "Create Library",
  "Delete Library",
  "Select Library",
  "Exit"
};

typedef enum {
  CREATE_LIBRARY_I = 1,
  DELETE_LIBRARY_I,
  SELECT_LIBRARY_I,
  EXIT_I,
} MAIN_MENU_INDEX;

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
  w->libraries = null;
  w->cur = null;
  //  w->libraries = dbGetLibrariesList();
  w->user_input = null;
  
  initscr();
  raw();
  noecho();
  keypad(stdscr, true);
  start_color();
  init_pair(REGULAR_COLOR_PAIR, COLOR_WHITE, COLOR_BLACK);
  init_pair(CURSOR_COLOR_PAIR, COLOR_BLACK, COLOR_WHITE);
  init_pair(MENU_COLOR_PAIR, COLOR_WHITE, COLOR_BLUE);
  init_pair(ERROR_COLOR_PAIR, COLOR_WHITE, COLOR_RED);
  refresh();

  return w;
}

void mWindowSave(MWindow *w) {
  mLibrarySave(w->cur);
  //TODO(maxim): when config will be add, here need to save config also
}

void mWindowDestroy(MWindow *w) {
  if (!w->saved) {
    if (mWindowAskYesNoQuestion(w, "Do you want to save changes?")) {
      mWindowSave(w);      
    }
  }
  
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
  endwin();
  return;
  if (!w->saved) {
    if (mWindowAskYesNoQuestion(w, "Do you want to save changes?")) {
      mWindowSave(w);
    }
  }
  //TODO(maxim) ask if wasn't saved befor last junge
}

void mWindowRunMainMenu(MWindow *w) {
  clear();
  attron(COLOR_PAIR(MENU_COLOR_PAIR));
  mvprintw(0, 0, "%-*s", COLS - 1, "MAIN MENU");
  attroff(COLOR_PAIR(MENU_COLOR_PAIR));
  u32 cursor_pos = 1;
  i32 ch = 0;
  mWindowDrawTempMessage("Welcome to the Memorize application!");

  while (true) {
    for (i32 i = 1; i <= MAIN_MENU_LEN; i++) {
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
    refresh();
    ch = getch();
    switch (ch) {
    case CTRL_Q: {
      mWindowShutdown(w);
      return;
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
      } else {
        cursor_pos--;
      }
    } break;
    case ENTER: {
      switch (cursor_pos) {
      case CREATE_LIBRARY_I: {
	mWindowGetUserInput(w, "Enter library name");
	if (!mLibraryCreate(w->user_input)) {
	  
	}
      } break;
      case DELETE_LIBRARY_I: {

      } break;
      case SELECT_LIBRARY_I: {

      } break;
      case EXIT_I: {

      } break;
      }
    }
    }
  }
}

void mWindowRunLibsMenu(MWindow *w) {
  
}

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
str mWindowAskQuestion(MWindow *w, str question) {
  return null;
}

bool mWindowAskYesNoQuestion(MWindow *w, str question) {
  str q = strConcat(question, " Enter: y\n(yes\no)");
  mWindowGetUserInput(w, q);
  DEALLOC(q);
  if ((streql(w->user_input, "y")) || (streql(w->user_input, "yes")) || (streql(w->user_input, "YES")) || streql(w->user_input, "Y")) {
    return true;
  }
  if ((streql(w->user_input, "n")) || (streql(w->user_input, "no")) || (streql(w->user_input, "NO")) || streql(w->user_input, "N")) {
    return false;
  }
  return false;
}

void mWindowDrawErrorMessage(MWindow *w, str err_mesage) {}

ptr mWindowDrawTempMessageHelper(ptr _message) {
  pthread_mutex_lock(&temp_mutex);
  attron(COLOR_PAIR(REGULAR_COLOR_PAIR));
  mvwprintw(temp_window, 1, 1, "%s", cast(str, _message));
  wrefresh(temp_window);
  attroff(COLOR_PAIR(REGULAR_COLOR_PAIR));


  show_panel(temp_panel);
  update_panels();
  doupdate();
  sleep(3);
  
  hide_panel(temp_panel);
  update_panels();
  doupdate();
  pthread_mutex_unlock(&temp_mutex);
}

void mWindowDrawTempMessage(str message) {
  pthread_t tmp;
  pthread_create(&tmp, null, mWindowDrawTempMessageHelper, message);
  pthread_detach(tmp);
}
