#ifndef KIT_WEBSERVER
#define KIT_WEBSERVER

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

typedef struct SpmcQueueI32 {
    i32* items;
    u32 capacity;
    a32 head;
    a32 tail;
} SpmcQueueI32;

static void spmcI32_init(SpmcQueueI32* queue, u32 capacity, i32* items) {
    queue->items = items;
    queue->capacity = capacity;
    queue->head = 0;
    queue->tail = 0;
}

static bool spmcI32_enqueue(SpmcQueueI32* queue, i32 item) {
    u32 currentTail = a32_load(&queue->tail);
    if ((currentTail + 1) % queue->capacity == a32_load(&queue->head)) return false;
    queue->items[currentTail] = item;
    a32_store(&queue->tail, (currentTail + 1) % queue->capacity);
    return true;
}

static bool spmcI32_dequeue(SpmcQueueI32 *queue, i32* dequedValue) {
    a32 currentHead;
    do {
        currentHead = a32_load(&queue->head);
        if (currentHead == a32_load(&queue->tail)) {
            return false;
        }
    } while (!a32_compareAndSwapFull(&queue->head, &currentHead, (currentHead + 1) % queue->capacity));
    *dequedValue = queue->items[currentHead];
    return true;
}

typedef struct ws_Context {
    SpmcQueueI32 socketQueue;
    struct {
        os_Thread* items;
        u32 count;
        u32 capacity;
    } workers;
    void (*handleSocket)(struct ws_Context* context, int clientSocket, void* userCtx);
    void* userCtx;
    i32 socketDesc;
    SSL* serverCtx;
    u32 activeConnections;
} ws_Context;

static i32 ws__worker(os_Thread* thread, void* arg) {
    ws_Context* context = (ws_Context*) arg;
    i32 continuedSleeping = 0;
    while (1) {
        i32 socketHandle = 0;
        if (!spmcI32_dequeue(&context->socketQueue, &socketHandle)) {
            // No socket connection to handle
            os_sleep(20 + 20 * continuedSleeping);

            if (continuedSleeping < 10) {
                continuedSleeping++;
            }
            continue;
        }
        continuedSleeping = 0;
        context->handleSocket(context, socketHandle, context->userCtx);
    }
}

typedef struct ws_Desc {
    i32 port;
    S8 certPath;
    S8 privKeyPath;
    u32 maxWatingConnection;
    u32 maxActiveConnection;
    u32 maxWorkers;
    os_Thread* workerMemory;
    i32* queueSocketMem;
    u32 maxQueuedConnection;
    bool adjustWorkersToLoad;
    void (*handleSocket)(struct ws_Context* context, int clientSocket, void* userCtx);
    u32 activeConnections;
    u64 workerStackSize;
} ws_Desc;

typedef enum ws_error {
    ws_error_ok = 0,
    ws_error_genericError,
    ws_error_socketCreationFailed,
    ws_error_socketOptionSetFailed,
    ws_error_socketBindingFailed,
    ws_error_socketListenFailed,
    ws_error_sslServerContextCreationFailed,
    ws_error_sslPrivateKeyLoadFailed
} ws_error;

static ws_error ws_init(ws_Context* webServer, ws_Desc* desc) {
    assert(webServer);
    int socketDesc = socket(AF_INET , SOCK_STREAM , 0);

    if (socketDesc == -1) {
        return ws_error_socketCreationFailed;
    }

    webServer->handleSocket = desc->handleSocket;

    char enable = 1;


    int flag = 1;
    // Allow reuse of addresses (SO_REUSEADDR)
    if (setsockopt(socketDesc, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag))) {
        close(socketDesc);
        return ws_error_socketOptionSetFailed;
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
    server.sin_port = htons(desc->port);

    if (bind(socketDesc,(struct sockaddr *)&server , sizeof(server)) < 0) {
        close(socketDesc);
        return ws_error_socketBindingFailed;
    }

    if (listen(socketDesc , desc->maxWatingConnection)) {
        close(socketDesc);
        return ws_error_socketListenFailed;
    }

    SSL* serverCtx = SSL_CTX_new(SSLv3_server_method());
    if (!serverCtx) {
        close(socketDesc);
        return ws_error_sslServerContextCreationFailed;
    }


    mem_defineMakeStackArena(localArena, KILOBYTE(2));

    S8 certPathhNullPtr = str_copyNullTerminated(localArena, desc->certPath);
    S8 privKeyPathNullPtr = str_copyNullTerminated(localArena, desc->privKeyPath);

    SSL_CTX_use_certificate_file(serverCtx, (const char *) certPathhNullPtr.content, SSL_SERVER_RSA_CERT);
    SSL_CTX_use_PrivateKey_file(serverCtx, (const char *) privKeyPathNullPtr.content, SSL_SERVER_RSA_KEY);

    if (!SSL_CTX_check_private_key(serverCtx)) {
        SSL_CTX_free(serverCtx);
        close(socketDesc);
        return ws_error_sslPrivateKeyLoadFailed;
    }

    webServer->socketDesc = socketDesc;
    webServer->serverCtx = serverCtx;

    webServer->workers.items = desc->workerMemory;
    webServer->workers.capacity = desc->maxWorkers;
    spmcI32_init(&webServer->socketQueue, desc->maxQueuedConnection, desc->queueSocketMem);

    u64 workerStackSize = desc->workerStackSize ? desc->workerStackSize : MEGABYTE(2);
    
    for (u32 idx = 0; idx < webServer->workers.capacity; idx++) {
        mem_defineMakeStackArena(nameArena, sizeof("ws_worker00000000000000000000000000000000000000000000000"));
        S8 name = str_join(nameArena, s8("ws_worker"), idx);
        os_threadCreate(&webServer->workers.items[0], ws__worker, webServer, workerStackSize, name);
    }
    webServer->workers.count = webServer->workers.capacity;

    return ws_error_ok;
}

static void ws_run(ws_Context* webServer) {
    i32 clientSock = accept(webServer->socketDesc, NULL, NULL);
    if (clientSock < 0) {
        return;
    }

    if (webServer->workers.count > 0) {
        spmcI32_enqueue(&webServer->socketQueue, clientSock);
    } else {
        webServer->handleSocket(webServer, clientSock, webServer->userCtx);
    }
}

typedef enum ws_clientConnectionError {
    ws_clientConnectionError_ok = 0,
    ws_clientConnectionError_handshakeFailed,
} ws_clientConnectionError;

typedef struct ws_ClientConnection {
    i32 clientSock;
    SSL *client;
} ws_ClientConnection;

static ws_clientConnectionError ws_nextConnection(ws_Context* context, i32 clientSock, ws_ClientConnection* connection) {
    char enable = 1;
    if (setsockopt(clientSock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {
        close(clientSock);
        return ws_clientConnectionError_handshakeFailed;
    }

    struct timeval timeout = {};
    timeout.tv_sec = 2;  // 10 seconds
    timeout.tv_usec = 0;

    // accept timeout
    /*
    if (setsockopt(socketDesc, SOL_SOCKET, SO_RCVBUF, (const char*)&timeout, sizeof(timeout)) < 0) {
        close(socketDesc);
        return sl_error_socketOptionSetFailed;
    }*/

    char buffer[3];
    int bytes = recv(clientSock, buffer, sizeof(buffer), MSG_PEEK);
    if (bytes >= 3 && !(buffer[0] == 0x16 && buffer[1] >= 0x03 && buffer[2] <= 0x03)) {
        S8 errorResponse = s8("HTTP/1.1 400 Bad Request\r\n\r\nUse HTTPS\r\n");
        send(clientSock, errorResponse.content, errorResponse.size, 0);
        close(clientSock);
        return ws_clientConnectionError_handshakeFailed;
    }

    // rec timeout
    if (setsockopt(clientSock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
        close(clientSock);
        return ws_clientConnectionError_handshakeFailed;
    }
    //fcntl(clientSock, F_SETFL, fcntl(clientSock, F_GETFL) | O_NONBLOCK);

    #if 0
    char buffer[8];
    int bytes = recv(clientSock, buffer, sizeof(buffer) - 1, MSG_PEEK);
    if (bytes > 0 && strncmp(buffer, "GET ", 4) == 0) {
        send(clientSock, "HTTP/1.1 400 Bad Request\r\n\r\nUse HTTPS\r\n", 37, 0);
        close(socketDesc);
        return;
    }
    #endif


    SSL *sslClient = SSL_new(context->serverCtx);
    if (!sslClient) {
        fprintf(stderr, "Error creating SSL client\n");
        return -4;
    }
    SSL_set_fd(sslClient, clientSock);
    if (!SSL_accept(sslClient)) {
        return ws_clientConnectionError_handshakeFailed;
    }
    connection->client = sslClient;
    connection->clientSock = clientSock;
    context->activeConnections++;
    return ws_clientConnectionError_ok;
}

static i32 ws_readData(ws_ClientConnection* connection, u8* clientMessage, i32 readSize) {
    return SSL_read(connection->client, clientMessage, readSize);
}

static bool ws_writeData(ws_ClientConnection* connection, const u8* clientMessage, i32 writeSize) {
    return SSL_write(connection->client, clientMessage, writeSize) != writeSize;
}

static void ws_closeClientConnection(ws_ClientConnection* connection) {
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

static void ws_shutdown(ws_Context* context) {
    for (u32 idx = 0; idx < context->workers.capacity; idx++) {
        os_threadShutdown(&context->workers.items[0]);
    }
    SSL_CTX_free(context->serverCtx);
    close(context->socketDesc);
}

static void ws_ignoreSigpipe(void) {
#if OS_WIN
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
#else
    signal(SIGPIPE, SIG_IGN);
#endif
}

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

#endif // KIT_WEBSERVER