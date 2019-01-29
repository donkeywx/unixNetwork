MyTinyhttp: MyTinyhttp.cpp
	g++ -g MyTinyhttp.cpp -o MyTinyhttp -lpthread

server: server.cpp
	g++ -g server.cpp -o server

client: client.cpp
	g++ -g client.cpp -o client
