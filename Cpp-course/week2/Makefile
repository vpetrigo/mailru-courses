CXX = clang++
CXX_FLAGS = -std=c++14
INCLUDE_DIR = C:\Users\Vladimir\Documents\Projects\boost_1_60_0

all: vect.o
	@$(CXX) -o vect.exe $<

vect.o: vect.cpp
	@$(CXX) -c $< $(CXX_FLAGS) -I$(INCLUDE_DIR)
