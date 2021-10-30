#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h> 

#define TAM 2048
#define GENERATIONS 2000
#define PRINTTAM 50
#define THREADS 4

void initiateMatrix(int **grid){
    int i = 0, j = 0;
    int lin;
    int col;

#pragma omp parallel private(i, j)

#pragma omp for
    for(i = 0; i < TAM; i++){
        for(j = 0; j < TAM; j++){
            grid[i][j] = 0;
        }
    }
    
    lin = 1; col = 1;
    
    grid[lin][col+1] = 1;
    grid[lin+1][col+2] = 1;
    grid[lin+2][col] = 1;
    grid[lin+2][col+1] = 1;
    grid[lin+2][col+2] = 1;

    lin =10; col = 30;
    grid[lin][col+1] = 1;
    grid[lin][col+2] = 1;
    grid[lin+1][col] = 1;
    grid[lin+1][col+1] = 1;
    grid[lin+2][col+1] = 1;
}

int getNeighbors(int **grid, int i, int j)
{
    int neighbors = 0, rowUp, rowDown, row = i, columnLeft, columnRight, column = j;

    if(i-1 < 0){
        rowUp = TAM - 1;
    }else{
        rowUp = i-1;
    }

    if(i+1 > TAM-1){
        rowDown = 0;
    }else{
        rowDown = i+1;
    }

    if(j-1 < 0){
        columnLeft = TAM - 1;
    }else{
        columnLeft = j-1;
    }

    if(j+1 > TAM - 1){
        columnRight = 0;
    }else{
        columnRight = j+1;
    }

    if(grid[rowUp][columnLeft] == 1) neighbors++;
    if(grid[rowUp][column] == 1) neighbors++;
    if(grid[rowUp][columnRight] == 1) neighbors++;
    if(grid[row][columnRight] == 1) neighbors++;
    if(grid[rowDown][columnRight] == 1) neighbors++;
    if(grid[rowDown][column] == 1) neighbors++;
    if(grid[rowDown][columnLeft] == 1) neighbors++;
    if(grid[row][columnLeft] == 1) neighbors++;

    return neighbors;

}


void printMatrix(int **grid){
    int i = 0, j = 0;

    for(i = 0; i < PRINTTAM; i++){
        for(j = 0; j < PRINTTAM; j++){
            printf("%d  ", grid[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

int newCellStatus(int **grid, int i, int j){
    int neighbors = getNeighbors(grid, i, j);
    
    if(grid[i][j] == 1 && neighbors != 2 && neighbors != 3){
        return 0;
    }
    
    if(grid[i][j] == 0 && (neighbors == 3 || neighbors == 6)){
        return 1;
    }

    return grid[i][j];
}

void changeGeneration(int **grid, int **newGrid){
    int i, j;

#pragma omp parallel private(i, j)

#pragma omp for
    for(i = 0; i < TAM; i++){
        for(j = 0; j < TAM; j++){
            newGrid[i][j] = newCellStatus(grid, i, j);
        }
    }

    for(i = 0; i < TAM; i++){
        for(j = 0; j < TAM; j++){
            grid[i][j] = newGrid[i][j];
        }
    }
}

int getCellsAlive(int **grid){
    int i, j;
    int cellsAlive = 0;
#pragma omp parallel private(i, j)

#pragma omp for
    for(i = 0; i < TAM; i++){
        for(j = 0; j < TAM; j++){
            if(grid[i][j] == 1) cellsAlive = cellsAlive + 1 ;
        }
    }

    return cellsAlive;
}

int main (){

    omp_set_num_threads(THREADS);
    time_t start, end;
    int **grid, **newGrid, i, j, cellsAlive = 0;
    grid = (int**) malloc (sizeof(int*) * TAM);
    newGrid = (int**) malloc (sizeof(int*) * TAM);

    for(i = 0; i < TAM; i++){
        grid[i] = (int*) malloc (sizeof(int) * TAM);
        newGrid[i] = (int*) malloc (sizeof(int) * TAM);
    }
    
    initiateMatrix(grid);
    initiateMatrix(newGrid);
    
    time(&start);
    for(i = 0; i < GENERATIONS-1; i++){
        changeGeneration(grid, newGrid);
        if(i < 5){
            printMatrix(grid);
        }
        printf("Geração %d: %d\n", i+1, getCellsAlive(grid));
    }
    changeGeneration(grid, newGrid);
    time(&end);
    printf("Geração %d: %d\n", GENERATIONS, getCellsAlive(grid));
    double dif = difftime (end,start); 
    printf ("Tempo passado: %.2lf segundos.\n", dif ); 
    return 0;
}