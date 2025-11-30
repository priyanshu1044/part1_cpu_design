CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Iinclude

all: cpu assembler

cpu: src/main.cpp src/cpu.cpp src/memory.cpp include/cpu.h include/memory.h
	$(CXX) $(CXXFLAGS) -o cpu src/main.cpp src/cpu.cpp src/memory.cpp

assembler: src/assembler.cpp include/cpu.h
	$(CXX) $(CXXFLAGS) -o assembler src/assembler.cpp

clean:
	rm -f cpu assembler *.bin
