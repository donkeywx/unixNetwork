#include "ttcp_blocking.h"

int main(int argc, char** argv)
{
    net::Options options;
    if (parseCommandLine(argc, argv, &options))
    {
        if (options.transmit)
        {
            // ./ttcp -p 8008 -l 1024000 -t 127.0.0.1 2.52152s, 3172.69Mib/
            transmit(options);  // ./ttcp -p 8008 -l 65535 -n 60000 -t 127.0.0.1
        }
        else if (options.receive)
        {
            receive(options);   // ./ttcp -p 8008 -r 127.0.0.1
        }
        else
        {
            std::cout << "param mismatch" << std::endl;
        }
    }
    // net::acceptOrDie(8008); //  netstat -anp查看状态 
    return 0;
}