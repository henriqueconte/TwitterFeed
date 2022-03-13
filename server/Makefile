rm -f server_tcp && server_tcp: $/server_tcp.cpp
	g++ -g -o server_tcp server_tcp.cpp $(shell find ../shared/src -name \*.cpp) $(shell find ./src -name \*.cpp) -lpthread 
