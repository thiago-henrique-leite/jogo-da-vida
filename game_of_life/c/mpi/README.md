# Jogo da Vida implementado com MPI em C

Resultados obtidos:

Threads	|  Tempo(s)	|   SpeedUp	  |  Eficiência
-------------------------------------------------
1	      |   202	    |     1	      |     1
2	      |   158	    | 1,278481013	| 0,6392405063
4	      |   142	    | 1,422535211	| 0,3556338028
8	      |    -	    |     -       |	    -

### Para testar o código em execução, siga as etapas abaixo:

É necessário ter instalado o MPI na sua máquina. [Link](https://askubuntu.com/questions/1236553/mpich-installation) para instalação.

```bash
  git clone git@github.com:thiago-henrique-leite/jogo-da-vida.git

  cd jogo-da-vida/game_of_life/c/mpi

  mpicc game_of_life.c -o game_of_life.x

  mpirun -np 4 game_of_life.x
```