#ifndef MEMORIZE_LIBRARY_H
#define MEMORIZE_LIBRARY_H
#include "database.h"
#include "pairs.h"
#include <cstdext/core.h>
#include <cstdext/container/map.h>



typedef struct {
  str name;
  DA_ARR(Pair *) content;
  bool saved;
} MLibrary;

bool      mLibraryCreate             (str lib_name);
MLibrary *mLibraryLoad               (str lib_name);
void      mLibraryDestroy            (MLibrary *lib);
bool      mLibraryAddPair            (MLibrary *lib, str word, str translation);
bool      mLibrarySave               (MLibrary *lib);
bool      mLibraryRemovePair         (MLibrary *lib, str word);
bool      mLibraryChangeTranslation  (MLibrary *lib, u32 word_index, str new_translation);
bool      mLibraryChangeWord         (MLibrary *lib, u32 word_index, str new_word);

#endif//MEMORIZE_LIBRARY_H
