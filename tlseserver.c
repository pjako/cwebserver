#include <stdio.h>
#include <string.h>    //strlen

#include "base.c"
#include "kit_tlseServer.h"
#include "kit_webServer.h"

static void g_handleSocket(int clientSocket) {

}

typedef struct SpmcQueueI32 {
    i32* items;
    u32 capacity;
    a32 head;
    a32 tail;
} SpmcQueueI32;

static void spmcI32_init(SpmcQueueI32* queue, u32 capacity, i32* items) {
    queue->items = items;
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

static i32 spmcI32_dequeue(SpmcQueueI32 *queue, i32* dequedValue) {
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

int main(int argc , char *argv[]) {
    S8 certPath = s8(PROJECT_ROOT "/certs/cert.pem");
    S8 privKeyPath = s8(PROJECT_ROOT "/certs/privatekey.pem");
    u32 coreCount = os_coreCount();

    {
        enum {
            optionCert = 'c',
            optionPrivKey = 'p',
        };
        arg_Opt options[] = {
            {s8("certificate"), optionCert, arg_optType_string, arg_flag_required | arg_flag_requiredValue, s8(PROJECT_ROOT "certs/cert.pem"), s8("Certificate file path."), s8("Path to the certificate file. (absolute path)")},
            {s8("privateKey"), optionPrivKey, arg_optType_string, arg_flag_required | arg_flag_requiredValue, s8(PROJECT_ROOT "certs/privatekey.pem"), s8("Private key file path."), s8("Path to the private key file. (absolute path)")},
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

        
        //struct sockaddr_in client;

        // socklen_t c = sizeof(struct sockaddr_in);
        i32 clientSock = accept(webServer.socketDesc, NULL, NULL);
        if (clientSock < 0) {
            continue;
        }

        sl_ClientConnectionError result = sl_NextConnection(&webServer, clientSock, &clientConnection);
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

