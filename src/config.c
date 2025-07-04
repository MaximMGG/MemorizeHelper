#include "../headers/config.h"
#include <cstdext/core.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>

#define DEFAULT_CONFIG_PATH "/home/%s/.local/share/memhelper/memhelper.config"
byte def_conf_path_buf[512] = {0};
#define DEFAULT_CONFIG_DIR_PATH "/home/%s/.local/share/memhelper"
byte def_conf_dir_path[512] = {0};

#define DEF_CONFIG_CONTENT "LIB_PATH: \"\"\nLIBS: {\n}"

static void _config_set_user_name(str buf) {
    str user_name = getlogin();
    if (user_name == null) {
        byte cwd_buf[512] = {0};
        str cwd = getcwd(cwd_buf, 512);
        u32 start_name = 6;
        u32 buf_i = 0;
        for(i32 i = start_name; i < 512; i++) {
            if (cwd[i] == '/') {
                break;
            }
            buf[buf_i++] = cwd[i];
        }
    } else {
        strcpy(buf, user_name);
    }
}

static void _config_check_config_exists(str user_name) {
    sprintf(def_conf_path_buf, DEFAULT_CONFIG_PATH, user_name);
    if (access(def_conf_path_buf, F_OK) == 0) {
        return;
    } else {
        sprintf(def_conf_dir_path, DEFAULT_CONFIG_DIR_PATH, user_name);
        if (mkdir(def_conf_dir_path, S_IRUSR | S_IWUSR | S_IXUSR | S_IROTH)) {
            fprintf(stderr, "mkdir() error");
            exit(1);
        }
        FILE *f = fopen(def_conf_path_buf, "w");
        int write_bytes = fwrite(DEF_CONFIG_CONTENT, 1, sizeof(DEF_CONFIG_CONTENT), f);
        if (write_bytes != sizeof(DEF_CONFIG_CONTENT)) {
            fprintf(stderr, "fwrite(DEF_CONFIG_CONTENT) error\n");
            exit(1);
        }
        fclose(f);
    }
}

static void _config_parse_config(config *c) {
    _config_check_config_exists(c->user_name);
    FILE *conf = fopen(def_conf_path_buf, "r");
    c->libs = list_create(STR);
    byte conf_buf[4096] = {0};
    int read_bytes = fread(conf_buf, 1, 4096, conf);
    if (read_bytes < 1) {
        fprintf(stderr, "fread(conf_buf) error\n");
        exit(1);
    }

    byte temp[512] = {0};
    i32 i = 0;
    str lib_path = strstr(conf_buf, "\"");
    lib_path++;
    while(lib_path[i] != '"') {
        temp[i] = lib_path[i];
        i++;
    }
    if (i != 0) {
        strcpy(c->path_to_lib, temp);
        memset(temp, 0, 512);
    }

    str libs = strstr(&lib_path[i], "\"");
    if (libs == null) {
        return;
    }
    libs++;
    i = 0;
    i32 tmp_i = 0;

    while(true) {
        if (libs[i] == '"') {
            if (tmp_i != 0) {
                list_append(c->libs, alloc_copy(temp, tmp_i + 1));
                memset(temp, 0, 512);
                libs++;
                libs = strstr(libs, "\"");
                if (libs == null) {
                    break;
                }
                libs++;

                tmp_i = 0;
                i = 0;

            } else {
                break;
            }
        }
        temp[tmp_i++] = libs[i];
    }
}

config *config_init() {
    config *c = make(config);
    memset(c->user_name, 0, sizeof(c->user_name));
    _config_set_user_name(c->user_name);
    _config_parse_config(c);

    return c;
}

void config_deinit(config *conf);
