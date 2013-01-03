#compiles c files with gcc and c++ files with g++, one by one separatelly

CC = gcc
CXX = g++

CFLAGS = -Wall
CXXFLAGS = -Wall

LIBS = -lIrrlicht

IN_DIR=./
OUT_DIR=_out/
OUT_BNAME=main.out

OUT=$(OUT_DIR)$(OUT_BNAME)

.PHONY: all mkdir clean run

run: all
	./$(OUT)

all: mkdir $(OUT) run

mkdir:
	mkdir -p "$(OUT_DIR)"

#$(OUT_DIR)%$(OUT_EXT): $(IN_DIR)%.c
	#$(CC) $(CFLAGS) -o "$@" "$<" $(LIBS)

$(OUT): $(IN_DIR)main.cpp
	$(CXX) $(CXXFLAGS) -o "$@" "$<" $(LIBS)

clean:
	rm -rf "$(OUT_DIR)" "$(AUX_DIR)"
