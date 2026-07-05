#include "../headers/color_codes.h"
#include <cstdext/core.h>
#include <stdio.h>



#define ERROR_FMT_MSG CCOLOR(CCODE_BR_RED)"%s\n"CCODE_RESET
#define ERROR_OPT_MSG(o) CCOLOR_OPT(o, CCODE_BR_RED)"%s\n"CCODE_RESET

int main() {

  printf("\033[1;91m%s\033[0m\n", "Oopss");

  printf(ERROR_FMT_MSG, "i");


  printf(ERROR_OPT_MSG(CCODE_OPT_BOLD), "ERROROROR");
  printf(ERROR_OPT_MSG(CCODE_OPT_DIM), "ERROROROR");
  printf(ERROR_OPT_MSG(CCODE_OPT_ITALIC), "ERROROROR");
  printf(ERROR_OPT_MSG(CCODE_OPT_UNDERLINE), "ERROROROR");
  printf(ERROR_OPT_MSG(CCODE_OPT_BLINK), "ERROROROR");
  printf(ERROR_OPT_MSG(CCODE_OPT_REVERSE), "ERROROROR");


  printf(ERROR_OPT_MSG(CCODE_OPT_BOLD CCODE_OPT_UNDERLINE), "ERROROROR");

  return 0;
}





