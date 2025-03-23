#include <stdio.h>
#include <string.h>    //strlen

#include "base.c"
#include "kit_webServer.h"

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

static void g_handleSocket(ws_Context* context, int clientSocket, void* userCtx) {
    ws_ClientConnection clientConnection;
    ws_clientConnectionError result = ws_nextConnection(context, clientSocket, &clientConnection);

    if (result == ws_clientConnectionError_ok) {
        u8 clientMessage[0xFFFF];
        const char* clMsg = (const char*) &clientMessage[0];
        i32 size = ws_readData(&clientConnection, clientMessage, sizeOf(clientMessage));
        if (size <= 0) {
            S8 html403Error = s8(
                "HTTP/1.1 403 Forbidden\r\n"
                "Content-Type: text/plain\r\n"
                "Connection: close\r\n"
                "\r\n"
                "403 Forbidden: This server only accepts secure connections (HTTPS).\n"
            );
            
            send(clientConnection.clientSock, html403Error.content, html403Error.size, 0);
            ws_closeClientConnection(&clientConnection);
            return;
        }
        fwrite(clientMessage, size, 1, stdout);

        S8 headerData;
        headerData.content = &clientMessage[0];
        headerData.size = minVal(size, sizeOf(clientMessage));

        http_Header header;
        // Init parses the first line of the header
        http_headerInit(&header, headerData);

        enum {
            route_default = 0,
            route_about
        };

        static http_Route routes[] = {
            {
                .route = str_lit("/"),
                .shortRoute = route_default
            },
            {
                .route = str_lit("/about"),
                .shortRoute = route_about
            }
        };

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
            ws_writeData(&clientConnection, headerText.content, headerText.size);
        }

        // write content
        ws_writeData(&clientConnection, resposeText.content, resposeText.size);

        ws_closeClientConnection(&clientConnection);
    }
}

int main(int argc , char *argv[]) {
    S8 certPath = s8(PROJECT_ROOT "/certs/cert.pem");
    S8 privKeyPath = s8(PROJECT_ROOT "/certs/privatekey.pem");
    u32 workerCount = 0;
    bool maxCapacity = false;
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


    ws_ignoreSigpipe();

    ws_Context context;
    os_Thread threads[1280];
    i32 queueSockets[1280];
    workerCount = workerCount > 0 ? workerCount : (10 * os_coreCount());
    workerCount = minVal(workerCount, countOf(threads));
    ws_error result = ws_init(&context, &(ws_Desc) {
        .port = 2000,
        .workerMemory = &threads[0],
        .maxWorkers = workerCount,
        .certPath = certPath,
        .privKeyPath = privKeyPath,
        .queueSocketMem = &queueSockets[0],
        .maxQueuedConnection = countOf(queueSockets),
        .handleSocket = g_handleSocket
    });

    if (result != ws_error_ok) {
        return -1;
    }

    while (1) {
        ws_run(&context);
    }

    ws_shutdown(&context);

    return 0;
}