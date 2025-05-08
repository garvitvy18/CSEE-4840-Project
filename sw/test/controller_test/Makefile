# Compiler settings
CXX      := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -O2

# Source files and corresponding targets
SRCS  := debug_controller.cpp list_input_devices.cpp controller_test.cpp
PROGS := debug_controller list_input_devices controller_test

.PHONY: all clean help run

# Default: build everything
all: $(PROGS)

# Rule to build each executable
%: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

# Run any one of them:
#   make run PROG=debug_controller
run:
	if [ -z "$(PROG)" ]; then \
		echo "Usage: make run PROG=<program>"; exit 1; \
	fi; \
	./$(PROG)

# Clean up all executables
clean:
	rm -f $(PROGS)

# Handy list of targets
help:
	@echo "Usage:"; \
	echo "  make             Build all programs"; \
	echo "  make <name>      Build one (debug_controller, list_input_devices, controller_test)"; \
	echo "  make run PROG=<name>  Compile if needed, then run that program"; \
	echo "  make clean       Remove all executables"