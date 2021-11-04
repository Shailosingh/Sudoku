/*
------------------------------------------------------------------------------------------
File:    SudokuSolver.c
Purpose: This file is the code to find all the solutions to a sudoku puzzle
==========================================================================================
Program Description:
This program was made to accept a sudoku board, find all the solutions to the board and 
then print all the boards to a solutions.txt file in the same folder as the executable.
To input your sudoku board, go to the main method and locate the declaration of "sudokuGrid"
Then freely put in the numbers into that array for your board.

WARNING: This program was made for mostly my personal use. It isn't very idiot proof, so 
don't mess with it too much. Maybe soon, I'll make a full graphical version of this program
so others can use it without worry. For now though, this is a quick and dirty solver.
------------------------------------------------------------------------------------------
Author:  Shailendra Singh
Version  2020-10-04
------------------------------------------------------------------------------------------
*/

//Includes
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <time.h>

//Constants & Globals---------------------------------------------------------------------------------
#define DIM 9
#define SIZE_OF_TABLE 81

//Define number of solutions
int count = 0;

//Define pointer to array that has all solutions. It is stored as a series of integers all in one array.
int* solutions = NULL;

//Functions------------------------------------------------------------------------------------------------
//Check if entry n is possible at coordinate
bool isPossible(int x, int y, int n, int* grid);

//Expand solution array size
int* expandSolutions();

//Fill current grid into solutions
int* fillSolutions(int* grid);

//Print out specific solution to terminal and text file
void printSolution(int* grid, FILE* solutionsFile);

//Print out ALL solutions to terminal and text file
void printAllSolutionsText(FILE* solutionFile);

//Solve sudoku grid
int* solveGrid(int* grid);
//----------------------------------------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    //Turns standard output buffering off
    setbuf(stdout, NULL);
    
    //Initialize board
    int sudokuGrid[SIZE_OF_TABLE] = {0,2,0,0,0,9,0,5,0,
                                     0,7,0,0,0,0,9,0,1,
                                     0,0,0,0,5,3,0,0,0,
                                     0,5,0,6,0,1,0,0,0,
                                     8,0,0,0,3,0,0,0,4,
                                     0,0,0,4,0,7,0,3,0,
                                     0,0,0,1,7,0,0,0,0,
                                     2,0,1,0,0,0,0,8,0,
                                     0,4,0,9,0,0,0,6,0};
    
    //Start clock
    clock_t beginTime = clock();

    //To store execution time of code
    double timeSpent = 0.0;

    //Solve grid
    solveGrid(sudokuGrid);

    //End clock
    clock_t endTime = clock();

    //Calculate elapsed time by finding difference (end - begin) and dividing the difference by CLOCKS_PER_SEC to convert to seconds
    timeSpent += (double)(endTime - beginTime) / CLOCKS_PER_SEC;

    printf("%d Solutions\n", count);
    printf("%f secs\n\n", timeSpent);

    //Create file to print all sudoku solutions to
    printf("Creating solution file...\n");
    FILE* solutionFile = fopen("solutions.txt", "w");

    fprintf(solutionFile, "%d Solutions\n", count);
    fprintf(solutionFile, "%f secs\n\n", timeSpent);

    //Write solutions
    printAllSolutionsText(solutionFile);
    printf("End!");

    //Clear memory
    free(solutions);

    //Close up files
    fclose(solutionFile);

    //Return 0
    return 0;
}

/*
--------------------------------------------------------------------
Description:
Recursive method that accepts grid array and will solve all the solutions
for the grid. This method will then after every full solution, record it 
into in the global solutions memory block.
=====================================================================
Parameters:
int* grid: The pointer to the sudoku grid to be solved
=====================================================================
Returns: 
Partially or fully solved sudoku grid.
--------------------------------------------------------------------
*/
int* solveGrid(int* grid)
{
    //Cycle through every row and column
    for (int y = 0; y < DIM; y++)
    {
        for (int x = 0; x < DIM; x++)
        {
            if(grid[DIM*y +x] == 0)
            {
                //Cycle through possibilities
                for (int n = 1; n <= DIM; n++)
                {
                    if(isPossible(x,y,n,grid))
                    {
                        grid[DIM*y + x] = n;
                        solveGrid(grid);
                        grid[DIM*y + x] = 0;
                    }
                }

                //Return the grid (All possibilities for current cell exhausted)
                return grid;
            }
            
        }
        
    }

    //Expand and fill solutions
    solutions = expandSolutions();
    solutions = fillSolutions(grid);

    //Return solution
    return grid;    
}

/*
--------------------------------------------------------------------
Description:
Check if a certain value n is possible when placed at the given coordinate
row y, column x of the grid. 
=====================================================================
Parameters:
int x: Column index
int y: Row index
int n: Number to be checked if it is possible to be put in grid.
int* grid: The pointer to the sudoku grid to be checked.
=====================================================================
Returns: 
    False: If it the grid is not possible
    True: If the grid is possible
--------------------------------------------------------------------
*/
bool isPossible(int x, int y, int n, int* grid)
{
    //Cycle through column searching for n
    for (int row = 0; row < DIM; row++)
    {
        if(grid[DIM*row + x] == n)
        {
            return false;
        }
    }

    //Cycle through row searching for n
    for (int col = 0; col < DIM; col++)
    {
        if(grid[DIM*y + col] == n)
        {
            return false;
        }
    }
    

    //Get the starting index for row and column (x,y)'s square
    int rowIndex = y -(y%3);
    int colIndex = x -(x%3);

    
    //Cycle throw square searching for n
    for (int row = 0; row < 3; row++)
    {
        for (int col = 0; col < 3; col++)
        {
            if(grid[DIM*(rowIndex+row) + (colIndex+col)] == n)
            {
                return false;
            }
        }
        
    }

    //If the runtime has gotten to this point, n can fit in (x,y)
    return true;
    
}

/*
--------------------------------------------------------------------
Description:
Takes the memory block that stores all the solutions and reallocates
more space for another solution
=====================================================================
Returns: 
The reallocated new memory space for the solutions.
--------------------------------------------------------------------
*/
int* expandSolutions()
{
    //Increment count of solutions
    count += 1;

    //Reallocate old solution space
    int newByteSize = sizeof(int)*SIZE_OF_TABLE*count;
    int* tempSolutions = (int*)realloc(solutions, newByteSize);
    solutions = tempSolutions;

    if(solutions == NULL)
    {
        printf("AHHHH NO MORE MEMORY\n");
        exit(0);
    }

    //Return new solution array
    return solutions;
}

/*
--------------------------------------------------------------------
Description:
Takes solved grid and puts it in solution space as one of the solutions
=====================================================================
Parameters:
int* grid: The pointer to the solved sudoku grid.
=====================================================================
Returns: 
The pointer to the memory space of all the solutions, with the new
solution placed in.
--------------------------------------------------------------------
*/
int* fillSolutions(int* grid)
{
    //Get starting index of new solution
    int start = SIZE_OF_TABLE*(count-1);

    //Insert grid into solutions
    for (int index = 0; index < SIZE_OF_TABLE; index++)
    {
        solutions[start + index] = grid[index];
    }

    //Return new solutions
    return solutions;
}

/*
--------------------------------------------------------------------
Description:
Prints sudoku solutions to both a file and terminal
=====================================================================
Parameters:
int* grid: Solved grid.
FILE* solutionsFile: The pointer to the file that all solutions
shall be written to.
=====================================================================
Returns: 
N/A
--------------------------------------------------------------------
*/
void printSolution(int* grid, FILE* solutionsFile)
{
    for (int row = 0; row < DIM; row++)
    {
        for (int col = 0; col < DIM; col++)
        {
            printf("%d ", grid[DIM*row + col]);
            fprintf(solutionsFile, "%d ", grid[DIM*row + col]);
        }
        
        printf("\n");
        fprintf(solutionsFile, "\n");
        
    }

    printf("\n\n");
    fprintf(solutionsFile, "\n\n");
    
    
}

/*
--------------------------------------------------------------------
Description:
Cycles through every single solution in memory and calls the printSolution
function for every solution.
=====================================================================
Parameters:
FILE* solutionsFile: The pointer to the file that all solutions
shall be written to.
=====================================================================
Returns: 
N/A
--------------------------------------------------------------------
*/
void printAllSolutionsText(FILE* solutionsFile)
{
    //Initialize variables
    int* currentGrid;

    //Cycle through every solution
    for (int index = 0; index < count; index++)
    {
        //Increment to next solution
        currentGrid = solutions + index*SIZE_OF_TABLE;

        //Print to terminal and to file
        printSolution(currentGrid, solutionsFile);
    }
}