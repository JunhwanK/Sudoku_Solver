#ifndef SUDOKU_SOLVER_H
#define SUDOKU_SOLVER_H

#include <iostream>
#include <vector>
#include <utility> //pair, make_pair()
#include <string>
#include <unordered_set>
#include <unordered_map>

#include "Sudoku.h"

//A sudoku solver that uses depth first search and
//applys 'forward checking', 'conflict-direct backjumping'
//and 'dynamic variable ordering' to solve any n by n sudoku
class Sudoku_Solver {
public:
	//REQUIRES: istream argument satisfies the requirement for Sudoku class constructor
	//MODIFIES: sudoku, size
	//EFFECTS: create a Sudoku_Solver object
	Sudoku_Solver(std::istream &is);

	//MODIFIES: sudoku, tracker
	//EFFECTS: attempts to solve sudoku, returns true if solved
	// 		throws Sudoku_Error() if unsolvable or if sudoku board is invalid
	bool solve();

	//EFFECTS: prints sudoku board to ostream
	void print(std::ostream &os) const;

	//EFFECTS: returns true iff sudoku is solved
	bool is_solved() const;

private:
	
	//EFFECTS: throws Sudoku_Error() if initial sudoku block values
	// 		have an invalid duplicate in the same row, col or sqaure
	void pre_check() const;

	//MODIFIES: sudoku, tracker
	//EFFECTS: updates domains of all empty blocks, then solves sudoku only to 
	//		the point all values are 100% certain
	// 		i.e. fill in sudoku blocks with domain size 1, until none of the blocks have domain size of 1
	//		throws Sudoku_Error() if sudoku is unsolvable (created domain size of 0 during this process)
	void pre_solve();

	Sudoku sudoku;

	//for each row, tracks blocks with minimum remaining-values (i.e. min domain size)
	//using pair(col, domain size)
	std::vector<std::pair<unsigned short, unsigned short> > tracker;

	unsigned short size;

	//REQUIRES: row, col are smaller than size
	//			val is non-negative and smaller or equal to size
	//MODIFIES: tracker, value of sudoku block at (row,col),
	//			domain and conflict_sets of sudoku blocks in same row, same col, same sqaure
	//EFFECTS: sets value of sudoku block at (row,col) as val
	//			removes val from domains of blank blocks in the same row, same col, same sqaure
	//			adds key of the block at (row,col) to the conflict_set of block which had their domain reduced
	void set_val_and_update(unsigned short row, unsigned short col, unsigned short val);

	//REQUIRES: row, col are smaller than size
	//			sudoku block at (row,col) is not blank
	//MODIFIES: tracker, value of sudoku block at (row,col),
	//			domain and conflict_sets of sudoku blocks in same row, same col, same sqaure
	//EFFECTS: sets value of sudoku block at (row,col) back to blank.
	//			adds val of the block at (row,col) to domains of blank blocks in the same row,
	// 			same col, same sqaure only if the key of the block at (row,col) is in their conflict_set
	void unset_val_and_update(unsigned short row, unsigned short col);

	//REQUIRES: row is smaller than size
	//MODIFIES: tracker
	//EFFECTS: updates tracker at index row to contain pair(col, domain_size)
	//		whereby col indicates the index of the block at row that has the minimum domain size
	void track_row(unsigned short row);

	//REQUIRES: cumulative_conflict_set is empty
	//MODIFIES: cumulative_conflict_set, sudoku, tracker
	//EFFECTS: recursively calls itself to solve the sudoku using
	// 		depth first search that uses forward tracking and conflict-directed back jumping
	//		returns true is sudoku is solved
	//		returns false if found conflict or if current block's (key,val) is not in cumulative_conflict_set
	//		throws Sudoku_Error() if sudoku is unsolvable
	bool solve_helper(std::unordered_map<unsigned short, unsigned short>&cumulative_conflict_set, size_t depth = 0);
};


//Expection thrown when Sudoku has no solution
class Sudoku_Error {
public:
	std::string msg = "Sudoku has no solution / is invalid.";
};


#endif
