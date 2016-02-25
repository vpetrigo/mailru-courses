CXX = clang++
CC = clang
SRC = http_server.cpp
C_SRC = "D:\PVE\Projects\http-parser\"
OBJ = http_server.o http_parser.o

all: $(OBJ)
	clang++ -o http_server.exe $(OBJ) -L "D:\PVE\Projects\libuv" -luv -lws2_32 -lpsapi -liphlpapi -luserenv

http_server.o: http_server.cpp
	$(CXX) -I "D:\PVE\Projects\libuv\include" -I "D:\PVE\Projects\http-parser" $< -c -std=c++14 -DDEBUG

http_parser.o:
	$(CC) $(C_SRC)$< -c -std=c11

clean:
	rm -f *.o
