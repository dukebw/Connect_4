SDL_CFLAGS=`sdl2-config --cflags`
SDL_LDFLAGS=`sdl2-config --libs`

CFLAGS=$(SDL_CFLAGS) -c -Wall
LDFLAGS=$(SDL_LDFLAGS)

CC=g++

SOURCES= graphics.cpp\
			board.cpp\
			sdl2_connect4.cpp
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
	rm -f *.o $(BUILD)$(EXECUTABLE)

board.o: board.h
graphics.o: graphics.h board.h
sdl2_connect4.o: board.h graphics.h
