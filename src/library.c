#include "../headers/library.h"
#include <cstdext/io/logger.h>

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
    if (PAIR_STATE_CHECK(lib->content[i]->pair_state, PAIR_STATE_NEW)) {
      if (PAIR_STATE_CHECK(lib->content[i]->pair_state, PAIR_STATE_REMOVED)) {
	daRemoveOrdered(lib->content, i);
	i--;
	continue;
      } else {
        if (!dbInsertPair(lib->name, lib->content[i]->word, lib->content[i]->translation, lib->content[i]->learning_curve)) {
          log(ERROR, "MEMORIZE: %s error", __FUNCTION__);
          return false;
        }
        PAIR_STATE_SET(lib->content[i]->pair_state, PAIR_STATE_SAVED);
        PAIR_STATE_UNSET(lib->content[i]->pair_state, PAIR_STATE_NEW);
	lib->content[i]->pair_id = dbGetPairId(lib->name, lib->content[i]->word);
      }

    } else {
      if (PAIR_STATE_CHECK(lib->content[i]->pair_state, PAIR_STATE_REMOVED)) {
	dbDeletePair(lib->name, lib->content[i]->word);
        daRemoveOrdered(lib->content, i);
        i--;
        continue;
      }
      if (!PAIR_STATE_CHECK(lib->content[i]->pair_state, PAIR_STATE_SAVED)) {
        if (!dbUpdatePair(lib->name, lib->content[i]->pair_id,
                          lib->content[i]->word, lib->content[i]->translation,
                          lib->content[i]->learning_curve)) {
          log(ERROR, "MEMORIZE: %s error", __FUNCTION__);
          return false;
        }
        PAIR_STATE_SET(lib->content[i]->pair_state, PAIR_STATE_SAVED);
      }
    }
  }
  return true;
}

bool mLibraryAddPair(MLibrary *lib, str word, str translation) {
  Pair *new_pair = mPairCreate(word, translation);
  daAppend(lib->content, new_pair);
  return true;
}

bool mLibraryRemovePair(MLibrary *lib, str word) {
  for(i32 i = 0; i < DA_LEN(lib->content); i++) {
    if (streql(lib->content[i]->word, word)) {
      PAIR_STATE_SET(lib->content[i]->pair_state, PAIR_STATE_REMOVED);
    }
  }
  return true;
}

bool mLibraryChangeTranslation(MLibrary *lib, u32 pair_index, str new_translation) {
  if (streql(lib->content[pair_index]->translation, new_translation)) {
    return true;
  } else {
    DEALLOC(lib->content[pair_index]->translation);
    lib->content[pair_index]->translation = strCopy(new_translation);
    return true;
  }
}

bool mLibraryChangeWord(MLibrary *lib, u32 pair_index, str new_word) {
  if (streql(lib->content[pair_index]->word, new_word)) {
    return true;
  } else {
    DEALLOC(lib->content[pair_index]->word);
    lib->content[pair_index]->word = strCopy(new_word);
    return true;
  }
  return false;
}
