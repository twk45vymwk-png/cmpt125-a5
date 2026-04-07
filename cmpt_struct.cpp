#include <iostream>
#include <vector>
#include <string>

using namespace std;
//helper function that sets up the pieces 
string getPieceRow(char piece, int line)
{
    if (piece == 'x')
    {
        if (line == 1)
        {
            return "|xxx";
        }
        else
        {
            return "|xxx";
        }
    }

    else if (piece == 'y')
    {
        if (line == 1)
        {
            return "|yyy";
        }
        else
        {
            return "|yyy";
        }
    }

    else if (piece == 'A')
    {
        if (line == 1)
        {
            return "|xAx";
        }
        else
        {
            return "|xxx";
        }
    }
    else
    {
        return "|...";
    }
}

void printBoard(char board[6][7])
{
    for (int row = 0; row < 6; row++)
    {
        for (int line = 0; line < 3; line++)
        {
            for (int col = 0; col <7; col++)
            {
                cout << getPieceRow(board[row][col], line);
            }
            cout << "|\n";
        }
    }
}

// * FOR MAIN FUNCTION:

//int main() {
//     char board[6][7];
    
//     // fill everything as empty first
//     for (int r = 0; r < 6; r++)
//         for (int c = 0; c < 7; c++)
//             board[r][c] = '.';
    
//     printBoard(board);
// }


