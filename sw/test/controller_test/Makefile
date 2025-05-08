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
$(PROGS): %: %.cpp
<tab>$(CXX) $(CXXFLAGS) -o $@ $<

# Run any one of them:
#   make run PROG=debug_controller
#   make run PROG=list_input_devices
#   make run PROG=controller_test
run:
<tab>if [ -z "$(PROG)" ]; then \
<tab><tab>echo "Usage: make run PROG=<program>"; exit 1; \
<tab>fi
<tab>./$(PROG)

# Clean up all executables
clean:
<tab>rm -f $(PROGS)

# Handy list of targets
help:
<tab>@echo "Usage:"
<tab>@echo "  make             Build all programs"
<tab>@echo "  make <name>      Build one (debug_controller, list_input_devices, controller_test)"
<tab>@echo "  make run PROG=<name>  Compile if needed, then run that program"
<tab>@echo "  make clean       Remove all executables"