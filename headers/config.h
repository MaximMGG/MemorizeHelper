#ifndef CONFIG_H
#define CONFIG_H
#include <cstdext/core.h>
#include <cstdext/container/list.h>

typedef struct {
    byte path_to_lib[256];
    byte user_name[128];

    list *libs;
} config;

config *config_init();
void config_deinit(config *conf);


#endif //CONFIG_H
