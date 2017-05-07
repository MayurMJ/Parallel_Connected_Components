CC=mpicc
CFLAGS=-O0

all:
	$(CC) $(CFLAGS) -o Ran RanParCC.cpp 
	$(CC) $(CFLAGS) -o Det DetParCC.cpp 
clean:
	rm -rf Ran
	rm -rf Det
