#include "../headers/database.h"
#include "../headers/error.h"
#include <cstdext/io/logger.h>
#include <stdlib.h>

MDatabase *db = null;

#define DB_INSERT_QUERY       "INSERT INTO words (library_id, word, tranlation) VALUES(%d, '%s', '%s');"
#define DB_SELECT_LIB_ID      "SELECT library_id FROM libs where library_name = '%s';"
#define DB_SELECT_LIB_CONTENT "SELECT word, translation FROM words where library_id=%d;"
#define DB_INSERT_NEW_LIB     "INSET INTO libs (library_name) VALUES ('%s');"
#define DB_DELETE_WORD        "DELETE * FROM words WHERE word='%s' AND library_id=%d;"
#define DB_DELETE_LIBRARY     "DELETE library_id, library_name FROM libs WHERE library_name='%s';"
#define DB_SELECT_LIBRARIES_NAME "SELECT library_name FROM libs;"

bool dbConnect(str user_name, str user_password, str db_name) {
  db = databaseConnect(user_name, user_password, db_name);
  if (db == null) {
    log(ERROR, mErrorGetError());
    return false;
  }
  return true;
}

bool dbDisconnect() {
  if (!databaseDisconnect(db)) {
    log(ERROR, mErrorGetError());
    return false;
  }
  return true;
}


i32 dbGetLibId(str lib_name) {
  str query = strCreateFmt(DB_SELECT_LIB_ID, lib_name);
  MDatabaseResult *res = databaseExecQueryWithResult(db, query);
  if (res->cols < 0) {
    mErrorSetError("MEMORIZE: library %s does not exists\n", lib_name);
    return -1;
  }
  i32 id = cast(u32 ,atol(res->data[0][0]));
  databaseClearResult(res);

  DEALLOC(query);
  return id;
}

bool dbInsertPair(str lib_name, str word, str translation) {
  i32 lib_id = dbGetLibId(lib_name);
  str query = strCreateFmt(DB_INSERT_QUERY, lib_id, word, translation);
  if (!databaseExecQuryWithoutResult(db, query)) {
    log(ERROR, mErrorGetError());
    return false;
  }
  DEALLOC(query);
  return true;
}

bool dbDeletePair(str library_name, str word) {
  i32 lib_id = dbGetLibId(library_name);
  str query = strCreateFmt(DB_DELETE_WORD, word, lib_id);
  if (!databaseExecQuryWithoutResult(db, query)) {
    log(ERROR, mErrorGetError());
    return false;
  }
  return true;
}

Map *dbLoadLibrary(str lib_name) {
  i32 lib_id = dbGetLibId(lib_name);
  str query = strCreateFmt("SELECT (word, translation) FROM words where library_id=%d;", lib_id);
  MDatabaseResult *res = databaseExecQueryWithResult(db, query);

  if (res == null) {
    log(ERROR, mErrorGetError());
    exit(EXIT_FAILURE);
  }

  Map *m = mapCreate(POINTER(str), POINTER(str), null, MAP_EQL_STR_FUNC);
  for(i32 i = 0; i < res->rows; i++) {
    mapInsert(m, strCopy(res->data[i][0]), strCopy(res->data[i][1]));
  }

  databaseClearResult(res);
  return m;
}

bool dbCreateLibrary(str lib_name) {
  i32 exists_id = dbGetLibId(lib_name);
  if (exists_id != -1) {
    mErrorSetError("MEMORIZE: %s, library %s already exists\n", __FUNCTION__, lib_name);
    return false;
  }
  
  str query = strCreateFmt(DB_INSERT_NEW_LIB, lib_name);
  if (!databaseExecQuryWithoutResult(db, query)) {
    log(ERROR, mErrorGetError());
    return false;
  }
  return true;
}

bool dbDeleteLibrary(str name){
  str query = strCreateFmt(DB_DELETE_LIBRARY, name);
  if (!databaseExecQuryWithoutResult(db, query)) {
    log(ERROR, mErrorGetError());
    return false;
  }
  return true;
}

str *dbGetLibrariesList() {
  str query = strCreateFmt(DB_SELECT_LIBRARIES_NAME);
  MDatabaseResult *res = databaseExecQueryWithResult(db, query);
  if (res == null) {
    log(ERROR, mErrorGetError());
    return null;
  }

  str *libs = daCreate(str);
  for(i32 i = 0; res->rows; i++) {
    daAppend(libs, strCopy(res->data[i][0]));
  }
  databaseClearResult(res);
  return libs;
}
