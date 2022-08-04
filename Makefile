systemInfo: 
	gcc -o systemInfo systemInfo.c
equiqInfo: 
	gcc -o equipInfo equipInfo.c
writeLogProcess: 
	gcc -o writeLogProcess writeLogProcess.c
build: 
	gcc -o server server.c; gcc -o client client.c

info: systemInfo equipInfo
	./systemInfo; ./equipInfo

log: writeLogProcess
	./writeLogProcess
