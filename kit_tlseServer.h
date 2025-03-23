#ifndef KIT_TLSE_SERVER
#define KIT_TLSE_SERVER

#ifdef _WIN32
    #pragma comment(lib, "Ws2_32.lib")
    #include <winsock2.h>
    #define socklen_t int
    #define sleep(x)    Sleep(x*1000)
#else
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #include <fcntl.h>
#endif
#include "tlse.h"

#include <signal.h>


typedef enum sl_Error {
    sl_error_ok = 0,
    sl_error_genericError,
    sl_error_socketCreationFailed,
    sl_error_socketOptionSetFailed,
    sl_error_socketBindingFailed,
    sl_error_socketListenFailed,
    sl_error_sslServerContextCreationFailed,
    sl_error_sslPrivateKeyLoadFailed
} sl_Error;

typedef struct sl_Context {
    i32 socketDesc;
    SSL* serverCtx;
    i64 activeConnections;
} sl_Context;

#if 0
static int http__makeSocketNonBlocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) {
        perror("fcntl failed");
        return -1;
    }
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
        perror("fcntl failed to set non-blocking");
        return -1;
    }
    return 0;
}
#endif

static sl_Error sl_init(sl_Context* webServer, int port, S8 certPath, S8 privKeyPath, i32 maxWatingConnection) {
    assert(webServer);
    int socketDesc = socket(AF_INET , SOCK_STREAM , 0);

    if (socketDesc == -1) {
        return sl_error_socketCreationFailed;
    }

    char enable = 1;


    int flag = 1;
    // Allow reuse of addresses (SO_REUSEADDR)
    if (setsockopt(socketDesc, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag))) {
        close(socketDesc);
        return sl_error_socketOptionSetFailed;
    }
#if 0

    // Disable Nagle's algorithm (TCP_NODELAY)
    if (setsockopt(socketDesc, IPPROTO_TCP, TCP_NODELAY, &flag, sizeof(flag))) {
        close(socketDesc);
        return sl_error_socketOptionSetFailed;
    }


    // Increase the receive buffer size (SO_RCVBUF)
    int buf_size = 4096;
    if (setsockopt(socketDesc, SOL_SOCKET, SO_RCVBUF, &buf_size, sizeof(buf_size))) {
        close(socketDesc);
        return sl_error_socketOptionSetFailed;
    }

    // Enable keepalive to ensure long-lived connections are properly monitored (SO_KEEPALIVE)
    if (setsockopt(socketDesc, SOL_SOCKET, SO_KEEPALIVE, &flag, sizeof(flag))) {
        close(socketDesc);
        return sl_error_socketOptionSetFailed;
    }

    // Set a custom linger option (SO_LINGER)
    struct linger linger_opt;
    linger_opt.l_onoff = 1;
    linger_opt.l_linger = 30;  // Wait up to 30 seconds before closing the socket
    if (setsockopt(socketDesc, SOL_SOCKET, SO_LINGER, &linger_opt, sizeof(linger_opt))) {
        close(socketDesc);
        return sl_error_socketOptionSetFailed;
    }
#endif

    struct sockaddr_in server, client;
     
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port);

    if (bind(socketDesc,(struct sockaddr *)&server , sizeof(server)) < 0) {
        close(socketDesc);
        return sl_error_socketBindingFailed;
    }

    if (listen(socketDesc , maxWatingConnection)) {
        close(socketDesc);
        return sl_error_socketListenFailed;
    }

    SSL* serverCtx = SSL_CTX_new(SSLv3_server_method());
    if (!serverCtx) {
        close(socketDesc);
        return sl_error_sslServerContextCreationFailed;
    }


    mem_defineMakeStackArena(localArena, KILOBYTE(2));

    S8 certPathhNullPtr = str_copyNullTerminated(localArena, certPath);
    S8 privKeyPathNullPtr = str_copyNullTerminated(localArena, privKeyPath);

    SSL_CTX_use_certificate_file(serverCtx, (const char *) certPathhNullPtr.content, SSL_SERVER_RSA_CERT);
    SSL_CTX_use_PrivateKey_file(serverCtx, (const char *) privKeyPath.content, SSL_SERVER_RSA_KEY);

    if (!SSL_CTX_check_private_key(serverCtx)) {
        SSL_CTX_free(serverCtx);
        close(socketDesc);
        return sl_error_sslPrivateKeyLoadFailed;
    }

    webServer->socketDesc = socketDesc;
    webServer->serverCtx = serverCtx;

    return sl_error_ok;
}

typedef enum sl_ClientConnectionError {
    sl_ClientConnectionError_ok = 0,
    sl_ClientConnectionError_handshakeFailed,
} sl_ClientConnectionError;
typedef struct sl_ClientConnection {
    i32 clientSock;
    SSL *client;
} sl_ClientConnection;
static sl_ClientConnectionError sl_NextConnection(sl_Context* context, i32 clientSock, sl_ClientConnection* connection) {

    char enable = 1;
    if (setsockopt(clientSock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {
        close(clientSock);
        return sl_ClientConnectionError_handshakeFailed;
    }

    struct timeval timeout = {};
    timeout.tv_sec = 2;  // 10 seconds
    timeout.tv_usec = 0;

    // accept timeout
    /*
    if (setsockopt(socketDesc, SOL_SOCKET, SO_RCVBUF, (const char*)&timeout, sizeof(timeout)) < 0) {
        close(socketDesc);
        return sl_error_socketOptionSetFailed;
    }
    */

    // rec timeout
    if (setsockopt(clientSock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
        close(clientSock);
        return sl_ClientConnectionError_handshakeFailed;
    }
    fcntl(clientSock, F_SETFL, fcntl(clientSock, F_GETFL) | O_NONBLOCK);


    SSL *sslClient = SSL_new(context->serverCtx);
    if (!sslClient) {
        fprintf(stderr, "Error creating SSL client\n");
        return -4;
    }
    SSL_set_fd(sslClient, clientSock);
    if (!SSL_accept(sslClient)) {
        return sl_ClientConnectionError_handshakeFailed;
    }
    connection->client = sslClient;
    connection->clientSock = clientSock;
    context->activeConnections++;
    return sl_ClientConnectionError_ok;
}

static i32 sl_ReadData(sl_ClientConnection* connection, u8* clientMessage, i32 readSize) {
    return SSL_read(connection->client, clientMessage, readSize);
}

static bool sl_WriteData(sl_ClientConnection* connection, const u8* clientMessage, i32 writeSize) {
    return SSL_write(connection->client, clientMessage, writeSize) != writeSize;
}

static void sl_CloseClientConnection(sl_ClientConnection* connection) {
    SSL_shutdown(connection->client);
#if OS_WIN
    shutdown(connection->clientSock, SD_BOTH);
    closesocket(connection->clientSock);
#else
    shutdown(connection->clientSock, SHUT_RDWR);
    close(connection->clientSock);
#endif
    SSL_free(connection->client);
}

static void sl_Shutdown(sl_Context* context) {
    SSL_CTX_free(context->serverCtx);
    close(context->socketDesc);
}

void g_ignoreSigpipe(void) {
#if OS_WIN
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
#else
    signal(SIGPIPE, SIG_IGN);
#endif
}

#endif // KIT_TLSE_SERVER
