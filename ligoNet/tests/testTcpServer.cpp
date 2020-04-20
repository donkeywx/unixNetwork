#include "../libgoNet.h"

int main()
{
    donkey::TCPServer::Ptr tcpServer = std::make_shared<donkey::TCPServer>();
    tcpServer->listen("127.0.0.1", 8080);
    tcpServer->start();
    
    // donkey::Socket sock(donkey::Socket::Domain::IPv4, donkey::Socket::Type::TCP, 0);
    // sock.listen(30);
    // sleep(2);

    // socket(AF_INET, SOCK_STREAM, 0);
    // go[]
    // {
    //     socket(AF_INET, SOCK_STREAM, 0);
    //     sleep(2);
    //     std::cout << "1" << std::endl;
    //     co_yield;
    //     std::cout << "2" << std::endl;
    //     co_sched.Stop();
    // };
    // co_sched.Start();
}