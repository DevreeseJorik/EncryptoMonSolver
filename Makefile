# Makefile

# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -std=c++17 -O2

# Target executable
TARGET = output/encryptomon.exe

# Temporary directory for object files
TEMP_DIR = output/temp

# Source files directory
SRC_DIR = src

# Object files
OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(TEMP_DIR)/%.o,$(wildcard $(SRC_DIR)/*.cpp))

# Linking
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) -lstdc++
	# rm -rf $(TEMP_DIR)  # Remove the temporary directory after linking

# Compilation
$(TEMP_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(TEMP_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean
clean:
	rm -rf $(TEMP_DIR)
