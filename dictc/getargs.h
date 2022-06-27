#ifndef GETARGS_H
#define GETARGS_H

#define COMMON_VERSION "1.0.0"

int conf_get_options(int argc, char* argv[]);

const char* conf_get_input();
const char* conf_get_output();

const char* conf_get_word();
const char* conf_get_translation();

#endif // !GETARGS_H
