#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#include "GameFunctions.h"
#include "graphics.h"

int main(int argc, char ** argv )
{
	// INPUT ARGUMENTS //
    if (argc !=7)
    {
        printf("ERROR: unexpected number of input arguments! Should be: nrRows nrCols nrTime nrThreads graphics graphicsTerminal\n");
        exit(1);
    }
	const int nrRows = atoi(argv[1]);
	const int nrCols = atoi(argv[2]);
	const int nrTime = atoi(argv[3]);
	const int nrThreads = atoi(argv[4]);
	const int graphics = atoi(argv[5]);
	const int graphicsTerminal = atoi(argv[6]);
	if ((graphics != 0 && graphics != 1) || (graphicsTerminal != 0 && graphicsTerminal != 1))
	{
		printf("The arguments for graphics and graphicsTerminal should be either 0 for off or 1 for on\n");
		return 1;
	}
	
	// CREATING GRID //
	const int nrCells = nrRows*nrCols;
	cell_t * MyGrid = (cell_t *)malloc(nrCells*sizeof(cell_t));
	srand(time(0)); 			// Set seed for random function

	const double x_step = (double) 1.0/(nrCols+1); // defines the locations in the grid where the circles will be set
	const double y_step = (double) 1.0/(nrRows+1); // ...
	
	int * CounterSpecies; // Stores the number of cells taken by species A and B every time step 
	CounterSpecies = CreateGrid(MyGrid, nrRows, nrCols, x_step, y_step);

	// SETTING UP  GRAPHICS //	
	const float circleRadius=0.5*x_step, circleColorA=0, circleColorB = 0.6;
    const int windowWidth=800;
    const float L=1, W=1;

    if (graphics == 1) 
    {
        InitializeGraphics(argv[0],windowWidth,windowWidth);     
        SetCAxes(0,1); 
    }
	
    // SETTING UP THE OUTPUT FILE //
	FILE *MyFile = fopen("PopulationGrowth.txt", "w");
	fprintf(MyFile, "%d, %d\n", nrRows, nrCols);

	// SIMULATIONS OVER TIME //
	for (int t = 0; t<nrTime; t++)
	{
		printf("Time: %d\n", t);
		
		if (graphics == 1) 
        	GraphicSimulation(MyGrid, nrCells, L, W, circleRadius, circleColorA, circleColorB);

		if (graphicsTerminal == 1)
		{
			printGrid(MyGrid, nrRows, nrCols);
			printf("\n");
		}

		fprintf(MyFile, "%d, %d, %d\n", t, CounterSpecies[0], CounterSpecies[1]); // Outputting the nr of cells take by both species

		CounterSpecies = UpdateGrid(MyGrid, nrRows, nrCols, nrThreads); // Calculate values for the next time step.
	}

	// One more run, to write and show the final results
	printf("Time: %d\n", nrTime);
	fprintf(MyFile, "%d, %d, %d\n", nrTime, CounterSpecies[0], CounterSpecies[1]);
	fclose(MyFile);

	if (graphics == 1) 
        GraphicSimulation(MyGrid, nrCells, L, W, circleRadius, circleColorA, circleColorB);

	if (graphicsTerminal == 1)
		{
			printGrid(MyGrid, nrRows, nrCols);
			printf("\n");
		}

	// END OF PROGRAM //
	if (graphics == 1) 
    	while(!CheckForQuit()) {}

   	free(MyGrid);
	printf("Finished the game of life. \n");

	return 0;
}
