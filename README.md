 #Conway's Game of Life
 For basic description of game, read about it on following [link](https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life)
 
 This code is parallel Pthreads version of this [code](https://www.cs.utexas.edu/users/djimenez/utsa/cs1713-3/c/life.txt). 
 
 # Execution
 Navigate to the project directory using ubuntu terminal. The screenshots attached gives more details about the execution of the code.
 
 Enter the following command on your pc's terminal:
 
 `gcc L174236.c -lpthread`
 
 
 execute the code with this command:
 
 `./a.out input.txt`
 
 
input file for the code has been attached. 

## A little Code Description

For the function of play to act as runner function for thread we convert it into void* function with void* parameters
hence to pass the board and to divide task between threads a struct is used to pass arguments which contains board and the 
corresponding row number. Another approach could have been to avoid the struct and introduce board as a global variable but
struct seemed to be a better approach. 2-D pointer for board was declared in main and then memory was allocated on heap in this
code.

Barrier is implemented as a struct.
The functionality of barrier is implemented in two functions/parts following from ['The Little Book of Semaphores'](http://greenteapress.com/semaphores/LittleBookOfSemaphores.pdf) The phase1 part is before the critical
section whereas phase2 part is after the critical section.

An additional call to sleep is added at the end to slow the speed of the process and to easily observe the changes.


