package gameOfLife;

import gameOfLife.GameOfLife;

public class GoLNextGenerationThread implements Runnable {

    public static int[][] newGrid = new int[GameOfLife.MatrixLength][GameOfLife.MatrixLength];
    private int[][] grid = new int[GameOfLife.MatrixLength][GameOfLife.MatrixLength];
    private int threadNum;
    private int numThreads;

    public GoLNextGenerationThread(int[][] grid, int threadNum, int numThreads) {
        this.grid = grid;
        this.threadNum = threadNum;
        this.numThreads = numThreads;
    }

    @Override
    public void run() {
        int i, j;

        for(i = threadNum; i < GameOfLife.MatrixLength; i += numThreads)
            for(j = 0; j < GameOfLife.MatrixLength; j++)
                newGrid[i][j] = getNewCellStatus(grid, i, j);
    }

    public int getNewCellStatus(int grid[][], int i, int j) {
        int neighbors = getNeighborsAmount(grid, i, j);

        if(grid[i][j] == 1 && neighbors != 2 && neighbors != 3)
            return 0;

        if(grid[i][j] == 0 && neighbors == 3)
            return 1;

        return grid[i][j];
    }

    public int getNeighborsAmount(int grid[][], int row, int column) {
        int border_limit = GameOfLife.MatrixLength - 1;

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
}