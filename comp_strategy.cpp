#include <iostream>
#include <vector>
#include <limits>
#include "anvil.cpp"

using namespace std;

const int ROWS = 6;
const int COLS = 7;
const int WIN_LEN = 4;
const int MAX_DEPTH = 7;

enum class Cell { Empty, player1, player2, anvil1, anvil2}; // Cell type

using Board = vector<vector<Cell>>; // Board type

// Check if the cell belongs to player (including anvil)
bool is_player (Cell c, int player){
    if (player == 1){
        return c == Cell::player1 || c == Cell::anvil1;
    }
    return c == Cell::player2 || c == Cell::anvil2;
}

// Check all four directions for a win by player
bool win (const Board& board, int player){
    const int dr[] = {0,1,1,1};
    const int dc[] = {1,0,1,-1};

    for (int r = 0; r < ROWS; r++){
        for (int c = 0; c < COLS; c++){
            if (!is_player(board[r][c], player)){
                continue;
            }
            for (int i = 0; i<4; i++){
                int count = 1;
                for (int j = 1; j < WIN_LEN; j++){
                    int next_r = r + dr[i] * j;
                    int next_c = c + dc[i] * j;
                    if (next_r < 0 || next_r >= ROWS || next_c < 0 || next_c >= COLS){
                        break;
                    }
                    if (!is_player(board[next_r][next_c], player)){
                        break;
                    }
                    count ++;
                }
                if (count == WIN_LEN){
                    return true;
                }
            }
        }
    }
    return false;
}

// Check whether the board is full
bool board_full (const Board& board){
    for (int c = 0; c < COLS; c++){
        if (!col_full(board,c)){
            return false;
        }
    }
    return true;
}

// Minimax and computer strategy
// opponent player
int opponent (int player){
    if (player == 1){
        return 2;
    }
    else if (player == 2){
        return 1;
    }
    else{
        cout << "Invalid" << endl;
        return -1;
    }
}

// Score a window of 4 cells for maximizing player
int score_window(const vector<Cell>& window, int comp_player){
    int comp = 0;
    int opp = 0;
    int empty = 0;
    int opp_player = opponent(comp_player);
    for (Cell c : window){
        if (is_player (c, comp_player)){
            comp++;
        }
        else if (is_player (c, opp_player)){
            opp++;
        }
        else{
            empty++;
        }
    }
    if (comp == 4){
        return 100;
    }
    else if (comp == 3 && empty ==1){
        return 5;
    }
    else if (comp == 2 && empty == 2){
        return 2;
    }
    else if (opp == 3 && empty == 1){
        return -4;
    }
    return 0;
}

// Heuristic board score for computer
int heuristic (const Board& board, int comp_player){
    int score = 0;

    // Centre column
    for (int r = 0; r < ROWS; r++){
        if (is_player(board[r][COLS/2], comp_player)){
            score += 3; // Since centre column has the most winning direction
        }
    }

    // Horizontal
    for (int r = 0; r < ROWS; r++){
        for (int c = 0; c <= COLS - WIN_LEN; c++){
            vector <Cell> win = {board[r][c], board[r][c+1],board[r][c+2],board[r][c+3]};
            score += score_window(win, comp_player);
        }
    }

    // Vertical
    for (int c = 0; c < COLS; c++){
        for (int r = 0; r <= ROWS - WIN_LEN; r++){
            vector <Cell> win = {board[r][c], board[r+1][c],board[r+2][c],board[r+3][c]};
            score += score_window(win, comp_player);
        }
    }

    // Diagonal down-right
    for (int r = 0; r <= ROWS - WIN_LEN; r++){
        for (int c = 0; c <= COLS - WIN_LEN; c++){
            vector <Cell> win = {board[r][c], board[r+1][c+1],board[r+2][c+2],board[r+3][c+3]};
            score += score_window(win, comp_player);
        }
    }

    // Diagonal up-right
    for (int r = WIN_LEN - 1; r < ROWS; r++){
        for (int c = 0; c <= COLS - WIN_LEN; c++){
            vector <Cell> win = {board[r][c], board[r-1][c+1],board[r-2][c+2],board[r-3][c+3]};
            score += score_window(win, comp_player);
        }
    }

    return score;
}

int minimax(Board board,
            int depth,
            int alpha,
            int beta,
            bool maximizing,
            int current_player,
            int comp_player)
{
    int human_player = opponent(comp_player);

    //Terminal checks
    if (win(board, comp_player)){
        return 10000 + depth; 
    }
    if (win(board, human_player)){
        return -10000 - depth; 
    }
    if (board_full(board) || depth == 0){
        return heuristic(board,comp_player);
    }

    int next_player = opponent (current_player);

    if (maximizing){
        int best = numeric_limits<int>::min();
        for (int c = 0; c < COLS; c++){
            if (col_full(board,c)){
                continue;
            }
            Board next = board;
            drop (next, c, current_player);
            int val = minimax (next, depth - 1, alpha, beta, false, next_player, comp_player);
            best = max(best, val);
            alpha = max(alpha, val);
            if (alpha >= beta){
                break;
            }
        }
        return best;
    }

    else{
        int best = numeric_limits <int>::max();
        for (int c = 0; c < COLS; c++){
            if (col_full(board, c)){
                continue;
            }
            Board next = board;
            drop (next, c, current_player);
            int val = minimax (next, depth - 1, alpha, beta, true, next_player, comp_player);
            best = min(best, val);
            beta = min(beta, val);
            if (alpha >= beta){
                break;
            }
        }
        return best;
    }
}

// Choose the best column for the computer to play a normal piece
int best_computer_move (const Board& board, int comp_player){
    int best_score = numeric_limits<int>::min();
    int best_col = -1;

    //TRy columns in order from centre outward
    vector <int> order;
    int mid = COLS/2;  // Start with the middle column because it's usually the strongest
    order.push_back(mid);
    for (int off = 1; off <= mid; off++){
        if (mid + off < COLS){
            order.push_back(mid + off);
        }
        if (mid - off >= 0){
            order.push_back(mid - off);
        }
    }

    for (int c : order){
        if (col_full(board,c)){
            continue;
        }
        Board next = board;
        drop (next, c , comp_player);
        int score = minimax (next, MAX_DEPTH - 1,
                             numeric_limits <int>::min(),
                             numeric_limits <int>::max(),
                             false,
                             opponent(comp_player),
                             comp_player); // Ask minimax
        if (score > best_score){
            best_score = score;
            best_col = c;
        }
    }
    return best_col;
}

// Decide whether the computer should use its anvil this turn
int best_computer_anvil(const Board& board, int comp_player, bool anvil_available){
    if (!anvil_available){
        return -1; // Don't use anvil
    }

    int opp = opponent(comp_player);

    // (1) Check if dropping anvil wins immediately for computer
    for (int c = 0; c<COLS; c++){
        if (col_full(board,c)){
            continue;
        }
        Board next = board;
        drop_anvil (next, c, comp_player);
        if (win(next, comp_player)){
            return c;
        }
    }

    // (2) Check if dropping anvil blocks an opponent win
    for (int c = 0; c<COLS; c++){
        if (col_full(board,c)){
            continue;
        }
        Board test = board;
        if (drop(test, c, opp) && win(test, opp)){
            Board next = board;
            drop_anvil(next, c, comp_player);
            if (!win(next, opp)){
                return c;
            }
        }
    }

    // (3) Use anvil to break a long opponent chain a column
    for (int c = 0; c<COLS; c++){
        if (col_full(board,c)){
            continue;
        }
        int opp_count = 0;
        for (int r = 0; r < ROWS; r++){
            if (is_player(board[r][c], opp)){
                opp_count++;
            }
        }
        if (opp_count >= 3){
            return c;
        }
    }

    return -1; // don't use anvil
}