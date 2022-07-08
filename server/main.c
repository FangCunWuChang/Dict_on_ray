#include "https-common.h"
#include "dic.h"
#include "log.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include <sys/types.h>
#include <sys/stat.h>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#ifndef S_ISDIR
#define S_ISDIR(x) (((x) & S_IFMT) == S_IFDIR)
#endif
#else
#include <sys/stat.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#endif

#include <openssl/ssl.h>
#include <openssl/err.h>

#include <event2/bufferevent.h>
#include <event2/bufferevent_ssl.h>
#include <event2/event.h>
#include <event2/http.h>
#include <event2/buffer.h>
#include <event2/util.h>
#include <event2/keyvalq_struct.h>

#ifdef EVENT__HAVE_NETINET_IN_H
#include <netinet/in.h>
# ifdef _XOPEN_SOURCE_EXTENDED
#  include <arpa/inet.h>
# endif
#endif

#ifdef _WIN32
#define stat _stat
#define fstat _fstat
#define open _open
#define close _close
#define O_RDONLY _O_RDONLY
#endif

FILE* log_file = NULL;
unsigned short serverPort = COMMON_HTTPS_PORT;

/* Instead of casting between these types, create a union with all of them,
 * to avoid -Wstrict-aliasing warnings. */
typedef union
{
    struct sockaddr_storage ss;
    struct sockaddr sa;
    struct sockaddr_in in;
    struct sockaddr_in6 i6;
} sock_hop;


/* This callback gets invoked when we get any http request that doesn't match
 * any other callback.  Like any evhttp server callback, it has a simple job:
 * it must eventually call evhttp_send_error() or evhttp_send_reply().
 */
static void send_document_cb(struct evhttp_request* req, void* arg)
{
    dic_https_new_connection();
	
    /* Determine peer */
    char* peer_addr = NULL;
    ev_uint16_t peer_port;
    struct evhttp_connection* con = evhttp_request_get_connection(req);
    evhttp_connection_get_peer(con, &peer_addr, &peer_port);
	
	/* Get uri */
    const char* uri = evhttp_request_get_uri(req);

    if (evhttp_request_get_command(req) == EVHTTP_REQ_POST)
    {
        struct evbuffer* buf = evbuffer_new();
        if (buf == NULL) {
            log_error("[%s] Buffer returns NULL. Sending SERVUNAVAIL\n", peer_addr);
            evhttp_send_error(req, HTTP_SERVUNAVAIL, NULL);
            dic_https_close_connection();
			return;
        }
        evbuffer_add_printf(buf, "Requested: %s\n", uri);
        evhttp_send_reply(req, HTTP_OK, "OK", buf);
        evbuffer_free(buf);
        dic_https_close_connection();
        return;
    }

    /* We only handle GET requests. */
    if (evhttp_request_get_command(req) != EVHTTP_REQ_GET)
    {
        evhttp_send_reply(req, HTTP_OK, "OK", NULL);
        dic_https_close_connection();
        return;
    }

    log_info("[%s] Got a GET request for <%s>\n", peer_addr, uri);
	
	/* Get word list */
    if (!strcmp(uri, conf_get_list())) {
        log_info("[%s] Sending word list\n", peer_addr);
		
        struct evbuffer* buf = evbuffer_new();
        if (buf == NULL) {
            log_error("[%s] Buffer returns NULL. Sending SERVUNAVAIL\n", peer_addr);
            evhttp_send_error(req, HTTP_SERVUNAVAIL, NULL);
            dic_https_close_connection();
			return;
        }
        evhttp_add_header(evhttp_request_get_output_headers(req),
            "Content-Type", "text/plain;charset=utf-8");
        evbuffer_add_printf(buf, get_dic_list());
        evhttp_send_reply(req, HTTP_OK, "OK", buf);
        evbuffer_free(buf);
        dic_https_close_connection();
        return;
    }

    /* Decode the URI */
    struct evhttp_uri* decoded = evhttp_uri_parse(uri);
    if (!decoded)
    {
        log_warn("[%s] It's not a good URI. Sending BADREQUEST\n", peer_addr);
        evhttp_send_error(req, HTTP_BADREQUEST, NULL);
        dic_https_close_connection();
        return;
    }

    const char* query = (char*)evhttp_uri_get_query(decoded);
    if (!query) {
        log_warn("[%s] Query returns NULL. Sending NOTFOUND\n", peer_addr);
        evhttp_send_error(req, HTTP_NOTFOUND, NULL);
        evhttp_uri_free(decoded);
        dic_https_close_connection();
        return;
    }

    log_info("[%s] Got a query <%s>\n", peer_addr, query);
	
    /* Decode the query */
    struct evkeyvalq kv;
    memset(&kv, 0, sizeof(kv));
    evhttp_parse_query_str(query, &kv);

    /* Extract passcode */
    const char* word = evhttp_find_header(&kv, conf_get_query());

    if (!word) {
        log_warn("[%s] It's not a good query. Sending BADREQUEST\n", peer_addr);
        evhttp_send_error(req, HTTP_BADREQUEST, NULL);
        evhttp_uri_free(decoded);
        evhttp_clear_headers(&kv);
        dic_https_close_connection();
        return;
    }
	
    const char* response = get_dic_word(word, peer_addr, peer_port);
    evhttp_clear_headers(&kv);

    if (!response) {
        log_error("[%s] Dic returns NULL. Sending SERVUNAVAIL\n", peer_addr);
        evhttp_send_error(req, HTTP_SERVUNAVAIL, NULL);
        evhttp_uri_free(decoded);
        dic_https_close_connection();
        return;
    }
    /* This holds the content we're sending. */
    struct evbuffer* evb = evbuffer_new();
    if (evb == NULL) {
        log_error("[%s] Buffer returns NULL. Sending SERVUNAVAIL\n", peer_addr);
        evhttp_send_error(req, HTTP_SERVUNAVAIL, NULL);
        evhttp_uri_free(decoded);
        dic_https_close_connection();
        free((void*)response);
        return;
    }

    evhttp_add_header(evhttp_request_get_output_headers(req),
        "Content-Type", "text/plain;charset=utf-8");
    evbuffer_add(evb, response, strlen(response));

    evhttp_send_reply(req, HTTP_OK, "OK", evb);

    evhttp_uri_free(decoded);
    evbuffer_free(evb);
    free((void*)response);
	
    dic_https_close_connection();
}

/**
 * This callback is responsible for creating a new SSL connection
 * and wrapping it in an OpenSSL bufferevent.  This is the way
 * we implement an https server instead of a plain old http server.
 */
static struct bufferevent* bevcb(struct event_base* base, void* arg)
{
    struct bufferevent* r;
    SSL_CTX* ctx = (SSL_CTX*)arg;

    r = bufferevent_openssl_socket_new(base,
        -1,
        SSL_new(ctx),
        BUFFEREVENT_SSL_ACCEPTING,
        BEV_OPT_CLOSE_ON_FREE);
    return r;
}

static void server_setup_certs(SSL_CTX* ctx,
    const char* certificate_chain,
    const char* private_key)
{
    log_info("Loading certificate chain from '%s'\n"
        "and private key from '%s'\n",
        certificate_chain, private_key);

    if (1 != SSL_CTX_use_certificate_chain_file(ctx, certificate_chain))
        die_most_horribly_from_openssl_error("SSL_CTX_use_certificate_chain_file");

    if (1 != SSL_CTX_use_PrivateKey_file(ctx, private_key, SSL_FILETYPE_PEM))
        die_most_horribly_from_openssl_error("SSL_CTX_use_PrivateKey_file");

    if (1 != SSL_CTX_check_private_key(ctx))
        die_most_horribly_from_openssl_error("SSL_CTX_check_private_key");
}

static int serve_some_http(void)
{
    static int result = 0;
    struct event_base* base;
    struct evhttp* http;
    struct evhttp_bound_socket* handle;

#ifdef _WIN32
    WSADATA WSAData;
    result = WSAStartup(0x101, &WSAData);
#endif

    base = event_base_new();
    if (!base)
    {
        log_fatal ("Couldn't create an event_base: exiting\n");
        return 1;
    }

    /* Create a new evhttp object to handle requests. */
    http = evhttp_new(base);
    if (!http)
    {
        log_fatal ("couldn't create evhttp. Exiting.\n");
        return 1;
    }

    SSL_CTX* ctx = SSL_CTX_new(SSLv23_server_method());
    SSL_CTX_set_options(ctx,
        SSL_OP_SINGLE_DH_USE |
        SSL_OP_SINGLE_ECDH_USE |
        SSL_OP_NO_SSLv2);

    /* Cheesily pick an elliptic curve to use with elliptic curve ciphersuites.
     * We just hardcode a single curve which is reasonably decent. */
    EC_KEY* ecdh = EC_KEY_new_by_curve_name(NID_X9_62_prime256v1);
    if (!ecdh)
        die_most_horribly_from_openssl_error("EC_KEY_new_by_curve_name");
    if (1 != SSL_CTX_set_tmp_ecdh(ctx, ecdh))
        die_most_horribly_from_openssl_error("SSL_CTX_set_tmp_ecdh");

    /* Find and set up our server certificate. */
    const char* certificate_chain = conf_get_ssl_cer();
    const char* private_key = conf_get_ssl_key();
    server_setup_certs(ctx, certificate_chain, private_key);

    /* This is the magic that lets evhttp use SSL. */
    evhttp_set_bevcb(http, bevcb, ctx);

    /* This is the callback that gets called when a request comes in. */
    evhttp_set_gencb(http, send_document_cb, NULL);

    /* Now we tell the evhttp what port to listen on */
    handle = evhttp_bind_socket_with_handle(http, "0.0.0.0", serverPort);
    if (!handle)
    {
        log_fatal ("couldn't bind to port %d. Exiting.\n",
            (int)serverPort);
        return 1;
    }

    { /* Extract and display the address we're listening on. */
        sock_hop ss;
        evutil_socket_t fd;
        ev_socklen_t socklen = sizeof(ss);
        char addrbuf[128];
        void* inaddr;
        const char* addr;
        int got_port = -1;
        fd = evhttp_bound_socket_get_fd(handle);
        memset(&ss, 0, sizeof(ss));
        if (getsockname(fd, &ss.sa, &socklen))
        {
            log_fatal("getsockname() failed");
            return 1;
        }
        if (ss.ss.ss_family == AF_INET)
        {
            got_port = ntohs(ss.in.sin_port);
            inaddr = &ss.in.sin_addr;
        }
        else if (ss.ss.ss_family == AF_INET6)
        {
            got_port = ntohs(ss.i6.sin6_port);
            inaddr = &ss.i6.sin6_addr;
        }
        else
        {
            log_fatal("Weird address family %d\n", ss.ss.ss_family);
            return 1;
        }
        addr = evutil_inet_ntop(ss.ss.ss_family, inaddr, addrbuf,
            sizeof(addrbuf));
        if (addr)
            log_info("Listening on %s:%d\n", addr, got_port);
        else
        {
            log_fatal("evutil_inet_ntop failed\n");
            return 1;
        }
    }

    event_base_dispatch(base);

    /* not reached; runs forever */

    return 0;
}

static bool log_lock = false;
void log_locker(bool lock, void* udata)
{
    if (!lock) {
        log_lock = false;
        return;
    }
    while (log_lock);
    log_lock = true;
}

int main(int argc, char** argv)
{
    if (conf_init(argc, argv) != 0) {
        return 0;
    }
    
    const char* log = conf_get_log();
    log_file = fopen(log, "w");
    if (log_file == NULL) {
        printf("[ERROR]Can't open log file:%s\n", log);
        return -1;
    }
    log_add_fp(log_file, 0);
    log_set_lock(log_locker, NULL);

    common_setup();              /* Initialize OpenSSL */
	
    serverPort = conf_get_port();
    const char* source = conf_get_source();

    log_info("Using query:%s\n", conf_get_query());
    log_info("Using list query:%s\n", conf_get_list());
    log_info("Using max connections:%lld\n", conf_get_max_connection());
    log_info("Using dictionary:%s\n", source);

    if (dic_init(source) != 0) {
        fclose(log_file);
        return -1;
    }

    /* now run http server (never returns) */
    return serve_some_http();
}