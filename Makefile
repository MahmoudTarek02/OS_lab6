# Define the compiler
CXX = g++

# Define the executable name
TARGET = lab6

# Define the source file
SRC = lab6.cpp

# Define the run command
RUN_CMD = ./$(TARGET)

# Default target to build the program
all: $(TARGET)

# Rule to build the executable
$(TARGET): $(SRC)
	$(CXX) -o $(TARGET) $(SRC)

# Rule to run the program
run: $(TARGET)
	$(RUN_CMD)

# Rule to clean up generated files
clean:
	rm -f $(TARGET)
