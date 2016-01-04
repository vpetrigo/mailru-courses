CXX = clang++
CXX_FLAGS = -std=c++11 -O2
SRC = find_div.cpp max_array.cpp bin_search.cpp bin_search.cpp
OBJ = find_div.o max_array.o bin_search.o bin_search.o
EXES = $(subst .o,.exe,$(OBJ))

.PHONY: all clean

all: $(EXES)

%.exe: %.o
	@$(CXX) -o $@ $<

%.o: %.cpp
	@$(CXX) -c $< $(CXX_FLAGS)

clean:
	@rm -f *.exe *.o
