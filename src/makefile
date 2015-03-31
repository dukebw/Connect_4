# /********************************************
#  * A Connect Four Game                      *
#  * Designed by struct by_lightning{};       *
#  * Group 6                                  *
#  * Kuir Aguer, Brendan Duke, Jean Ferreira, *
#  * Zachariah Levine and Pranesh Satish      *
#  ********************************************/

SDL_CFLAGS=`sdl2-config --cflags`
SDL_LDFLAGS=`sdl2-config --libs`

CFLAGS=$(SDL_CFLAGS) -c -Wall -std=c++11 -O2
LDFLAGS=$(SDL_LDFLAGS)

CC=g++

SOURCES=connect4.cpp\
			graphics.cpp\
			board.cpp\
			sdl2_connect4.cpp\
			gameLogic.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=connect4
# Build directory
BUILD = ../build/

all: directories $(SOURCES) $(BUILD)$(EXECUTABLE)

$(BUILD)$(EXECUTABLE): $(OBJECTS)
	$(CC) -o $@ $(OBJECTS) $(LDFLAGS)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

MKDIR_P = mkdir -p

.PHONY: directories OUT_DIR clean spotless

OUT_DIR = ../build/

directories: OUT_DIR

OUT_DIR:
	$(MKDIR_P) $(OUT_DIR)

clean:
	rm -f *.o

spotless:
	rm -f *.o *~ .*~ $(BUILD)$(EXECUTABLE)

board.o: board.h
connect4.o: sdl2_connect4.h
graphics.o: graphics.h board.h linkedList.h
sdl2_connect4.o: sdl2_connect4.h gameLogic.h board.h graphics.h
gameLogic.o: gameLogic.h graphics.h linkedList.h board.h
