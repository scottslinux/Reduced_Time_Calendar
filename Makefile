# 						Makefile with Raylib integration

# Variable definitions
# Separated out the Raylib includes and libraries
# Makefile was updated to compile and link all .cpp and .h files in the directory-old makefile_improved(copy) is in this direcotry

# Compiler location
CXX = /usr/bin/g++

# Raylib installation directories
RAYLIBINCLUDES = /usr/local/include
RAYLIB_LIBRARY= /usr/local/lib 

# Compiler flags, include flags, linker flags, and library locations
CXXFLAGS = -Wall -Wextra -g -std=c++17 -I$(RAYLIBINCLUDES)
LDFLAGS = -L$(RAYLIB_LIBRARY) -lraylib -lbox2d -lm -ldl -lpthread -lrt -lX11 

# Project name and target executable
PROJECT_NAME= ReducedTime
TARGET = $(PROJECT_NAME)_bin  # Output binary name

# Automatically include all .cpp files in the current directory
SRCS = $(wildcard *.cpp)
OBJS = $(SRCS:.cpp=.o)

#-----------------------------
# Build the project
all: $(TARGET)

# Link objects to produce executable TARGET
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)

# Compile each .cpp file into its corresponding .o file without linking
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up object files and the executable
clean:
	rm -f $(OBJS) $(TARGET)

