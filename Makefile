# Compiler and flags
CXX = g++
CXXFLAGS = -Iinclude -Wall -Wextra 

# Source files and output
SRCS := $(wildcard src/*.cpp)
OUT := bin/main

# Build target
all: $(OUT)

# Link all object files into the final binary
$(OUT): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $@ $^ 

# Run the program
run: $(OUT)
	./$(OUT)

# Clean up
clean:
	rm -f $(OUT)
