#include <iostream>
#include <vector>

using namespace std;

const int ROWS = 6;
const int COLS = 7;

enum class Cell { Empty, player1, player2, anvil1, anvil2}; // Cell type

using Board = vector<vector<Cell>>; // Board type

Board make_board(){
    return Board (ROWS, vector<Cell>(COLS, Cell::Empty));
}

int landing_row (const Board& board, int col){
    for (int r = ROWS - 1; r >= 0; r--){
        if (board[r][col] == Cell::Empty){
            return r;
        }
    }
    return -1; //column is full
}

bool col_full (const Board& board, int col){
    return board[0][col] != Cell::Empty;
}

bool drop (Board& board, int col, int player){
    int r = landing_row (board, col);
    if (r == -1){
        return false;
    }
    if (player == 1){
        board[r][col] = Cell::player1;
        return true;
    }
    else if (player == 2){
        board[r][col] = Cell::player2;
        return true;
    }
    else{
        cout << "Invalid" << endl;
        return false;
    }
}

//Anvil
bool drop_anvil (Board& board, int col, int player){
    if (col_full(board,col)){
        cout << "Cannot drop an anvil in a full column!" << endl;
        return false;
    }

    for (int r = 0; r < ROWS; r++){
        board [r][col] = Cell::Empty; //Destroy all pieces in the column
    }

    // Place anvil at the bottom
    if (player == 1){
        board[ROWS - 1][col] = Cell::anvil1;
        return true;
    }
    else if (player == 2){
        board[ROWS - 1][col] = Cell::anvil2;
        return true;
    }
    else{
        cout << "Invalid" << endl;
        return false;
    }
}