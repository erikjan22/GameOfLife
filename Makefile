
CFLAGS= -O3 -funroll-loops -Wall -fopenmp
INCLUDES=-I/opt/X11/include
LDFLAGS=-L/opt/X11/lib -lX11 -lm

game: GameOfLife.o GameFunctions.o graphics.o
	gcc $(CFLAGS) -o game GameOfLife.o GameFunctions.o graphics.o $(LDFLAGS) 

GameOfLife.o: GameOfLife.c
	gcc  $(CFLAGS) -c GameOfLife.c -lm 

GameFunctions.o: GameFunctions.c
	gcc  $(CFLAGS) -c GameFunctions.c -lm 

graphics.o: graphics.c graphics.h
	gcc $(CFLAGS) $(INCLUDES) -c graphics.c  

clean:
	rm -f ./game *.o PopulationGrowth.txt
