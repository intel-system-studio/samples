SHELL = /bin/sh

SRC = diskio.cpp
G++      = g++
CFLAGS  += -g -O3
LDFLAGS += -lrt

EXE = diskio

all:
	$(G++) $(CFLAGS) $(SRC) -o $(EXE) $(LDFLAGS)

clean:
	$(RM) $(EXE)

.PHONE: all clean

