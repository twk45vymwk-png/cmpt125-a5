
// a5.cpp

#include <iostream>
#include <string>
#include <vector>
#include <limits>
#include <cstdlib>
#include <ctime>

using namespace std;

const int ROWS = 6;
const int COLS = 7;
const int WIN_LEN = 4;
const int MAX_DEPTH = 7;

enum class Cell { Empty, player1, player2, anvil1, anvil2 };
using Board = vector<vector<Cell>>;

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


// Board: 
string getPieceRow(Cell cell, int line)
{
    if (cell == Cell::player1)
    {
        return "|🅾️🅾️🅾️🅾️🅾️🅾️";
    }
    else if (cell == Cell::player2)
    {
        return "|❎❎❎";
    }
    else if (cell == Cell::anvil1 || cell == Cell::anvil2)
    {
        if (line == 1) return "|🔩🔩🔩";
        else           return "|🔩🔩🔩";
    }
    else
    {
        return "|. .  .";
    }
}

void printBoard(const Board& board)
{
    cout << "\n         1      2      3      4      5     6       7 \n";

    for (int row = 0; row < ROWS; row++)
    {
        for (int line = 0; line < 3; line++)
        {
            if (line == 1)
            {
                cout << "   " << (ROWS - row ) << " ";
            } 
            else
            {
                 cout << "     ";  
            }

            for (int col = 0; col < COLS; col++)
            {
                cout << getPieceRow(board[row][col], line);
            }
            cout << "|\n";
        }
    }
}

// Input and Validation
// Help --> lookd up info about stoi and was recommended to use

int getPlayerInput(const Board& board, bool hasAnvil, bool &playedAnvil)
{
    playedAnvil = false;
    string input;

    while (true)
    {
        if (hasAnvil)
        {
            cout << "Enter a column (1-7) or 'a' for anvil: ";
        }
        else 
        {
            cout << "Enter a column (1-7): ";
        }

        cin >> input;

        if (hasAnvil && input == "a")
        {
            playedAnvil = true;
            cout << "Enter a column for the anvil (1-7): ";
            cin >> input;
        }
        
        int col; 

        try
        {
            // stoi converts the string input to an integer since both "a" 
            // and numbers are read as strings from the same cin
            col = stoi(input) - 1;
        }

        catch (...) {
            cout << "Invalid input, please try again.\n";
            continue;
        }

        if (col < 0 || col > 6) 
        {
            cout << "Invalid input, please try again.\n";
        } 
        else if (!playedAnvil && board[0][col] != Cell::Empty) 
        {
            cout << "Column is full, please try again.\n";
        } 
        else 
        {
            return col;
        }
    }
}

// Game display (title screen, )

void printTitleScreen()
{
    cout << " _______________________________________________________________________________\n";
    cout << "   ______                            __     __ __      ______  __           __  \n";
    cout << "  / ____/___  ____  ____  ___  _____/ /_   / // /    _/_/ __ \\/ /_  _______/ /  \n";
    cout << " / /   / __ \\/ __ \\/ __ \\/ _ \\/ ___/ __/  / // /_   / // /_/ / / / / / ___/ /\n";
    cout << "/ /___/ /_/ / / / / / / /  __/ /__/ /_   /__  __/  / // ____/ / /_/ (__  )_/\n";
    cout << "\\____/\\____/_/ /_/_/ /_/\\___/\\___/\\__/     /_/    / //_/   /_/\\__,_/____(_)  \n";
    cout << "________________________________________________________________________________\n";
    cout << "\n";
    cout << "                                (1) Start Game\n";
    cout << "                                (2) How to Play\n ";
    cout << "                               (3) The Anvil Piece\n ";
    cout << " Select an Option: ";


    int choice;  
    while (true)
    {
        cin >> choice;
        if (cin.fail()) 
        {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "Invalid choice, please try again: ";
            continue;
        }

        if (choice == 1) 
        {
            break;
        }

        else if (choice == 2)
        {
            cout << "\nGame Instructions: \n";
            cout << "  - Players take turns dropping pieces into a 7-column, 6-row grid\n";
            cout << "  - First to connect 4 pieces in a row wins\n";
            cout << "  - 4 in a row can be horizontal, vertical, or diagonal\n";
            cout << "  - If the board fills up without a winner, it's a tie\n";

            cout << "\n(1) Start Game\n";
            cout << "(2) How to Play\n";
            cout << "(3) The Anvil Piece\n";
            cout << "Select an Option: ";
        }
        else if (choice == 3)
        {
            cout << "\n The Anvil Piece: \n";
            cout << "  - Each player gets one anvil per game\n";
            cout << "  - Dropping an anvil destroys all pieces in that column\n";
            cout << "  - The anvil then sits at the bottom of the column\n";
            cout << "  - You cannot drop an anvil in a full column\n";
            cout << "  - Type 'a' on your turn to use your anvil\n";

            cout << "\n(1) Start Game\n";
            cout << "(2) How to Play\n";
            cout << "(3) The Anvil Piece\n";
            cout << "Select an Option: ";
        }
        else 
        {
            cout << "Invalid choice, please try again: ";
            continue;
        }
    }
}

// starting game 
void gameSetup(string &name1, string &name2, bool &vsComputer, int &firstPlayer) 
{
    cout << "\nSelect Opponent:\n";
    cout << "   (1) Another Player\n";
    cout << "   (2) Computer\n";

    int choice;
    while (true) 
    {
        cin >> choice;
        if (cin.fail()) 
        {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "Invalid choice, please enter 1 or 2: ";
        }   
        else if (choice == 1 || choice == 2) 
        {
        break;
        }
        else 
        {
        cout << "Invalid choice, please enter 1 or 2: ";
        }
    }

    vsComputer = (choice == 2);

    cout << "Enter Player 1 name: ";
    cin >> name1;
    if (vsComputer)
        name2 = "Computer";
    else 
    {
        cout << "Enter Player 2 name: ";
        cin >> name2;
    }

    
    cout << "Who would like to go first? (1) " << name1 << " (2) " << name2 << " (3) Random: ";
    while (true) 
    {
        cin >> firstPlayer;
        if (cin.fail()) 
        {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "Invalid choice, please enter 1, 2, or 3: ";
        } 
        else if (firstPlayer == 1 || firstPlayer == 2 || firstPlayer == 3) 
        {
            break;
        } 
        else 
        {
            cout << "Invalid choice, please enter 1, 2, or 3: ";
        }

    }

    if (firstPlayer == 3) 
    {
        srand(time(0));
        firstPlayer = (rand() % 2) + 1;
    }
}

//Game loop 
void gameLoop(Board& board, string name1, string name2, bool vsComputer, int firstPlayer)
{
    bool p1_anvil = true, p2_anvil = true;  // track per-player
    int currentPlayer = firstPlayer;

    while (true)
    {
        printBoard(board);

        string currentName = (currentPlayer == 1) ? name1 : name2;
        cout << currentName << "'s turn.\n";

        bool playedAnvil = false;
        int col = -1;

         // Computer turn
        if (vsComputer && currentPlayer == 2)
        {
            int anvil_col = best_computer_anvil(board, 2, p2_anvil);
            if (anvil_col != -1)
            {
                col = anvil_col;
                playedAnvil = true;
                p2_anvil = false;
                cout << "Computer drops an anvil in column " << col + 1 << "!\n";
            }
            else
            {
                col = best_computer_move(board, 2);
                cout << "Computer plays column " << col + 1 << ".\n";
            }
        }
        // Human turn
        else
        {
            bool hasAnvil = (currentPlayer == 1) ? p1_anvil : p2_anvil;
            col = getPlayerInput(board, hasAnvil, playedAnvil);
            if (playedAnvil)
            {
                if (currentPlayer == 1) p1_anvil = false;
                else p2_anvil = false;
            }
        }

        // Drop the piece
        if (playedAnvil)
            drop_anvil(board, col, currentPlayer);
        else
            drop(board, col, currentPlayer);

        // Check win
        if (win(board, currentPlayer))
        {
            printBoard(board);
            string winner = (currentPlayer == 1) ? name1 : name2;
            cout << winner << " wins!\n";
            break;
        }

        // Check draw
        if (board_full(board))
        {
            printBoard(board);
            cout << "It's a draw!\n";
            break;
        }

        currentPlayer = (currentPlayer == 1) ? 2 : 1;
    }

// Play again loop:

    while (true) 
    {
        cout << "Would you like to play again? (y/n): ";
        char answer;
        cin >> answer;
        if (answer == 'y' || answer == 'Y') 
        {
            board = make_board();
            currentPlayer = firstPlayer;
            gameLoop(board, name1, name2, vsComputer, firstPlayer);
            break;
        } 
        else if (answer == 'n' || answer == 'N') 
        {
            cout << "Thanks for playing!\n";
            break;
        }
        else 
        {
            cout << "Invalid choice, please enter y or n: ";
        }
    }
}

int main()
{
    printTitleScreen();

    string name1, name2;
    bool vsComputer;
    int firstPlayer;
    gameSetup(name1, name2, vsComputer, firstPlayer);

    Board board = make_board();
    gameLoop(board, name1, name2, vsComputer, firstPlayer);

    return 0;
}
