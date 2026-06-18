#include "udp.h"
#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

static SOCKET sockfd = INVALID_SOCKET;
static struct sockaddr_in remote_addr;
static int wsa_initialized = 0;

int udp_io_init(const char *local_ip, int local_port, const char *remote_ip, int remote_port)
{
    if (!wsa_initialized) {
        WSADATA wsa_data;
        if (WSAStartup(MAKEWORD(2,2), &wsa_data) != 0) {
            fprintf(stderr, "WSAStartup failed\n");
            return -1;
        }
        wsa_initialized = 1;
    }

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == INVALID_SOCKET) {
        fprintf(stderr, "socket() failed: %d\n", WSAGetLastError());
        return -1;
    }

    struct sockaddr_in local_addr;
    memset(&local_addr, 0, sizeof(local_addr));
    local_addr.sin_family = AF_INET;
    local_addr.sin_port = htons((u_short)local_port);
    local_addr.sin_addr.s_addr = inet_addr(local_ip);

    if (bind(sockfd, (struct sockaddr*)&local_addr, sizeof(local_addr)) == SOCKET_ERROR) {
        fprintf(stderr, "bind() failed: %d\n", WSAGetLastError());
        closesocket(sockfd);
        return -1;
    }

    memset(&remote_addr, 0, sizeof(remote_addr));
    remote_addr.sin_family = AF_INET;
    remote_addr.sin_port = htons((u_short)remote_port);
    remote_addr.sin_addr.s_addr = inet_addr(remote_ip);

    // Non-blocking
    u_long mode = 1;
    ioctlsocket(sockfd, FIONBIO, &mode);

    printf("UDP IO: listening %s:%d, sending to %s:%d\n",
           local_ip, local_port, remote_ip, remote_port);
    return 0;
}

int udp_io_poll(void *dst, size_t n)
{
    int r = recvfrom(sockfd, (char*)dst, (int)n, 0, NULL, NULL);
    if (r == (int)n) return 1;
    if (r == SOCKET_ERROR) {
        int err = WSAGetLastError();
        if (err == WSAEWOULDBLOCK) return 0;   // no packet available
        if (err == WSAECONNRESET)  return 0;   // stale ICMP port-unreachable, ignore
        fprintf(stderr, "recvfrom error: %d\n", err);
        return -1;
    }
    fprintf(stderr, "udp_io_poll: size mismatch (%d vs %zu)\n", r, n);
    return -1;
}

int udp_io_receive(void *dst, size_t n)
{
    return udp_io_poll(dst, n) == 1 ? 0 : -1;
}

int udp_io_send(const void *src, size_t n)
{
    int s = sendto(sockfd, (const char*)src, (int)n, 0,
                    (struct sockaddr*)&remote_addr, sizeof(remote_addr));
    return (s == (int)n) ? 0 : -1;
}

void udp_io_close(void)
{
    if (sockfd != INVALID_SOCKET) {
        closesocket(sockfd);
        sockfd = INVALID_SOCKET;
    }
    if (wsa_initialized) {
        WSACleanup();
        wsa_initialized = 0;
    }
}

int udp_io_receive_timeout(void *dst, size_t n, uint32_t timeout_ms) {
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(sockfd, &fds);

    struct timeval tv = {
        .tv_sec  = timeout_ms / 1000,
        .tv_usec = (timeout_ms % 1000) * 1000
    };

    int ready = select(0, &fds, NULL, NULL, &tv);
    if (ready <= 0) return -1;   // timeout or error

    int r = recvfrom(sockfd, (char*)dst, (int)n, 0, NULL, NULL);
    return (r == (int)n) ? 0 : -1;
}