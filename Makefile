CXX = g++-13
CXXFLAGS = --std=c++14 -Wall
STD = c++-14
OBJS = main.o cpu.o Chip8.o

.PHONY : all clean

all : $(OBJS)
	$(CXX) -o chips8 $(OBJS) -Lsrc/lib -lSDL2 $(CXXFLAGS)

main.o : src/main.cpp src/Chip8.h src/core/cpu.h
	$(CXX) -c -o $@ $< -Isrc/core -Isrc/include $(CXXFLAGS)

cpu.o : src/core/cpu.cpp src/core/cpu.h src/core/utils.h
	$(CXX) -c -o $@ $< $(CXXFLAGS)

Chip8.o : src/Chip8.cpp  src/Chip8.h src/core/cpu.h
	$(CXX) -c -o $@ $< -Isrc/core -Isrc/include $(CXXFLAGS)

clean :
	rm *.o