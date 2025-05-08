# Compiler and flags
CXX      := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -I.

# Sources and objects
SRCS   := main.cpp tetris.cpp
OBJS   := $(SRCS:.cpp=.o)

# Final executable
TARGET := tetris

.PHONY: all run clean

# Default target: build everything
all: $(TARGET)

# Link objects into the final executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS)

# Compile .cpp to .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Build and then run the game
run: all
	./$(TARGET)

# Remove build artifacts
clean:
	rm -f $(OBJS) $(TARGET)