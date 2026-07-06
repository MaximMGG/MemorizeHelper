#include "../headers/library.h"
#include <cstdext/io/logger.h>

bool mLibraryCreate(str lib_name) {
  if (!dbCreateLibrary(lib_name)) {
    return false;
  }
  return true;
}

MLibrary *mLibraryLoad(str lib_name) {
  MLibrary *lib = make(MLibrary);
  lib->name = strCopy(lib_name);
  lib->saved = true;

  lib->content = dbLoadLibrary(lib_name);
  return lib;
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
  if (lib->saved) {
    return true;
  }
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

        if (!dbGetPairId(lib->name, lib->content[i]->word, &lib->content[i]->pair_id)) {
          log(ERROR, "MEMORIZE: %s error", __FUNCTION__);
          return false;
        }
        PAIR_STATE_SET(lib->content[i]->pair_state, PAIR_STATE_SAVED);
        PAIR_STATE_UNSET(lib->content[i]->pair_state, PAIR_STATE_NEW);
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
  lib->saved = true;
  return true;
}

bool mLibraryAddPair(MLibrary *lib, str word, str translation) {
  Pair *new_pair = mPairCreate(word, translation);
  daAppend(lib->content, new_pair);
  lib->saved = false;
  return true;
}

bool mLibraryRemovePair(MLibrary *lib, str word) {
  for(i32 i = 0; i < DA_LEN(lib->content); i++) {
    if (streql(lib->content[i]->word, word)) {
      PAIR_STATE_SET(lib->content[i]->pair_state, PAIR_STATE_REMOVED);
    }
  }
  lib->saved = false;
  return true;
}

bool mLibraryChangeTranslation(MLibrary *lib, u32 pair_index, str new_translation) {
  if (streql(lib->content[pair_index]->translation, new_translation)) {
    return true;
  } else {
    DEALLOC(lib->content[pair_index]->translation);
    lib->content[pair_index]->translation = strCopy(new_translation);
    lib->saved = false;
    return true;
  }
}

bool mLibraryChangeWord(MLibrary *lib, u32 pair_index, str word, str new_word) {
  if (pair_index != -1) {
    if (streql(lib->content[pair_index]->word, new_word)) {
      return true;
    } else {
      DEALLOC(lib->content[pair_index]->word);
      lib->content[pair_index]->word = strCopy(new_word);
      lib->saved = false;
      return true;
    }
  } else {
    for(i32 i = 0; i < DA_LEN(lib->content); i++) {
      if (streql(word, lib->content[i]->word)) {
        if (streql(new_word, lib->content[i]->word)) {
          return true;
        }
        DEALLOC(lib->content[i]->word);
        lib->content[i]->word = strCopy(new_word);
        lib->saved = false;
        return true;
      }
    }
  }
  return false;
}

Pair *mLibraryGetPair(MLibrary *lib, i32 word_index, str word) {
  if (word_index == -1) {
    for(i32 i = 0; i < DA_LEN(lib->content); i++) {
      if (streql(word, lib->content[i]->word)) {
        return lib->content[i];
      }
    }
    return null;
  } else {
    return lib->content[word_index];
  }
  return null;
}

