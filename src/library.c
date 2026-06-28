#include "../headers/library.h"

MLibrary *mLibraryCreate     (str lib_name) {
  MLibrary *lib = make(MLibrary);
  lib->name = strCopy(lib_name);
  lib->loaded = false;
  lib->content = null;
  dbCreateLibrary(lib_name);
  return lib;
}

bool mLibraryLoad(MLibrary *lib) {
  lib->content = dbLoadLibrary(lib->name);
  return true;
}

void mLibraryDestroy(MLibrary *lib) {
  if (lib->content != null) {
    mapDestroy(lib->content);
  }
  DEALLOC(lib->name);
  DEALLOC(lib);
}


bool mLibraryAddPair(MLibrary *lib, str word, str translation) {
  dbInsertPair(lib->name, word, translation);
  return true;
}

bool mLIbraryRemovePair(MLibrary *lib, str word) {
  dbDeletePair(lib->name, word);
  return true;
}


