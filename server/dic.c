#include "dic.h"
#include "https-common.h"
#include "ini.h"
#include "getopt.h"
#include "log.h"

#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include <stdlib.h>

struct dic_mem_struct {
	uint32_t file_size;
	void* memory_ptr;

	uint32_t idw_size;
	void* idw_ptr;

	uint32_t idt_size;
	void* idt_ptr;

	uint32_t lst_size;
	void* lst_ptr;

	uint32_t tsl_size;
	void* tsl_ptr;
}dic_mem;

#define SECTION_ID_CMP(m,n) \
((m)[0]==(n)[0] && \
 (m)[1]==(n)[1] && \
 (m)[2]==(n)[2] && \
 (m)[3]==(n)[3])

int dic_init(const char* file_name)
{
	FILE* file = fopen(file_name, "rb");
	if (file == NULL) {
		log_fatal("Can't open file: %s\n", file_name);
		return -1;
	}

	fseek(file, 0, SEEK_END);
	size_t file_size = ftell(file);
	if (file_size > UINT32_MAX) {
		log_fatal("File too big: %s\n", file_name);
		return -2;
	}
	rewind(file);
	dic_mem.file_size = (uint32_t)file_size;

	dic_mem.memory_ptr = malloc(file_size);
	if (dic_mem.memory_ptr == NULL) {
		log_fatal("Out of memory!\n");
		return -3;
	}

	fread(dic_mem.memory_ptr, file_size, 1, file);
	fclose(file);

	const char* sec_idw = COMMON_SECTION_IDW;
	const char* sec_idt = COMMON_SECTION_IDT;
	const char* sec_lst = COMMON_SECTION_LST;
	const char* sec_tsl = COMMON_SECTION_TSL;

	bool idw_on = false;
	bool idt_on = false;
	bool lst_on = false;
	bool tsl_on = false;

	for (const char* ptr = dic_mem.memory_ptr; (size_t)((size_t)ptr - (size_t)dic_mem.memory_ptr) < dic_mem.file_size;) {
		uint32_t sec_size = *((uint32_t*)((ptr + 4)));
		if (SECTION_ID_CMP(ptr, sec_idw)) {
			dic_mem.idw_size = sec_size;
			dic_mem.idw_ptr = (void*)(ptr + 4 + sizeof(uint32_t));
			idw_on = true;
		}
		else if (SECTION_ID_CMP(ptr, sec_idt)) {
			dic_mem.idt_size = sec_size;
			dic_mem.idt_ptr = (void*)(ptr + 4 + sizeof(uint32_t));
			idt_on = true;
		}
		else if (SECTION_ID_CMP(ptr, sec_lst)) {
			dic_mem.lst_size = sec_size;
			dic_mem.lst_ptr = (void*)(ptr + 4 + sizeof(uint32_t));
			lst_on = true;
		}
		else if (SECTION_ID_CMP(ptr, sec_tsl)) {
			dic_mem.tsl_size = sec_size;
			dic_mem.tsl_ptr = (void*)(ptr + 4 + sizeof(uint32_t));
			tsl_on = true;
		}
		ptr += (4 + sizeof(uint32_t) + sec_size);
		if ((size_t)((size_t)ptr - (size_t)dic_mem.memory_ptr) > dic_mem.file_size) {
			log_fatal("Data format error: %s\n", file_name);
			return 1;
		}
	}

	if (!idw_on) {
		log_fatal("Miss %s section: %s\n", sec_idw, file_name);
		return 2;
	}
	if (!idt_on) {
		log_fatal("Miss %s section: %s\n", sec_idt, file_name);
		return 2;
	}
	if (!lst_on) {
		log_fatal("Miss %s section: %s\n", sec_lst, file_name);
		return 2;
	}
	if (!tsl_on) {
		log_fatal("Miss %s section: %s\n", sec_tsl, file_name);
		return 2;
	}

	return 0;
}

int dic_strcmp(const char* str1, const char* str2) {
	while ((*str1 != '\n') && (*str2 != '\0') && (*str1 == *str2))
	{
		str1++;
		str2++;
	}
	if ((*str1 == '\n') && (*str2 == '\0')) {
		return 0;
	}
	return *str1 - *str2;
}

#define LST_CHAR_PTR(n) \
(const char*)((size_t)dic_mem.lst_ptr + \
*((uint32_t*)((size_t)dic_mem.idw_ptr + (size_t)((uint32_t)(n) * sizeof(uint32_t)))))
#define TSL_CHAR_PTR(n) \
(const char*)((size_t)dic_mem.tsl_ptr + \
*((uint32_t*)((size_t)dic_mem.idt_ptr + (size_t)((uint32_t)(n) * sizeof(uint32_t)))))

const char* get_dic_result(const char* word)
{
	uint32_t size = dic_mem.idw_size / sizeof(uint32_t);
	uint32_t left = 0, right = (uint32_t)size - 1;

	while (left <= right)
	{
		uint32_t mid = left + (right - left) / 2;
		const char* pw = LST_CHAR_PTR(mid);

		if (dic_strcmp(pw, word) < 0)
		{
			left = mid + 1;
		}
		else if (dic_strcmp(pw, word) > 0)
		{
			right = mid - 1;
		}
		else
		{
			return TSL_CHAR_PTR(mid);
		}
	}
	
	return NULL;
}

const char* get_dic_word(const char* word, const char* peer_address, uint16_t peer_port)
{
	bool status = true;
	const char* result = get_dic_result(word);
	if (result == NULL) {
		result = "";
		status = false;
	}

	const char* format = "{\"peer\":{\"address\":\"%s\",\"port\":%d},\"status\":%s,\"word\":\"%s\",\"result\":\"%s\"}";
	size_t maxsize = strlen(format) + strlen(word) + strlen(peer_address) + strlen(result) + 5;
	const char* res = malloc(maxsize);
	if (!res) {
		return NULL;
	}
	memset((void*)res, '\0', maxsize);
	int r = sprintf((char*)res, format, peer_address, peer_port, status ? "true" : "false", word, result);
	return res;
}

const char* get_dic_list()
{
	return dic_mem.lst_ptr;
}

struct conf_opt_struct {
	bool config_on : 1;
	bool port_on : 1;
	bool max_connection_on : 1;
	bool ssl_cer_on : 1;
	bool ssl_key_on : 1;
	bool query_on : 1;
	bool list_on : 1;
	bool source_on : 1;
	bool log_on : 1;

	const char* config;
	const char* port;
	const char* max_connection;
	const char* ssl_cer;	
	const char* ssl_key;
	const char* query;	
	const char* list;
	const char* source;	
	const char* log;
}conf_opt;

static uint16_t conf_port = COMMON_HTTPS_PORT;
static uint64_t conf_max_connection = COMMON_MAX_CONNECTION;

static const char* conf_ssl_cer = COMMON_SSL_CER;
static const char* conf_ssl_key = COMMON_SSL_KEY;

static const char* conf_query = COMMON_QUERY;
static const char* conf_list = COMMON_LIST;

static const char* conf_source = COMMON_SOURCE;

static const char* conf_log = COMMON_LOG;

uint64_t toInt(const char* str)
{
	uint64_t temp = 0;
	int len = strlen(str);
	for (int i = 0; i < len; i++) {
		if (str[i] >= '0' && str[i] <= '9') {
			uint64_t n = (uint64_t)(str[i] - '0') * (uint64_t)pow(10, (len - i - 1));
			if (n > UINT64_MAX - temp) {
				return 0;
			}
			temp += n;
		}
		else {
			return 0;
		}
	}
	return temp;
}

#define STR_CPY(dst, src) \
do { \
	const char* str_temp = malloc(strlen(src) + 1); \
	if (str_temp != NULL) { \
		(dst) = str_temp; \
		memset((void*)(dst), '\0', strlen(src) + 1); \
		strcpy((char*)(dst), (const char*)(src)); } \
} while (0);


#define STR_CPY_IF_SRC(dst, src) \
if ((src) != NULL) { \
	STR_CPY(dst, src) \
}

int conf_parse_opt(int argc, char** argv)
{
	int opt;
	int digit_optind = 0;
	int option_index = 0;
	const char* optstring = "p:v";
	static struct option long_options[] = {
		{"help", no_argument, NULL, 'h'},
		{"version", no_argument, NULL, 'v'},
		{"config", required_argument, NULL, 'c'},
		{"port", required_argument, NULL, 'p'},
		{"max_connection", required_argument, NULL, 'm'},
		{"ssl_cer", required_argument, NULL, 's'},
		{"ssl_key", required_argument, NULL, 'k'},
		{"query", required_argument, NULL, 'q'},
		{"list", required_argument, NULL, 'l'},
		{"source", required_argument, NULL, 'd'},
		{"log", required_argument, NULL, 'o'},
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
		case 'c':
		{
			if (optarg != NULL) {
				conf_opt.config_on = true;
				STR_CPY(conf_opt.config, optarg);
			}
			break;
		}
		case 'p':
		{
			if (optarg != NULL) {
				conf_opt.port_on = true;
				STR_CPY(conf_opt.port, optarg);
			}
			break;
		}
		case 'm':
		{
			if (optarg != NULL) {
				conf_opt.max_connection_on = true;
				STR_CPY(conf_opt.max_connection, optarg);
			}
			break;
		}
		case 's':
		{
			if (optarg != NULL) {
				conf_opt.ssl_cer_on = true;
				STR_CPY(conf_opt.ssl_cer, optarg);
			}
			break;
		}
		case 'k':
		{
			if (optarg != NULL) {
				conf_opt.ssl_key_on = true;
				STR_CPY(conf_opt.ssl_key, optarg);
			}
			break;
		}
		case 'q':
		{
			if (optarg != NULL) {
				conf_opt.query_on = true;
				STR_CPY(conf_opt.query, optarg);
			}
			break;
		}
		case 'l':
		{
			if (optarg != NULL) {
				conf_opt.list_on = true;
				STR_CPY(conf_opt.list, optarg);
			}
			break;
		}
		case 'd':
		{
			if (optarg != NULL) {
				conf_opt.source_on = true;
				STR_CPY(conf_opt.source, optarg);
			}
			break;
		}
		case 'o':
		{
			if (optarg != NULL) {
				conf_opt.log_on = true;
				STR_CPY(conf_opt.log, optarg);
			}
			break;
		}
		}
	}
	
	if (is_help) {
		printf("Dict_on_ray Server %s\n", COMMON_VERSION);
		printf("Copyright(c) WuChang.All rights reserved.\n\n");
		printf("Usage: server <options> [args] ...\n");
		printf("\t--help, -h\n\t\tHelp informations.\n\n");
		printf("\t--version\n\t\tVersion informations.\n\n");
		printf("\t--config <file>\n\t\tSet config file.\n\n");
		printf("\t--port, -p <port>\n\t\tSet local port.\n\n");		
		printf("\t--max_connection <num>\n\t\tSet max connections.\n\n");		
		printf("\t--ssl_cer <file>\n\t\tSet SSL certificate.\n\n");
		printf("\t--ssl_key <file>\n\t\tSet SSL private key.\n\n");
		printf("\t--query <string>\n\t\tSet http GET query.\n\n");
		printf("\t--list <string>\n\t\tSet http GET query of list.\n\n");
		printf("\t--source <file>\n\t\tSet dictionary source.\n\n");
		printf("\t--log <file>\n\t\tSet log file.\n\n");
		return -1;
	}
	if (is_version) {
		printf("Dict_on_ray Server %s\n", COMMON_VERSION);
		return -2;
	}
	return 0;
}

int conf_init(int argc, char** argv)
{
	int cor = conf_parse_opt(argc, argv);
	if (cor != 0) {
		return cor;
	}
	
	const char* conf = COMMON_CONFIG;
	if (conf_opt.config_on) {
		conf = conf_opt.config;
	}
	
	ini_t* cfg = ini_load(conf);
	if (cfg == NULL) {
		printf("Using default configs.\n");
	}
	else {
		printf("Using config file: %s\n", conf);

		const char* https_port = ini_get(cfg, "net", "port");
		const char* max_connection = ini_get(cfg, "net", "max_connection");
		const char* ssl_key = ini_get(cfg, "ssl", "ssl_key");
		const char* ssl_cer = ini_get(cfg, "ssl", "ssl_cer");
		const char* query = ini_get(cfg, "request", "query");
		const char* list = ini_get(cfg, "request", "list");
		const char* source = ini_get(cfg, "dictionary", "source");
		const char* log = ini_get(cfg, "log", "log");

		if (https_port != NULL) conf_port = toInt(https_port);
		if (max_connection != NULL) conf_max_connection = toInt(max_connection);
		STR_CPY_IF_SRC(conf_ssl_key, ssl_key);
		STR_CPY_IF_SRC(conf_ssl_cer, ssl_cer);
		STR_CPY_IF_SRC(conf_query, query);
		STR_CPY_IF_SRC(conf_list, list);
		STR_CPY_IF_SRC(conf_source, source);
		STR_CPY_IF_SRC(conf_log, log);

		ini_free(cfg);
	}
	
	if (conf_opt.port_on) {
		conf_port = toInt(conf_opt.port);
	}
	if (conf_opt.max_connection_on) {
		conf_max_connection = toInt(conf_opt.max_connection);
	}
	if (conf_opt.ssl_key_on) {
		conf_ssl_key = conf_opt.ssl_key;
	}
	if (conf_opt.ssl_cer_on) {
		conf_ssl_cer = conf_opt.ssl_cer;
	}
	if (conf_opt.query_on) {
		conf_query = conf_opt.query;
	}
	if (conf_opt.list_on) {
		conf_list = conf_opt.list;
	}
	if (conf_opt.source_on) {
		conf_source = conf_opt.source;
	}
	if (conf_opt.log_on) {
		conf_log = conf_opt.log;
	}
	
	return 0;
}

uint16_t conf_get_port()
{
	return conf_port;
}

uint64_t conf_get_max_connection()
{
	return conf_max_connection;
}

const char* conf_get_ssl_cer()
{
	return conf_ssl_cer;
}

const char* conf_get_ssl_key()
{
	return conf_ssl_key;
}

const char* conf_get_query()
{
	return conf_query;
}

const char* conf_get_list()
{
	return conf_list;
}

const char* conf_get_source()
{
	return conf_source;
}

const char* conf_get_log()
{
	return conf_log;
}

static uint64_t connection_count = 0;

void dic_https_new_connection()
{
	connection_count++;
	while (connection_count >= conf_max_connection);
}

void dic_https_close_connection()
{
	connection_count--;
}
