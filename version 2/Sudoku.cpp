#include "Sudoku.h"
#include <iomanip> //std::setw(), std::left

using namespace std;

/*Look at Sudoku.h for documention on member functions' constraints and (side-)effects*/

Sudoku::Sudoku(istream &is) {
	is >> small_size;
	size = (unsigned short) (small_size * small_size);
	
	board.resize(size);

	for (unsigned short row = 0; row < size; ++row) {
		board[row].resize(size);
	}

	num_blank = 0;

	unsigned short val;	
	for (unsigned short row = 0; row < size; ++row) {
		for (unsigned short col = 0; col < size; ++col) {
			is >> val;
			if (val == Block::BLANK) {
				++num_blank;
				board[row][col].key = key(row, col);
			}
			board[row][col].val = val;
		}
	}

	for (unsigned short i = 1; i < size+1; ++i) {
		Domain.insert(i);
	}
}


unsigned short Sudoku::get_val(unsigned short row, unsigned short col)  const {
	if (row >= size || col >= size) {
		throw Coordinate_Error("Sudoku::get_val", row, col, size);
	}

	return board[row][col].val;
}


void Sudoku::set_val(unsigned short row, unsigned short col, unsigned short val) {
	if (row >= size || col >= size) {
		throw Coordinate_Error("Sudoku::set_val", row, col, size);
	} else if (val > size) {
		throw Value_Error("Sudoku::set_val", val, size);
	}

	unsigned short original = board[row][col].val;
	board[row][col].val = val;
	if (val == Block::BLANK && original != Block::BLANK) {
		++num_blank;
	} else if (original == Block::BLANK && val != Block::BLANK) {
		--num_blank;
	}
}


bool Sudoku::check_row(unsigned short row) const {
	if (row >= size) {
		throw Coordinate_Error("Sudoku::check_row", row, 0, size);
	}
	unordered_set <unsigned short> look_up;
	for (unsigned short col = 0; col < size; ++col) {
		unsigned short val = board[row][col].val;
		if (val == Block::BLANK) {
			continue;
		}

		//if val of block is already in look_up and hence insertion fails
		if (look_up.insert(val).second == false) {
			return false; //found duplicate
		}
	}
	return true;
}


bool Sudoku::check_all_rows() const {
	for (unsigned short i = 0; i < size; ++i) {
		if (!check_row(i)) {
			return false;
		}
	}
	return true;
}


bool Sudoku::check_col(unsigned short col) const {
	if (col >= size) {
		throw Coordinate_Error("Sudoku::check_col", 0, col, size);
	}
	unordered_set <unsigned short> look_up;
	for (unsigned short row = 0; row < size; ++row) {
		unsigned short val = board[row][col].val;
		if (val == Block::BLANK) {
			continue;
		}

		//if val of block is already in look_up and hence insertion fails
		if (look_up.insert(val).second == false) {
			return false; //found duplicate
		}
	}
	return true;
}


bool Sudoku::check_all_cols() const {
	for (unsigned short i = 0; i < size; ++i) {
		if (!check_col(i)) {
			return false;
		}
	}
	return true;
}


bool Sudoku::check_square(unsigned short index) const {
	if (index >= size) {
		throw Index_Error();
	}

	unordered_set <unsigned short> look_up;
	for (int i = 0; i < small_size; ++i) {
		int row = int(index/small_size)*small_size + i;

		for (int j = 0; j < small_size; ++j) {
			int col = (index%small_size)*small_size + j;

			unsigned short val = board[row][col].val;
			if (val == Block::BLANK) {
				continue;
			}

			//if val of block is already in look_up and hence insertion fails
			if (look_up.insert(val).second == false) {
				return false; //found duplicate
			}
		}
	}
	return true;
}


bool Sudoku::check_all_squares() const {
	for (unsigned short i = 0; i < size; ++i) {
		if (!check_square(i)) {
			return false;
		}
	}
	return true;
}


void Sudoku::print_board(ostream &os) const {
	auto spacing = int (size/10+1);

	for (unsigned short row = 0; row < size; ++row) {
		for (unsigned short col = 0; col < size; ++col) {
			os << setw(spacing) << std::left << board[row][col].val << " ";
		}
		os << "\n";
	}
}


unsigned short Sudoku::get_size() const {
	return size;
}


void Sudoku::update_domain(unsigned short row, unsigned short col) {
	if (row >= size || col >= size) {
		throw Coordinate_Error("Sudoku::update_domain", row, col, size);
	}

	if (board[row][col].val != Block::BLANK) {
		return;
	}

	auto &domain = board[row][col].domain;
	domain = Domain;

	//check row
	for (unsigned short j = 0; j < size; ++j) {
		unsigned short val = board[row][j].val;
		if (val == Block::BLANK) {
			continue;
		}
		domain.erase(val);
	}

	//check column
	for (unsigned short i = 0; i < size; ++i) {
		unsigned short val = board[i][col].val;
		if (val == Block::BLANK) {
			continue;
		}
		domain.erase(val);
	}

	//check square
	int start_row = row - row%small_size;
	int start_col = col - col%small_size;
	for (int i = start_row; i < start_row + small_size; ++i) {
		for (int j = start_col; j < start_col + small_size; ++j) {
			unsigned short val = board[i][j].val;
			if (val == Block::BLANK) {
				continue;
			}
			domain.erase(val);
		}
	}
}

void Sudoku::update_all_domains() {
	for (unsigned short row = 0; row < size; ++row) {
		for (unsigned short col = 0; col < size; ++col) {
			update_domain(row, col);
		}
	}
}

bool Sudoku::domain_insert(unsigned short row, unsigned short col, unsigned short val) {
	if (row >= size || col >= size) {
		throw Coordinate_Error("Sudoku::domain_insert", row, col, size);
	} else if (val > size) {
		throw Value_Error("Sudoku::domain_insert", val, size);
	}
	return board[row][col].domain.insert(val).second;
}

bool Sudoku::domain_erase(unsigned short row, unsigned short col, unsigned short val) {
	if (row >= size || col >= size) {
		throw Coordinate_Error("Sudoku::domain_erase", row, col, size);
	} else if (val > size) {
		throw Value_Error("Sudoku::domain_erase", val, size);
	}
	return board[row][col].domain.erase(val);
}

const unordered_set<unsigned short>& Sudoku::get_domain(unsigned short row, unsigned short col) const {
	if (row >= size || col >= size) {
		throw Coordinate_Error("Sudoku::get_domain", row, col, size);
	}
	return board[row][col].domain;
}

bool Sudoku::is_solved() const {
	if (num_blank == 0
		&& check_all_rows()
		&& check_all_cols()
		&& check_all_squares()) {
		return true;
	}
	return false;
}

unsigned short Sudoku::get_small_size() const {
	return small_size;
}

void Sudoku::conflict_set_insert(unsigned short row, unsigned short col,
								 unsigned short other_row, unsigned short other_col) {
	if (row >= size || col >= size || other_row >= size || other_col >= size) {
		throw Coordinate_Error("Sudoku::conflict_set_insert", row, col, other_row, other_col, size);
	}
	auto &cs = board[row][col].conflict_set;
	unsigned short key = get_key(other_row, other_col);
	unsigned short val = get_val(other_row, other_col);
	cs[key] = val;
}

bool Sudoku::conflict_set_erase(unsigned short row, unsigned short col,
								unsigned short other_row, unsigned short other_col) {
	if (row >= size || col >= size || other_row >= size || other_col >= size) {
		throw Coordinate_Error("Sudoku::conflict_set_erase", row, col, other_row, other_col, size);
	}
	unsigned short key = get_key(other_row, other_col);
	return board[row][col].conflict_set.erase(key);
}

bool Sudoku::conflict_set_find(unsigned short row, unsigned short col,
								unsigned short other_row, unsigned short other_col) const {
	if (row >= size || col >= size || other_row >= size || other_col >= size) {
		throw Coordinate_Error("Sudoku::conflict_set_find", row, col, other_row, other_col, size);
	}
	auto &conf_set = board[row][col].conflict_set;
	unsigned short key = get_key(other_row, other_col);
	return conf_set.find(key) != conf_set.end();
}

void Sudoku::cumulative_conflict_set_propagate(unsigned short row, unsigned short col,
								 unsigned short other_row, unsigned short other_col) {
	if (row >= size || col >= size || other_row >= size || other_col >= size) {
		throw Coordinate_Error("Sudoku::cumulative_conflict_set_propagate", row, col, other_row, other_col, size);
	}
	//propagate cumulative conflict set from block at (other_row, other_col) to
	//the block at (row,col)
	auto &ccs = board[row][col].cumulative_conflict_set;
	auto &other_ccs = board[other_row][other_col].cumulative_conflict_set;
	for (auto it = other_ccs.begin(); it != other_ccs.end(); ++it) {
		ccs[it->first] = it->second;
	}
}

void Sudoku::conflict_set_propagate(unsigned short row, unsigned short col,
							 unsigned short other_row, unsigned short other_col) {
	if (row >= size || col >= size || other_row >= size || other_col >= size) {
		throw Coordinate_Error("Sudoku::conflict_set_propagate",
								row, col, other_row, other_col, size);
	}
	//propagate conflict set from block at (other_row, other_col) to
	//the block at (row,col)
	auto &ccs = board[row][col].cumulative_conflict_set;
	auto &other_cs = board[other_row][other_col].conflict_set;
	for (auto it = other_cs.begin(); it != other_cs.end(); ++it) {
		ccs[it->first] = it->second;
	}
}

bool Sudoku::cumulative_conflict_set_find(unsigned short row, unsigned short col,
										  unsigned short other_row, unsigned short other_col) {
	if (row >= size || col >= size || other_row >= size || other_col >= size) {
		throw Coordinate_Error("Sudoku::cumulative_conflict_set_find",
								row, col, other_row, other_col, size);
	}

	//find (key,val) of block at (other_row,other_col) inside conflict_set or cumulative_conflict_set of
	//block at (row,col)
	auto key = get_key(other_row,other_col);
	auto val = board[other_row][other_col].val;

	auto &cs = board[row][col].conflict_set;
	auto cs_it = cs.find(key);

	auto &ccs = board[row][col].cumulative_conflict_set;
	auto ccs_it = ccs.find(key);

	if (cs_it != cs.end()) {
		return cs_it->second == val;
	} else if (ccs_it != ccs.end()) {
		return ccs_it->second == val;
	} else{
		return false;
	}
}

void Sudoku::cumulative_conflict_set_erase_self(unsigned short row, unsigned short col) {
	if (row >= size || col >= size) {
		throw Coordinate_Error("Sudoku::cumulative_conflict_set_erase_self", row, col, size);
	}
	//eliminate self from self's cumulative_conflcit_set
	auto &ccs = board[row][col].cumulative_conflict_set;
	auto key = get_key(row,col);
	ccs.erase(key);
}

unsigned short Sudoku::get_num_blank() const {
	return num_blank;
}

const unordered_map<unsigned short, unsigned short>& Sudoku::get_conflict_set(unsigned short row, unsigned short col) const {
	if (row >= size || col >= size) {
		throw Coordinate_Error("Sudoku::get_conflict_set", row, col, size);
	}
	return board[row][col].conflict_set;
}

unsigned short Sudoku::get_domain_size(unsigned short row, unsigned short col) const {
	if (row >= size || col >= size) {
		throw Coordinate_Error("Sudoku::get_domain_size", row, col, size);
	}
	return (unsigned short) (board[row][col].domain.size());
}


unsigned short Sudoku::get_key(unsigned short row, unsigned short col) const {
	if (row >= size || col >= size) {
		throw Coordinate_Error("Sudoku::get_key", row, col, size);
	}
	return board[row][col].key;
}
