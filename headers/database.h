#ifndef MEMORIZE_DATABASE_H
#define MEMORIZE_DATABASE_H
#include "db_engine.h"
#include <cstdext/container/map.h>

void dbConnect(str user_name, str user_password, str db_name);
void dbDisconnect();
void dbInsertPair(str lib_name, str word, str tranlation);
void dbRemovePair(str word);
Map *dbLoadLibrary(str name);
void dbCreateLibrary(str name);
void dbRemoveLibrary(str name);

#endif //MEMORIZE_DATABASE_H
