CXX = clang++
CXX_FLAGS = -std=c++11 -O2
SRC = deque.cpp
OBJ = $(subst .cpp,.o,$(SRC))
EXEC = $(subst .o,.exe,$(OBJ))

.PHONY: all clean

all: $(EXEC)

%.exe: %.o
	@$(CXX) -o $@ $<

%.o: %.cpp
	@$(CXX) -c $< $(CXX_FLAGS)

clean:
	@rm -f *.exe *.o
