#include <stdio.h>
#include <string.h>    //strlen
#ifdef _WIN32
    #include <winsock2.h>
    #define socklen_t int
    #define sleep(x)    Sleep(x*1000)
#else
    #include <sys/socket.h>
    #include <arpa/inet.h>
#endif

#include <signal.h>
#include <unistd.h>

#include "base.c"

#include "tlse.h"

#if 0
typedef struct http_Router {
    int foo;
} http_Router;

void website_indexHtml(HtmlBuilder* builder) {
    html(builder) {
        html_head(builder) {

        }
        html_body(builder) {
            html_div(builder) {
                html_textFmt(builder, "Hello world");
            }
        }
    }
}
void website_aboutHtml(HtmlBuilder* builder) {
    html(builder) {
        html_head(builder) {

        }
        html_body(builder) {
            html_div(builder) {
                html_textFmt(builder, "About this");
            }
        }
    }
}
void website_404Html(HtmlBuilder* builder) {
    html(builder) {
        html_head(builder) {

        }
        html_body(builder) {
            html_div(builder) {
                html_textFmt(builder, "404");
            }
        }
    }
}

static int ssl_writeStr(S8 str) {
    return SSL_write(client, str.content, str.size);
}
static int ssl_readStr(S8* str) {
    return SSL_write(client, str->content, str.size);
}
#endif

typedef enum http_methodType {
    http_methodType_get = 0,
    http_methodType_post = 1,
    http_methodType_any = 1,
} http_methodType;

#if 0
typedef struct http_RouterPath {
    s8 path;
    u32 short;
    struct {u8 type;} dynamicRoutes[20];
} http_RouterPath;
#endif

typedef enum http_webserverError {
    http_webserverError_ok = 0,
    http_webserverError_genericError,
    http_webserverError_socketCreationFailed,
    http_webserverError_socketOptionSetFailed,
    http_webserverError_socketBindingFailed,
    http_webserverError_socketListenFailed,
    http_webserverError_sslServerContextCreationFailed,
    http_webserverError_sslPrivateKeyLoadFailed
} http_webserverError;

typedef struct http_WebServer {
    i32 socketDesc;
    SSL* serverCtx;
} http_WebServer;

static http_webserverError http_webServerInit(http_WebServer* webServer, int port, S8 certPath, S8 privKeyPath, i32 maxWatingConnection) {
    assert(webServer);
    int socketDesc = socket(AF_INET , SOCK_STREAM , 0);

    if (socketDesc == -1) {
        return http_webserverError_socketCreationFailed;
    }

    i32 enable = 1;

    if (setsockopt(socketDesc, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {
        close(socketDesc);
        return http_webserverError_socketOptionSetFailed;
    }

    struct sockaddr_in server, client;
     
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port);

    if (bind(socketDesc,(struct sockaddr *)&server , sizeof(server)) < 0) {
        close(socketDesc);
        return http_webserverError_socketBindingFailed;
    }

    if (listen(socketDesc , maxWatingConnection)) {
        close(socketDesc);
        return http_webserverError_socketListenFailed;
    }

    SSL* serverCtx = SSL_CTX_new(SSLv3_server_method());
    if (!serverCtx) {
        close(socketDesc);
        return http_webserverError_sslServerContextCreationFailed;
    }


    mem_defineMakeStackArena(localArena, KILOBYTE(2));

    S8 certPathhNullPtr = str_copyNullTerminated(localArena, certPath);
    S8 privKeyPathNullPtr = str_copyNullTerminated(localArena, privKeyPath);

    SSL_CTX_use_certificate_file(serverCtx, (const char *) certPathhNullPtr.content, SSL_SERVER_RSA_CERT);
    SSL_CTX_use_PrivateKey_file(serverCtx, (const char *) privKeyPath.content, SSL_SERVER_RSA_KEY);

    if (!SSL_CTX_check_private_key(serverCtx)) {
        SSL_CTX_free(serverCtx);
        close(socketDesc);
        return http_webserverError_sslPrivateKeyLoadFailed;
    }

    webServer->socketDesc = socketDesc;
    webServer->serverCtx = serverCtx;

    return http_webserverError_ok;
}

typedef enum http_webserverClientConnectionError {
    http_webserverClientConnectionError_ok = 0,
    http_webserverClientConnectionError_handshakeFailed,
} http_webserverClientConnectionError;
typedef struct http_WebServerClientConnection {
    i32 clientSock;
    SSL *client;
} http_WebServerClientConnection;
static http_webserverClientConnectionError http_webServerNextConnection(http_WebServer* webServer, http_WebServerClientConnection* connection) {
    struct sockaddr_in client;

    socklen_t c = sizeof(struct sockaddr_in);
    i32 clientSock = accept(webServer->socketDesc, (struct sockaddr *)&client, &c);
    if (clientSock < 0) {
        fprintf(stderr, "Accept failed\n");
        return -3;
    }
    SSL *sslClient = SSL_new(webServer->serverCtx);
    if (!sslClient) {
        fprintf(stderr, "Error creating SSL client\n");
        return -4;
    }
    SSL_set_fd(sslClient, clientSock);
    if (!SSL_accept(sslClient)) {
        return http_webserverClientConnectionError_handshakeFailed;
    }
    return http_webserverClientConnectionError_ok;
}

static i32 http_webServerReadData(http_WebServerClientConnection* connection, u8* clientMessage, i32 readSize) {
    return SSL_read(connection->client, clientMessage, readSize);
}

static bx http_webServerWriteData(http_WebServerClientConnection* connection, const u8* clientMessage, i32 writeSize) {
    return SSL_write(connection->client, clientMessage, writeSize) != writeSize;
}

static void http_webServerCloseClientConnection(http_WebServerClientConnection* connection) {
    SSL_shutdown(connection->client);
#if OS_WIN
    shutdown(clientSock, SD_BOTH);
    closesocket(clientSock);
#else
    shutdown(connection->clientSock, SHUT_RDWR);
    close(connection->clientSock);
#endif
    SSL_free(connection->client);
}

static void http_webServerShutdown(http_WebServer* webServer) {
    SSL_CTX_free(webServer->serverCtx);
    close(webServer->socketDesc);
}

static http_webserverError g_startWebserver(int port, S8 certPath, S8 privKeyPath, i32 maxWatingConnection) {
    int socketDesc = socket(AF_INET , SOCK_STREAM , 0);

    if (socketDesc == -1) {
        return http_webserverError_socketCreationFailed;
    }

    i32 enable = 1;

    if (setsockopt(socketDesc, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {
        close(socketDesc);
        return http_webserverError_socketOptionSetFailed;
    }

    struct sockaddr_in server, client;
     
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port);

    if (bind(socketDesc,(struct sockaddr *)&server , sizeof(server)) < 0) {
        close(socketDesc);
        return http_webserverError_socketBindingFailed;
    }

    if (listen(socketDesc , maxWatingConnection)) {
        close(socketDesc);
        return http_webserverError_socketListenFailed;
    }

    SSL* serverCtx = SSL_CTX_new(SSLv3_server_method());
    if (!serverCtx) {
        close(socketDesc);
        return http_webserverError_sslServerContextCreationFailed;
    }


    mem_defineMakeStackArena(localArena, KILOBYTE(2));

    S8 certPathhNullPtr = str_copyNullTerminated(localArena, certPath);
    S8 privKeyPathNullPtr = str_copyNullTerminated(localArena, privKeyPath);

    SSL_CTX_use_certificate_file(serverCtx, (const char *) certPathhNullPtr.content, SSL_SERVER_RSA_CERT);
    SSL_CTX_use_PrivateKey_file(serverCtx, (const char *) privKeyPath.content, SSL_SERVER_RSA_KEY);

    if (!SSL_CTX_check_private_key(serverCtx)) {
        SSL_CTX_free(serverCtx);
        close(socketDesc);
        return http_webserverError_sslPrivateKeyLoadFailed;
    }

    char clientMessage[0xFFFF];
    while (1) {
        socklen_t c = sizeof(struct sockaddr_in);
        i32 clientSock = accept(socketDesc, (struct sockaddr *)&client, &c);
        if (clientSock < 0) {
            fprintf(stderr, "Accept failed\n");
            return -3;
        }
        SSL *client = SSL_new(serverCtx);
        if (!client) {
            fprintf(stderr, "Error creating SSL client\n");
            return -4;
        }
        SSL_set_fd(client, clientSock);
        if (SSL_accept(client)) {
            fprintf(stderr, "Cipher %s\n", tls_cipher_name(client));
            i32 readSize = 0;
            while ((readSize = SSL_read(client, clientMessage, sizeof(clientMessage))) >= 0) {
                fwrite(clientMessage, readSize, 1, stdout);
                const char msg[] = "HTTP/1.1 200 OK\r\nContent-length: 31\r\nContent-type: text/plain\r\n\r\nHello world from TLSe (TLS 1.3)";
                i32 messageSize = strlen(msg);
                if (SSL_write(client, msg, messageSize) != messageSize) {
                    fprintf(stderr, "Error in SSL write\n");
                } else {
                    fprintf(stdout, "SSL write success!\n");
                }
                break;
            }
        } else {
            fprintf(stderr, "Error in handshake\n");
        }
            
        
        SSL_shutdown(client);
#if OS_WIN
        shutdown(clientSock, SD_BOTH);
        closesocket(clientSock);
#else
        shutdown(clientSock, SHUT_RDWR);
        close(clientSock);
#endif
        SSL_free(client);
    }

    SSL_CTX_free(serverCtx);
    close(socketDesc);
    return http_webserverError_ok;
}

void g_ignoreSigpipe(void) {
    #if OS_WIN
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
#else
    signal(SIGPIPE, SIG_IGN);
#endif
}

int main(int argc , char *argv[]) {
    S8 certPath = s8("/Users/peterjakobs/pjako/cwebserver/certs/localhost.pem");
    S8 privKeyPath = s8("/Users/peterjakobs/pjako/cwebserver/certs/localhost-key.pem");

    {
        arg_Opt options[] = {
            {s8("certificate"), 'c', arg_optType_string, arg_flag_required | arg_flag_requiredValue, s8("certs/localhost.pem"), s8("Certificate file path."), s8("Path to the certificate file. (absolute path)")},
            {s8("privateKey"),  'p', arg_optType_string, arg_flag_required | arg_flag_requiredValue, s8("certs/localhost-key.pem"), s8("Private key file path."), s8("Path to the private key file. (absolute path)")},
        };

        arg_Ctx argCtx = arg_makeCtx(&options[0], countOf(options), argv, argc);

        bx errorFound = false;
        bx showHelp = false;

        for (i32 opt = 0; (opt = arg_nextOpt(&argCtx)) != arg_end;) {
            switch (opt) {
                case arg_error_duplicate:
                case arg_error_missingArg:
                case arg_error_missingValue:
                case arg_error_invalidValue: {
                    errorFound = true;
                    showHelp = true;
                } break;
                case 'c': {
                    certPath = argCtx.foundValue;
                } break;
                case 'p': {
                    privKeyPath = argCtx.foundValue;
                } break;
            }
        }

        if (certPath.size == 0 || privKeyPath.size == 0) {
            //S8 text = arg_createHelpText(&argCtx);
            return -1;
        }
    }

    g_ignoreSigpipe();

    http_WebServer webServer;
    if (http_webServerInit(&webServer, 2000, certPath, privKeyPath, 5) == http_webserverError_ok) {
        return -1; // Error while creating webserver
    }

    while (1) {
        // Listen to clients connecting to socket
        http_WebServerClientConnection clientConnection;
        http_webserverClientConnectionError result = http_webServerNextConnection(&webServer, &clientConnection);
        if (result == http_webserverClientConnectionError_ok) {
            u8 clientMessage[0xFFFF];

            i32 bytesLeft = http_webServerReadData(&clientConnection, clientMessage, sizeof(clientMessage));

            const u8 msg[] = "HTTP/1.1 200 OK\r\nContent-length: 31\r\nContent-type: text/plain\r\n\r\nHello world from TLSe (TLS 1.3)";
            i32 msgSize = sizeof(msg);
            bool succcessSendingData = !http_webServerWriteData(&clientConnection, msg, msgSize);

            http_webServerCloseClientConnection(&clientConnection);

        }

    }

    #if 0
    http_webserverError result = g_startWebserver(2000, certPath, privKeyPath, 5);

    if (result != http_webserverError_ok) {
        return -1;
    }
    #endif

    return 0;

#if 0

    int socket_desc , client_sock , read_size;
    // socklen_t c;
    struct sockaddr_in server , client;
    char client_message[0xFFFF];
    const char msg[] = "HTTP/1.1 200 OK\r\nContent-length: 31\r\nContent-type: text/plain\r\n\r\nHello world from TLSe (TLS 1.3)";

#if OS_WIN
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
#else
    signal(SIGPIPE, SIG_IGN);
#endif

    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1) {
        printf("Could not create socket");
        return 0;
    }
     
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(2000);
     
    int enable = 1;
    setsockopt(socket_desc, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));

    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0) {
        perror("bind failed. Error");
        return 1;
    }
     
    listen(socket_desc , 3);
     
    socklen_t c = sizeof(struct sockaddr_in);

    unsigned int size;

    SSL *server_ctx = SSL_CTX_new(SSLv3_server_method());
    if (!server_ctx) {
        fprintf(stderr, "Error creating server context\n");
        return -1;
    }
    SSL_CTX_use_certificate_file(server_ctx, (const char *) certPath.content, SSL_SERVER_RSA_CERT);
    SSL_CTX_use_PrivateKey_file(server_ctx, (const char *) privKeyPath.content, SSL_SERVER_RSA_KEY);

    if (!SSL_CTX_check_private_key(server_ctx)) {
        fprintf(stderr, "Private key not loaded\n");
        return -2;
    }

    while (1) {
        client_sock = accept(socket_desc, (struct sockaddr *)&client, &c);
        if (client_sock < 0) {
            fprintf(stderr, "Accept failed\n");
            return -3;
        }
        SSL *client = SSL_new(server_ctx);
        if (!client) {
            fprintf(stderr, "Error creating SSL client\n");
            return -4;
        }
        SSL_set_fd(client, client_sock);
        if (SSL_accept(client)) {
            fprintf(stderr, "Cipher %s\n", tls_cipher_name(client));
            while ((read_size = SSL_read(client, client_message, sizeof(client_message))) >= 0) {
                fwrite(client_message, read_size, 1, stdout);
                
                i32 messageSize = strlen(msg);
                if (SSL_write(client, msg, messageSize) != messageSize) {
                    fprintf(stderr, "Error in SSL write\n");
                } else {
                    fprintf(stdout, "SSL write success!\n");
                }
                break;
            }
        } else {
            fprintf(stderr, "Error in handshake\n");
        }
            
        
        SSL_shutdown(client);
#if OS_WIN
        shutdown(client_sock, SD_BOTH);
        closesocket(client_sock);
#else
        shutdown(client_sock, SHUT_RDWR);
        close(client_sock);
#endif
        SSL_free(client);
    }
    SSL_CTX_free(server_ctx);
    return 0;
#endif
}