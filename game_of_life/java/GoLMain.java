package gameOfLife;

import gameOfLife.GameOfLife;

public class GoLMain {
    public static void main(String[] args) throws InterruptedException {
        GameOfLife gameOfLife = new GameOfLife(4);

        gameOfLife.perform();
    }
}