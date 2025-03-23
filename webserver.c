#include <stdio.h>
#include <string.h>    //strlen
#ifdef _WIN32
    #pragma comment(lib, "Ws2_32.lib")
    #include <winsock2.h>
    #define socklen_t int
    #define sleep(x)    Sleep(x*1000)
#else
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <unistd.h>
#endif

#include <signal.h>

#include "base.c"

#include "tlse.h"


typedef struct html_Builder {
    Arena* arena;
    i32 depth;
    u64 startIdx;
} html_Builder;

static void html_builderStart(html_Builder* builder, Arena* arena) {
    builder->arena = arena;
    builder->depth = 0;
    builder->startIdx = mem_getArenaMemOffsetPos(arena);
    u64 startIdx = mem_arenaStartUnsafeRecord(arena) + 1;
    mem_arenaStartUnsafeRecord(arena);
}
static S8 html_builderEnd(html_Builder* builder) {
    u64 startIdx = builder->startIdx;
    Arena* arena = builder->arena;
    i32 depth = builder->depth;

    mem_arenaStopUnsafeRecord(arena);

    builder->arena = NULL;
    builder->depth = -1;
    builder->startIdx = -1;

    if (depth != 0) {
        return STR_EMPTY;
    }

    S8 result;
    result.content = &arena->memory[startIdx];
    result.size = (mem_getArenaMemOffsetPos(arena) - (startIdx - 1));
    return result;
}

typedef struct htm_HtmlProperties {
    S8 class;
    S8 id;
    S8 lang;
    S8 dir;
    struct { S8 key; S8 value; } props[5];
} htm_HtmlProperties;


void html_start(html_Builder* builder, htm_HtmlProperties props) {
    builder->depth += 1;
    str_join(builder->arena, s8("<html"));
    if (!str_isEmpty(props.class)) {
        str_join(builder->arena, s8(" class=\""), props.class, s8("\""));
    }
    if (!str_isEmpty(props.id)) {
        str_join(builder->arena, s8(" id=\""), props.id, s8("\""));
    }
    if (!str_isEmpty(props.lang)) {
        str_join(builder->arena, s8(" lang=\""), props.dir, s8("\""));
    }
    if (!str_isEmpty(props.dir)) {
        str_join(builder->arena, s8(" dir=\""), props.dir, s8("\""));
    }

    for (u32 idx = 0; idx < countOf(props.props); idx++) {
        if (!str_isEmpty(props.props[idx].key)) {
            if (str_isEmpty(props.props[idx].value)) {
                str_join(builder->arena, props.props[idx].key);
            } else {
                str_join(builder->arena, props.id, s8("=\""), props.props[idx].value, s8("\""));
            }
        }
    }

    str_join(builder->arena, s8(">\n"));
}
void html_end(html_Builder* builder) {
    builder->depth -= 1;
    str_join(builder->arena, s8("</html>"));

}

#define html_html(BUILDER, ...) html_start(BUILDER, (htm_HtmlProperties) {__VA_ARGS__}); for (int __i__ = 1; __i__ != 0; (__i__ = 0, html_end(BUILDER)))


typedef struct htm_HeadProps {
    S8 title;
    S8 class;
    S8 id;
    S8 lang;
    S8 dir;
    struct { S8 key; S8 value; } props[5];
} htm_HeadProps;


void html_headStart(html_Builder* builder, htm_HeadProps props) {
    builder->depth += 1;
    str_join(builder->arena, s8("<head"));
    if (!str_isEmpty(props.class)) {
        str_join(builder->arena, s8(" class=\""), props.class, s8("\""));
    }
    if (!str_isEmpty(props.id)) {
        str_join(builder->arena, s8(" id=\""), props.id, s8("\""));
    }
    if (!str_isEmpty(props.lang)) {
        str_join(builder->arena, s8(" lang=\""), props.dir, s8("\""));
    }
    if (!str_isEmpty(props.dir)) {
        str_join(builder->arena, s8(" dir=\""), props.dir, s8("\""));
    }

    for (u32 idx = 0; idx < countOf(props.props); idx++) {
        if (!str_isEmpty(props.props[idx].key)) {
            if (str_isEmpty(props.props[idx].value)) {
                str_join(builder->arena, props.props[idx].key);
            } else {
                str_join(builder->arena, props.id, s8("=\""), props.props[idx].value, s8("\""));
            }
        }
    }
    str_join(builder->arena, s8(">\n"));

    if (!str_isEmpty(props.title)) {
        str_join(builder->arena, s8("<title>"), props.title, s8("</title>\n"));
    }
}
void html_headEnd(html_Builder* builder) {
    builder->depth -= 1;
    str_join(builder->arena, s8("</head>"));
}

#define html_head(BUILDER, ...) html_headStart(BUILDER, (htm_HeadProps) {__VA_ARGS__}); for (int __i__ = 1; __i__ != 0; (__i__ = 0, html_headEnd(BUILDER)))

typedef struct htm_BodyProps {
    S8 title;
    S8 class;
    S8 id;
    S8 style;
    S8 lang;
    S8 dir;
    struct { S8 key; S8 value; } props[5];
} htm_BodyProps;

void html_bodyStart(html_Builder* builder, htm_BodyProps props) {
    builder->depth += 1;
    str_join(builder->arena, s8("<body"));
    if (!str_isEmpty(props.class)) {
        str_join(builder->arena, s8(" class=\""), props.class, s8("\""));
    }
    if (!str_isEmpty(props.id)) {
        str_join(builder->arena, s8(" id=\""), props.id, s8("\""));
    }
    if (!str_isEmpty(props.style)) {
        str_join(builder->arena, s8(" style=\""), props.style, s8("\""));
    }
    if (!str_isEmpty(props.lang)) {
        str_join(builder->arena, s8(" lang=\""), props.dir, s8("\""));
    }
    if (!str_isEmpty(props.dir)) {
        str_join(builder->arena, s8(" dir=\""), props.dir, s8("\""));
    }

    for (u32 idx = 0; idx < countOf(props.props); idx++) {
        if (!str_isEmpty(props.props[idx].key)) {
            if (str_isEmpty(props.props[idx].value)) {
                str_join(builder->arena, props.props[idx].key);
            } else {
                str_join(builder->arena, props.id, s8("=\""), props.props[idx].value, s8("\""));
            }
        }
    }
    str_join(builder->arena, s8(">\n"));
}
void html_bodyEnd(html_Builder* builder) {
    builder->depth -= 1;
    str_join(builder->arena, s8("</body>"));
}

#define html_body(BUILDER, ...) html_bodyStart(BUILDER, (htm_BodyProps) {__VA_ARGS__}); for (int __i__ = 1; __i__ != 0; (__i__ = 0, html_bodyEnd(BUILDER)))

typedef struct htm_DivProps {
    S8 title;
    S8 class;
    S8 id;
    S8 style;
    S8 lang;
    S8 dir;
    S8 onclick;
    S8 onmouseover;
    S8 onmouseout;
    bool hidden;
    struct { S8 key; S8 value; } props[5];
} htm_DivProps;

void html_divStart(html_Builder* builder, htm_DivProps props) {
    builder->depth += 1;
    str_join(builder->arena, s8("<div"));
    if (!str_isEmpty(props.class)) {
        str_join(builder->arena, s8(" class=\""), props.class, s8("\""));
    }
    if (!str_isEmpty(props.id)) {
        str_join(builder->arena, s8(" id=\""), props.id, s8("\""));
    }
    if (!str_isEmpty(props.title)) {
        str_join(builder->arena, s8(" title=\""), props.title, s8("\""));
    }
    if (!str_isEmpty(props.style)) {
        str_join(builder->arena, s8(" style=\""), props.style, s8("\""));
    }
    if (!str_isEmpty(props.lang)) {
        str_join(builder->arena, s8(" lang=\""), props.dir, s8("\""));
    }
    if (!str_isEmpty(props.dir)) {
        str_join(builder->arena, s8(" dir=\""), props.dir, s8("\""));
    }
    if (!str_isEmpty(props.onclick)) {
        str_join(builder->arena, s8(" onclick=\""), props.onclick, s8("\""));
    }
    if (!str_isEmpty(props.onmouseover)) {
        str_join(builder->arena, s8(" onmouseover=\""), props.onmouseover, s8("\""));
    }
    if (!str_isEmpty(props.onmouseout)) {
        str_join(builder->arena, s8(" onmouseout=\""), props.onmouseout, s8("\""));
    }

    for (u32 idx = 0; idx < countOf(props.props); idx++) {
        if (!str_isEmpty(props.props[idx].key)) {
            if (str_isEmpty(props.props[idx].value)) {
                str_join(builder->arena, props.props[idx].key);
            } else {
                str_join(builder->arena, props.id, s8("=\""), props.props[idx].value, s8("\""));
            }
        }
    }
    if (props.hidden) {
        str_join(builder->arena, s8(" hidden"));
    }
    str_join(builder->arena, s8(">\n"));
}
void html_divEnd(html_Builder* builder) {
    builder->depth -= 1;
    str_join(builder->arena, s8("</div>\n"));
}

#define html_div(BUILDER, ...) html_divStart(BUILDER, (htm_DivProps) {__VA_ARGS__}); for (int __i__ = 1; __i__ != 0; (__i__ = 0, html_divEnd(BUILDER)))


void html_text(html_Builder* builder, S8 text) {
    str_join(builder->arena, text);
}

void route_index(html_Builder* builder) {
    html_html(builder) {
        html_head(builder, .title = s8("Hello world"));
        html_body(builder) {
            html_div(builder) {
                html_text(builder, s8("Hello World!"));
            }
        }
    }
}


typedef enum http_methodType {
    http_methodType_unknown = 0,
    http_methodType_get = 1,
    http_methodType_post = 2,
} http_methodType;

typedef struct http_Header {
    S8 _headerData;
    i32 _headerStart;
    i32 _needle;
    
    http_methodType method;
    S8 methodText;
    S8 path;
    S8 protocol;

    S8 currentHeaderKey;
    S8 currentHeaderValue;
} http_Header;

static void http_headerInit(http_Header* header, S8 headerData) {
    i32 headerTextLastEnd = 0;
    i32 headerTextIdx = 0;
    S8 headerText[3] = {};
    mem_setZero(header, sizeOf(*header));
    mem_setZero(&headerText[0], sizeOf(headerText));
    i32 idx = 0;
    for (;idx < headerData.size; idx++) {
        // skip whitespace
        if (str_isSpacingChar(headerData.content[idx])) {
            headerTextLastEnd = idx + 1;
            continue;
        }
        if (str_isEndOfLineChar(headerData.content[idx])) {
            idx += 1;
            break;
        }
        // parse text
        for (;idx < headerData.size; idx++) {
            if (countOf(headerText) > headerTextIdx) {
                bx eol = str_isEndOfLineChar(headerData.content[idx]);
                if (eol || str_isSpacingChar(headerData.content[idx])) {
                    headerText[headerTextIdx].content = &headerData.content[headerTextLastEnd];
                    headerText[headerTextIdx].size = idx - headerTextLastEnd;
                    headerTextIdx += 1;
                    headerTextLastEnd = idx + 1;
                    if (eol) {
                        idx += 1;
                        goto endHeaderScan;
                    }
                    break;
                }
            }
        }
    }
endHeaderScan:
    header->_headerData = headerData;
    i32 headerStart = idx + 1;
    headerStart = minVal(headerStart, (header->_headerData.size - 1));
    header->_headerStart = headerStart;
    header->_needle = headerStart;

    // method
    if (headerText[0].content) {
        S8 methodText = headerText[0];
        header->methodText = methodText;
        header->method = http_methodType_unknown;
        if (str_isEqual(methodText, s8("GET"))) {
            header->method = http_methodType_get;
        } else if (str_isEqual(methodText, s8("POST"))) {
            header->method = http_methodType_post;
        }
        // Other http request methods seems to be never really used
    } else {
        header->method = http_methodType_unknown;
        header->methodText = s8("");
    }

    // path
    if (headerText[1].content) {
        S8 path = headerText[1];
        header->path = path;
    } else {
        header->path = s8("");
    }

    // protocol/version
    if (headerText[2].content) {
        S8 protocol = headerText[2];
        header->protocol = protocol;
    } else {
        header->protocol = s8("");
    }


    header->currentHeaderKey = s8("");
    header->currentHeaderValue = s8("");
}

static bx http_headerNext(http_Header* header) {
    // parse key
    i32 keyStart = header->_needle;
    S8 key = {};
    for (;header->_needle < header->_headerData.size; header->_needle++) {
        if (header->_headerData.content[header->_needle] == ':') {
            i32 keyEnd = header->_needle;
            key.size = maxVal(0, (keyEnd - keyStart));
            key.content = &header->_headerData.content[keyStart];
            break;
        }
    }
    if (!key.content) {
        return false;
    }
    i32 valueStart = header->_needle + 1;
    S8 value = {};
    for (;header->_needle < header->_headerData.size; header->_needle++) {
        if (str_isEndOfLineChar(header->_headerData.content[header->_needle])) {
            i32 valueEnd = header->_needle;
            value.size = maxVal(0, (valueEnd - valueStart));
            value.content = &header->_headerData.content[valueStart];
            break;
        }
    }
    if (!value.content) {
        return false;
    }
    
    header->currentHeaderKey = str_skipWhiteSpace(key);
    header->currentHeaderValue = str_skipWhiteSpace(value);

    
    for (;header->_needle < header->_headerData.size && str_isEndOfLineChar(header->_headerData.content[header->_needle]); header->_needle++);
    //header->_needle = minVal((header->_needle + 1), (header->_headerData.size - 1));

    return true;
}

static void http_headerReset(http_Header* header) {
    header->_needle = header->_headerStart;
}

typedef struct http_Route {
    S8 route;
    i32 shortRoute;
} http_Route;

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

    if (setsockopt(socketDesc, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {
        close(socketDesc);
        return sl_error_socketOptionSetFailed;
    }

    struct timeval timeout;
    timeout.tv_sec = 5;  // 10 seconds
    timeout.tv_usec = 0;

    // accept timeout
    if (setsockopt(socketDesc, SOL_SOCKET, SO_RCVBUF, (const char*)&timeout, sizeof(timeout)) < 0) {
        close(socketDesc);
        return sl_error_socketOptionSetFailed;
    }

    // rec timeout
    if (setsockopt(socketDesc, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
        close(socketDesc);
        return sl_error_socketOptionSetFailed;
    }

#if 0
    int flag = 1;

    // Disable Nagle's algorithm (TCP_NODELAY)
    if (setsockopt(socketDesc, IPPROTO_TCP, TCP_NODELAY, &flag, sizeof(flag))) {
        close(socketDesc);
        return sl_error_socketOptionSetFailed;
    }

    // Allow reuse of addresses (SO_REUSEADDR)
    if (setsockopt(socketDesc, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag))) {
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
static sl_ClientConnectionError sl_NextConnection(sl_Context* context, sl_ClientConnection* connection) {
    struct sockaddr_in client;

    socklen_t c = sizeof(struct sockaddr_in);
    i32 clientSock = accept(context->socketDesc, (struct sockaddr *)&client, &c);
    if (clientSock < 0) {
        fprintf(stderr, "Accept failed\n");
        return -3;
    }
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
    return sl_ClientConnectionError_ok;
}

static i32 sl_ReadData(sl_ClientConnection* connection, u8* clientMessage, i32 readSize) {
    return SSL_read(connection->client, clientMessage, readSize);
}

static bx sl_WriteData(sl_ClientConnection* connection, const u8* clientMessage, i32 writeSize) {
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

int main(int argc , char *argv[]) {
    S8 certPath = s8(PROJECT_ROOT "/certs/cert.pem");
    S8 privKeyPath = s8(PROJECT_ROOT "/certs/privatkey.pem");

    {
        enum {
            optionCert = 'c',
            optionPrivKey = 'p',
        };
        arg_Opt options[] = {
            {s8("certificate"), optionCert, arg_optType_string, arg_flag_required | arg_flag_requiredValue, s8("certs/localhost.pem"), s8("Certificate file path."), s8("Path to the certificate file. (absolute path)")},
            {s8("privateKey"), optionPrivKey, arg_optType_string, arg_flag_required | arg_flag_requiredValue, s8("certs/localhost-key.pem"), s8("Private key file path."), s8("Path to the private key file. (absolute path)")},
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
                case optionCert: {
                    certPath = argCtx.foundValue;
                } break;
                case optionPrivKey: {
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

    sl_Context webServer;
    if (sl_init(&webServer, 2000, certPath, privKeyPath, 5) != sl_error_ok) {
        return -1; // Error while creating webserver
    }

    enum {
        route_default = 0,
        route_about
    };

    http_Route routes[] = {
        {
            .route = s8("/"),
            .shortRoute = route_default
        },
        {
            .route = s8("/about"),
            .shortRoute = route_about
        }
    };



    while (1) {
        // Listen to clients connecting to socket
        sl_ClientConnection clientConnection;
        sl_ClientConnectionError result = sl_NextConnection(&webServer, &clientConnection);
        if (result == sl_ClientConnectionError_ok) {
            u8 clientMessage[0xFFFF];
            const char* clMsg = (const char*) &clientMessage[0];
            i32 size = sl_ReadData(&clientConnection, clientMessage, sizeOf(clientMessage));
            if (size <= 0) {
                S8 html403Error = s8(
                    "HTTP/1.1 403 Forbidden\r\n"
                    "Content-Type: text/plain\r\n"
                    "Connection: close\r\n"
                    "\r\n"
                    "403 Forbidden: This server only accepts secure connections (HTTPS).\n"
                );
                
                send(clientConnection.clientSock, html403Error.content, html403Error.size, 0);
                sl_CloseClientConnection(&clientConnection);
                continue;
            }
            fwrite(clientMessage, size, 1, stdout);

            S8 headerData;
            headerData.content = &clientMessage[0];
            headerData.size = minVal(size, sizeOf(clientMessage));

            http_Header header;
            // Init parses the first line of the header
            http_headerInit(&header, headerData);

            // figure out the route for the path in the header
            http_Route currentRoute = routes[route_default]; // default route
            for (i32 routeIdx = 0; routeIdx < countOf(routes); routeIdx++) {
                if (str_isEqual(header.path, routes[routeIdx].route)) {
                    currentRoute = routes[routeIdx];
                    break;
                }
            }

            mem_defineMakeStackArena(responseArena, KILOBYTE(2));

            S8 resposeText = s8("");
            switch (currentRoute.shortRoute) {
                case route_default: {
                    html_Builder builder;
                    html_builderStart(&builder, responseArena);
                    // build html
                    route_index(&builder);
                    resposeText = html_builderEnd(&builder);
                    #if 0
                    S8 host = s8("");
                    while (http_headerNext(&header)) {
                        if (str_isEqual(header.currentHeaderKey, str_lit("Host"))) {
                            host = header.currentHeaderValue;
                            break;
                        }
                    }
                    resposeText = str_fmt(responseArena, s8("Hello world!\nHost: {}"), host);
                    #endif
                } break;
                case route_about: {
                    resposeText = s8("About Me!");
                } break;
            }
            
            {
                mem_defineMakeStackArena(response, KILOBYTE(2));
                S8 headerText = str_fmt(response, s8("HTTP/1.1 200 OK\r\nContent-length: {}\r\nContent-type: text/html; charset=UTF-8\r\n\r\n"), resposeText.size);
                // write header
                sl_WriteData(&clientConnection, headerText.content, headerText.size);
            }

            // write content
            sl_WriteData(&clientConnection, resposeText.content, resposeText.size);

            sl_CloseClientConnection(&clientConnection);
        }

    }

    sl_Shutdown(&webServer);

    return 0;
}

#if 0

            S8 host = s8("");
            S8 origin = s8("");
            S8 contentType = s8("");
            S8 connection = s8("");
            S8 upgrade = s8("");
            i64 contentLength = 0;
            S8 websocketKey = s8("");
            S8 websocketExtension = s8("");
            while (http_headerNext(&header)) {
                S8 key = header.currentHeaderKey;
                S8 value = header.currentHeaderValue;
                if (host.size == 0 && str_isEqual(key, str_lit("Host"))) {
                    host = value;
                }
                    
                if (contentType.size == 0 && str_isEqual(key, str_lit("Content-Type"))) {
                    contentType = value;
                    continue;
                }
                if (contentLength == 0 && str_isEqual(key, str_lit("Content-Length"))) {
                    contentLength = str_parseS64(value);
                    continue;
                }
                if (origin.size == 0 && str_isEqual(key, str_lit("Origin"))) {
                    origin = value;
                    continue;
                }
                if (connection.size == 0 && str_isEqual(key, str_lit("Connection"))) {
                    connection = value;
                    continue;
                }
                if (upgrade.size == 0 && str_isEqual(key, str_lit("Upgrade"))) {
                    upgrade = value;
                    continue;
                }
                if (websocketKey.size == 0 && str_isEqual(key, str_lit("Sec-WebSocket-Key"))) {
                    websocketKey = value;
                    continue;
                }
                if (websocketExtension.size == 0 && str_isEqual(key, str_lit("Sec-WebSocket-Extensions"))) {
                    websocketExtension = value;
                    continue;
                }
            }
#endif

