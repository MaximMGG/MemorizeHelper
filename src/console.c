#include "../headers/consol.h"
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdarg.h>
#include <string.h>

#define ERROR_MSG_FMT CCOLOR_OPT(CCODE_OPT_BOLD, CCODE_BR_RED) "MEMORIZE: ERROR -> %s\n" CCODE_RESET
#define INFO_MSG_FMT CCOLOR_OPT(CCODE_OPT_ITALIC, CCODE_BR_BLUE) "MEMORIZE: INFO -> %s\n" CCODE_RESET

DA_ARR(str) libraries;
MLibrary *current_lib;

void mConsolInit() {
  libraries = dbGetLibrariesList();
  current_lib = null;
}

void mConsolSetCurrentLibrary(str lib_name) {
  if (current_lib != null) {
    if (!current_lib->saved) {
      if (mConsolAskYesNo("Do you wont to save current library?")) {
        mLibrarySave(current_lib);
      } 
      mLibraryDestroy(current_lib);
    }
  } 
  current_lib = mLibraryLoad(lib_name);
}


void mConsolPrintError(str error_msg, ...) {
  va_list li;
  va_start(li, error_msg);
  byte err_buf[1024] = {0};
  vsprintf(err_buf, error_msg, li);
  fprintf(stderr, ERROR_MSG_FMT, err_buf);
  va_end(li);
}

void mConsolPrintInfo(str info_msg, ...) {
  va_list li;
  va_start(li, info_msg);
  byte info_buf[512] = {0};
  sprintf(info_buf, info_msg, li);
  printf(INFO_MSG_FMT, info_buf);
  va_end(li);
}

bool mConsolAskYesNo(str question, ...) {
  va_list li;
  va_start(li, question);

  byte question_buf[256] = {0};
  vsprintf(question_buf, question, li);

  printf("%s (yes/no)\n", question_buf);
  byte buf[64] = {0};
  i32 read_bytes = read(STDIN_FILENO, buf, 64);
  if (read_bytes <= 0) {
    mConsolPrintError("Read user input Error");
  }
  buf[read_bytes] = 0;
  if (streql(buf, "yes") || 
      streql(buf, "Yes") || 
      streql(buf, "YES") || 
      streql(buf, "y") || 
      streql(buf, "Y")) {
    return true;
  }
  if (streql(buf, "No") ||
      streql(buf, "no") ||
      streql(buf, "NO") ||
      streql(buf, "N") ||
      streql(buf, "n")) {
    return false;
  }
  return false;
}

void mConsolDestroy() {
  if (libraries) {
    for(i32 i = 0; i < DA_LEN(libraries); i++) {
      DEALLOC(libraries[i]);
    }
    daDestroy(libraries);
  }
  if (current_lib) {
    if (!current_lib->saved) {
      if (mConsolAskYesNo("Do you want to save current library?")) {
        mLibrarySave(current_lib);
      }
    }
    mLibraryDestroy(current_lib);
  }
}

str lib_menu[] = {
  "Add pair",
  "Delete pair",
  "Change word",
  "Change translation",
  "Memorize",
  "Back to main menu"
};


static void mConsolRunLibMenu() {
  printf(CCOLOR_OPT(CCODE_OPT_BOLD, CCODE_GREEN)"%s LIBRARY\n"CCODE_RESET, current_lib->name);


}


static void mConsolRunSelectLibMenu() {
  printf(CCOLOR_OPT(CCODE_OPT_BOLD, CCODE_GREEN)"SELECT LIBRARY\n"CCODE_RESET);
  for(i32 i = 0; i < DA_LEN(libraries); i++) {
    printf("%d - %s\n", i + 1, libraries[i]);
  }
  printf("Choose library (enter index number, type 'b' for back to main menu)\n");
  byte input[32] = {0};
  i32 read_bytes = read(STDIN_FILENO, input, 32);
  if (read_bytes <= 0) {
    mConsolPrintError("read user input error %s %d", __FUNCTION__, __LINE__);
    return;
  }
  if (input[0] == 'b') {
    return;
  }
  input[read_bytes] = 0;
  i32 index = atol(input);
  if (index > DA_LEN(libraries) || index <= 0) {
    mConsolPrintError("library with index %d do not exist", index);
    return;
  }
  str selected_lib = libraries[index - 1];
  mConsolPrintInfo("selected %s library", selected_lib);
  mConsolSetCurrentLibrary(selected_lib);
  mConsolRunLibMenu();
}



static void mConsolCreateLibrary() {
  printf(CCOLOR_OPT(CCODE_OPT_BOLD, CCODE_GREEN)"CREATE LIBRARY\n"CCODE_RESET);
  printf("Enter library name: ");
  fflush(stdout);
  byte input[128] = {0};

  i32 read_bytes = read(STDIN_FILENO, input, 128);
  if (read_bytes <= 0) {
    mConsolPrintError("read user input error %s %d", __FUNCTION__, __LINE__);
    return;
  }
  input[read_bytes] = 0;
  if (mLibraryCreate(input)) {
    daAppend(libraries, input);
    if (mConsolAskYesNo("Select this library like current working library?")) {
      mConsolSetCurrentLibrary(input);
      mConsolPrintInfo("current working library is %s!", input);
      mConsolRunLibMenu();
    }
  } else {
    mConsolPrintError("Library %s already exists", input);
    return;
  }
  mConsolPrintInfo("library %s created!", input);
}

static void mConsolDeleteLibrary() {
  printf(CCOLOR_OPT(CCODE_OPT_BOLD, CCODE_GREEN)"DELETE LIBRARY\n"CCODE_RESET);
  printf("Enter library name: ");
  fflush(stdout);
  byte input[128] = {0};
  i32 read_bytes = read(STDIN_FILENO, input, 128);
  if (read_bytes <= 0) {
    mConsolPrintError("read user input error %s %d", __FUNCTION__, __LINE__);
    return;
  }
  input[read_bytes] = 0;
  if (streql(input, current_lib->name)) {
    mLibraryDestroy(current_lib);
    current_lib = null;
  }
  if (mConsolAskYesNo("Are you shure, delete library %s?", input)) {
    dbDeleteLibrary(input);
  }
  mConsolPrintInfo("library %s deleted!", input);
}


static void mConsolExit() {
  if (current_lib) {
    if (!current_lib->saved) {
      if (mConsolAskYesNo("Do you want to save current library?")) {
        mLibrarySave(current_lib);
      }
    }
    mLibraryDestroy(current_lib);
  }
  if (libraries) {
    for(i32 i = 0; i < DA_LEN(libraries); i++) {
      DEALLOC(libraries[i]);
    }
    daDestroy(libraries);
  }
  mConsolPrintInfo("Good bye!");
}

#define MAIN_MENU_LEN 4
str main_menu[] = {
  "Create Library",
  "Select Library",
  "Delete Library",
  "Exit"
};


void mConsolRunMainMenu() {
  printf(CCOLOR_OPT(CCODE_OPT_BOLD, CCODE_GREEN)"MAIN MENU\n"CCODE_RESET);
  byte input[128] = {0};
  i32 read_bytes;

  while(true) {
    for(i32 i = 0; i < MAIN_MENU_LEN; i++) {
      printf("%d - %s\n", i + 1, main_menu[i]);
    }
    printf("Choose option: ");
    fflush(stdout);
    read_bytes = read(STDIN_FILENO, input, 128);
    if (read_bytes <= 0) {
      mConsolPrintError("read error %s %d", __FUNCTION__, __LINE__);
    }

    switch (input[0]) {
      case '1': //create library
        mConsolCreateLibrary();
        break;
      case '2': //select library
        mConsolRunSelectLibMenu();
        break;
      case '3': //delete library
        mConsolDeleteLibrary();
        break;
      case '4': //exit
        break;
    }

  }
}
