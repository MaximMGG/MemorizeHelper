#include "../headers/error.h"
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

#define ERROR_BUF_LEN 1024
byte error_buf[ERROR_BUF_LEN];
bool error_exist;

str no_error = "MEMORIZE: NO ERROR";

void mErrorSetError(str error_msg_fmt, ...) {
  memset(error_buf, 0, ERROR_BUF_LEN);
  va_list li;
  va_start(li, error_msg_fmt);
  vsprintf(error_buf, error_msg_fmt, li);
  va_end(li);
  error_exist = true;
}

str mErrorGetError() {
  if (!error_exist) {
    return no_error;;
  } else {
    error_exist = false;
    return error_buf;
  }
}
