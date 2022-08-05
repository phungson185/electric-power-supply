build: 
	gcc -o server ./src/server.c; gcc -o client ./src/client.c

info: 
	cd info && make info

log:
	cd log && make log
