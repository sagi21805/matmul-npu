# Define the compiler
CXX = g++

# Define the source file
SRC = example.cpp

# Define the include directories
INC_DIRS = -I/usr/local/include/rknpu -I./

# Define the libraries to link against
LIBS = -lrknnrt -fopenmp

# Define the compilation flags
CXXFLAGS = $(INC_DIRS) $(LIB_DIRS) $(LIBS) -Wall -Wextra -pedantic

# Define the rule to build the target
example: example.cpp
	$(CXX) example.cpp -o example $(CXXFLAGS)

test: test.cpp
	$(CXX) test.cpp -o test $(CXXFLAGS)

# Define the rule to clean up generated files
.PHONY: clean
clean:
	rm -f example test