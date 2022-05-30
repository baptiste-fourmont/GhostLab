PROG = gcc -Wall -Wextra -pedantic  -g -I include/
DIR_C = exec/c/
DIR_JAVA = exec/java/

all: client server
 
$(DIR_C)Tchat.o: src/Game/Tchat.c
	$(PROG) -c src/Game/Tchat.c -o $@
 
$(DIR_C)Multi.o: src/Game/Multi.c
	$(PROG) -c src/Game/Multi.c -lsocket -o $@
 
$(DIR_C)Game.o: src/Lobby/Game.c 
	$(PROG) -c src/Lobby/Game.c -o $@

$(DIR_C)GReceive.o: src/Game/GReceive.c
	$(PROG) -c src/Game/GReceive.c -o $@
 
$(DIR_C)GMessages.o: src/Game/GMessages.c
	$(PROG) -c src/Game/GMessages.c -o $@
 
$(DIR_C)LMessages.o: src/Lobby/LMessages.c
	$(PROG) -c src/Lobby/LMessages.c -o $@

$(DIR_C)LReceive.o: src/Lobby/LReceive.c
	$(PROG) -c src/Lobby/LReceive.c -o $@

$(DIR_C)User.o: src/Lobby/User.c
	$(PROG) -c src/Lobby/User.c -o $@

$(DIR_C)Util.o: src/Util.c
	$(PROG) -c src/Util.c -lsocket -o $@

$(DIR_C)Labyrinth.o: src/Game/Labyrinth.c 
	$(PROG) -c src/Game/Labyrinth.c -o $@
 
$(DIR_C)Server.o: src/Server.c
	$(PROG) -c src/Server.c -o $@

server: $(DIR_C)Util.o $(DIR_C)User.o $(DIR_C)GReceive.o $(DIR_C)GMessages.o $(DIR_C)LMessages.o $(DIR_C)LReceive.o $(DIR_C)Multi.o $(DIR_C)Tchat.o $(DIR_C)Game.o $(DIR_C)Labyrinth.o $(DIR_C)Server.o
	$(PROG) -pthread -o server $(DIR_C)*.o

client:
	javac -d $(DIR_JAVA) src/client/*.java

clean: 
	find . -type f -name '*.o' -delete
	find . -type f -name '*.class' -delete
	rm -f server

