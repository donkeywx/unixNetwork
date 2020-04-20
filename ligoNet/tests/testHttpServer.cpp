#include "../libgoNet.h"

int main()
{
    donkey::HttpServer::Ptr httpServer = std::make_shared<donkey::HttpServer>();
    httpServer->listen("127.0.0.1", 8080);
    httpServer->start();

}