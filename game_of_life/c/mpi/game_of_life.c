#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define MATRIX_LENGTH 2048
#define MATRIX_TOTAL_CELLS 2048 * 2048
#define GENERATIONS 2000
#define TARGET 0
#define TAG 0

int *grid, *newgrid;

void initialize_grid_and_newgrid() 
{
    grid = (int*) malloc (MATRIX_TOTAL_CELLS * sizeof(int));
    newgrid = (int*) malloc (MATRIX_TOTAL_CELLS * sizeof(int));

    grid[MATRIX_LENGTH * 01 + 02] = 1;
    grid[MATRIX_LENGTH * 02 + 03] = 1;
    grid[MATRIX_LENGTH * 03 + 01] = 1;
    grid[MATRIX_LENGTH * 03 + 02] = 1;
    grid[MATRIX_LENGTH * 03 + 03] = 1;
    grid[MATRIX_LENGTH * 10 + 31] = 1;
    grid[MATRIX_LENGTH * 10 + 32] = 1;
    grid[MATRIX_LENGTH * 11 + 30] = 1;
    grid[MATRIX_LENGTH * 11 + 31] = 1;
    grid[MATRIX_LENGTH * 12 + 31] = 1;
}

int get_num_cells_alive(int n_process, int process_id) 
{
    int portion_cells_alive = 0, total_cells_alive = 0;
    
    int proc_limit = MATRIX_LENGTH / n_process;

    for(int i = proc_limit * process_id; i < (process_id + 1) * proc_limit; i++) {
        for(int j = 0; j <  MATRIX_LENGTH; j++) {
            portion_cells_alive += grid[i * MATRIX_LENGTH + j];
        }
    }

    MPI_Reduce(&portion_cells_alive, &total_cells_alive, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD); 
    
    return total_cells_alive;
}

int get_neighbors_amount(int row, int column)
{
    int border_limit = MATRIX_LENGTH - 1;

    int row_up = row - 1;
    int column_left = column - 1;
    int row_down = (row + 1) % MATRIX_LENGTH;
    int column_right = (column + 1) % MATRIX_LENGTH;

    if(row_up % MATRIX_LENGTH < 0)
        row_up = border_limit;

    if(column_left % MATRIX_LENGTH < 0)
        column_left = border_limit;
    
    return grid[row_up   * MATRIX_LENGTH + column_left ] +
           grid[row_up   * MATRIX_LENGTH + column      ] + 
           grid[row_up   * MATRIX_LENGTH + column_right] + 
           grid[row      * MATRIX_LENGTH + column_left ] + 
           grid[row      * MATRIX_LENGTH + column_right] + 
           grid[row_down * MATRIX_LENGTH + column_left ] + 
           grid[row_down * MATRIX_LENGTH + column      ] + 
           grid[row_down * MATRIX_LENGTH + column_right];
}

int get_new_cell_status(int i, int j)
{
    int neighbors = get_neighbors_amount(i, j);

    int cell = grid[i * MATRIX_LENGTH + j];
    
    if(cell == 1 && neighbors != 2 && neighbors != 3)
        return 0;
    
    if(cell == 0 && neighbors == 3)
        return 1;

    return cell;
}

void update_new_grid(int n_process, int process_id) {
    for(int i = process_id; i < MATRIX_LENGTH; i += n_process) {
        for (int j = 0; j < MATRIX_LENGTH; j++) {
            newgrid[i * MATRIX_LENGTH + j] =  get_new_cell_status(i, j);
        }
    }
}

void master_update_grid(int n_process) {
    for(int i = 0; i < MATRIX_LENGTH; i += n_process)
        for(int j = 0; j < MATRIX_LENGTH; j++)
            grid[i * MATRIX_LENGTH + j] = newgrid[i * MATRIX_LENGTH + j];
}

void master_process_next_generation(int n_process, int process_id) {
    update_new_grid(n_process, process_id);
    
    master_update_grid(n_process);
    
    for(int process = 1; process < n_process; process++) {
        MPI_Recv(newgrid, MATRIX_TOTAL_CELLS, MPI_INT, process, TAG, MPI_COMM_WORLD , MPI_STATUS_IGNORE);

        for(int i = process; i < MATRIX_LENGTH; i += n_process)
            for(int j = 0; j < MATRIX_LENGTH; j++)
                grid[i * MATRIX_LENGTH + j] = newgrid[i * MATRIX_LENGTH + j];
    }

    MPI_Bcast(grid, MATRIX_TOTAL_CELLS, MPI_INT, 0, MPI_COMM_WORLD);
}

void other_processes_next_generation(int n_process, int process_id) {
    update_new_grid(n_process, process_id);

    MPI_Send(newgrid, MATRIX_TOTAL_CELLS, MPI_INT, TARGET, TAG, MPI_COMM_WORLD);
    
    MPI_Bcast(grid, MATRIX_TOTAL_CELLS, MPI_INT, 0, MPI_COMM_WORLD);
}

int main(int argc, char **argv) {
    int process_id, n_processes, cells_alive;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &n_processes);
    MPI_Comm_rank(MPI_COMM_WORLD, &process_id);

    initialize_grid_and_newgrid();

    time_t start, end;

    time(&start);

    for(int i = 1; i <= GENERATIONS; i++) {
        cells_alive = get_num_cells_alive(n_processes, process_id);

        if(process_id == 0) {
            printf("Geração %d: %d\n", i, cells_alive);
            master_process_next_generation(n_processes, process_id);
        }
        else {
            other_processes_next_generation(n_processes, process_id);
        }
    }

    time(&end);

    printf("Tempo de execução: %.2lf segundos.\n", difftime(end, start));

    MPI_Finalize();

    return 0;
}
