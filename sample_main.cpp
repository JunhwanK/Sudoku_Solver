#include "Sudoku.h"
#include "Sudoku_Solver.h"

#include <fstream>
#include <iostream>

using namespace std;

int main() {
	ifstream file_in("sample_sudoku_1.txt");

	if (!file_in.is_open()) {
		cout << "Input file not opened\n";
		return 1;
	}

	try {
		Sudoku_Solver test_solver(file_in);
		test_solver.solve();
		test_solver.print(cout);
	} catch (Sudoku_Error &err) {
		cout << err.msg << "\n";
		return 1;
	} catch (Value_Error &err) {
		cout << err.msg << "\n";
		return 1;
	} catch (Coordinate_Error &err) {
		cout << err.msg << "\n";
		return 1;
	}

	return 0;
}
