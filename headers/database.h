#ifndef MEMORIZE_DATABASE_H
#define MEMORIZE_DATABASE_H
#include "db_engine.h"
#include <cstdext/container/map.h>

bool dbConnect(str user_name, str user_password, str db_name);
bool dbDisconnect();
bool dbInsertPair(str lib_name, str word, str tranlation);
bool dbDeletePair(str lib_name, str word);
Map *dbLoadLibrary(str name);
bool dbCreateLibrary(str name);
bool dbDeleteLibrary(str name);
str *dbGetLibrariesList();

#endif //MEMORIZE_DATABASE_H
