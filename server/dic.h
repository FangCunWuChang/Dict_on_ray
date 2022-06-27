#ifndef DIC_H_INCLUDED
#define DIC_H_INCLUDED

#include <stdint.h>

int dic_init(const char* file_name);

const char* get_dic_word(const char* word, const char* peer_address, uint16_t peer_port);
const char* get_dic_list();

int conf_init(int argc, char** argv);

uint16_t conf_get_port();
uint64_t conf_get_max_connection();

const char* conf_get_ssl_cer();
const char* conf_get_ssl_key();

const char* conf_get_query();
const char* conf_get_list();

const char* conf_get_source();

const char* conf_get_log();

void dic_https_new_connection();
void dic_https_close_connection();

#endif // !DIC_H_INCLUDED
