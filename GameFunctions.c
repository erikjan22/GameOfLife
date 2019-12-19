/* FameFunctions.c */
/* Functions file. Contains the code of all the functions used in the Game Of Life program*/

#include <stdio.h>
#include <stdlib.h>
#include "graphics.h"
#include <omp.h>
#include "GameFunctions.h"

#define WaitingTime 500000  // The time in microseconds (1 s = 1000 000 ms) the simulation waits between steps before execution continues
#define probabilityLifeA 10
#define probabilityLifeB 10

int * CreateGrid (cell_t * MyGrid, int nrRows, int nrCols, double x_step, double y_step)
{
	static int CounterSpecies[2];
	int CounterSpeciesA = 0;
	int CounterSpeciesB = 0;
	double positx = 0.0;
	for (int i=0; i<nrRows; i++)
	{
		double posity = 0.0;
		positx = positx + x_step;
		for(int j=0; j<nrCols; j++)
		{
			// Initialize the current cell in the grid
			int A_exists = 0;	// This is a checker to set to value 1 is a the cell is set to species A
			MyGrid[i*nrCols + j].status = 0;	// default value is that nothing exists in this cell.
			posity = posity + y_step;
			MyGrid[i*nrCols + j].positx = positx;
			MyGrid[i*nrCols + j].posity = posity;

			// Check whether we will build species A (1) or species B (2) in this cell of the grid
			float existenceA = 100*((float)rand()/RAND_MAX);	// Will give random floating point value from 0.0 to 100.0
			float existenceB = 100*((float)rand()/RAND_MAX);	// Will give random floating point value from 0.0 to 100.0
			
			if (existenceA > (100-probabilityLifeA)) 
			{
				// Setting cell as species A
				MyGrid[i*nrCols + j].status = 1;
				A_exists = 1;
				CounterSpeciesA++;
			}

			if (existenceB > (100-probabilityLifeB))
			{
				if (A_exists == 1 && existenceB > existenceA)	// Using the checker to see if cell was already set to species A
				{
					// Changing cell from species A to speciesB
					MyGrid[i*nrCols + j].status = 2;
					CounterSpeciesA--;
					CounterSpeciesB++;
				}
				else
				{
					// Setting cell as species B
					MyGrid[i*nrCols + j].status = 2;
					CounterSpeciesB++;
				}
			}
		}
	}
	CounterSpecies[0] = CounterSpeciesA;
	CounterSpecies[1] = CounterSpeciesB;
	return CounterSpecies;
}

// Calculate the number of neighbours for a specified border cell
void CountingNeighboursBorderCell(cell_t * MyGrid, int i, int j, int nrRows, int nrCols, int currentIndex)
{
	MyGrid[currentIndex].NrOfNeighboursA = 0; 
	MyGrid[currentIndex].NrOfNeighboursB = 0;

	// Start the counting process.
	// Look at all the cells around the current cell, but be sure to avoid curtain ones if we'rea t the edge of the grid
	for (int row = -1; row <= 1; row++)
		for (int col = -1; col <= 1; col++)
		{
			int row_index = i+row;
			int col_index = j+col;
			// Making sure that the cell we're gonna look at isn't outside of the grid
			if ((row_index >= 0 && row_index < nrRows && col_index >=0 && col_index < nrCols)
				&& !(row_index==i && col_index==j))
			{
				if (MyGrid[row_index*nrCols+col_index].status == 1) 	
					MyGrid[currentIndex].NrOfNeighboursA++;
				else if (MyGrid[row_index*nrCols+col_index].status == 2) 	
					MyGrid[currentIndex].NrOfNeighboursB++;
			}
		}
	return;
}


// Calculate the number of neighbours for a specified inner cell
void CountingNeighboursInnerCell(cell_t * MyGrid, int i, int j, int nrRows, int nrCols, int currentIndex)
{
	MyGrid[currentIndex].NrOfNeighboursA = 0; 
	MyGrid[currentIndex].NrOfNeighboursB = 0;
	// Start the counting process.
	
	int indexes[8];
	indexes[0] = (i-1)*nrCols+j-1;
	indexes[1] = (i-1)*nrCols+j;
	indexes[2] = (i-1)*nrCols+j+1;
	indexes[3] = i*nrCols+j-1;
	indexes[4] = i*nrCols+j+1;
	indexes[5] = (i+1)*nrCols+j-1;
	indexes[6] = (i+1)*nrCols+j;
	indexes[7] = (i+1)*nrCols+j+1;

	for (int c = 0; c < 8; c ++)
		{
			int ComparisonCellIndex = indexes[c];
			if (MyGrid[ComparisonCellIndex].status == 1) 	
				MyGrid[currentIndex].NrOfNeighboursA++;
			else if (MyGrid[ComparisonCellIndex].status == 2) 	
				MyGrid[currentIndex].NrOfNeighboursB++;
		}
	return;
}


// Update the status of a specified cell in the grid based on it's number of neighbours
// Return the status (species type) which the specified cell is set to
int UpdateCell(cell_t * MyGrid, int currentIndex)
{
	int NrOfNeighbours = MyGrid[currentIndex].NrOfNeighboursA + MyGrid[currentIndex].NrOfNeighboursB; // Total number of neighbours
	if (MyGrid[currentIndex].status == 0)
	{
		// Species A is the stronger competitor.
		if (MyGrid[currentIndex].NrOfNeighboursA == 3)	MyGrid[currentIndex].status = 1;
		else if (MyGrid[currentIndex].NrOfNeighboursB == 3)	MyGrid[currentIndex].status = 2;
	}
	else if (MyGrid[currentIndex].status == 1)
	{
		if (MyGrid[currentIndex].NrOfNeighboursA < 2 || NrOfNeighbours > 3)
			MyGrid[currentIndex].status = 0;
	}
	else
	{
		if (MyGrid[currentIndex].NrOfNeighboursB < 2 || NrOfNeighbours > 3)
			MyGrid[currentIndex].status = 0;
	}
	return MyGrid[currentIndex].status;
}


// Calculate the number of neigbours for each cell in the grid and update the status of the cell accordingly
// Return the number of cells occupied by species A and species B
int * UpdateGrid(cell_t * MyGrid, int nrRows, int nrCols, int nrThreads)
{
	int currentIndex, SpeciesType, i, j, c, r;
	static int CounterSpecies[2];
	int CounterSpeciesA = 0;
	int CounterSpeciesB = 0;
	
	// First do the outer two columns of cells
	int outer_cols[2] = {0, nrCols-1};
	for (i = 0; i<nrRows; i++)
	{
		for (c = 0; c < 2; c++)
		{
			j = outer_cols[c];
			currentIndex = i*nrCols + j;
			CountingNeighboursBorderCell(MyGrid, i, j, nrRows, nrCols, currentIndex);
			SpeciesType = UpdateCell(MyGrid, currentIndex);
			
			if (SpeciesType == 1)
				CounterSpeciesA++;
			if (SpeciesType == 2)
				CounterSpeciesB++;
		}
	}

	// Then do the upper and bottom rows of cells (without the outer column values)
	int outer_rows[2] = {0, nrRows-1};
	for (j = 1; j<(nrCols-1); j++)
	{
		for (r = 0; r < 2; r++)
		{
			i = outer_rows[r];
			currentIndex = i*nrCols + j;
			CountingNeighboursBorderCell(MyGrid, i, j, nrRows, nrCols, currentIndex);
			SpeciesType = UpdateCell(MyGrid, currentIndex);
			
			if (SpeciesType == 1)
				CounterSpeciesA++;
			if (SpeciesType == 2)
				CounterSpeciesB++;
		}
	}

	// Then do the inner cells
	// Initialization for OpenMP
	omp_lock_t SpeciesA_lock, SpeciesB_lock;
    omp_init_lock(&SpeciesA_lock);
    omp_init_lock(&SpeciesB_lock);

    omp_set_num_threads(nrThreads);    
    #pragma omp parallel for private(j, currentIndex, SpeciesType) schedule(static, 1)
	for (i=1; i<(nrRows-1); i++)
	{
		for(j=1; j<(nrCols-1); j++)
		{
			currentIndex = i*nrCols + j;
			CountingNeighboursInnerCell(MyGrid, i, j, nrRows, nrCols, currentIndex);
			
			SpeciesType = UpdateCell(MyGrid, currentIndex);
			
			if (SpeciesType == 1)
			{
				omp_set_lock(&SpeciesA_lock);
				CounterSpeciesA++;
				omp_unset_lock(&SpeciesA_lock);
			}
			if (SpeciesType == 2)
			{
				omp_set_lock(&SpeciesB_lock);
				CounterSpeciesB++;
				omp_unset_lock(&SpeciesB_lock);
			}
		}
	}

	CounterSpecies[0] = CounterSpeciesA;
	CounterSpecies[1] = CounterSpeciesB;
	return CounterSpecies;
}


// Print the grid layout, including the occupation of the cells, in the terminal
void printGrid(cell_t * MyGrid, int nrRows, int nrCols)
{
	for (int i=0; i<nrRows; i++)
		{
			printf("|");
			for(int j=0; j<nrCols; j++)
			{
				if (MyGrid[i*nrCols + j].status == 0)
					printf("   |");
				else
					printf(" %d |", MyGrid[i*nrCols + j].status);
			}
			printf("\n");
		}
	return;
}		


// Use the graphics.c package to simulate the grid in a seperate screen
void GraphicSimulation(cell_t * MyGrid, int nrCells, float L, float W, float circleRadius, float circleColorA, float circleColorB)
{
	for (int c = 0; c<nrCells; c++)
    {
        if (MyGrid[c].status == 1)
           	DrawCircle(MyGrid[c].positx, MyGrid[c].posity, L, W, circleRadius, circleColorA);
        if (MyGrid[c].status == 2)
           	DrawCircle(MyGrid[c].positx, MyGrid[c].posity, L, W, circleRadius, circleColorB);
    }
    usleep(WaitingTime); // Amount of time we wait before the next step in time
    Refresh();
    ClearScreen();
    return;
}
