#ifndef __NET_COMMON_H__
#define __NET_COMMON_H__

#include <iostream>
#include <errno.h>
#include <string>

namespace net
{

struct Options
{
    uint16_t port;
    int length;
    int number;
    bool transmit, receive, nodelay;
    std::string host;
    Options()
        : port(0), length(0), number(0),
        transmit(false), receive(false), nodelay(false)
    {
    }
};
struct SessionMessage
{
    int32_t number;
    int32_t length;
} __attribute__ ((__packed__));
struct PayloadMessage
{
    int32_t length;
    char data[0];
};

bool parseCommandLine(int argc, char* argv[], Options* opt);

void printErr(const char* msg, int errnum);
}

#endif