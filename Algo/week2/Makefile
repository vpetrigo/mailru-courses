CXX = clang++
CXX_FLAGS = -std=c++11 -O2
HEADERS = Deque.hpp
SRC = deque.cpp bracket.cpp
OBJ = $(subst .cpp,.o,$(SRC))
EXEC = $(subst .o,.exe,$(OBJ))

.PHONY: all clean

all: $(EXEC)

%.exe: %.o
	@$(CXX) -o $@ $<

bracket.o: bracket.cpp $(HEADERS)
	@$(CXX) -c $< $(CXX_FLAGS)

%.o: %.cpp
	@$(CXX) -c $< $(CXX_FLAGS)

clean:
	@rm -f *.exe *.o
