CXX = g++-13
CXXFLAGS = --std=c++17 -Wall
OBJS = main.o cpu.o Chip8.o emu_window_sdl.o InputMapping.o

.PHONY : all clean

all : $(OBJS)
	$(CXX) -o build/chips8 $(OBJS) -L$(SDL_LIBS) -lSDL2 $(CXXFLAGS)

main.o : src/main.cpp src/Chip8.h src/emu_window_sdl.h
	$(CXX) -c -o $@ $< -Isrc/core -I$(SDL_INCLUDE) $(CXXFLAGS)

cpu.o : src/core/cpu.cpp src/core/cpu.h src/core/utils.h
	$(CXX) -c -o $@ $< $(CXXFLAGS)

Chip8.o : src/Chip8.cpp  src/Chip8.h src/core/cpu.h src/core/utils.h
	$(CXX) -c -o $@ $< -Isrc/core -I$(SDL_INCLUDE) $(CXXFLAGS)

emu_window_sdl.o : src/emu_window_sdl.cpp  src/Chip8.h src/emu_window_sdl.h src/InputMapping.h
	$(CXX) -c -o $@ $< -Isrc/core -I$(SDL_INCLUDE) $(CXXFLAGS)

InputMapping.o : src/InputMapping.cpp src/InputMapping.h
	$(CXX) -c -o $@ $< -I$(SDL_INCLUDE) $(CXXFLAGS)

clean :
	rm *.o