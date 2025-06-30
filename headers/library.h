#ifndef LIBRARY_H
#define LIBRARY_H

#include <cstdext/core.h>
#include <cstdext/container/list.h>

typedef struct {
    str w;
    str t;
    bool known_how_to_write;
    bool known_how_to_translate;
    f32 total_level_of_knowelage;
} word;

typedef struct {
    str lib_name;
    list *lib_cont; //list of struct word *

    bool chagned;
}lib;

void lib_add_word(lib *l, str _w, str _t, bool write, bool trans, f32 level);
void lib_remove_word(lib *l, str _w);
void lib_change_translation(lib *l, str _w, str _t);
void lib_shufle(lib *l);
word *lib_get_word(lib *l, str _w);
word *lib_get_word_index(lib *l, u32 index);

#endif //LIBRARY_H
