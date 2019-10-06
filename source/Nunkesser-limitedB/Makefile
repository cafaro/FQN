# NUNKESSER_limitedB
#
# Massimo Cafaro, Italo Epicoco, Catiuscia Melle, Marco Pulimeno
# University of Salento, Lecce, Italy
#

CFLAGS=-std=c99 -DCSV -O3
LDFLAGS=-lm
CC=icc
SOURCES=bostree.c qn.c qnonline.c main.c
EXECUTABLE=nunkesser_limitedB

all:
	$(CC) $(CFLAGS) -o $(EXECUTABLE) $(SOURCES) $(LDFLAGS)

clean:
	rm -f *~ $(EXECUTABLE)
