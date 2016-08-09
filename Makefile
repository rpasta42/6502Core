
CPP=g++
CFLAGS=-O3 -g -std=c++11
OBJ=m6502.o main.o ppu.o

all: test

test: $(OBJ) *.h
	$(CPP) $(CFLAGS) $(OBJ) -o test

%.o: %.cpp
	$(CPP) $(CFLAGS) -c $<

clean:
	rm -f *.o *~ test
