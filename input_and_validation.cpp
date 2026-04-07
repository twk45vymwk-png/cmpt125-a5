int getPlayerInput(char board[6][7], bool hasAnvil, bool &playedAnvil)
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
            col = stoi(input)- 1;
        }

        catch (...) {
            cout << "Invalid input, please try again.\n";
            continue;
        }

        if (col < 0 || col > 6) 
        {
            cout << "Invalid input, please try again.\n";
        } 
        else if (!playedAnvil && board[0][col] != '.') 
        {
            cout << "Column is full, please try again.\n";
        } 
        else 
        {
            return col;
        }
    }
}

