#include "../headers/db_engine.h"
#include "../headers/error.h"
#include <libpq-fe.h>

#define MDB_CONNECT_QUERY "dbname=%s user=%s passwrod=%s"
#define MDB_CHECK_TABLE_EXISTS "SELECT COUNT(*) FROM pg_tables WHERE schemaname='public' AND tablename = '%s';"
#define MDB_SELECT_QUERY ""
#define MDB_INSERT_QUERY ""
#define MDB_UPDATE_QUERY ""

MDatabase *databaseConnect(str user_name, str user_password, str db_name) {
  MDatabase *db = make(MDatabase);
  str connect_str = strCreateFmt(MDB_CONNECT_QUERY, db_name, user_name, user_password);
  db->conn = PQconnectdb(connect_str);
  if (PQstatus(db->conn) != CONNECTION_OK) {
    mErrorSetError("MEMORIZE: PQconnectdb with conn_string %s failed\n", connect_str);
    DEALLOC(db);
    DEALLOC(connect_str);
    return null;
  }
  
  DEALLOC(connect_str);
  db->user_name =     strCopy(user_name);
  db->user_password = strCopy(user_password);
  db->db_name =       strCopy(db_name);
  
  return db;
}

bool databaseDisconnect(MDatabase *db) {
  PQfinish(db->conn);
  DEALLOC(db->user_name);
  DEALLOC(db->user_password);
  DEALLOC(db->db_name);
  return false;
}

bool databaseInsertQuery(MDatabase *db, str query) {
  db->res = PQexec(db->conn, query);
  if (PQresultStatus(db->res) != PGRES_COMMAND_OK) {
    PQclear(db->res);
    mErrorSetError("MEMORIZE: PQexec whith query: %s failed\n", query);
    return false;
  }
  PQclear(db->res);
  return false;
}

MDatabaseResult *databaseSelectQuery(MDatabase *db, str query) {
  db->res = PQexec(db->conn, query);
  if (PQresultStatus(db->res) != PGRES_TUPLES_OK) {
    PQclear(db->res);
    mErrorSetError("MEMORIZE: PQexec whith query: %s failed\n", query);
    return false;
  }
  
  MDatabaseResult *result = make(MDatabaseResult);
  result->rows = PQntuples(db->res);
  result->cols = PQnfields(db->res);
  result->data = make_many(str *, result->rows);

  for(i32 i = 0; i < result->rows; i++) {
    result->data[i] = make_many(str, result->cols);
    for(i32 j = 0; j < result->cols; j++) {
      result->data[i][j] = strCopy(PQgetvalue(db->res, i, j));
    }
  }
  
  return result;
}

void databaseClearResult(MDatabaseResult *res) {
  for(i32 i = 0; i < res->rows; i++) {
    for(i32 j = 0; j < res->cols; j++) {
      DEALLOC(res->data[i][j]);
    }
    DEALLOC(res->data[i]);
  }
  DEALLOC(res->data);
  DEALLOC(res);
}

bool databaseUpdateQuery(MDatabase *db, str query) {
  return false;
}
