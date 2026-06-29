#ifndef MEMORIZE_DATABASE_H
#define MEMORIZE_DATABASE_H
#include "db_engine.h"
#include "pairs.h"
#include <cstdext/container/map.h>


bool dbConnect          (str user_name, str user_password, str db_name);
bool dbDisconnect       ();
bool dbInsertPair       (str lib_name, str word, str tranlation, f32 learning_curve);
bool dbDeletePair       (str lib_name, str word);
Pair **dbLoadLibrary    (str name);
bool dbCreateLibrary    (str name);
bool dbDeleteLibrary    (str name);
str *dbGetLibrariesList ();
bool dbUpdatePair       (str lib_name, u64 pair_id, str word, str translation, f32 learning_curve);
bool dbCheckExistsTables();
bool dbGetPairId        (str lib_name, str word, u64 *pair_id);

#endif //MEMORIZE_DATABASE_H
