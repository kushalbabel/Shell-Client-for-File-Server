all: server client

server: server-slow.cpp
	g++ -pthread server-slow.cpp -o server-slow

client: get-one-file.cpp get-one-file-sig.cpp
	g++ get-one-file.cpp -o get-one-file
	g++ get-one-file-sig.cpp -o get-one-file-sig

clean:
	rm -f server-slow
	rm -f get-one-file
	rm -f get-one-file-sig