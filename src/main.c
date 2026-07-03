#include <stdio.h>
#include <cstdext/core.h>
#include <cstdext/io/logger.h>
#include <cstdext/io/json.h>
#include "../headers/window.h"
#include <fcntl.h>
#include <stdlib.h>

#define CONFIG_PATH "~/.config/memword/user_config.json"

str user_name = null;
str user_password = null;
str db_name = null;

void checkUserConfig() {
  if (access(CONFIG_PATH, F_OK) != 0) {
    i32 fd = open(CONFIG_PATH, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
    if (fd <= 0) {
      log(ERROR, "Create user config file error");
      exit(EXIT_FAILURE);
    }
    close(fd);

    byte buf[128] = {0};
    i32 read_bytes = 0;
    printf("Hello new user!\n");
    printf("Enter here your name: ");
    read_bytes = read(STDIN_FILENO, buf, 128);
    if (read_bytes <= 0) {
      log(ERROR, "Read user name error");
      exit(EXIT_FAILURE);
    }
    buf[read_bytes] = 0;
    user_name = strCopy(buf);
    memset(buf, 0, 128);
    printf("Enter here database name: ");
    read_bytes = read(STDIN_FILENO, buf, 128);
    if (read_bytes <= 0) {
      log(ERROR, "Read database name error");
      exit(EXIT_FAILURE);
    }
    buf[read_bytes] = 0;
    db_name = strCopy(buf);
    memset(buf, 0, 128);
    printf("Enter here password to your database: ");
    read_bytes = read(STDIN_FILENO, buf, 128);
    if (read_bytes <= 0) {
      log(ERROR, "Read database password error");
      exit(EXIT_FAILURE);
    }
    buf[read_bytes] = 0;
    user_password = strCopy(buf);
    
    json_obj *config = json_create_obj(null);
    json_add_to_obj(config, json_create_str("user_name", user_name));
    json_add_to_obj(config, json_create_str("user_password", user_password));
    json_add_to_obj(config, json_create_str("db_name", db_name));
    json_write_to_file(config, CONFIG_PATH);
  } else {
    json_obj *o = json_connection(CONFIG_PATH);
    json_obj *json_user_name = json_get_obj(o, "user_name");
    if (!json_user_name) {
      log(ERROR, "Read from json user_name error");
      exit(EXIT_FAILURE);
    }
    json_obj *json_user_password = json_get_obj(o, "user_password");
    if (!json_user_password) {
      log(ERROR, "Read from json user_password error");
      exit(EXIT_FAILURE);
    }
    json_obj *json_db_name = json_get_obj(o, "db_name");
    if (!json_db_name) {
      log(ERROR, "Read from json database name error");
      exit(EXIT_FAILURE);
    }
    user_name = strCopy(json_user_name->val.str_val);
    user_password = strCopy(json_user_password->val.str_val);
    db_name = strCopy(json_db_name->val.str_val);
  }
}


int main() {
  MWindow *w = mWindowInit();
  dbConnect("", "", "");
  if (!dbCheckExistsTables()) {
    log(ERROR, "dbCheckExistsTables");
    mWindowDestroy(w);
    return 1;
  }
  mWindowRunMainMenu(w);
  if (user_name) {
    DEALLOC(user_name);
  }
  if (user_password) {
    DEALLOC(user_name);
  }
  if (db_name) {
    DEALLOC(user_name);
  }
  return 0;
}
