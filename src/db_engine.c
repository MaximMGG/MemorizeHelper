#include "../headers/db_engine.h"


MDatabase *databaseConnect(str user_name, str user_password, str db_name) {
  MDatabase *db = make(MDatabase);

  return db;
}

bool databaseDisconnect(MDatabase *db) {
  return false;
}

bool databaseInsertQuery(MDatabase *db, str query) {
  return false;
}

MDatabaseResult databaseSelectQuery(MDatabase *db, str query) {
  return null;
}

bool databaseUpdateQuery(MDatabase *db, str query) {
  return false;
}
