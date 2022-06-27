#include <signal.h>

#include "https-common.h"
#include "log.h"

#include <openssl/ssl.h>
#include <openssl/err.h>

#include <event2/event.h>

void die_most_horribly_from_openssl_error(const char* func)
{
    extern FILE* log_file;

    log_fatal("%s failed:\n", func);

    /* This is the OpenSSL function that prints the contents of the
     * error stack to the specified file handle. */
    ERR_print_errors_fp(log_file);

    exit(-1);
}

void error_exit(const char* fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    log_fatal(fmt, ap);
    va_end(ap);

    exit(-1);
}

/* OpenSSL has a habit of using uninitialized memory.  (They turn up their
 * nose at tools like valgrind.)  To avoid spurious valgrind errors (as well
 * as to allay any concerns that the uninitialized memory is actually
 * affecting behavior), let's install a custom malloc function which is
 * actually calloc.
 */
static void* my_zeroing_malloc(size_t howmuch)
{
    return calloc(1, howmuch);
}

void common_setup(void)
{
#ifndef WIN32
    signal(SIGPIPE, SIG_IGN);
#endif // !WIN32

    CRYPTO_set_mem_functions(my_zeroing_malloc, realloc, free);
    SSL_library_init();
    SSL_load_error_strings();
    OpenSSL_add_all_algorithms();

    log_info("Using OpenSSL version \"%s\"\nand libevent version \"%s\"\n",
        SSLeay_version(SSLEAY_VERSION),
        event_get_version());
}