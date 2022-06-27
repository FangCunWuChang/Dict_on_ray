#include "getargs.h"
#include "getopt.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

struct conf_opt_struct {
	bool in_on : 1;
	bool out_on : 1;
	bool word_on : 1;
	bool trans_on : 1;

	const char* in;
	const char* out;
	const char* word;
	const char* translation;
}conf_opt;

#define STR_CPY(dst, src) \
do { \
	const char* str_temp = malloc(strlen(src) + 1); \
	if (str_temp != NULL) { \
		(dst) = str_temp; \
		memset((dst), '\0', strlen(src) + 1); \
		strcpy((dst), (src)); } \
} while (0);

void set_out_file()
{
	int inlen = strlen(conf_opt.in);
	int pindex = 0;
	for (int i = 0; i <= inlen; i++) {
		if (conf_opt.in[i] == '.') {
			pindex = i;
		}
	}
	
	if (pindex == 0) {
		conf_opt.out = malloc(inlen + 5 + 1);
		if (conf_opt.out == NULL) { printf("Fatal!\n"); exit(1); }
		memset(conf_opt.out, '\0', inlen + 5 + 1);
		memcpy(conf_opt.out, conf_opt.in, inlen);
		memcpy(conf_opt.out + inlen, ".dicx", 5);
	}
	else {
		conf_opt.out = malloc(pindex + 5 + 1);
		if (conf_opt.out == NULL) { printf("Fatal!\n"); exit(1); }
		memset(conf_opt.out, '\0', pindex + 5 + 1);
		memcpy(conf_opt.out, conf_opt.in, pindex);
		memcpy(conf_opt.out + pindex, ".dicx", 5);
	}
}

int conf_get_options(int argc, char* argv[])
{
	int opt;
	int digit_optind = 0;
	int option_index = 0;
	const char* optstring = "";
	static struct option long_options[] = {
		{"help", no_argument, NULL, 'h'},
		{"version", no_argument, NULL, 'v'},
		{"in", required_argument, NULL, 'i'},
		{"out", required_argument, NULL, 'o'},
		{"word", required_argument, NULL, 'w'},
		{"translation", required_argument, NULL, 't'},
		{0, 0, 0, 0}
	};

	bool is_help = false;
	bool is_version = false;

	while ((opt = getopt_long(argc, argv, optstring, long_options, &option_index)) != -1)
	{
		switch (opt)
		{
		case 'h':
		{
			is_help = true;
			break;
		}
		case 'v':
		{
			is_version = true;
			break;
		}
		case 'i':
		{
			if (optarg != NULL) {
				conf_opt.in_on = true;
				STR_CPY(conf_opt.in, optarg);
			}
			break;
		}
		case 'o':
		{
			if (optarg != NULL) {
				conf_opt.out_on = true;
				STR_CPY(conf_opt.out, optarg);
			}
			break;
		}
		case 'w':
		{
			if (optarg != NULL) {
				conf_opt.word_on = true;
				STR_CPY(conf_opt.word, optarg);
			}
			break;
		}
		case 't':
		{
			if (optarg != NULL) {
				conf_opt.trans_on = true;
				STR_CPY(conf_opt.translation, optarg);
			}
			break;
		}
		}
	}

	if (is_help) {
		printf("Dict_on_ray Dictionary Database Complier %s\n", COMMON_VERSION);
		printf("Copyright(c) WuChang.All rights reserved.\n\n");
		printf("Usage: dictc <options> --in <file> (--out <file>) (--word <title>) (--translation <title>) ...\n");
		printf("\t--help\n\t\tHelp informations.\n\n");
		printf("\t--version\n\t\tVersion informations.\n\n");
		printf("\t--in <file>\n\t\tSet input file.\n\n");
		printf("\t--out <file>\n\t\tSet output file.\n\n");
		printf("\t--word <title>\n\t\tSet the column title of words.\n\n");
		printf("\t--translation <title>\n\t\tSet the column title of translations.\n\n");
		return -1;
	}
	if (is_version) {
		printf("Dict_on_ray Dictionary Database Complier %s\n", COMMON_VERSION);
		return -2;
	}

	if (!conf_opt.in_on) {
		printf("Choose a input file!\n");
		printf("See 'dictc --help'\n");
		return -3;
	}
	if (!conf_opt.out_on) {
		set_out_file();
		printf("Set default output file name: %s\n", conf_opt.out);
	}
	
	return 0;
}

const char* conf_get_input()
{
	return conf_opt.in;
}

const char* conf_get_output()
{
	return conf_opt.out;
}

const char* conf_get_word()
{
	return conf_opt.word;
}

const char* conf_get_translation()
{
	return conf_opt.translation;
}
