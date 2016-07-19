TRG	= MathPic
SRC	= main.cpp
THREADS	= 4

CFLAG	+= -std=c++11 -Wall -g
LIBS	+= -lm -lpthread

include Makefile_generic.defs

run: $(TRG)
#	./$(TRG) $(THREADS) | ppmtobmp | bmptopnm | pnmtopng > MathPic.png
