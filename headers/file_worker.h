#ifndef FILE_WORKER_H
#define FILE_WORKER_H
#include <cstdext/core.h>
#include "config.h"
#include "library.h"

typedef struct {
    config *conf;
    lib *cur_lib;

} file_worker;

file_worker *fw_create();
void fw_select_lib(file_worker *fw, u32 index);
void fw_write_lib(file_worker *fw);
void fw_lib_from_file(file_worker *fw, const str path_to_lib, str lib_name);

#endif //FILE_WORKER_H
