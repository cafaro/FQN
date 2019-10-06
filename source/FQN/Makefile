#*************************************************************************
#
# @file Makefile
# @author Catiuscia Melle
# @brief Fast Qn algorithm
#
# @note
# The macro 'CHECKS' enables the execution of exact computations
# to compare the values computed by FQN vs the exact ones.
#
# The macro 'TESTMODE' enables the input file buffering and disables the
# output to files of inliers and outliers (in file 'Outlierness.c').
#
#*************************************************************************

CC=icc -std=c11
CFLAGS=-I. -O3 -DTESTMODE -DCSV
#CFLAGS=-I. -Os -DCHECKS



EXECUTABLE=FQN
DEPS=FQN.c Options.c Outlierness.c XXSelect.c QuickSelect.c Sorter.c Debugger.c Timing.c

all: $(EXECUTABLE)


$(EXECUTABLE):$(DEPS)
	$(CC) -o $@ $(DEPS) $(CFLAGS)

clean:
	rm -rf $(EXECUTABLE)
	rm -f data/Inliers/*.* data/Outliers/*.*
