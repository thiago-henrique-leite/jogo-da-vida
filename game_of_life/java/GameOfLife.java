package gameOfLife;

public class GameOfLife {

    public static int MatrixLength = 2048;
    public static int PrintLength = 50;
    public static int Generations = 2000;

    private int[][] grid = new int[MatrixLength][MatrixLength];

    private int numThreads;

    private Thread[] arrayThreads;

    public GameOfLife(int numThreads)  {
        this.numThreads = numThreads;
        this.arrayThreads = new Thread[numThreads];
    }

    public void perform() throws InterruptedException {
        long startTime = System.nanoTime();

        initializeMatrix();

        System.out.println("Condição Inicial: " + String.valueOf(getNumCellsAlive()));

        for(int i=0; i < Generations; i++) {
            generateNextGenerations();

            if(i < 4 || i == Generations - 1) {
                printMatrix();
                System.out.println("Geração " + String.valueOf(i+1) + ": " + String.valueOf(getNumCellsAlive()));
            }

        }

        long stopTime = System.nanoTime();

        System.out.println("Tempo passado: " + String.valueOf((stopTime - startTime) / 1000000000) + "s");
    }

    public void initializeMatrix() {
        int i = 0, j = 0;
        int lin, col;

        for(i = 0; i < MatrixLength; i++)
            for(j = 0; j < MatrixLength; j++)
                grid[i][j] = 0;

        lin = 1; col = 1;

        this.grid[lin][col+1] = 1;
        this.grid[lin+1][col+2] = 1;
        this.grid[lin+2][col] = 1;
        this.grid[lin+2][col+1] = 1;
        this.grid[lin+2][col+2] = 1;

        lin = 10; col = 30;

        this.grid[lin][col+1] = 1;
        this.grid[lin][col+2] = 1;
        this.grid[lin+1][col] = 1;
        this.grid[lin+1][col+1] = 1;
        this.grid[lin+2][col+1] = 1;
    }

    public int getNumCellsAlive() {
        int i, j;
        int cellsAlive = 0;

        for(i = 0; i < MatrixLength; i++) {
            for (j = 0; j < MatrixLength; j++) {
                cellsAlive += grid[i][j];
            }
        }

        return cellsAlive;
    }

    public void generateNextGenerations() throws InterruptedException {
        int i, j;

        for (i = 0; i < numThreads; i++) {
            arrayThreads[i] = new Thread(new GoLNextGenerationThread(grid, i, numThreads));
            arrayThreads[i].start();
        }

        for (i = 0; i < numThreads; i++) {
            arrayThreads[i].join();
        }

        for(i = 0; i < MatrixLength; i++) {
            for(j = 0; j < MatrixLength; j++) {
                this.grid[i][j] = GoLNextGenerationThread.newGrid[i][j];
            }
        }
    }

    void printMatrix() {
        int i, j;

        for(i = 0; i < PrintLength; i++) {
            for(j = 0; j < PrintLength; j++) {
                System.out.printf("%d  ", this.grid[i][j]);
            }
            System.out.println("");
        }
        System.out.println("");
    }
}