#ifndef UDP_IO_H
#define UDP_IO_H

#include <stddef.h>
#include <stdint.h> 

int udp_io_init(const char *local_ip, int local_port, const char *remote_ip, int remote_port);

void udp_io_close(void);

int udp_io_receive(void *dst, size_t n);

int udp_io_send(const void *src, size_t n);

int udp_io_poll(void *dst, size_t n);

int udp_io_receive_timeout(void *dst, size_t n, uint32_t timeout_ms);

#endif