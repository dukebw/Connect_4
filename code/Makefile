# Build directory
BUILD = ../build/

# For making a directory.
# TODO(brendan): add change for Windows cmd
MKDIR_P = mkdir -p

.PHONY: directories

OUT_DIR = ../build/

directories: OUT_DIR

OUT_DIR:
	$(MKDIR_P) $(OUT_DIR)


# OBJS specifies which files to compile as part of the project
OBJS = sdl2_connect4.cpp

# CC specifies which compiler we're using
CC = g++

# COMPILER_FLAGS specifies the additional compilation options we're using
# -w suppresses all warnings
COMPILER_FLAGS = -w

# LINKER_FLAGS specifies the libraries we're linking against
LINKER_FLAGS = -lSDL2

# OBJ_NAME speicifies the name of our executable
OBJ_NAME = $(BUILD)sdl2_connect4

# This is the target that compiles our executable
all: directories $(OBJS)
	$(CC) $(OBJS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)
