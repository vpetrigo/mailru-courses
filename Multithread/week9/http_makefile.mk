CXX = clang++
CC = clang
SRC = http_server.cpp
PARSER_SRC = "..\..\..\http-parser\http_parser.c"
PARSER_OBJ = "..\..\..\http-parser\http_parser.o"
OBJ = http_server.o http_parser.o
LIB_PATH = "..\..\..\libuv"
LIBUV_INC_DIR = "..\..\..\libuv\include"
PARSER_INC_DIR = "..\..\..\http-parser"

all: $(OBJ)
	clang++ -o http_server.exe $(OBJ) -L $(LIB_PATH) -luv -lws2_32 -lpsapi -liphlpapi -luserenv

http_server.o: http_server.cpp
	$(CXX) -I $(PARSER_INC_DIR) -I $(LIBUV_INC_DIR) $< -c -std=c++14 -DDEBUG

http_parser.o:
	$(CC) -c $(PARSER_SRC) -std=c11

clean:
	rm -f *.o
