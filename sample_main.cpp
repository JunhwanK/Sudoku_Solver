#include "Sudoku.h"
#include "Sudoku_Solver.h"

#include <fstream>
#include <iostream>

using namespace std;

int main(int argc, char* argv[]) {
	if (argc != 2) {
		cout << "Usage: "<< argv[0] <<" <sudoku_file_name>\n";
		return 1;
	}

	ifstream file_in(argv[1]);

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
