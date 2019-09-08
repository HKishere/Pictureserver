server:server.cc
	g++ $^ -g -o $@ -std=c++11 -L /usr/lib64/mysql -lpthread -lmysqlclient -ljsoncpp 

.PHONY:clean
clean:
	rm server
