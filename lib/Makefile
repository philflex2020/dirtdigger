all: array sock vars servo thread digital

CFLAGS+=-D_MAIN

array: array.h array.cpp
	g++  -Wall -g -D_MAIN -o $@ array.cpp -lpthread -lrt

array.o: array.h array.cpp
	g++   -Wall -g -c array.cpp -lpthread -lrt

tests.o: tests.h tests.cpp
	g++   -Wall -g -c tests.cpp -lpthread -lrt

servo.o: servo.h servo.cpp
	g++  -Wall -g -c servo.cpp -lpthread -lrt

digital.o: digital.h digital.cpp
	g++  -Wall -g -c digital.cpp -lpthread -lrt

vars.o: vars.h vars.cpp
	g++  -Wall -g -c vars.cpp -lpthread -lrt

thread.o: thread.h thread.cpp
	g++  -Wall -g -c thread.cpp -lpthread -lrt


sock: sock.h sock.cpp array.o tests.o
	g++  -Wall -g -D_MAIN -o $@ sock.cpp array.o tests.o -lpthread -lrt

thread: thread.h thread.cpp array.o tests.o vars.o servo.o digital.o
	g++  -Wall -g -D_MAIN -o $@ thread.cpp array.o tests.o vars.o servo.o digital.o -lpthread -lrt

servo: servo.h servo.cpp array.o tests.o vars.o thread.o digital.o
	g++  -Wall -g -D_MAIN -o $@ servo.cpp array.o tests.o vars.o thread.o digital.o -lpthread -lrt

digital: digital.h digital.cpp array.o tests.o vars.o thread.o servo.o
	g++  -Wall -g -D_MAIN -o $@ digital.cpp array.o tests.o vars.o thread.o servo.o -lpthread -lrt

vars: vars.h vars.cpp array.o tests.o servo.o thread.o digital.o
	g++  -Wall -g -D_MAIN -o $@ vars.cpp array.o tests.o servo.o thread.o digital.o -lpthread -lrt

clean:
	rm -f *.o array sock vars servo thread digital
