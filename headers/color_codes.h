#ifndef MEMORIZE_COLOR_CODES_H
#define MEMORIZE_COLOR_CODES_H

#define CCODE_RESET   "\033[0m"

#define CCODE_BLACK          "30m"
#define CCODE_RED            "31m"
#define CCODE_GREEN          "32m"
#define CCODE_YELLOW         "33m"
#define CCODE_BLUE           "34m"
#define CCODE_MAGENTA        "35m"
#define CCODE_CYAN           "36m"
#define CCODE_WHITE          "37m"
#define CCODE_DEFAULT        "39m"

#define CCODE_BR_RED         "91m"
#define CCODE_BR_GREEN       "92m"
#define CCODE_BR_YELLOW      "93m"
#define CCODE_BR_BLUE        "94m"
#define CCODE_BR_MAGENTA     "95m"
#define CCODE_BR_CYAN        "96m"
#define CCODE_BR_WHITE       "97m"

#define CCODE_BG_BLACK       "40;"
#define CCODE_BG_RED         "41;"
#define CCODE_BG_GREEN       "42;"
#define CCODE_BG_YELLOW      "43;"
#define CCODE_BG_BLUE        "44;"
#define CCODE_BG_MAGENTA     "45;"
#define CCODE_BG_WHITE       "47;"
#define CCODE_BG_CYAN        "46;"

#define CCODE_OPT_BOLD       "1;"
#define CCODE_OPT_DIM        "2;"
#define CCODE_OPT_ITALIC     "3;"
#define CCODE_OPT_UNDERLINE  "4;"
#define CCODE_OPT_BLINK      "5;"
#define CCODE_OPT_REVERSE    "7;"
#define CCODE_OPT_RESET      "0;"


#define CCOLOR_OPT(o, c) "\033[" o c
#define CCOLOR(c) "\033[" c

#endif //MEMORIZE_COLOR_CODES_H
