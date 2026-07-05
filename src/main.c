#include <stdio.h>
#include <cstdext/core.h>
#include <cstdext/io/logger.h>
#include <cstdext/io/json.h>
#include "../headers/consol.h"
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>

#define CONFIG_PATH     "/home/%s/.config/memword/user_config.json"
#define CONFIG_DIR_PATH "/home/%s/.config/memword"

str user_name = null;
str user_password = null;
str db_name = null;

void checkUserConfig() {
  str user = getenv("USER");
  if (!user) {
    log(ERROR, "getenv(USER) error");
    exit(EXIT_FAILURE);
  }
  str _config_path = strCreateFmt(CONFIG_PATH, user);
  str _config_dir_path = strCreateFmt(CONFIG_DIR_PATH, user);
  
  if (access(_config_dir_path, F_OK) != 0) {
    mkdir(_config_dir_path, S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP);
  }
  if (access(_config_path, F_OK) != 0) {
    i32 fd = open(_config_path, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
    if (fd <= 0) {
      log(ERROR, "Create user config file error");
      exit(EXIT_FAILURE);
    }
    close(fd);

    byte buf[128] = {0};
    i32 read_bytes = 0;
    printf("Hello new user!\n");
    printf("Enter here your name: ");
    fflush(stdout);
    read_bytes = read(STDIN_FILENO, buf, 128);
    if (read_bytes <= 0) {
      log(ERROR, "Read user name error");
      exit(EXIT_FAILURE);
    }
    buf[read_bytes - 1] = 0;
    user_name = strCopy(buf);
    memset(buf, 0, 128);
    printf("Enter here database name: ");
    fflush(stdout);
    read_bytes = read(STDIN_FILENO, buf, 128);
    if (read_bytes <= 0) {
      log(ERROR, "Read database name error");
      exit(EXIT_FAILURE);
    }
    buf[read_bytes - 1] = 0;
    db_name = strCopy(buf);
    memset(buf, 0, 128);
    printf("Enter here password to your database: ");
    fflush(stdout);
    read_bytes = read(STDIN_FILENO, buf, 128);
    if (read_bytes <= 0) {
      log(ERROR, "Read database password error");
      exit(EXIT_FAILURE);
    }
    buf[read_bytes - 1] = 0;
    user_password = strCopy(buf);
    
    json_obj *config = json_create_obj(null);
    json_add_to_obj(config, json_create_str("user_name", user_name));
    json_add_to_obj(config, json_create_str("user_password", user_password));
    json_add_to_obj(config, json_create_str("db_name", db_name));
    json_write_to_file(config, _config_path);
    json_destroy_obj(config);
  } else {
    json_obj *o = json_connection(_config_path);
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
    json_destroy_obj(o);
  }
}


int main() {
  checkUserConfig();
  log(INFO, "Read user config");
  dbConnect(user_name, user_password, db_name);
  log(INFO, "dbConnect");
  mConsolInit();
  //mWindowInit();
  log(INFO, "mWindowInit");
  if (!dbCheckExistsTables()) {
    log(ERROR, "dbCheckExistsTables");
    mConsolDestroy();
    return 1;
  }

  log(INFO, "dbCheckExistsTables");
  //mWindowRunMainMenu();
  mConsolRunMainMenu();
  mConsolDestroy();
  if (user_name) {
    DEALLOC(user_name);
  }
  if (user_password) {
    DEALLOC(user_password);
  }
  if (db_name) {
    DEALLOC(db_name);
  }
  dbDisconnect();
  log(INFO, "Shutdown");
  return 0;
}
