#ifndef MEMORIZE_DB_ENGINE_H
#define MEMORIZE_DB_ENGINE_H
#include <cstdext/core.h>
#include <libpq-fe.h>

typedef struct {
  str user_name;
  str user_password;
  str db_name;
  PGconn *conn;
  PGresult *res;

} MDatabase;

typedef struct {
  i32 rows;
  i32 cols;
  str **data;
} MDatabaseResult;

MDatabase *        databaseConnect    (str user_name, str user_password, str db_name);
bool               databaseDisconnect (MDatabase *db);
bool               databaseInsertQuery(MDatabase *db, str query);
MDatabaseResult *  databaseExecQueryWithResult(MDatabase *db, str query);
bool               databaseExecQuryWithoutResult(MDatabase *db, str query);
void               databaseClearResult(MDatabaseResult *res);

#endif //MEMORIZE_DB_ENGINE_H
