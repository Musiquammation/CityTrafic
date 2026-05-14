#pragma once


#if (defined(SSL_KEY_PATH) && defined(SSL_CERT_PATH))
    #define USING_SSL 1
#else
    #define USING_SSL 0
#endif
