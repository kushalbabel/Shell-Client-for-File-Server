all: server client

server: server-slow.cpp
	g++ -pthread server-slow.cpp -o server-slow

client: get-one-file.cpp
	g++ get-one-file.cpp -o get-one-file

clean:
	rm -f server-slow
	rm -f get-one-file