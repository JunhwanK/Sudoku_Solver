#include "Sudoku.h"
#include "Sudoku_Solver.h"

using namespace std;

/*Look at Sudoku_Solver.h for documention on member functions' constraints and (side-)effects*/

Sudoku_Solver::Sudoku_Solver(istream &is)
	: sudoku(is) {
	size = sudoku.get_size();
	tracker.resize(size);
}


bool Sudoku_Solver::solve_helper(std::unordered_map<unsigned short, unsigned short> &cumulative_conflict_set, size_t depth) {
	//find block with smallest domain
	unsigned short row = (unsigned short) size; //temporary place holder
	unsigned short col = (unsigned short) size; //temporary place holder
	unsigned short min_domain_size = (unsigned short) (size+1); //temporary place holder
	for (unsigned short i = 0; i < size; ++i) {
		auto domain_size = tracker[i].second;
		if (domain_size == 0) {
			//found conflict
			row = i;
			col = tracker[i].first;

			auto &cs = sudoku.get_conflict_set(row, col);
			//cumulative_conflict_set.clear(); //DO NOT clear conflict_set, all conflicts matter!
			for (auto it = cs.begin(); it != cs.end(); ++it) {
				cumulative_conflict_set[it->first] = it->second;
			}
			return false;

		} else if (domain_size < min_domain_size) {
			row = i;
			col = tracker[i].first;
			min_domain_size = domain_size;
		}
	}

	unsigned short key = sudoku.get_key(row, col);

	//assign value and continue search
	auto &domain = sudoku.get_domain(row, col);
	for (auto it = domain.begin(); it != domain.end(); ++it) {
		set_val_and_update(row, col, *it);

		//stop search when sudoku board is full (which is entirely through legel moves)
		if (sudoku.get_num_blank() == 0) {
			return true;
		}

		//if conflict was found in the next iteration
		//or if next iteration could not solve conflict
		if (solve_helper(cumulative_conflict_set, depth+1) == false) {
			unset_val_and_update(row, col); //undo
			//if current block's key and val is in cumulative conflict set
			auto ccs_it = cumulative_conflict_set.find(key);
			if (ccs_it != cumulative_conflict_set.end() && ccs_it->second == *it) {
				//if erase here, little less memory overhead but slower speed
				//cumulative_conflict_set.erase(key);
				continue; //continue search with next value
			} else {
				return false; //move back up a depth
			}
		} else {
			return true; //propagate return true from when board is full
		}
	}

	if (depth == 0) {
		//i.e. current block is initial block the search started with
		throw Sudoku_Error(); //exhausted search space
	} else {
		//current block was in cumulative_conflict_set,
		//but all its values led to a conflict.
		//if erase here, little more memory overhead but faster speed
		cumulative_conflict_set.erase(key);
		auto &cs = sudoku.get_conflict_set(row, col);
		for (auto it = cs.begin(); it != cs.end(); ++it) {
			cumulative_conflict_set[it->first] = it->second;
		}
		return false;
	}
}


bool Sudoku_Solver::solve() {
	if (sudoku.is_solved()) {
		return true; //sudoku is already solved
	} else if (sudoku.get_num_blank() == 0) {
		throw Sudoku_Error(); //sudoku is invalid
	}

	//check sudoku is valid
	pre_check();

	//solve sudoku until CBJ algorithm is needed
	pre_solve();

	if (sudoku.is_solved()) {
		return true;
	}

	//depth first search that uses forward checking and conflict-directed backjumping
	unordered_map <unsigned short, unsigned short> cumulative_conflict_set;
	solve_helper(cumulative_conflict_set);

	return sudoku.is_solved();
}


void Sudoku_Solver::track_row(unsigned short row) {
	unsigned short min_col;
	size_t min_domain_size = size+1; //temporary place holder
	for (unsigned short col = 0; col < size; ++col) {
		if (sudoku.get_val(row, col) != Block::BLANK) {
			continue;
		}
		auto new_size = sudoku.get_domain(row,col).size();
		if (new_size < min_domain_size) {
			min_col = col;
			min_domain_size = new_size;
		}
	}
	tracker[row] = make_pair(min_col, min_domain_size);
}


void Sudoku_Solver::pre_check() const {
	for (unsigned short i = 0; i < size; ++ i) {
		if (sudoku.check_row(i) == false) {
			throw Sudoku_Error();
		}
		if (sudoku.check_col(i) == false) {
			throw Sudoku_Error();
		}
		if (sudoku.check_square(i) == false) {
			throw Sudoku_Error();
		}
	}
}


void Sudoku_Solver::pre_solve() {
	//update all domain
	sudoku.update_all_domains();

	//start keeping track of blocks with minimum remaing values (min domain)
	for (unsigned short i = 0; i < size; ++i) {
		track_row(i);
	}

	unsigned short next = 0;
	while (next != size) {
		//find next block to fill (i.e. block with domain size 1)
		for (next = 0; next < size; ++next) {
			auto domain_size = tracker[next].second;
			if (domain_size == 0) {
				throw Sudoku_Error();
			} else if (domain_size == 1) {
				unsigned short row = next;
				unsigned short col = tracker[next].first;
				auto &cur_domain = sudoku.get_domain(row, col);
				auto val_it = cur_domain.begin();
				set_val_and_update(row, col, *val_it); //set_val, update domains and tracker
				break;
			}
		}
	}
}


void Sudoku_Solver::set_val_and_update(unsigned short row, unsigned short col, unsigned short val) {
	sudoku.set_val(row, col, val);

	//remove from domains in same col
	//add its key to conflict_sets in same col
	for (unsigned short i = 0; i < size; ++i) {
		if (sudoku.get_val(i, col) != Block::BLANK) {
			continue;
		}
		auto tracker_val = tracker[i].second;
		if (sudoku.domain_erase(i, col, val)) {
			sudoku.conflict_set_insert(i,col,row,col);
		}
		auto new_size = sudoku.get_domain_size(i, col);
		//update tracking (note: domain sizes change by max 1)
		if (new_size <= tracker_val) {
			//faster pruning using operator<= by moving to a related block next (maybe?)
			tracker[i] = make_pair(col, new_size);
		}
	}

	//remove from domains in same row
	//add its key to conflict_sets in same row
	for (unsigned short j = 0; j < size; ++j) {
		if (sudoku.get_val(row, j) != Block::BLANK) {
			continue;
		}
		if (sudoku.domain_erase(row, j, val)) {
			sudoku.conflict_set_insert(row,j,row,col);
		}
	}

	//remove in same square
	//add its key to conflict_sets in same square
	auto small_size = sudoku.get_small_size();
	unsigned short start_row = (unsigned short) (row - row%small_size);
	unsigned short start_col = (unsigned short) (col - col%small_size);
	for (unsigned short i = start_row; i < start_row + small_size; ++i) {
		for (unsigned short j = start_col; j < start_col + small_size; ++j) {
			if (i == row) {
				continue;
			} else if (j == col) {
				continue;
			} else if (sudoku.get_val(i, j) != Block::BLANK) {
				continue;
			}
			auto tracker_val = tracker[i].second;
			if (sudoku.domain_erase(i, j, val)) {
				sudoku.conflict_set_insert(i,j,row,col);
			}
			auto new_size = sudoku.get_domain_size(i,j);
			//update tracking (note domain size changes by max 1)
			if (new_size <= tracker_val) {
				//faster pruning using operator<= by moving to a related block next (maybe?)
				tracker[i] = make_pair(j, new_size);
			}
		}
	}

	//update tracking for the row which the block val was set
	track_row(row);
}


void Sudoku_Solver::unset_val_and_update(unsigned short row, unsigned short col) {
	unsigned short val = sudoku.get_val(row, col);

	//need re-tracking only if the block modified is the one in tracker
	unordered_set<unsigned short> need_track;

	//add to domains in same col
	//delete its key to conflict_sets in same col
	for (unsigned short i = 0; i < size; ++i) {
		if (sudoku.get_val(i, col) != Block::BLANK) {
			continue;
		}
		if (sudoku.conflict_set_erase(i, col, row, col)) {
			sudoku.domain_insert(i, col, val);
			if (tracker[i].first == col) {
				//need tracking if the block pointed by the tracker is being modified
				need_track.insert(i);
			}
		}
	}

	//add to domains in same row
	//delete its key to conflict_sets in same row
	for (unsigned short j = 0; j < size; ++j) {
		if (sudoku.get_val(row, j) != Block::BLANK) {
			continue;
		}
		if (sudoku.conflict_set_erase(row, j, row, col)) {
			sudoku.domain_insert(row, j, val);
		}
	}
	need_track.insert(row); //whole row was modified, definitely need re-tracking

	//add to domains in same square
	//delete its key to conflict_sets in same square
	auto small_size = sudoku.get_small_size();
	unsigned short start_row = (unsigned short) (row - row%small_size);
	unsigned short start_col = (unsigned short) (col - col%small_size);
	for (unsigned short i = start_row; i < start_row + small_size; ++i) {
		for (unsigned short j = start_col; j < start_col + small_size; ++j) {
			if (sudoku.get_val(i, j) != Block::BLANK) {
				continue;
			}
			if (sudoku.conflict_set_erase(i, j, row, col)) {
				sudoku.domain_insert(i, j, val);
				if (tracker[i].first == j) {
					//need tracking if the block pointed by the tracker is being modified
					need_track.insert(i);
				}
			}
		}
	}

	//unset value here so we don't triple check current block
	sudoku.set_val(row, col, Block::BLANK); //unset val to BLANK

	//update tracking
	for (auto it = need_track.begin(); it != need_track.end(); ++it) {
		track_row(*it);
	}
}


void Sudoku_Solver::print(ostream &os) const {
	sudoku.print_board(os);
	os << "\n";
}


bool Sudoku_Solver::is_solved() const {
	return sudoku.is_solved();
}
