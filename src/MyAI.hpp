// ======================================================================
// FILE:        MyAI.hpp
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

#ifndef MINE_SWEEPER_CPP_SHELL_MYAI_HPP
#define MINE_SWEEPER_CPP_SHELL_MYAI_HPP

#include "Agent.hpp"
#include <iostream> // temporary use
#include <queue> // for priority queue
#include <vector>
#include <map>
#include <set>
#include <algorithm>

using namespace std;

class MyAI : public Agent
{
public:
    MyAI ( int _rowDimension, int _colDimension, int _totalMines, int _agentX, int _agentY );

    Action getAction ( int number ) override;

    // ======================================================================
    // YOUR CODE BEGINS
    // ======================================================================

    void printBoard();
    pair<int,int> nextCov(); // returns random uncovered pair in random order
    void addZeroes(int x, int y); //add "tuples" of x, y coordinates around a zero into vector checked
    bool isCovered(int x, int y); //return true when tile x,y still has not been uncovered
    void fillBoard(int x, int y, int number); //fill the x,y tile with number
    int getType(int x, int y, string type); //returns how many of "type" surrounding tile x,y

private:
    int row;
    int col;
    int mines;
    int x;
    int y;
    int uncovered;
    int covered;
    vector<vector<string>> board;
    set<vector<int>> checked;

    struct priority_comp
    {
      bool operator()(std::vector<int> const& a, std::vector<int> const& b) const
      {
        // sanity checks
        assert(a.size() == 4);
        assert(b.size() == 4);

        // reverse sort puts the lowest value at the top
        return a[3] > b[3];
      }
    };

    priority_queue<vector<int>, vector<vector<int>>, priority_comp> frontier;

    // ======================================================================
    // YOUR CODE ENDS
    // ======================================================================
};

#endif //MINE_SWEEPER_CPP_SHELL_MYAI_HPP
