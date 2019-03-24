all: client server
CCFLAGS=-std=c++11 -pthread  -pedantic -Wall -Werror -ggdb3 -lpqxx -lpq

client: market_client.cpp
	g++ -o client market_client.cpp $(CCFLAGS)

server: market_server.cpp
	g++ -o server market_server.cpp $(CCFLAGS)

clean:
	rm -f *.o  *~ client
	rm -f *.o  *~ server
