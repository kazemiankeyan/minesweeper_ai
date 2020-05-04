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

MyAI::MyAI ( int _rowDimension, int _colDimension, int _totalMines, int _agentX, int _agentY ) : Agent()
{
    // ======================================================================
    // YOUR CODE BEGINS
    // ======================================================================
    row = _rowDimension;
    col = _colDimension;
    mines = _totalMines;
    x = _agentX;
    y = _agentY;
    uncovered = 0;
    covered = row*col;
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

Agent::Action MyAI::getAction( int number )
{

    // ======================================================================
    // YOUR CODE BEGINS
    // ======================================================================
    // double remaining_time = 0.0;
    fillBoard(x, y, number);

    cout << "ZEROES CHECKED GRID ----- " << endl;
    for(int size = checked.size() - 1; size >= 0; size--)
    {
      for(int c = 0; c < checked[size].size(); c++)
        std::cout << checked[size][c] + 1 << ' ';
      std::cout << std::endl;
    }

    if(covered != mines)
    {
      // uses the checked vector to uncover all boxes
      // around a zero, keeps going till done.
      if(checked.size() > 0)
      {
        vector<int> zero = checked[checked.size() - 1];
        checked.pop_back();
        int new_x = zero[0];
        int new_y = zero[1];
        x = new_x;
        y = new_y;
        return {UNCOVER, new_x, new_y};
      }

      int flags = getType(x, y, "-1");
      int marked = 0;
      for(int i = 0; i < 9; i++)
      {
        marked += getType(x, y, to_string(i));
      }
      marked += flags;
      int unmarked = getType(x, y, ".");

      int effective = number - flags;

      cout << "LABEL: " << number << endl;
      cout << "MARKED: " << marked << endl;
      cout << "UNMARKED: " << unmarked << endl;
      cout << "FLAGS: " << flags << endl;
      cout << "EFFECTIVE: " << effective << endl;
      cout << "COVERED: " << covered << endl;
      cout << "MINES: " << mines << endl;
      cout << "UNCOVERED: " << uncovered << endl;

      if(effective == 0)
      {
        if(unmarked > 0)
        {
          addZeroes(x, y);
          // uses the checked vector to uncover all boxes
          // around a zero, keeps going till done.
          if(checked.size() > 0)
          {
            vector<int> zero = checked[checked.size() - 1];
            checked.pop_back();
            int new_x = zero[0];
            int new_y = zero[1];
            x = new_x;
            y = new_y;
            return {UNCOVER, new_x, new_y};
          }
        }
      }

      // if(effective == 0 || (effective==unmarked))
      // {
      //   int adj8 [8][2] = {{-1, 1}, {-1, 0}, {1 , 1},
      //                     {-1, 0},           {1, 0},
      //                     {-1, -1}, {0, -1}, {1, -1}};
      //
      //   for(int *n : adj8)
      //   {
      //     int new_x = x + n[1];
      //     int new_y = y + n[0];
      //
      //     if((new_x < col && new_x >= 0) && (new_y < row && new_y > 0) && isUncovered(new_x, new_y))
      //     {
      //       x = new_x;
      //       y = new_y;
      //
      //       if(effective == 0)
      //       {
      //         if(unmarked > 0)
      //         {
      //           addZeroes(x, y);
      //           if(checked.size() > 0)
      //           {
      //             vector<int> zero = checked[checked.size() - 1];
      //             checked.pop_back();
      //             int new_x = zero[0];
      //             int new_y = zero[1];
      //             x = new_x;
      //             y = new_y;
      //             return {UNCOVER, new_x, new_y};
      //           }
      //         }
      //         return {UNCOVER, x,y};
      //       }
      //
      //       if(effective == unmarked)
      //         return {FLAG, x, y};
      //     }
      //   }
      // }
    }
    return {LEAVE,-1,-1};
    // ======================================================================
    // YOUR CODE ENDS
    // ======================================================================

}


// ======================================================================
// YOUR CODE BEGINS
// ======================================================================

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

    if((new_x < col && new_x >= 0) && (new_y < row && new_y >= 0))
    {
      if(board[new_y][new_x] == ".")
      {
        vector<int> zero;
        zero.push_back(new_x);
        zero.push_back(new_y);
        checked.push_back(zero);
      }
    }
  }
}

bool MyAI::isUncovered(int x, int y)
{
  if(board[y][x] == ".")
    return true;
  return false;
}

void MyAI::fillBoard(int x, int y, int number)
{
  std::cout << "NUMBER: " << number << std::endl;
  std::cout << "X: " << x + 1<< std::endl;
  std::cout << "Y: " << y + 1<< std::endl;
  covered -= 1;
  uncovered +=1;
  board[y][x] = to_string(number);
  printBoard();
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
