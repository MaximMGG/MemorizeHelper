#include "../headers/library.h"
#include <string.h>
#include <stdio.h>


int main() {
    lib *l = make(l);
    l->lib_cont = list_create(STRUCT);

    word *a = make(word);
    memset(a, 0, sizeof(word));
    a->w = alloc_copy("Hello1", sizeof("Hello1") + 1);
    a->t = alloc_copy("THello1", sizeof("THello1") + 1);
    word *b = make(word);
    memset(b, 0, sizeof(word));
    b->w = alloc_copy("Hello2", sizeof("Hello2") + 1);
    b->t = alloc_copy("THello2", sizeof("THello2") + 1);
    word *c = make(word);
    memset(c, 0, sizeof(word));
    c->w = alloc_copy("Hello3", sizeof("Hello3") + 1);
    c->t = alloc_copy("THello3", sizeof("THello3") + 1);
    word *d = make(word);
    memset(d, 0, sizeof(word));
    d->w = alloc_copy("Hello4", sizeof("Hello4") + 1);
    d->t = alloc_copy("THello4", sizeof("THello4") + 1);
    word *e = make(word);
    memset(e, 0, sizeof(word));
    e->w = alloc_copy("Hello5", sizeof("Hello5") + 1);
    e->t = alloc_copy("THello5", sizeof("THello5") + 1);
    list_append(l->lib_cont, a);
    list_append(l->lib_cont, b);
    list_append(l->lib_cont, c);
    list_append(l->lib_cont, d);
    list_append(l->lib_cont, e);

    for(i32 i = 0; i < l->lib_cont->len; i++) {
        word *tmp = list_get(l->lib_cont, i);
        printf("%s - %s\n", tmp->w, tmp->t);
    }

    lib_shufle(l);

    for(i32 i = 0; i < l->lib_cont->len; i++) {
        word *tmp = list_get(l->lib_cont, i);
        printf("%s - %s\n", tmp->w, tmp->t);
        dealloc(tmp->w);
        dealloc(tmp->t);
        dealloc(tmp);
    }
    list_destroy(l->lib_cont);
    dealloc(l);

    return 0;
}
