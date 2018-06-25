//=======================================================================

// SAMPLE SOURCE CODE - SUBJECT TO THE TERMS OF END-USER LICENSE AGREEMENT FOR
// Intel® Advisor 2017.

// /* Copyright (C) 2010-2017 Intel Corporation. All Rights Reserved.
 
 // The source code, information and material ("Material") 
 // contained herein is owned by Intel Corporation or its 
 // suppliers or licensors, and title to such Material remains 
 // with Intel Corporation or its suppliers or licensors.
 // The Material contains proprietary information of Intel or 
 // its suppliers and licensors. The Material is protected by 
 // worldwide copyright laws and treaty provisions.
 // No part of the Material may be used, copied, reproduced, 
 // modified, published, uploaded, posted, transmitted, distributed 
 // or disclosed in any way without Intel's prior express written 
 // permission. No license under any patent, copyright or other
 // intellectual property rights in the Material is granted to or 
 // conferred upon you, either expressly, by implication, inducement, 
 // estoppel or otherwise. Any license under such intellectual 
 // property rights must be express and approved by Intel in writing.
 // Third Party trademarks are the property of their respective owners.
 // Unless otherwise agreed by Intel in writing, you may not remove 
 // or alter this notice or any other notice embedded in Materials 
 // by Intel or Intel's suppliers or licensors in any way.
 
// ========================================================================

// [DESCRIPTION]
// Solve the nqueens problem  - how many positions of queens can fit on a chess
// board of a given size without attacking each other.
//
// [RUN]
// To set the board size in Visual Studio, right click on the project,
// select Properies > Configuration Properties > General > Debugging.  Set
// Command Arguments to the desired value.  14 has been set as the default.
//
// [EXPECTED OUTPUT]
// Depends upon the board size.
//
// Board Size   Number of Solutions
//     4                2
//     5               10
//     6                4
//     7               40
//     8               92
//     9              352
//    10              724
//    11             2680
//    12            14200
//    13            73712
//    14           365596
//    15          2279184

#include <iostream>
#include <cstdlib>

#ifdef _WIN32
#include <windows.h>
#include <mmsystem.h>
#define TimeType        DWORD
#define GET_TIME(t)     t = timeGetTime()
#define TIME_IN_MS(t)   (t)
#else
#include <sys/time.h>
#define TimeType        struct timeval
#define GET_TIME(t)     gettimeofday((&t), NULL)
#define TIME_IN_MS(t)   (((t).tv_sec * 1000000 + (t).tv_usec) / 1000)
#endif

#include "tbb/task_scheduler_init.h"
#include "tbb/blocked_range.h"
#include "tbb/parallel_for.h"
#include "tbb/spin_mutex.h"

//ADVISOR COMMENT: This is a TBB version of the nqueens application

using namespace tbb;
using namespace std;

int nrOfSolutions = 0;     // Counts the number of solutions.
int size = 0;              // The board-size; read from command-line.

// The number of correct solutions for each board size.
const int correctSolution[16] = {     0,     1,      0,       0, //  0 -  3
                                      2,    10,      4,      40, //  4 -  7
                                     92,   352,    724,    2680, //  8 - 11
                                  14200, 73712, 365596, 2279184  // 12 - 15
};

// Define TBB mutexes.
typedef spin_mutex NrOfSolutionsMutexType;
NrOfSolutionsMutexType NrOfSolutionsMutex;


/*
 * Recursive function to find all solutions on a board, represented by the
 * argument "queens", when we place the next queen at location (row, col).
 *
 * On Return: nrOfSolutions has been increased by the number of solutions for
 *            this board.
 */
void setQueen(int queens[], int row, int col) {
    //ADVISOR COMMENT: The accesses to the "queens" array in this function
    //                 create an incidental sharing correctness issue.
    //ADVISOR COMMENT: Each task should have its own copy of the queens array.
    //ADVISOR COMMENT: Look at the solve() function to see how to fix this.

    // Check all previously placed rows for attacks.
    for (int i=0; i < row; i++) {
        // Check vertical attacks.
        if (queens[i] == col) {
            return;
        }
        // Check diagonal attacks.
        if (abs(queens[i] - col) == (row - i) ) {
            return;
        }
    }

    // Column is ok, set the queen.
    //ADVISOR COMMENT: See comment at top of function.
    queens[row]=col;

    if (row == (size - 1)) {
        // Increment is not atomic, so locking is required here.
        NrOfSolutionsMutexType::scoped_lock mylock(NrOfSolutionsMutex);
        nrOfSolutions++;  // Placed final queen, found a solution!
        //  Block close releases the lock via the scoped_lock's destructor.
    } else {
        // Try to fill next row.
        for (int i=0; i < size; i++) {
            setQueen(queens, row+1, i);
        }
    }
}


class SetQueens {
public:
	void operator()(const blocked_range<size_t>& r) const {
		for (size_t i = r.begin(); i != r.end(); ++i) {
			// Try all positions in first row.
			// Create a separate array for each recursion started here.
			int *queens = new int[size];
			setQueen(queens, 0, (int)i);
            delete [] queens;
		}
	}
};


/*
 * Find all solutions for nQueens problem on size x size chessboard.
 *
 * On Return: nrOfSolutions = number of solutions for size x size chessboard.
 */
void solve() {
	// Do a parallel for over the n positions in the first row.
	// Let the scheduler decide how the n tasks should be distributed
	//     among the different threads.
	parallel_for(blocked_range<size_t>(0, size, 1), SetQueens() );
}


int main(int argc, char *argv[]) {
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " boardSize [default is 14].\n";
        size = 14;
    } else {
        size = atoi(argv[1]);
        // Limit the range of inputs we accept.  If the board is not large
        // enough, the program may finish before suitability and other analyses
        // can produce an accurate result.  If it is too large, the program
        // may appear to hang (even though it is just taking a lot of time.)
        if ((size < 4) || (size > 15)) {
            cerr << "Boardsize should be between 4 and 15; "
                "setting it to 14. \n" << endl;
            size = 14;
        }
    }

    cout << "Starting nqueens (" << argv[0] << ") solver for size " << size
         << "...\n";

    TimeType startTime, endTime;
    GET_TIME(startTime);
    solve();
    GET_TIME(endTime);

    cout << "Number of solutions: " << nrOfSolutions << endl;
    if (nrOfSolutions != correctSolution[size])
        cout << "!!Incorrect result!! Number of solutions should be " <<
            correctSolution[size] << endl << endl;
    else
        cout << "Correct result!" << endl;

    cout << endl << "Calculations took " <<
        TIME_IN_MS(endTime) - TIME_IN_MS(startTime) << "ms." << endl;
    return 0;
}
