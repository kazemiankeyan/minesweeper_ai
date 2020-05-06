// ======================================================================
// FILE:        MyAI.cpp
//
// AUTHOR:      Jian Li
//
// DESCRIPTION: This file contains your agent class, which you will
//              implement. You are responsible for implementing the
//              'getAction' function and any helper methods you feel you
//              need.
//
// NOTES:       - If you are having trouble understanding how the shell
//                works, look at the other parts of the code, as well as
//                the documentation.
//
//              - You are only allowed to make changes to this portion of
//                the code. Any changes to other portions of the code will
//                be lost when the tournament runs your code.
// ======================================================================

#include "MyAI.hpp"

//constructor
MyAI::MyAI ( int _rowDimension, int _colDimension, int _totalMines, int _agentX, int _agentY ) : Agent()
{
    // ======================================================================
    // YOUR CODE BEGINS
    // ======================================================================
    row = _rowDimension;
    col = _colDimension;
    mines = _totalMines;
    x = _agentX; //initial zero coord
    y = _agentY; //initial zero coord
    uncovered = 0;
    covered = row*col;

    //initialize 2D vector for board, fill the board with "."
    board.resize(row);
    for(int r = 0; r < row; r++)
    {
      board[r].resize(col);
      for(int c = 0; c < col; c++)
        board[r][c] = ".";
    }


    // ======================================================================
    // YOUR CODE ENDS
    // ======================================================================
};

//
Agent::Action MyAI::getAction( int number )
{

    // ======================================================================
    // YOUR CODE BEGINS
    // ======================================================================
    // double remaining_time = 0.0;


    //fill the tile at x, y with number
    fillBoard(x, y, number);

    // cout << "ZEROES CHECKED GRID ----- " << endl;
    // for(int size = checked.size() - 1; size >= 0; size--)
    // {
    //   for(int c = 0; c < checked[size].size(); c++)
    //     std::cout << checked[size][c] + 1 << ' ';
    //   std::cout << std::endl;
    // }

    //if remaining tiles are not equal to amount of mines
    if(covered != mines)
    {
      //count the flags around current tile
      int flags = getType(x, y, "-1");

      int marked = 0;
      for(int i = 0; i < 9; i++)
      {
        marked += getType(x, y, to_string(i));
      }
      marked += flags; //what is this supposed to be?

      //effective is amount of potential bomb(s) minus flags around current tile
      int effective = number - flags;
      //unmarked is number of tiles still covered around current tile
      int unmarked = getType(x, y, ".");
      //if there are still covered tiles, and it is confirmed that there are no more bombs
      if(unmarked > 0 && effective == 0)
        //add all the surrounding tiles into uncoverable vector
        addZeroes(x, y);

      //if theres no potential bomb around a tile and there are still tiles in checked to pop
      if(effective == 0 || checked.size() > 0)
      {
        //first uncover remaining coords in checked
        if(checked.size() > 0)
        {
          for(int i = 0; i < checked.size(); i++)
          {
            vector<int> zero = checked[checked.size() - 1];
            checked.pop_back();
            int new_x = zero[0];
            int new_y = zero[1];
            if(isCovered(new_x, new_y))
            {
              x = new_x;
              y = new_y;
              return {UNCOVER, new_x, new_y};
            }
          }
        }
      }

      // cout << "LABEL: " << number << endl;
      // cout << "MARKED: " << marked << endl;
      // cout << "UNMARKED: " << unmarked << endl;
      // cout << "FLAGS: " << flags << endl;
      // cout << "EFFECTIVE: " << effective << endl;
      // cout << "COVERED: " << covered << endl;
      // cout << "MINES: " << mines << endl;
      // cout << "UNCOVERED: " << uncovered << endl;
      // cout << "CHECKED SIZE: " << checked.size() << endl;

      //if the amount of potential bombs equal the
      //number of tiles that are still uncovered
      if(effective==unmarked)
      {
        int adj8 [8][2] = {{-1, 1}, {-1, 0}, {1 , 1},
                          {-1, 0},           {1, 0},
                          {-1, -1}, {0, -1}, {1, -1}};
        for(int *n : adj8)
        {
          int new_x = x + n[1];
          int new_y = y + n[0];
          //iterate through all the tiles around current tile and flag them
          if((new_x < col && new_x >= 0) && (new_y < row && new_y > 0) && isCovered(new_x, new_y))
          {
            x = new_x;
            y = new_y;
            return {FLAG, x, y};
          }
        }
      }
    }


    return {LEAVE,-1,-1};
    // ======================================================================
    // YOUR CODE ENDS
    // ======================================================================

}


// ======================================================================
// YOUR CODE BEGINS
// ======================================================================

//helper functions
void MyAI::printBoard()
{
  for(int r = row - 1; r >= 0; r--)
  {
    for(int c = 0; c < col; c++)
      std::cout << board[r][c] << ' ';
    std::cout << std::endl;
  }
}

void MyAI::addZeroes(int x, int y)
{
  int adj8 [8][2] = {{-1, 1}, {-1, 0}, {1 , 1},
                    {-1, 0},           {1, 0},
                    {-1, -1}, {0, -1}, {1, -1}};

  for(int *n : adj8)
  {
    int new_x = x + n[1];
    int new_y = y + n[0];

    // cout << "NEW_X: " << new_x << endl;
    // cout << "NEW_Y: " << new_y << endl;

    if((new_x < col && new_x >= 0) && (new_y < row && new_y >= 0))
    {
      if(board[new_y][new_x] == ".") //so this already checks for covered
      {
        vector<int> zero;
        zero.push_back(new_x);
        zero.push_back(new_y);
        checked.push_back(zero);
      }
    }
  }
}

bool MyAI::isCovered(int x, int y)
{
  if(board[y][x] == ".")
    return true;
  return false;
}

void MyAI::fillBoard(int x, int y, int number)
{
  // std::cout << "NUMBER: " << number << std::endl;
  // std::cout << "X: " << x + 1<< std::endl;
  // std::cout << "Y: " << y + 1<< std::endl;
  covered -= 1;
  uncovered +=1;
  board[y][x] = to_string(number);
  //printBoard();
}

int MyAI::getType(int x, int y, string type)
{
  int type_count = 0;
  int adj8 [8][2] = {{-1, 1}, {-1, 0}, {1 , 1},
                    {-1, 0},           {1, 0},
                    {-1, -1}, {0, -1}, {1, -1}};

  for(int *n : adj8)
  {
    int new_x = x + n[1];
    int new_y = y + n[0];

    if((new_x < col && new_x >= 0) && (new_y < row && new_y >= 0))
    {
      if(board[new_y][new_x] == type)
        type_count++;
    }
  }

  return type_count;
}


// ======================================================================
// YOUR CODE ENDS
// ======================================================================
