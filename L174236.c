#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>

/* dimensions of the screen */

#define BOARD_WIDTH	79
#define BOARD_HEIGHT	24

int noOfThreads=24;	//no of threadss

struct threadArguments{
	int** board;
	int height;
};

struct Barrier{
	int count;
	sem_t mutex;
	sem_t turnstile;
	sem_t turnstile2;
};
void initialise(struct Barrier *barrier)
{
	barrier->count = 0;
	sem_init(&barrier->mutex,0,1);
	sem_init(&barrier->turnstile,0,0);
	sem_init(&barrier->turnstile2 ,0,0);
	
	
}

void* phase1(struct Barrier* barrier )
{
	sem_wait(&barrier->mutex);
	(barrier->count++);
	
	
	if(barrier->count==noOfThreads)
	{
			int i=0;
		while(i< noOfThreads){
			sem_post(&barrier->turnstile);
				i++;
		}		
	}
	sem_post(&barrier->mutex);
	sem_wait(&barrier->turnstile);
}


void* phase2 (struct Barrier* barrier )
{
	sem_wait(&barrier->mutex); 
	barrier->count -= 1;
	if(barrier->count == 0)
	{
		int i=0;
		while(i< noOfThreads)
		{
				sem_post(&barrier->turnstile2);
				i++;
		}
	}
	 sem_post(&barrier->mutex);
	 sem_wait(&barrier->turnstile2);
}

struct Barrier barrierUsed;

//void* wait ()
//{
	//phase1 (barrierUsed);
	//phase2 (barrierUsed);
//}

/* set everthing to zero */
void initialize_board (int **board) {
	int	i, j;

	for (i=0; i<BOARD_WIDTH; i++) for (j=0; j<BOARD_HEIGHT; j++) 
		board[i][j] = 0;
}

/* add to a width index, wrapping around like a cylinder */

int xadd (int i, int a) {
	i += a;
	while (i < 0) i += BOARD_WIDTH;
	while (i >= BOARD_WIDTH) i -= BOARD_WIDTH;
	return i;
}

/* add to a height index, wrapping around */

int yadd (int i, int a) {
	i += a;
	while (i < 0) i += BOARD_HEIGHT;
	while (i >= BOARD_HEIGHT) i -= BOARD_HEIGHT;
	return i;
}

/* return the number of on cells adjacent to the i,j cell */

int adjacent_to (int** board, int i, int j) {
	int	k, l, count;

	count = 0;

	/* go around the cell */

	for (k=-1; k<=1; k++) for (l=-1; l<=1; l++)

		/* only count if at least one of k,l isn't zero */

		if (k || l)
			if (board[xadd(i,k)][yadd(j,l)]) count++;
	return count;
}

void* play (void* boards) {
/*
	(copied this from some web page, hence the English spellings...)

	1.STASIS : If, for a given cell, the number of on neighbours is 
	exactly two, the cell maintains its status quo into the next 
	generation. If the cell is on, it stays on, if it is off, it stays off.

	2.GROWTH : If the number of on neighbours is exactly three, the cell 
	will be on in the next generation. This is regardless of the cell's 		current state.

	3.DEATH : If the number of on neighbours is 0, 1, 4-8, the cell will 
	be off in the next generation.
*/
	int	i, j, a, newboard[BOARD_WIDTH][BOARD_HEIGHT];

	/* for each cell, apply the rules of Life */
	
	struct threadArguments *argument= (struct threadArguments(*)) boards;
	j= argument->height;
		
	
	for (i=0; i<BOARD_WIDTH; i++){
		a = adjacent_to (argument->board, i, j);
		if (a == 2) newboard[i][j] = argument->board[i][j];
		if (a == 3) newboard[i][j] = 1;
		if (a < 2) newboard[i][j] = 0;
		if (a > 3) newboard[i][j] = 0;
	}
	
	/* copy the new board back into the old board */

	
	phase1(&barrierUsed);
	for (i=0; i<BOARD_WIDTH; i++) {
		argument->board[i][j] = newboard[i][j];
	}
	phase2(&barrierUsed);
	pthread_exit(0);
}

/* print the life board */

void print (int** board) {
	int	i, j;

	/* for each row */

	for (j=0; j<BOARD_HEIGHT; j++) {

		/* print each column position... */

		for (i=0; i<BOARD_WIDTH; i++) {
			printf ("%c", board[i][j] ? 'x' : ' ');
		}

		/* followed by a carriage return */

		printf ("\n");
	}
}

/* read a file into the life board */

void read_file (int **board, char *name) {
	FILE	*f;
	int	i, j;
	char	s[100];

	f = fopen (name, "r");
	for (j=0; j<BOARD_HEIGHT; j++) {

		/* get a string */

		fgets (s, 100, f);

		/* copy the string to the life board */

		for (i=0; i<BOARD_WIDTH; i++) {
			board[i][j] = s[i] == 'x';
		}
	}
	fclose (f);
}

/* main program */

int main (int argc, char *argv[]) {
	int j;
	int **Board;//[BOARD_WIDTH][BOARD_HEIGHT];

	Board = malloc(sizeof(int*)*BOARD_WIDTH);
		int i=0;
	for(; i<BOARD_WIDTH; i++)
		Board[i]=malloc(sizeof(int)*BOARD_HEIGHT);


	initialize_board(Board);
	initialise(&barrierUsed);
	read_file (Board, argv[1]);


	/* play game of life 100 times */

	for (i=0; i<100; i++) 
	{
		pthread_t thread_id[noOfThreads];
		struct threadArguments argument[noOfThreads];
//
		for(j=0; j < noOfThreads; j++)
		{
			argument[j].board=Board; 
			argument[j].height=j;
			int k=pthread_create(&thread_id[j],NULL,&play,&argument[j]);

			if( k< 0)
			{
				printf("Thread creation failed. Hence Stopped");
				return -1;
			}
		}
//
		for(j=0; j <noOfThreads; j++)
		{
			pthread_join(thread_id[j],NULL);
		}
		print (Board);
		sleep(2);
//
		//clear the screen using VT100 escape codes 
		puts ("\033[H\033[J");
		
		
	}
}
