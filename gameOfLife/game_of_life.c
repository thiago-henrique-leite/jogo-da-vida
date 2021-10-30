#include "game_of_life.h"

int valid_parameters(int argc, char **argv, int *num_threads, int *run_test)
{
    if(argc > 1) 
    {
        *num_threads = atoi(argv[1]);
        *run_test = (argc == 3 && strcmp(argv[2], "-t") == 0) ? 1 : 0;

        return 1;
    }
    else 
    {
        printf("\nParâmetros inválidos! Execute da seguinte forma: ./executavel <num_threads>\n");
        printf("Passe a flag '-t' para executar o teste do programa: ./executavel <num_threads> -t\n\n");
        
        return 0;
    }
}

int** allocate_memory_to_matrix()
{
    int **matrix = (int**)malloc(sizeof(int*) * MATRIX_LENGTH);
    int i;

#pragma omp parallel private(i)
#pragma omp for
    for(i = 0; i < MATRIX_LENGTH; i++)
       matrix[i] = (int*)calloc(MATRIX_LENGTH, sizeof(int));
    
    return matrix;
}

void initialize_matrix(int **grid)
{
    int lin = 1, col = 1;
    
    grid[lin][col+1] = 1;
    grid[lin+1][col+2] = 1;
    grid[lin+2][col] = 1;
    grid[lin+2][col+1] = 1;
    grid[lin+2][col+2] = 1;

    lin = 10; col = 30;

    grid[lin][col+1] = 1;
    grid[lin][col+2] = 1;
    grid[lin+1][col] = 1;
    grid[lin+1][col+1] = 1;
    grid[lin+2][col+1] = 1;
}

int get_neighbors_amount(int **grid, int row, int column)
{
    int border_limit = MATRIX_LENGTH - 1;

    int row_up = row - 1;
    int row_down = row + 1;
    int column_left = column - 1;
    int column_right = column + 1;

    if(row_up < 0) row_up = border_limit;
    if(row_down > border_limit) row_down = 0;
    if(column_left < 0) column_left = border_limit;
    if(column_right > border_limit) column_right = 0;

    int neighbors = 0;

    if(grid[row_up][column_left] == 1) neighbors++;
    if(grid[row_up][column] == 1) neighbors++;
    if(grid[row_up][column_right] == 1) neighbors++;
    if(grid[row][column_right] == 1) neighbors++;
    if(grid[row][column_left] == 1) neighbors++;
    if(grid[row_down][column_left] == 1) neighbors++;
    if(grid[row_down][column] == 1) neighbors++;
    if(grid[row_down][column_right] == 1) neighbors++;

    return neighbors;
}

int get_new_cell_status(int **grid, int i, int j)
{
    int neighbors = get_neighbors_amount(grid, i, j);
    
    if(grid[i][j] == 1 && neighbors != 2 && neighbors != 3)
        return 0;
    
    if(grid[i][j] == 0 && neighbors == 3)
        return 1;

    return grid[i][j];
}

int get_num_cells_alive(int **grid)
{
    int i, j;
    int cells_alive = 0;

#pragma omp parallel private(i,j)
#pragma omp for
    for(i = 0; i < MATRIX_LENGTH; i++)
        for(j = 0; j < MATRIX_LENGTH; j++)
            if(grid[i][j]) cells_alive++;

    return cells_alive;
}

void next_generation(int **grid, int **new_grid)
{
    int i, j;

#pragma omp parallel private(i,j)
#pragma omp for
    for(i = 0; i < MATRIX_LENGTH; i++)
        for(j = 0; j < MATRIX_LENGTH; j++)
            new_grid[i][j] = get_new_cell_status(grid, i, j);

#pragma omp parallel private(i,j)
#pragma omp for
    for(i = 0; i < MATRIX_LENGTH; i++)
        for(j = 0; j < MATRIX_LENGTH; j++)
            grid[i][j] = new_grid[i][j];
}

void generate_next_generations_for_test(int **grid, int **new_grid)
{
    for(int i = 0; i < 5; i++) 
    {
        next_generation(grid, new_grid);
        print_matrix(grid);
        printf("Geração %d: %d\n", i+1, get_num_cells_alive(grid));
    }
}

void generate_next_generations(int **grid, int **new_grid)
{
    for(int i = 0; i < GENERATIONS; i++)
    {
        next_generation(grid, new_grid);

        printf("Geração %d: %d\n", i+1, get_num_cells_alive(grid));
    }
}

void print_matrix(int **grid)
{
    int i, j;

    for(i = 0; i < PRINT_LENGTH; i++)
    {
        for(j = 0; j < PRINT_LENGTH; j++)
            printf("%d  ", grid[i][j]);

        printf("\n");
    }

    printf("\n");
}

int main(int argc, char **argv) 
{
    int num_threads, run_test; 

    if(!valid_parameters(argc, argv, &num_threads, &run_test))
        exit(0);
    
    omp_set_num_threads(num_threads);

    int **grid = allocate_memory_to_matrix();
    int **new_grid = allocate_memory_to_matrix();

    initialize_matrix(grid);
    initialize_matrix(new_grid);

    time_t start, end;

    time(&start);
    
    if(run_test)
        generate_next_generations_for_test(grid, new_grid);
    else 
        generate_next_generations(grid, new_grid);
    
    time(&end);
    
    printf("Tempo de execução: %.2lf segundos.\n", difftime(end, start));

    return 0;
}