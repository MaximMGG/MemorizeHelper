#include "../headers/database.h"
#include "../headers/error.h"
#include <cstdext/io/logger.h>
#include <stdlib.h>

MDatabase *db = null;

#define DB_CHECK_LIBS_EXISTS     "SELECT * FROM pg_tables WHERE schemaname='public' AND tablename='libs';"
#define DB_CHECK_WORDS_EXISTS    "SELECT * FROM pg_tables WHERE schemaname='public' AND tablename='words';"
#define DB_CREATE_LIBS_TABLE     "CREATE TABLE libs (library_id SERIAL, library_name VARCHAR(64) NOT NULL);"
#define DB_CREATE_WORDS_TABLE    "CREATE TABLE words (pair_id SERIAL, library_id BIGINT, word VARCHAR(64), translation VARCHAR(128), learning_curve FLOAT);"



#define DB_INSERT_PAIR_QUERY     "INSERT INTO words (library_id, word, tranlation, learning_curve) VALUES(%d, '%s', '%s', %f);"
#define DB_SELECT_LIB_ID         "SELECT library_id FROM libs where library_name='%s';"
#define DB_SELECT_LIB_CONTENT    "SELECT pair_id, word, translation, learning_curve FROM words where library_id=%d;"
#define DB_INSERT_NEW_LIB        "INSERT INTO libs (library_name) VALUES ('%s');"
#define DB_DELETE_WORD           "DELETE FROM words WHERE word='%s' AND library_id=%d;"
#define DB_DELETE_LIBRARY        "DELETE FROM libs WHERE library_name='%s';"
#define DB_SELECT_LIBRARIES_NAME "SELECT library_name FROM libs;"
#define DB_UPDATE_PAIR           "UPDATE words SET word='%s', translation='%s', learning_curve=%f WHERE pair_id=%lu;"

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

bool dbInsertPair(str lib_name, str word, str translation, f32 learning_curve) {
  i32 lib_id = dbGetLibId(lib_name);
  str query = strCreateFmt(DB_INSERT_PAIR_QUERY, lib_id, word, translation, learning_curve);
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

Pair **dbLoadLibrary(str lib_name) {
  i32 lib_id = dbGetLibId(lib_name);
  str query = strCreateFmt(DB_SELECT_LIB_CONTENT, lib_id);
  MDatabaseResult *res = databaseExecQueryWithResult(db, query);

  if (res == null) {
    log(ERROR, mErrorGetError());
    exit(EXIT_FAILURE);
  }

  Pair **p = daCreate(Pair *);
  
  for(i32 i = 0; i < res->rows; i++) {
    daAppend(p, mPairCreate((u64)atol(res->data[i][0]), res->data[i][1], res->data[i][2], atof(res->data[i][3]), false, false));
  }

  databaseClearResult(res);
  return p;
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

bool dbUpdatePair(str lib_name, u64 pair_id, str word, str translation, f32 learning_curve) {
  u32 lib_id = dbGetLibId(lib_name);
  str query = strCreateFmt(DB_UPDATE_PAIR, pair_id, word, translation, learning_curve);
  if (!databaseExecQuryWithoutResult(db, query)) {
    log(ERROR, mErrorGetError());
    return false;
  }
  return true;
}

bool dbCheckExistsTables() {
  MDatabaseResult *res = databaseExecQueryWithResult(db, DB_CHECK_LIBS_EXISTS);
  if (res->rows == 0) {
    if (!databaseExecQuryWithoutResult(db, DB_CREATE_LIBS_TABLE)) {
      log(FATAL, "%s: %s", __FUNCTION__, mErrorGetError());
      return false;
    }
  }
  databaseClearResult(res);
  res = databaseExecQueryWithResult(db, DB_CHECK_WORDS_EXISTS);
  if (res->rows == 0) {
    if (!databaseExecQuryWithoutResult(db, DB_CREATE_WORDS_TABLE)) {
      log(FATAL, "%s: %s", __FUNCTION__, mErrorGetError());
      return false;
    }
  }
  databaseClearResult(res);
  return true;
}
