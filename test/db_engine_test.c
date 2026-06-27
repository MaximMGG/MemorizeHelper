#include <cstdext/assert.h>
#include "../headers/db_engine.h"
#include <cstdext/io/reader.h>
#include <cstdext/io/logger.h>


TEST(db_select_test) {
  str pass = readEntyreFile("test/.test_pass");
  str user_name = "mhrun";
  str db_name = "mydb";
  MDatabase *db = databaseConnect(user_name, pass, db_name);

  assert_not_null(db);
  assert_not_null(db->conn);

  MDatabaseResult *res = databaseSelectQuery(db, "SELECT * FROM test_table;");
  assert_not_null(res);

  for(i32 i = 0; i < res->rows; i++) {
    log(INFO, "%s", res->data[i][0]);
  }
  databaseClearResult(res);
  databaseDisconnect(db);
}


TEST(db_connect_test) {
  str pass = readEntyreFile("test/.test_pass");
  log(INFO, pass);
  str user_name = "mhrun";
  str db_name = "mydb";

  MDatabase *db = databaseConnect(user_name, pass, db_name);
  assert_not_null(db);
  databaseDisconnect(db);
}


TEST_LIST(db_connect_test, db_select_test);
