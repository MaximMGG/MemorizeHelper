#include "../headers/library.h"

MLibrary *mLibraryCreate     (str lib_name) {
  MLibrary *lib = make(MLibrary);
  lib->name = strCopy(lib_name);
  lib->loaded = false;
  lib->content = null;
  lib->saved = true;
  dbCreateLibrary(lib_name);
  return lib;
}

bool mLibraryLoad(MLibrary *lib) {
  lib->content = dbLoadLibrary(lib->name);
  return true;
}

void mLibraryDestroy(MLibrary *lib) {
  if (lib->content != null) {
    for(i32 i = 0; i < DA_LEN(lib->content); i++) {
      mPairDestroy(lib->content[i]);
    }
    daDestroy(lib->content);
  }
  DEALLOC(lib->name);
  DEALLOC(lib);
}

bool mLibrarySave(MLibrary *lib) {
  for(i32 i = 0; i < DA_LEN(lib->content); i++) {
    if (lib->content[i]->new_pair) {
      dbInsertPair(lib->name, lib->content[i]->word, lib->content[i]->translation, lib->content[i]->learning_curve);
      lib->content[i]->saved = true;
      lib->content[i]->new_pair = false;
    } else {
      if (lib->content[i]->saved == false) {
	
      }
    }
  }

  return true;
}


bool mLibraryAddPair(MLibrary *lib, str word, str translation) {
  dbInsertPair(lib->name, word, translation, 0.0);
  return true;
}

bool mLIbraryRemovePair(MLibrary *lib, str word) {
  dbDeletePair(lib->name, word);
  return true;
}


