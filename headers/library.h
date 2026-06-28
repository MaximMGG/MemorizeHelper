#ifndef MEMORIZE_LIBRARY_H
#define MEMORIZE_LIBRARY_H
#include "database.h"
#include <cstdext/core.h>
#include <cstdext/container/map.h>

typedef struct {
  str name;
  bool loaded;
  Map *content;
} MLibrary;

MLibrary *mLibraryCreate     (str lib_name);
bool      mLibraryLoad       (MLibrary *lib);
void      mLibraryDestroy    (MLibrary *lib);
bool      mLibraryAddPair    (MLibrary *lib, str word, str translation);
bool      mLIbraryRemovePair (MLibrary *lib, str word);

#endif//MEMORIZE_LIBRARY_H
