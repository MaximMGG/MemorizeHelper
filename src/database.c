#include "../headers/database.h"
#include "../headers/error.h"
#include <cstdext/io/logger.h>
#include <stdlib.h>

MDatabase *db = null;

#define DB_INSERT_QUERY       "INSERT INTO words (library_id, word, tranlation) VALUES(%d, '%s', '%s');"
#define DB_SELECT_LIB_ID      "SELECT library_id FROM libs where library_name = '%s';"
#define DB_SELECT_LIB_CONTENT "SELECT word, translation FROM words where library_id=%d;"
#define DB_INSERT_NEW_LIB     "INSET INTO libs (library_name) VALUES ('%s');"

void dbConnect(str user_name, str user_password, str db_name) {
  db = databaseConnect(user_name, user_password, db_name);
  if (db == null) {
    log(FATAL, mErrorGetError());
    exit(EXIT_FAILURE);
  }
}

void dbDisconnect() {
  if (!databaseDisconnect(db)) {
    log(FATAL, mErrorGetError());
    exit(EXIT_FAILURE);
  }
}


u32 dbGetLibId(str lib_name) {
  str query = strCreateFmt(DB_SELECT_LIB_ID, lib_name);
  MDatabaseResult *res = databaseSelectQuery(db, query);
  if (res->cols < 0) {
    mErrorSetError("MEMORIZE: library %s does not exists\n", lib_name);
    return 0;
  }
  u32 id = cast(u32 ,atol(res->data[0][0]));
  databaseClearResult(res);

  DEALLOC(query);
  return id;
}

void dbInsertPair(str lib_name, str word, str translation) {
  u32 lib_id = dbGetLibId(lib_name);
  str query = strCreateFmt(DB_INSERT_QUERY, lib_id, word, translation);
  if (!databaseInsertQuery(db, query)) {
    log(FATAL, mErrorGetError());
    exit(EXIT_FAILURE);
  }

  DEALLOC(query);
}

void dbRemovePair(str word) {
 //TODO(Check remove thyntacs) 
}

Map *dbLoadLibrary(str lib_name) {
  u32 lib_id = dbGetLibId(lib_name);
  str query = strCreateFmt("SELECT (word, translation) FROM words where library_id=%d;", lib_id);
  MDatabaseResult *res = databaseSelectQuery(db, query);

  if (res == null) {
    log(FATAL, mErrorGetError());
    exit(EXIT_FAILURE);
  }

  Map *m = mapCreate(POINTER(str), POINTER(str), null, MAP_EQL_STR_FUNC);
  for(i32 i = 0; i < res->rows; i++) {
    mapInsert(m, strCopy(res->data[i][0]), strCopy(res->data[i][1]));
  }

  databaseClearResult(res);
  return m;
}

void dbCreateLibrary(str lib_name) {
  str query = strCreateFmt(DB_INSERT_NEW_LIB, lib_name);
  if (!databaseInsertQuery(db, query)) {
    log(FATAL, mErrorGetError());
    exit(EXIT_FAILURE);
  }
}

void dbRemoveLibrary(str name){
  
}
