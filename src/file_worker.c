#include "../headers/file_worker.h"
#include "../headers/config.h"
#include <stdlib.h>
#include <cstdext/core.h>


file_worker *fw_create() {
    file_worker *w = make(file_worker);
    w->conf = config_init();
    w->cur_lib = null;

    return w;
}
void fw_select_lib(file_worker *fw, u32 index) {
    if (fw->cur_lib == null) {

    } else {
        if (fw->cur_lib->chagned) {

        } else {

        }
    }
}

void fw_write_lib(file_worker *fw);
void fw_lib_from_file(file_worker *fw, const str path_to_lib, str lib_name);
