#include "../headers/consol.h"
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdarg.h>

#define ERROR_MSG_FMT CCOLOR_OPT(CCODE_OPT_BOLD, CCODE_BR_RED) "MEMORIZE: ERROR -> %s\n" CCODE_RESET


DA_ARR(str) libraries;
MLibrary *current_lib;
str last_user_input;

void mConsolInit() {
  libraries = dbGetLibrariesList();
  current_lib = null;
  last_user_input = null;
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
  if (last_user_input) {
    DEALLOC(last_user_input);
  }
}

static void mConsolRunLibMenu() {

}


static void mConsolRunSelectLibMenu() {

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
      mConsolRunLibMenu();
    }
  } else {
    mConsolPrintError("Library %s already exists", input);
    return;
  }
}

static void mConsolDeleteLibrary() {
  printf(CCOLOR_OPT(CCODE_OPT_BOLD, CCODE_GREEN)"DELETE LIBRARY\n"CCODE_RESET);
  printf("Enter library name: ");
  fflush(stdout);
  byte input[128] = {0};
  i32 read_bytes = read(STDIN_FILENO, input, 128);
  if (read_bytes <= 0) {
    mConsolPrintError("read user input error %s %d", __FUNCTION__, __LINE__);
  }
  input[read_bytes] = 0;
  if (streql(input, current_lib->name)) {
    mLibraryDestroy(current_lib);
    current_lib = null;
  }
  if (mConsolAskYesNo("Are you shure, delete library %s?", input)) {
    dbDeleteLibrary(input);
  }
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
        break;
      case '4': //exit
        break;
    }

  }



}
