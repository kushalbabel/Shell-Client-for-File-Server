all: server client shell

server: server-slow.cpp
	g++ -pthread server-slow.cpp -o server-slow

client: get-one-file.cpp get-one-file-sig.cpp
	g++ get-one-file.cpp -o get-one-file
	g++ get-one-file-sig.cpp -o get-one-file-sig

shell: client-shell.cpp
	g++ -pthread client-shell.cpp -o client-shell

clean:
	rm -f server-slow
	rm -f get-one-file
	rm -f get-one-file-sig
	rm -f client-shell
