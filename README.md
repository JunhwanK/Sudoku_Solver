# Sudoku_Solver

A sudoku solver that uses depth first search and conflict-based backjumping.
With every value assignment, it uses forward checking to elminiate domain values for other blank block. It also uses minimum-remaning values heuristic for dynamic variable ordering.

This sudoku solver can solve any (n^2)x(n^2) sudoku with inner squares of size (n)x(n).

Input file format is (n) followed by (n^2)x(n^2) number of values, all seperated by whitespace, where 0 indicates a blank sudoku block.

Sample sudoku input files are provided.

**9x9 sudoku:**
* sample_sudoku_1.txt
* sample_sudoku_2.txt
* sample_sudoku_3.txt
* sample_sudoku_6.txt

**16x16 sudoku:**
* sample_sudoku_5.txt
* sample_sudoku_8.txt
* sample_sudoku_9.txt
* sample_sudoku_10.txt
* sample_sudoku_12.txt

**Runtime:**
The average runtime for solving 9x9 sudoku is about 0.2s. (runtime vary between computers, but in general)

The runtime for solving 16x16 sudoku vary from sudoku to sudoku:
* 2min (sample 12)
* 4min (sample 10)
* 11~13min (samples 5 and 9)
* 100min (sample 8)

**Sample use:**
Download necessary files or clone repository and compile:
> g++ -std=c++1z -Wconversion -Wall -Werror -Wextra -pedantic  -O3 -DNDEBUG -march=native -c Sudoku.cpp

> g++ -std=c++1z -Wconversion -Wall -Werror -Wextra -pedantic  -O3 -DNDEBUG -march=native -c Sudoku_Solver.cpp

> g++ -std=c++1z -Wconversion -Wall -Werror -Wextra -pedantic  -O3 -DNDEBUG -march=native -c sample_main.cpp

> g++ -std=c++1z -Wconversion -Wall -Werror -Wextra -pedantic  -O3 -DNDEBUG -march=native sample_main.o Sudoku.o Sudoku_Solver.o -o Sudoku_Solver

Then run program:
> ./Sudoku_Solver
