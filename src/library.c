#include "../headers/library.h"
#include <cstdext/io/logger.h>
#include <cstdext/container/list.h>

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
      if (PAIR_STATE_CHECK(lib->content[i]->pair_state, PAIR_STATE_LEARNING_CURVE)) {
        if (!dbUpdatePair(lib->name, lib->content[i]->pair_id, lib->content[i]->word, lib->content[i]->translation, lib->content[i]->learning_curve)) {
          log(ERROR, "MEMORIZE: %s error", __FUNCTION__);
          return false;
        }
        PAIR_STATE_UNSET(lib->content[i]->pair_state, PAIR_STATE_LEARNING_CURVE);
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

bool mLibraryChangeTranslation(MLibrary *lib, u32 word_index, str word, str new_translation) {
  if (word_index != -1) {
    if (streql(lib->content[word_index]->translation, new_translation)) {
      return true;
    } else {
      DEALLOC(lib->content[word_index]->translation);
      lib->content[word_index]->translation = strCopy(new_translation);
      lib->saved = false;
      PAIR_STATE_UNSET(lib->content[word_index]->pair_state, PAIR_STATE_SAVED);
      return true;
    }
  } else {
    for(i32 i = 0; i < DA_LEN(lib->content); i++) {
      if (streql(lib->content[i]->word, word)) {
        if (streql(lib->content[i]->translation, new_translation)) {
          continue;
        } else {
          DEALLOC(lib->content[i]->translation);
          lib->content[i]->translation = strCopy(new_translation);
          lib->saved = false;
          PAIR_STATE_UNSET(lib->content[i]->pair_state, PAIR_STATE_SAVED);
          return true;
        }
      }
    }
  }
  return false;
}

bool mLibraryChangeWord(MLibrary *lib, u32 word_index, str word, str new_word) {
  if (word_index != -1) {
    if (streql(lib->content[word_index]->word, new_word)) {
      return true;
    } else {
      DEALLOC(lib->content[word_index]->word);
      lib->content[word_index]->word = strCopy(new_word);
      lib->saved = false;
      PAIR_STATE_UNSET(lib->content[word_index]->pair_state, PAIR_STATE_SAVED);
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
        PAIR_STATE_UNSET(lib->content[i]->pair_state, PAIR_STATE_SAVED);
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

MLibrary *mLibraryCreateFromFile(str lib_name, str file_name, str separator) {
  List *l = listFromFile(file_name, STR, 0);
  if (!mLibraryCreate(lib_name)) {
    listDestroy(l);
    return null;
  }
  MLibrary *lib = mLibraryLoad(lib_name);

  for(i32 i = 0; i < l->len; i++) {
    str *pair = strSplit(listGet(l, i), separator);
    if ((strlen(pair[0]) <= 1) || strlen(pair[1]) <= 1) {
      DEALLOC(pair[0]);
      DEALLOC(pair[1]);
      DEALLOC(pair);
      continue;
    }
    if (!pair) {
      listDestroy(l);
      return null;
    }
    mLibraryAddPair(lib, pair[0], pair[1]);
    DEALLOC(pair[0]);
    DEALLOC(pair[1]);
    DEALLOC(pair);
  }
  listDestroy(l);
  return lib;
}
