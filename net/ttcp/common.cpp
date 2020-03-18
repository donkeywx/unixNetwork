#include "common.h"
#include <string.h>
#include <boost/program_options.hpp>
namespace net
{

namespace po = boost::program_options;

bool parseCommandLine(int argc, char* argv[], Options* opt)
{
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "Help")
        ("port,p", po::value<uint16_t>(&opt->port)->default_value(5001), "TCP port")
        ("length,l", po::value<int>(&opt->length)->default_value(65536), "Buffer length")
        ("number,n", po::value<int>(&opt->number)->default_value(8192), "Number of buffers")
        ("trans,t",  po::value<std::string>(&opt->host), "Transmit")
        ("recv,r", "Receive")
        ("nodelay,D", "set TCP_NODELAY")
        ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    opt->transmit = vm.count("trans");
    opt->receive = vm.count("recv");
    opt->nodelay = vm.count("nodelay");
    if (vm.count("help"))
    {
        std::cout << desc << std::endl;
        return false;
    }

    if (opt->transmit == opt->receive)
    {
        printf("either -t or -r must be specified.\n");
        return false;
    }

    printf("port = %d\n", opt->port);
    if (opt->transmit)
    {
        printf("buffer length = %d\n", opt->length);
        printf("number of buffers = %d\n", opt->number);
    }
    else
    {
        printf("accepting...\n");
    }
    return true;
}

void printErr(const char* msg, int errnum)
{
    std::cout << "error messgae: " << msg
        << ". errno is: " << errno
        << ". strerror  is: " << strerror(errnum)
        << std::endl;
}
}