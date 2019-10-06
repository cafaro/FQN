# NUNKESSER
#
# Massimo Cafaro, Italo Epicoco, Catiuscia Melle, Marco Pulimeno
# University of Salento, Lecce, Italy
#

CFLAGS=-O3 -std=c99 -DCSV
LDFLAGS=-lm
CC=icc
SOURCES=bostree.c qn.c qnonline.c main.c
EXECUTABLE=nunkesser

all:
	$(CC) $(CFLAGS) -o $(EXECUTABLE) $(SOURCES) $(LDFLAGS)

clean:
	rm -f *~ $(EXECUTABLE)
