#ifndef COMMON_MAN
#define COMMON_MAN

#define COMMON_VERSION "1.0.0"

#if WIN32
#define COMMON_CONFIG "dict-on-ray.conf"
#else
#define COMMON_CONFIG "/etc/dict-on-ray.conf"
#endif

#define COMMON_HTTPS_PORT 443
#define COMMON_MAX_CONNECTION 1000

#define COMMON_QUERY "word"
#define COMMON_LIST "/word-list"

#if WIN32
#define COMMON_SSL_KEY "server-private-key.pem"
#define COMMON_SSL_CER "server-certificate-chain.pem"
#else
#define COMMON_SSL_KEY "/root/.ssl/server-private-key.pem"
#define COMMON_SSL_CER "/root/.ssl/server-certificate-chain.pem"
#endif

#if WIN32
#define COMMON_SOURCE "ENG_ZH.dicx"
#else
#define COMMON_SOURCE "/root/dict/ENG_ZH.dicx"
#endif

#define COMMON_SECTION_IDW ".idw"
#define COMMON_SECTION_IDT ".idt"
#define COMMON_SECTION_LST ".lst"
#define COMMON_SECTION_TSL ".tsl"

#if WIN32
#define COMMON_LOG "dict-on-ray.log"
#else
#define COMMON_LOG "/var/log/dict-on-ray.log"
#endif

void die_most_horribly_from_openssl_error(const char* func);

void error_exit(const char* fmt, ...);

/**
 * Calls some OpenSSL setup functions, which both the client and
 * server need to do.
 */
void common_setup(void);

#endif  /* COMMON_MAN */