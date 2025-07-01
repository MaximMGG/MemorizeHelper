#include "../headers/library.h"
#include <string.h>
#include <time.h>
#include <stdlib.h>

void lib_add_word(lib *l, str _w, str _t, bool write, bool trans, f32 level) {
    word *w = make(word);
    u32 _w_len = strlen(_w);
    u32 _t_len = strlen(_t);
    w->w = alloc_copy(_w, _w_len + 1);
    w->t = alloc_copy(_t, _t_len + 1);
    w->known_how_to_write = write;
    w->known_how_to_translate = trans;
    w->total_level_of_knowelage = level;
    list_append(l->lib_cont, w);
}

void lib_remove_word(lib *l, str _w) {
    for(i32 i = 0; i < l->lib_cont->len; i++) {
        word *tmp = list_get(l->lib_cont, i);
        if (strcmp(tmp->w, _w) == 0) {
            list_remove(l->lib_cont, i);
        } else {
            //TODO(maxim) show error message
        }
    }
}

void lib_change_translation(lib *l, str _w, str _t) {
    word *tmp = lib_get_word(l, _w);
    if (!tmp) {
        //TODO(maxim) show error message
    } else {
        dealloc(tmp->t);
        u32 _t_len = strlen(_t);
        tmp->t = alloc_copy(_t, _t_len + 1);
    }
}

static bool _lib_number_contain(i32 *arr, u32 arr_len, i32 number) {
    for(u32 i = 0; i < arr_len; i++) {
        if (arr[i] == number) {
            return false;
        } 
    }
    return true;
}

void lib_shufle(lib *l) {
    srand(time(null));
    i32 num_buf[l->lib_cont->len];
    i32 num_buf_i = 0;
    memset(num_buf, -1, l->lib_cont->len);
    bool num_contain = false;
    i32 num_of_contains = l->lib_cont->len;

    list *tmp_lib = list_create(STRUCT);
    while(num_of_contains > 0) {
        i32 rand_index = rand() % l->lib_cont->len;
        if (_lib_number_contain(num_buf, l->lib_cont->len, rand_index)) {
            num_of_contains--;
            word *w = list_get(l->lib_cont, rand_index);
            list_append(tmp_lib, w);
            num_buf[num_buf_i++] = rand_index;
        } else {
            continue;
        }
    }
    list_destroy(l->lib_cont);
    l->lib_cont = tmp_lib;
}

word *lib_get_word(lib *l, str _w) {
    for(i32 i = 0; i < l->lib_cont->len; i++) {
        word *tmp = list_get(l->lib_cont, i);
        if (strcmp(tmp->w, _w) == 0) {
            return tmp;
        }
    }
    return null;
}

word *lib_get_word_index(lib *l, u32 index) {
    word *tmp = list_get(l->lib_cont, index);
    if (!tmp) {
        //TODO(maxim) show error message
    } else {
        return tmp;
    }
    return null;
}

