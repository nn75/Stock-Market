all: client server

client: market_client.cpp
	g++ -std=c++11 -o client market_client.cpp

server: market_server.cpp
	g++ -std=c++11 -o server market_server.cpp -lpqxx -lpq
