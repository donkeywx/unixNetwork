#ifndef __NET_TTCP_BLOCKING_H__
#define __NET_TTCP_BLOCKING_H__

#include <stdint.h>
#include <arpa/inet.h>
#include "common.h"

namespace net
{
int acceptOrDie(uint16_t port);
struct sockaddr_in resolveOrDie(const char* host, uint16_t port);
int write_n(int sockfd, const void* buf, int length);
int read_n(int sockfd, void* buf, int length);
void transmit(const Options& opt);
void receive(const Options& opt);
}

#endif