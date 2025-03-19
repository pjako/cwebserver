#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

#ifdef _WIN32
    #include <winsock2.h>
    #pragma comment (lib, "ws2_32.lib")
    #define socklen_t int
    #define sleep(x)    Sleep(x*1000)
#else
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <netdb.h>
    #include <unistd.h>
#endif
#include "tlse.h"
#include "base.c"

void error(char *msg) {
    perror(msg);
    exit(0);
}

/*
// Use this version with DTLS (preserving message boundary)
int send_pending_udp(int client_sock, struct TLSContext *context, struct sockaddr_in *clientaddr, socklen_t socket_len) {
    unsigned int out_buffer_len = 0;
    unsigned int offset = 0;
    int send_res = 0;
    const unsigned char *out_buffer;
    do {
        out_buffer = tls_get_message(context, &out_buffer_len, offset);
        if (out_buffer) {
            send_res += sendto(client_sock, out_buffer, out_buffer_len, 0, (struct sockaddr *)clientaddr, socket_len);
            offset += out_buffer_len;
        }
    } while (out_buffer);
    tls_buffer_clear(context);
    return send_res;
}
*/

int send_pending(int client_sock, struct TLSContext *context) {
    unsigned int out_buffer_len = 0;
    const unsigned char *out_buffer = tls_get_write_buffer(context, &out_buffer_len);
    unsigned int out_buffer_index = 0;
    int send_res = 0;
    while ((out_buffer) && (out_buffer_len > 0)) {
        int res = send(client_sock, (char *)&out_buffer[out_buffer_index], out_buffer_len, 0);
        if (res <= 0) {
            send_res = res;
            break;
        }
        out_buffer_len -= res;
        out_buffer_index += res;
    }
    tls_buffer_clear(context);
    return send_res;
}
#if 0
int validate_certificate(struct TLSContext *context, struct TLSCertificate **certificate_chain, int len) {
    int i;
    int err;
    if (certificate_chain) {
        for (i = 0; i < len; i++) {
            struct TLSCertificate *certificate = certificate_chain[i];
            // check validity date
            err = tls_certificate_is_valid(certificate);
            if (err)
                return err;
            // check certificate in certificate->bytes of length certificate->len
            // the certificate is in ASN.1 DER format
        }
    }
    // check if chain is valid
    err = tls_certificate_chain_is_valid(certificate_chain, len);
    if (err)
        return err;

    const char *sni = tls_sni(context);
    if ((len > 0) && (sni)) {
        err = tls_certificate_valid_subject(certificate_chain[0], sni);
        if (err)
            return err;
    }

    fprintf(stderr, "Certificate OK\n");

    //return certificate_expired;
    //return certificate_revoked;
    //return certificate_unknown;
    return no_error;
}
#else

typedef enum certificateState {
    certificateState_valid = 0,
    certificateState_noCertReserved = no_certificate_RESERVED,
    certificateState_bad = bad_certificate,
    certificateState_expired = certificate_expired,
    certificateState_revoked = certificate_revoked,
    certificateState_unknown = certificate_unknown,
} certificateState;

int validate_certificate(struct TLSContext *context, struct TLSCertificate **certificate_chain, int len) {
    int i;
    int err;
    
    if (certificate_chain) {
        for (i = 0; i < len; i++) {
            struct TLSCertificate *certificate = certificate_chain[i];
            
            // Check the validity of each certificate (e.g., expiration date, revoked status)
            err = tls_certificate_is_valid(certificate);
            if (err) {
                // If certificate is invalid (expired/revoked), return appropriate error
                return err;
            }
        }
    }
    
    // Validate the entire certificate chain
    err = tls_certificate_chain_is_valid(certificate_chain, len);
    if (err) {
        // If chain is invalid (invalid signature, incorrect chain order, etc.), return the error
        return err;
    }

    // Check the SNI (Server Name Indication) if available
    const char *sni = tls_sni(context);
    if ((len > 0) && (sni)) {
        err = tls_certificate_valid_subject(certificate_chain[0], sni);
        if (err) {
            // If the subject doesn't match the expected SNI, return an error
            return err;
        }
    }

    // If all checks pass, print success and return 'no_error'
    fprintf(stderr, "Certificate OK\n");
    return 0;
}
#endif

typedef struct HttpRequestContext {
    struct hostent *lastServer;
    i32 socket;
    u32 hostHash;
} HttpRequestContext;


typedef enum httpRequestStatus {
    httpRequestStatus_ok = 0,
    httpRequestStatus_resolveHostFailed = -1,
} httpRequestStatus;

httpRequestStatus sendRequest(HttpRequestContext* httpRequestContext, S8 url, S8 sendBody, S8* responseData) {
    Url urlParts = url_fromStr(url);
    S8 host = urlParts.host;
    u32 hostHash = str_hash32(host);

    // resolve host adress if it changed
    if (hostHash != httpRequestContext->hostHash || !httpRequestContext->lastServer) {
        if (httpRequestContext->socket > 0) {
            // close sockets first
#ifdef _WIN32
            closesocket(httpRequestContext->socket);
#else
            close(httpRequestContext->socket);
#endif
        }
        mem_defineMakeStackArena(localArena, KILOBYTE(2));
        S8 hostNullTerminated = str_copyNullTerminated(localArena, host);
        httpRequestContext->lastServer = gethostbyname( (const char*) hostNullTerminated.content);
        if (!httpRequestContext->lastServer) {
            httpRequestContext->hostHash = 0;
            return httpRequestStatus_resolveHostFailed;
        }
        httpRequestContext->hostHash = hostHash;
    }
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    memset((char *) &serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    memcpy((char *)&serv_addr.sin_addr.s_addr, (char *)server->h_addr, server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");

    struct TLSContext *context = tls_create_context(0, TLS_V13);
    // the next line is needed only if you want to serialize the connection context or kTLS is used
    tls_make_exportable(context, 1);
    
    tls_sni_set(context, (const char*) hostNullTerminated.content /*argv[1]*/);
    tls_client_connect(context);
    send_pending(sockfd, context);
    unsigned char client_message[0xFFFF];
    int read_size;
    int sent = 0;
    while ((read_size = recv(sockfd, client_message, sizeof(client_message) , 0)) > 0) {
        tls_consume_stream(context, client_message, read_size, validate_certificate);
        send_pending(sockfd, context);
        if (tls_established(context) == 1) {
            if (!sent) {
                S8 request = str_fmt(localArena, s8("GET {} HTTP/1.1\r\nConnection: close\r\n\r\n\0"), url.path);
                //const char *request = "GET / HTTP/1.1\r\nConnection: close\r\n\r\n";
                // try kTLS (kernel TLS implementation in linux >= 4.13)
                // note that you can use send on a ktls socket
                // recv must be handled by TLSe
                if (!tls_make_ktls(context, sockfd)) {
                    // call send as on regular TCP sockets
                    // TLS record layer is handled by the kernel
                    send(sockfd, request.content, request.size, 0);
                } else {
                    tls_write(context, (unsigned char *)request.content, request.size);
                    send_pending(sockfd, context);
                }
                sent = 1;
            }

            unsigned char read_buffer[0xFFFF];
            int read_size = tls_read(context, read_buffer, 0xFFFF - 1);
            if (read_size > 0)
                fwrite(read_buffer, read_size, 1, stdout);
        }
    }
}

int main(int argc, char *argv[]) {

#ifdef _WIN32
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
#else
    signal(SIGPIPE, SIG_IGN);
#endif

    HttpRequestContext httpRequestContext = {};
    
    u8 _responseData[KILOBYTE(2)];
    S8 response;
    response.content = &_responseData[0];
    response.size = sizeOf(_responseData);
    httpRequestStatus status = sendRequest(&httpRequestContext, s8("https://localhost:2000/"), s8(""), &response);
    return 0;
}

#if 0
int main_(int argc, char *argv[]) {
    int sockfd, portno, n;
    //tls_print_certificate("testcert/server.certificate");
    //tls_print_certificate("000.certificate");
    //exit(0);
    struct sockaddr_in serv_addr;
    struct hostent *server;

    mem_defineMakeStackArena(localArena, KILOBYTE(2));

    Url url = url_fromStr(s8("https://www.google.com:443/webhp"));
    S8 hostNullTerminated = str_copyNullTerminated(localArena, url.host);
    if (url.error != url_errorCode_ok) {
        return -1;
    }
    char buffer[256];
    char *ref_argv[] = {"", "google.com", "443"};
    if (argc < 3) {
        argv = ref_argv;
       //fprintf(stderr,"usage %s hostname port\n", argv[0]);
       //exit(0);
    }
#ifdef _WIN32
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
#else
    signal(SIGPIPE, SIG_IGN);
#endif
    portno = url.port; // atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd)
    if (sockfd < 0) 
        error("ERROR opening socket");
    server = gethostbyname( (const char*) hostNullTerminated.content);// gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    memset((char *) &serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    memcpy((char *)&serv_addr.sin_addr.s_addr, (char *)server->h_addr, server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");

    struct TLSContext *context = tls_create_context(0, TLS_V13);
    // the next line is needed only if you want to serialize the connection context or kTLS is used
    tls_make_exportable(context, 1);
    
    tls_sni_set(context, (const char*) hostNullTerminated.content /*argv[1]*/);
    tls_client_connect(context);
    send_pending(sockfd, context);
    unsigned char client_message[0xFFFF];
    int read_size;
    int sent = 0;
    while ((read_size = recv(sockfd, client_message, sizeof(client_message) , 0)) > 0) {
        tls_consume_stream(context, client_message, read_size, validate_certificate);
        send_pending(sockfd, context);
        if (tls_established(context) == 1) {
            if (!sent) {
                S8 request = str_fmt(localArena, s8("GET {} HTTP/1.1\r\nConnection: close\r\n\r\n\0"), url.path);
                //const char *request = "GET / HTTP/1.1\r\nConnection: close\r\n\r\n";
                // try kTLS (kernel TLS implementation in linux >= 4.13)
                // note that you can use send on a ktls socket
                // recv must be handled by TLSe
                if (!tls_make_ktls(context, sockfd)) {
                    // call send as on regular TCP sockets
                    // TLS record layer is handled by the kernel
                    send(sockfd, request.content, request.size, 0);
                } else {
                    tls_write(context, (unsigned char *)request.content, request.size);
                    send_pending(sockfd, context);
                }
                sent = 1;
            }

            unsigned char read_buffer[0xFFFF];
            int read_size = tls_read(context, read_buffer, 0xFFFF - 1);
            if (read_size > 0)
                fwrite(read_buffer, read_size, 1, stdout);
        }
    }
    fflush(stdout);
    return 0;
}
#endif