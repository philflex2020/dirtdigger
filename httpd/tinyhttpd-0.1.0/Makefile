all: httpd

#-lsocket ??
httpd: httpd.cpp motor.cpp
	g++ -W -Wall  -lpthread -o httpd motor.cpp httpd.cpp

clean:
	rm httpd
