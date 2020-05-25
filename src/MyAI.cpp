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
      // cout << "LABEL: " << number << endl;
      // cout << "UNMARKED: " << unmarked << endl;
      //if there are still covered tiles, and it is confirmed that there are no more bombs
      if(unmarked > 0 && effective == 0)
        //add all the surrounding tiles into uncoverable vector
        addZeroes(x, y);


      frontier.push({x, y, number, unmarked - number});

      //if theres no potential bomb around a tile or there are still tiles in checked to pop
      if(effective == 0 || checked.size() > 0)
      {
        //first uncover remaining coords in checked
        if(checked.size() > 0)
        {
          //get the first "tuple", also remove it from the set
          vector<int> zero = *checked.begin(); //iterator that points to the beginning
                                               //of the set, then dereference it to obtain value
          checked.erase(zero);
          int new_x = zero[0];
          int new_y = zero[1];
          x = new_x;
          y = new_y;

          //uncover that tile
          return {UNCOVER, new_x, new_y};
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

      //when checked is exhausted, start scanning the board to flag
      if (checked.size() == 0){
        for(int r = 0; r <row; r++){
          for(int c = 0; c < col; c++){
            if (getType(r, c, ".") > 0){ //if have uncovered around
              if (!(board[r][c] == "." || board[r][c] == "-1")){ //make sure current tile isnt covered and not a flag
                effective = std::stoi(board[r][c]) - getType(r, c, "-1");
                covered = getType(r, c, ".");
                if (effective - covered == 0){
                  addFlags(r, c);
                }
              }
            }
          }
        }
      }

      if(flagsSet.size() > 0)
      {
        //get the first "tuple", also remove it from the set
        vector<int> flg = *flagsSet.begin(); //iterator that points to the beginning
                                              //of the set, then dereference it to obtain value
        checked.erase(flg);
        int new_x = flg[0];
        int new_y = flg[1];
        x = new_x;
        y = new_y;
        //uncover that tile
        return {FLAG, new_x, new_y};
      }

      if(frontier.size() > 0)
      {
        int s = frontier.size();
        vector<vector<int>> holding;
        holding.resize(s);
        //re-evaluate entire priority queue with new board
        for(int i = 0; i < s; i++)
        {
          vector<int> f = frontier.top();
          frontier.pop();
          holding[i].resize(4);
          holding.push_back({f[0], f[1], f[2], f[2]-getType(f[0], f[1], ".")});
          // cout << "X: " << f[0] + 1 << " Y: " << f[1] + 1 << " label: " << f[2] << " priority: " << f[3] << endl;
        }

        for(int i = 0; i < s; i++)
        {
          vector<int> f = holding[i];
          // cout << "X: " << f[0] + 1 << " Y: " << f[1] + 1 << " label: " << f[2] << " priority: " << f[3] << endl;
          frontier.push({f[0],f[1],f[2],f[3]});
        }

        for(int i = 0; i < s; i++)
        {
          vector<int> f = frontier.top();
          frontier.pop();
          // cout << "X: " << f[0] + 1 << " Y: " << f[1] + 1 << " label: " << f[2] << " priority: " << f[3] << endl;
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
  int adj8 [8][2] = {{-1, 1}, {0, 1}, {1 , 1},
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
      if(isCovered(new_x, new_y))
      {
        vector<int> zero;
        zero.push_back(new_x);
        zero.push_back(new_y);
        checked.insert(zero);
      }
    }
  }
}

void MyAI::addFlags(int x, int y)
{
  int adj8 [8][2] = {{-1, 1}, {0, 1}, {1 , 1},
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
      if(isCovered(new_x, new_y))
      {
        vector<int> flg;
        flg.push_back(new_x);
        flg.push_back(new_y);
        flagsSet.insert(flg);
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
  printBoard();
}

int MyAI::getType(int x, int y, string type)
{
  int type_count = 0;
  int adj8 [8][2] = {{-1, 1}, {0, 1}, {1 , 1},
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
