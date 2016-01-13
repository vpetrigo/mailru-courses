CXX = clang++
CXX_FLAGS = -std=c++11
SRC = heap.cpp insertion_sort.cpp mergesort.cpp ksort.cpp
OBJ = $(subst .cpp,.o,$(SRC))
EXEC = $(subst .cpp,.exe,$(SRC))

.PHONY: all clean
.SECONDARY: $(OBJ)

all: $(EXEC)

%.exe: %.o
	@$(CXX) -o $@ $<

%.o: %.cpp
	@$(CXX) -c $< $(CXX_FLAGS)
	
clean:
	@rm -f *.o *.exe