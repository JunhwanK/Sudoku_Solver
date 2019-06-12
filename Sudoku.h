#ifndef SUDOKU_H
#define SUDOKU_H

#include <vector>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_set>
#include <unordered_map>


//Representaiton of a block in a sudoku
//a blank block is represented as a block with val of 0
class Block {
public:
	Block(): val{BLANK} {}

	Block(unsigned short val_in) : val{val_in} {}

	unsigned short key; //key to identify block on sudoku board
	unsigned short val;
	//domain: possible values the blank block can have
	std::unordered_set<unsigned short> domain;
	//conflict set: keys and values of other blocks that eliminated a value from this block's domain
	std::unordered_map<unsigned short, unsigned short> conflict_set;

	static const unsigned short BLANK = 0; //represents that a block is blank
};

//Representation of a nxn sudoku board
//where size = n and small_size = n^(1/2)
class Sudoku {
public:
	//REQUIRES: istream contains small_size, then small_size^4 number of sudoku values
	//			that are all seperated by whitespace. Sudoku values must be 
	//			non-negative values smaller than or equal to small_size^2
	//MODIFIES: board, small_size, size, num_blank, Domain
	//EFFECTS: creates sudoku object (does not compute sudoku blocks' domains)
	//			throws Value_Error() if input value is invalid (i.e. val > size)
	Sudoku(std::istream &is);

	//REQUIRES: row, col are smaller than size
	//EFFECTS: returns value of sudoku block at (row, col)
	unsigned short get_val(unsigned short row, unsigned short col) const;

	//REQUIRES: row, col are smaller than size
	//			val is non-negative and smaller or equal to size
	//MODIFIES: num_blank; value of sudoku block at (row, col)
	//EFFECTS: sets value of block at (row,col) as val,
	//		increases/decreases num_blank accordingly,
	//		throws Coordinate_Error() or Value_Error() if
	//		argument requirements are not met,
	void set_val(unsigned short row, unsigned short col, unsigned short val);

	//EFFECTS: returns true iff sudoku blocks in row have no duplicate values
	bool check_row(unsigned short row) const;
	
	//EFFECTS: returns true iff all sudoku blocks do not have duplicate
	//		values in the same row
	bool check_all_rows() const;
	
	//EFFECTS: returns true iff sudoku blocks in col have no duplicate values
	bool check_col(unsigned short col) const;
	
	//EFFECTS: returns true iff all sudoku blocks do not have duplicate
	//		values in the same col
	bool check_all_cols() const;
	
	//EFFECTS: returns true iff sudoku square with index have no duplicate values
	// 		index increases by across and down, starting from top left square
	// 		and ending bottom right square
	//		throws Index_Error() if argument requirement is not met
	bool check_square(unsigned short index) const;
	
	//EFFECTS: returns true iff all sudoku blocks do not have duplicate
	//		values in the same square
	bool check_all_squares() const;
	
	//EFFECTS: pretty prints the sudoku board to ostream
	void print_board(std::ostream &os) const;
	
	//EFFECTS: returns size
	unsigned short get_size() const;
	
	//EFFECTS: returns small_size
	unsigned short get_small_size() const;
	
	//REQUIRES: row, col are smaller than size
	//MODIFIES: domain of block at (row,col)
	//EFFECTS: updates the domain of block at (row,col) to contrain
	//			its potential values iff the block is blank
	void update_domain(unsigned short row, unsigned short col);
	
	//MODIFIES: domains of all blank blocks in sudoku
	//EFFECTS: updates the domain of all block to contrain
	//			their potential values iff the block is blank
	void update_all_domains();
	
	//REQUIRES: row, col are smaller than size
	//			val is non-negative and smaller or equal to size
	//MODIFIES: domain of block at (row,col)
	//EFFECTS: inserts val into the domain of block at (row,col)
	bool domain_insert(unsigned short row, unsigned short col, unsigned short val);
	
	//REQUIRES: row, col are smaller than size
	//			val is non-negative and smaller or equal to size
	//MODIFIES: domain of block at (row,col)
	//EFFECTS: erases val from the domain of block at (row,col)
	bool domain_erase(unsigned short row, unsigned short col, unsigned short val);
	
	//REQUIRES: row, col are smaller than size
	//EFFECTS: returns the size of domain of block at (row,col)
	unsigned short get_domain_size(unsigned short row, unsigned short col) const;
	
	//REQUIRES: row, col are smaller than size
	//EFFECTS: returns the domain of block at (row,col) by const reference
	const std::unordered_set<unsigned short>& 
			get_domain(unsigned short row, unsigned short col) const;
	
	//REQUIRES: row, col, other_row, other_col are smaller than size
	//MODIFIES: conflict_set of block at (row,col)
	//EFFECTS: inserts the (key,val) of the block at other_row, other_col
	//		into the conflict set of block at (row,col)
	void conflict_set_insert(unsigned short row, unsigned short col,
							unsigned short other_row, unsigned short other_col);
	
	//REQUIRES: row, col, other_row, other_col are smaller than size
	//MODIFIES: conflict_set of block at (row,col)
	//EFFECTS: erases the key of the block at other_row, other_col
	//		from the conflict set of block at (row,col)
	bool conflict_set_erase(unsigned short row, unsigned short col,
					unsigned short other_row, unsigned short other_col);
	
	//REQUIRES: row, col, other_row, other_col are smaller than size
	//EFFECTS: returns true iff the key of the block at other_row, other_col
	//		is in the conflict set of the block at (row,col)
	bool conflict_set_find(unsigned short row, unsigned short col,
					unsigned short other_row, unsigned short other_col) const;
	
	//REQUIRES: row, col are smaller than size
	//EFFECTS: returns the conflict_set of block at (row,col) by const reference
	const std::unordered_map<unsigned short, unsigned short>&
			get_conflict_set(unsigned short row, unsigned short col) const;

	//EFFECTS: returns true iff sudoku is solved
	bool is_solved() const;
	
	//EFFECTS: returns number of blank blocks in sudoku
	unsigned short get_num_blank() const;

	//REQUIRES: row, col are smaller than size
	//EFFECTS: returns the key for the block at (row,col)
	unsigned short get_key(unsigned short row, unsigned short col) const;

private:
	std::vector< std::vector<Block> > board;

	//small_size: width of the small square in sudoku
	unsigned short small_size;
	//size: small_size^2, the board's dimension is size x size
	unsigned short size; // size = small_size^2

	//num_blank: number of blank block in sudoku
	unsigned short num_blank;

	//contains natural numbers [1:size] inclusive for easy initialization of domains
	//MUST NOT be modified although it is not const
	std::unordered_set<unsigned short> Domain;

	//REQUIRES: row, col are smaller than size
	//EFFECTS: computes and returns the key for the block at (row,col)
	unsigned short key(unsigned short row, unsigned short col) const {
		return (unsigned short) (row*size + col);
	}
};


//Exception thrown when trying to access a non-existant, out-of-range sudoku block 
class Coordinate_Error {
public:
	Coordinate_Error(const char *function_name, 
					unsigned short row, unsigned short col,
					unsigned short other_row, unsigned short other_col,
					unsigned short size) {
		std::ostringstream os;
		os << "In function "<<function_name<<": attempted access invalid coordinate "
			<<"("<<row<<", "<<col<<") or ("<<other_row<<", "<<other_col<<") "
			<<"in a "<<size<<"x"<<size<<" sudoku.";
		msg = os.str();
	}

	Coordinate_Error(const char *function_name, 
					unsigned short row, unsigned short col,
					unsigned short size) {
		std::ostringstream os;
		os << "In function "<<function_name<<": attempted access invalid coordinate "
			<<"("<<row<<", "<<col<<") in a "<<size<<"x"<<size<<" sudoku.\n";
		msg = os.str();
	}

	std::string msg;
};


//Exception thrown when trying to access a non-existant, out-of-range sudoku sqaure
//thrown by check_square()
class Index_Error {
public:
	std::string msg = "Index out of range.\n";
};


//Exception thrown when trying to set a sudoku block's value that is
//out of range (negative or greater than width size)
//thrown by set_value()
class Value_Error {
public:
	Value_Error(const char *function_name, unsigned short val, unsigned short size) {
		std::ostringstream os;
		os << "In function "<<function_name<<": gave invalid, out-of-range val ("<<(val)<<") "
				"to a block in "<<size<<"x"<<size<<" sudoku.\n";
		msg = os.str();
	}

	std::string msg;
};


#endif
