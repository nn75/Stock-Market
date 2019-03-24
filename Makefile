all: client server
CCFLAGS=-std=c++11 -pthread  -pedantic -Wall -Werror -ggdb3 -lpqxx -lpq
XMLFLAGS=`pkg-config --cflags --libs libxml-2.0`
client: market_client.cpp
	g++ -o client market_client.cpp $(CCFLAGS) $(XMLFLAGS)

server: market_server.cpp
	g++ -o server market_server.cpp $(CCFLAGS) $(XMLFLAGS)

clean:
	rm -f *.o  *~ client
	rm -f *.o  *~ server
