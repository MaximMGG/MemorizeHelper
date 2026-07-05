#include "../headers/window.h"
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <menu.h>


MLibrary *current_lib;
str last_user_input;
DA_ARR(str) libraries;


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


void mWindowInit() {
  libraries = dbGetLibrariesList();
  current_lib = null;
  last_user_input = null;

  initscr();
  raw();
  noecho();
  keypad(stdscr, true);
  start_color();
  init_pair(REGULAR_COLOR_PAIR, COLOR_WHITE, COLOR_BLACK);
  init_pair(CURSOR_COLOR_PAIR, COLOR_BLACK, COLOR_WHITE);
  init_pair(MENU_COLOR_PAIR, COLOR_WHITE, COLOR_BLUE);
  init_pair(ERROR_COLOR_PAIR, COLOR_WHITE, COLOR_RED);

  temp_window = newwin(TEMP_WINDOW_HEIGHT, TEMP_WINDOW_WIDTH, 1, COLS - TEMP_WINDOW_WIDTH - 1);
  box(temp_window, 0, 0);
  temp_panel = new_panel(temp_window);
  hide_panel(temp_panel);
  update_panels();
  doupdate();

  pthread_mutex_init(&temp_mutex, null);
  refresh();
}

void mWindowSave() {
  mLibrarySave(current_lib);
}

void mWindowDestroy() {
  pthread_mutex_lock(&temp_mutex);
  del_panel(temp_panel);
  delwin(temp_window);
  pthread_mutex_unlock(&temp_mutex);
  pthread_mutex_destroy(&temp_mutex);
  echo();
  endwin();
  if (libraries != null) {
    for(i32 i = 0; i < DA_LEN(libraries); i++) {
      DEALLOC(libraries[i]);
    }
    daDestroy(libraries);
  }
  if (current_lib) {
    mLibraryDestroy(current_lib);
  }
  if (last_user_input) {
    DEALLOC(last_user_input);
  }
}


void mWindowShutdown() {
  if ((current_lib != null) && !current_lib->saved) {
    if (mWindowAskYesNoQuestion("Do you want to save current library?")) {
      mLibrarySave(current_lib);
    }
  }

  mWindowDestroy();
}

void mWindowReloadLibraries() {
  for(i32 i = 0; i < DA_LEN(libraries); i++) {
    DEALLOC(libraries[i]);
  }

  daDestroy(libraries);
  libraries = dbGetLibrariesList();
}

void mWindowRunSelectLibrayMenu() {
  WINDOW *sel_lib_win = newwin(DA_LEN(libraries) + 2, 20, LINES / 4, COLS / 4);
  PANEL *sel_lib_panel = new_panel(sel_lib_win);
  MENU *sel_menu;
  ITEM **items;
  box(sel_lib_win, 0, 0);
  wrefresh(sel_lib_win);

  update_panels();
  doupdate();

  items = ALLOCZERO(sizeof(ITEM *) * (DA_LEN(libraries) + 2));

  for(i32 i = 0; i < DA_LEN(libraries); i++) {
    items[i] = new_item(libraries[i], null);
  }

  items[DA_LEN(libraries)] = new_item("Exit", null);
  items[DA_LEN(libraries) + 1] = new_item(null, null);
  wrefresh(sel_lib_win);
  sel_menu = new_menu(items);
  //menu_opts_off(sel_menu, O_ONEVALUE);

  set_menu_win(sel_menu, sel_lib_win);
  set_menu_sub(sel_menu, derwin(sel_lib_win, DA_LEN(libraries), 18, 1, 1));
  set_menu_format(sel_menu, DA_LEN(libraries), 1);
  set_menu_mark(sel_menu, "> ");
  
  post_menu(sel_menu);
  wrefresh(sel_lib_win);

  i32 pos = 1;
  i32 c;

  while((c = wgetch(sel_lib_win)) != KEY_BACKSPACE) {
    switch(c) {
      case KEY('j'):
      case KEY_DOWN:
        menu_driver(sel_menu, REQ_DOWN_ITEM);
        break;
      case KEY('k'):
      case KEY_UP:
        menu_driver(sel_menu, REQ_UP_ITEM);
        break;
      case 10: {
        ITEM *cur = current_item(sel_menu);
        if (streql("Exit", item_name(cur))) {
          goto EXIT_MENU;
        }
        if (current_lib != null) {
          if (!current_lib->saved) {
            if (mWindowAskYesNoQuestion("Do you want to save current libray?")) {
              mLibrarySave(current_lib);
            } 
            mLibraryDestroy(current_lib);
          }
        }
        mLibraryCreate(cast(str, item_name(cur)));
        //TODO(maxim) change that with new API
        mLibraryLoad("");
      } break;
    } 
  }

EXIT_MENU:
  unpost_menu(sel_menu);
  for(i32 i = 0; i <= DA_LEN(libraries) + 1; i++) {
    free_item(items[i]);
  }
  free_menu(sel_menu);
  del_panel(sel_lib_panel);
  delwin(sel_lib_win);
}


void mWindowRunMainMenu() {
  clear();
  attron(COLOR_PAIR(MENU_COLOR_PAIR));
  mvprintw(0, 0, "%-*s", COLS - 1, "MAIN MENU");
  attroff(COLOR_PAIR(MENU_COLOR_PAIR));
  u32 cursor_pos = 1;
  i32 ch = 0;
  mWindowDrawTempMessage("Welcome to the Memorize application!");
  usleep(50000);

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
        mWindowShutdown();
        return;
      } break;
      case CTRL_S: {
        mWindowSave();
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
            mWindowGetUserInput("Enter library name");
            if (!mLibraryCreate(last_user_input)) {
              mWindowDrawErrorMessage("Can't create library %s", last_user_input);
            }
            mWindowDrawTempMessage("Library %s created", last_user_input);
            mWindowReloadLibraries();
          } break;
          case DELETE_LIBRARY_I: {
            mWindowGetUserInput("Enter library name");
            if (!dbDeleteLibrary(last_user_input)) {
              mWindowDrawErrorMessage("Can' t delete libray: %s", last_user_input);
            }
            if (streql(current_lib->name, last_user_input)) {
              mLibraryDestroy(current_lib);
              current_lib = null;
            }
          } break;
          case SELECT_LIBRARY_I: {
            mWindowRunSelectLibrayMenu();
          } break;
          case EXIT_I: {
            mWindowShutdown();
            return;
          } break;
        }
      }
    }
    clear();
  }
}

void mWindowRunLibsMenu() {
  
}

void mWindowGetUserInput(str title) {
  WINDOW *input = newwin(3, COLS / 1.5, LINES - (LINES / 8), COLS / 8);
  box(input, 0, 0);
  i32 title_len = strlen(title);
  mvwprintw(input, 0, (COLS / 2)  - (title_len / 2), "%s", title);
  wrefresh(input);
  wmove(input, 1, 1);
  i32 ch;
  byte in[32] = {0};
  i32 len = 0;
  i32 x = 1;
  while((ch = wgetch(input)) != '\n') {
    if (ch == KEY_EXIT) {
      if (last_user_input != null) {
        DEALLOC(last_user_input);
        last_user_input = null;
      }
      return;
      goto USER_INPUT_EXIT;
    }
    if (len== 31) {
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

  if (last_user_input != null) {
    DEALLOC(last_user_input);
  }
  last_user_input = strCopy(in);

USER_INPUT_EXIT:
  wborder(input, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
  wrefresh(input);
  delwin(input);
}
str mWindowAskQuestion(str question) {
  return null;
}

bool mWindowAskYesNoQuestion(str question) {
  str q = strConcat(question, " Enter: y\n(yes\no)");
  mWindowGetUserInput(q);
  DEALLOC(q);
  if ((streql(last_user_input, "y")) || (streql(last_user_input, "yes")) || (streql(last_user_input, "YES")) || streql(last_user_input, "Y")) {
    return true;
  }
  if ((streql(last_user_input, "n")) || (streql(last_user_input, "no")) || (streql(last_user_input, "NO")) || streql(last_user_input, "N")) {
    return false;
  }
  return false;
}

ptr mWindowDrawErrorMessageHelper(ptr msg) {
  pthread_mutex_lock(&temp_mutex);
  attron(COLOR_PAIR(ERROR_COLOR_PAIR));

  mvwprintw(temp_window, 1, 1, "%s", cast(str, msg));
  wrefresh(temp_window);
  attroff(COLOR_PAIR(ERROR_COLOR_PAIR));

  show_panel(temp_panel);
  update_panels();
  doupdate();
  sleep(3);

  hide_panel(temp_panel);
  update_panels();
  doupdate();

  DEALLOC(msg);
  pthread_mutex_unlock(&temp_mutex);
  return null;
}


void mWindowDrawErrorMessage(str err_message, ...) {
  pthread_t t;
  va_list li;
  va_start(li, err_message);
  byte *msg_buf = ALLOC(512);
  vsprintf(msg_buf, err_message, li);
  pthread_create(&t, null, mWindowDrawErrorMessageHelper, msg_buf);
  pthread_detach(t);
  va_end(li);
}

ptr mWindowDrawTempMessageHelper(ptr _message) {
  pthread_mutex_lock(&temp_mutex);
  // wmove(temp_window, 1, 1);
  // wclear(temp_window);
  box(temp_window, 0, 0);
  // attron(COLOR_PAIR(REGULAR_COLOR_PAIR));
  //wclear(temp_window);
  mvwprintw(temp_window, 1, 1, "%s", cast(str, _message));
  wrefresh(temp_window);
  
  // attroff(COLOR_PAIR(REGULAR_COLOR_PAIR));

  show_panel(temp_panel);
  update_panels();
  doupdate();
  sleep(3);
  
  hide_panel(temp_panel);
  update_panels();
  doupdate();
  DEALLOC(_message);
  pthread_mutex_unlock(&temp_mutex);
  return null;
}

void mWindowDrawTempMessage(str message, ...) {
  va_list li;
  va_start(li, message);
  byte *buf_msg = ALLOC(512);
  vsprintf(buf_msg, message, li);
  pthread_t tmp;
  pthread_create(&tmp, null, mWindowDrawTempMessageHelper, buf_msg);
  pthread_detach(tmp);
  va_end(li);
}

