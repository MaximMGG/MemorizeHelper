#include "../headers/consol.h"
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdarg.h>

#define ERROR_MSG_FMT CCOLOR(CCODE_BR_RED) ## "%s" ## CCODE_RESET


DA_ARR(str) libraries;
MLibrary *current_lib;
str last_user_input;

void mConsolInit() {
  libraries = dbGetLibrariesList();
  current_lib = null;
  last_user_input = null;
}
void print_error(str error_msg, ...) {
  va_list li;
  va_start(li, error_msg);
  byte err_buf[1024] = {0};
  vsprintf(err_buf, error_msg, li);
  fprintf(stderr, "\033[" "91m" "%s" "\033[0m", err_buf);
  va_end(li);
}

bool mConsolAskYesNo(str question) {

  printf("%s (yes/no)\n", question);
  byte buf[64] = {0};
  i32 read_bytes = read(STDIN_FILENO, buf, 64);
  if (read_bytes <= 0) {
    print_error("Read user input Error");
  }


  return true;
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

    }
  }

}

void mConsolRunMainMenu() {

}
