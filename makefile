IN_DIR=./
INS:=$(wildcard $(IN_DIR)*.cpp)
OUT_DIR=_out/
OUT_BNAME=main.out
OUT=$(OUT_DIR)$(OUT_BNAME)

CC=g++
CFLAGS = -Wall
INCLUDE_PATH=-I/usr/include/irrlicht
LIBS=-lIrrlicht

.PHONY: all mkdir clean run

all: mkdir $(OUT)

run: all
	./$(OUT)

mkdir:
	mkdir -p "$(OUT_DIR)"

$(OUT): $(INS)
	$(CC) $(CFLAGS) $(INCLUDE_PATH) -o "$@" $^ $(LIBS)

clean:
	rm -rf "$(OUT_DIR)"
