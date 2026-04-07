int getPlayerInput (char board[6][7])
{
    int col;
    while (true)
    {
        cout << "Enter a column 1-7: ";
        cin >> col;
        col--;

        if (cin.fail())
        {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "Invalid input, plz try again.\n ";
        }
        else if (col <0 || col >6)
        {
            cout << "Invalid input, plz try again.\n ";
        }
        else if (board[0][col] != '.')
        {
            cout << "This column is full! Select a new one.\n ";
        }
        else 
        {
            return col;
        }
    }
