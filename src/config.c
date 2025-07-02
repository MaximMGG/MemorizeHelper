#include "../headers/config.h"
#include <cstdext/core.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


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

static void _config_parse_config(config *c) {

}

config *config_init() {
    config *c = make(config);
    memset(c->user_name, 0, sizeof(c->user_name));
    _config_set_user_name(c->user_name);


}

void config_deinit(config *conf);
