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
  vsprintf(info_buf, info_msg, li);
  printf(INFO_MSG_FMT, info_buf);
  va_end(li);
}

void mConsolGetUserInput(str msg, byte *buf, u32 buf_size) {
  printf("%s", msg);
  fflush(stdout);
  i32 read_bytes = read(STDIN_FILENO, buf, buf_size);
  if (read_bytes <= 0) {
    mConsolPrintError("read user input error %s %d", __FUNCTION__, __LINE__);
  }
  buf[read_bytes - 1] = '\0';
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
  buf[read_bytes - 1] = 0;
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

#define LIB_MENU_LEN 7    
str lib_menu[] = {
  "Add pair",
  "Delete pair",
  "Change word",
  "Change translation",
  "Memorize",
  "Save library",
  "Back to main menu"
};


static void mConsolRunLibMenu() {
  printf(CCOLOR_OPT(CCODE_OPT_BOLD, CCODE_GREEN)"%s LIBRARY\n"CCODE_RESET, current_lib->name);

  while(true) {

    printf(CCOLOR(CCODE_BLUE)"----------------------------------------------------------\n"CCODE_RESET);
    for(i32 i = 0; i < DA_LEN(current_lib->content); i++) {
      printf(CCOLOR_OPT(CCODE_OPT_UNDERLINE, CCODE_DEFAULT)"%d. %s - %s"CCODE_RESET, i + 1, current_lib->content[i]->word, current_lib->content[i]->translation);
      if (mPairIsNew(current_lib->content[i])) {
        printf(CCOLOR(CCODE_GREEN)" - new\n"CCODE_RESET);
      } else if (mPairIsDeleted(current_lib->content[i])) {
        printf(CCOLOR(CCODE_RED)" - deleted\n"CCODE_RESET);
      } else {
        printf("\n");
      }
    }
    printf(CCOLOR(CCODE_BLUE)"----------------------------------------------------------\n"CCODE_RESET);

    for(i32 i = 0; i < LIB_MENU_LEN; i++) {
      printf("%d - %s\n", i + 1, lib_menu[i]);
    }
    printf("Choose option: ");
    fflush(stdout);

    byte input[32] = {0};
    i32 read_bytes = read(STDIN_FILENO, input, 32);
    if (read_bytes <= 0) {
      mConsolPrintError("read user input error %s %d", __FUNCTION__, __LINE__);
      continue;
    }

  // "Add pair",
  // "Delete pair",
  // "Change word",
  // "Change translation",
  // "Memorize",
  // "Save library",
  // "Back to main menu"
    byte word[64] = {0};
    byte translation[128] = {0};
    byte temp_buf[128] = {0};
    switch(input[0]) {
      case '1': // Add pair
        mConsolGetUserInput("Enter word: ", word, 64);
        mConsolGetUserInput("Enter translation: ", translation, 128);
        if (mLibraryAddPair(current_lib, word, translation)) {
          mConsolPrintInfo("Add pair %s - %s", word, translation);
        } else {
          mConsolPrintError("Fail to add paid %s - %s", word, translation);
        }
        break;
      case '2': // Delete Pair
        mConsolGetUserInput("Enter word or enter word index: ", input, 64);
        if (input[0] >= '0' && input[0] <= '9') {
          i32 index = atol(input);
          if ((index <= 0) || (index >= DA_LEN(current_lib->content))) {
            mConsolPrintError("Index %d out of librarys indexs range", index);
            continue;
          }         
          mConsolPrintInfo("Going to delete pair %s - %s", current_lib->content[index - 1]->word, current_lib->content[index - 1]->translation);
          if (!mLibraryRemovePair(current_lib, current_lib->content[index - 1]->word)) {
            mConsolPrintError("Failed to remove pair %s - %s", current_lib->content[index - 1]->word, current_lib->content[index - 1]->translation);
          }
        } else {
          Pair *p = mLibraryGetPair(current_lib, -1, input);
          if (!p) {
            mConsolPrintError("Failed to get pair %s", input);
            continue;
          }
          mConsolPrintInfo("Going delete pair %s - %s", p->word, p->translation);
          if (!mLibraryRemovePair(current_lib, input)) {
            mConsolPrintError("Failed to remove pair %s - %s", p->word, p->translation);
          }
        }
        break;
      case '3': //Change word
        mConsolGetUserInput("Enter word or enter word index: ", input, 64);
        if ((input[0] >= '0') && (input[0] <= '9')) {
          i32 index = atol(input);
          if ((index <= 0) || (index >= DA_LEN(current_lib->content))) {
            mConsolPrintError("Index %d out of librarys indexs range", index);
            continue;
          }
          byte new_word[64] = {0};
          mConsolGetUserInput("Enter new word: ", new_word, 64);
          Pair *p = mLibraryGetPair(current_lib, index - 1, null);
          if (!p) {
            mConsolPrintError("Failed to get pair on index %d", index);
            continue;
          }
          strcpy(temp_buf, p->word);
          if (!mLibraryChangeWord(current_lib, index - 1, null, new_word)) {
            mConsolPrintError("Failed to change word %s", p->word);
          }
          mConsolPrintInfo("Change word %s to %s", temp_buf, new_word);
        } else {
          byte new_word[64] = {0};
          mConsolGetUserInput("Enter new word: ", new_word, 64);
          Pair *p = mLibraryGetPair(current_lib, -1, input);
          if (!p) {
            mConsolPrintError("Failed to get pair %s", input);
            continue;
          }
          strcpy(temp_buf, p->word);
          if (!mLibraryChangeWord(current_lib, -1, input, new_word)) {
            mConsolPrintError("Failed to change word %s", input);
            continue;
          }
        }
        break;
      case '4': // Change translation
        break;
      case '5': // Memorize
        break;
      case '6': // Save Library
        mLibrarySave(current_lib);
        mConsolPrintInfo("Library %s saved!", current_lib->name);
        break;
      case '7': // Back to main menu
        if (!current_lib->saved) {
          if (mConsolAskYesNo("Do you wont to save library?")) {
            mLibrarySave(current_lib);
            mConsolPrintInfo("Library %s saved!", current_lib->name);
          }
        }
        goto LIB_MENU_EXIT;
        break;
    }
  }

LIB_MENU_EXIT:
  return;
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
  input[read_bytes - 1] = 0;
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
  input[read_bytes - 1] = '\0';
  if (mLibraryCreate(input)) {
    daAppend(libraries, strCopy(input));
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
  input[read_bytes - 1] = '\0';
  if (current_lib) {
    if (streql(input, current_lib->name)) {
      if (mConsolAskYesNo("Are you shure, delete library %s?", input)) {
        dbDeleteLibrary(input);
      }
      mLibraryDestroy(current_lib);
      current_lib = null;
    }
  }
  if (mConsolAskYesNo("Are you shure, delete library %s?", input)) {
    dbDeleteLibrary(input);
  }
  for(i32 i = 0; i < DA_LEN(libraries); i++) {
    if (streql(input, libraries[i])) {
      daRemoveOrdered(libraries, i);
      break;
    }
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
        return;
        break;
    }
  }
}
