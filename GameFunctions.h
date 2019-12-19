/* GameFunctions.h */
/* Header file. Contains declarations of all the functions used in the Game Of Life program */

// this structure stored all the information of one cell
typedef struct cell
{
    double positx;
    double posity;
    int status;
    int NrOfNeighboursA;
    int NrOfNeighboursB;
} cell_t;

void CountingNeighboursInnerCell(cell_t * MyGrid, int i, int j, int nrRows, int nrCols, int currentIndex);
void CountingNeighboursBorderCell(cell_t * MyGrid, int i, int j, int nrRows, int nrCols, int currentIndex);
int UpdateCell(cell_t * MyGrid, int currentIndex);
int * UpdateGrid(cell_t * MyGrid, int nrRows, int nrCols, int nrThreads);
void printGrid(cell_t *MyGrid, int nrRows, int nrCols);
int * CreateGrid (cell_t * MyGrid, int nrRows, int nrCols, double x_step, double y_step);
void GraphicSimulation(cell_t * MyGrid, int nrCells, float L, float W, float circleRadius, float circleColorA, float circleColorB);

