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

    pboard.resize(row);
    for(int r = 0; r < row; r++)
    {
      pboard[r].resize(col);
      for(int c = 0; c < col; c++)
        pboard[r][c] = ".";
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

    // cout << "LABEL: " << number << endl;
    // cout << "ROW: " << row << endl;
    // cout << "COL: " << col << endl;
    // cout << "COVERED: " << covered << endl;
    // cout << "MINES: " << mines << endl;
    // cout << "UNCOVERED: " << uncovered << endl;

    // cout << "ZEROES CHECKED GRID ----- " << endl;
    // for(int size = checked.size() - 1; size >= 0; size--)
    // {
    //   for(int c = 0; c < checked[size].size(); c++)
    //     std::cout << checked[size][c] + 1 << ' ';
    //   std::cout << std::endl;
    // }
    //if remaining tiles are not equal to amount of mines
    // if(uncovered == row*col)
    // {
    //   return {LEAVE, -1, -1};
    // }
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

    //if the amount of potential bombs equal the
    //number of tiles that are still uncovered
    if(effective==unmarked)
    {
      int adj8 [8][2] = {{-1, 1}, {0, 1}, {1 , 1},
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
          mines-=1;
          return {FLAG, x, y};
      // cout << "LABEL: " << number << endl;
      // cout << "MARKED: " << marked << endl;
      // cout << "UNMARKED: " << unmarked << endl;
      // cout << "FLAGS: " << flags << endl;
      // cout << "EFFECTIVE: " << effective << endl;
      // cout << "COVERED: " << covered << endl;
      // cout << "MINES: " << mines << endl;
      // cout << "UNCOVERED: " << uncovered << endl;
      // cout << "CHECKED SIZE: " << checked.size() << endl;
        }
      }
    }
      //when checked is exhausted, start scanning the board to flag
      if (checked.size() == 0){
        for(int r = 0; r <row; r++){
          for(int c = 0; c < col; c++){
            if (getType(c, r, ".") > 0){ //if have uncovered around
              if (!(board[r][c] == "." || board[r][c] == "-1")){ //make sure current tile isnt covered and not a flag
                effective = std::stoi(board[r][c]) - getType(c, r, "-1");
                covered = getType(c, r, ".");
                if (effective - covered == 0){
                  addFlags(c, r);
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
        flagsSet.erase(flg);
        int new_x = flg[0];
        int new_y = flg[1];
        x = new_x;
        y = new_y;
        //uncover that tile
        mines-=1;
        return {FLAG, new_x, new_y};
      }

      if (flagsSet.size() == 0){
        for(int r = 0; r <row; r++){
          for(int c = 0; c < col; c++){
            if (getType(c, r, ".") > 0){ //if have uncovered around
              if (!(board[r][c] == "." || board[r][c] == "-1")){ //make sure current tile isnt covered and not a flag
                effective = std::stoi(board[r][c]) - getType(c, r, "-1");
                if (effective == 0){
                  // std::cout << "current tile x col: " << c << " y row: " << r << endl;
                  addZeroes(c, r);
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
            }
          }
        }
      }

    // if(frontier.size() > 0)
    // {
    //   int s = frontier.size();
    //   vector<vector<int>> holding;
    //   holding.resize(s);
    //   //re-evaluate entire priority queue with new board
    //   for(int i = 0; i < s; i++)
    //   {
    //     vector<int> f = frontier.top();
    //     frontier.pop();
    //     holding[i].resize(4);
    //     holding.push_back({f[0], f[1], f[2], f[2]-getType(f[0], f[1], ".")});
    //     cout << "X: " << f[0] + 1 << " Y: " << f[1] + 1 << " label: " << f[2] << " priority: " << f[3] << endl;
    //   }
    //
    //   for(int i = 0; i < s; i++)
    //   {
    //     vector<int> f = holding[i];
    //     cout << "X: " << f[0] + 1 << " Y: " << f[1] + 1 << " label: " << f[2] << " priority: " << f[3] << endl;
    //     frontier.push({f[0],f[1],f[2],f[3]});
    //   }
    //
    //   // for(int i = 0; i < s; i++)
    //   // {
    //   //   vector<int> f = frontier.top();
    //   //   frontier.pop();
    //   //   cout << "X: " << f[0] + 1 << " Y: " << f[1] + 1 << " label: " << f[2] << " priority: " << f[3] << endl;
    //   // }
    // }
    // cout << "no good moves left, make an educated guess" << endl;

    if(frontier.size() > 0)
    {
      int s = frontier.size();
      for(int i = 0; i < s; i++)
      {
        vector<int> f = frontier.top();
        frontier.pop();
        // cout << "X: " << f[0] + 1 << " Y: " << f[1] + 1 << " priority: " << f[2] << endl;
      }
    }

    //test new heuristic
    for(int r = 0; r < row; r++){
      for(int c = 0; c < col; c++)
      {
        // ======================================================================
        // BEGINNING OF HARD CODED RULES
        // ======================================================================
        //check >=2 here
        int adj8 [8][2] = {{-1, 1}, {0, 1}, {1 , 1},
                  {-1, 0},           {1, 0},
                  {-1, -1}, {0, -1}, {1, -1}};
        vector<int> safe;
        if (!(board[r][c] == "." || board[r][c] == "-1")){
          int eff = std::stoi(board[r][c]) - getType(c, r, "-1");
          //if current tile has 3 uncovered AND effective >= 2:
          if (eff >= 2 && getType(c, r, ".") == 3){
            //put all uncovered into cur_overlap set
            set<vector<int>> cur_overlap = overlap;
            cout << "col: " << to_string(c+1) << " row: " << to_string(r+1) << endl;
            //iterate thru all tiles around to find number*
            for(int *n : adj8)
            {
              int new_x = c + n[1];
              int new_y = r + n[0];
              if((new_x < col && new_x >= 0) && (new_y < row && new_y >= 0)) //within boundary
              {
                //check if number, if yes
                if (!(board[new_y][new_x] == "." || board[new_y][new_x] == "-1")){
                  
                  cout << "colz: " << to_string(new_x+1) << " rowz: " << to_string(new_y+1) << endl;
                  //if effective of number* is less than effective, and that num has 3 uncovered
                  int cur_effective = std::stoi(board[new_y][new_x]) - getType(new_x, new_y, "-1");
                  if ((cur_effective < eff) && (getType(new_x, new_y, ".") == 3)){
                    cout << "cur_effective should be 1: " << to_string(cur_effective) << endl;
                    //get a covered* set (of coordinates) of that number
                    int count = 0;
                    cout << "size of overlap should be 3: " << to_string(overlap.size()) << endl;
                    for (auto elem: overlap){ //note here that overlap is updated to contain covered of the new coord
                      //check if there are 2 overlaps (or 2 elem in overlap also in cur_overlap)
                      if (cur_overlap.find(elem) != cur_overlap.end()){
                        cout << "this should appear twice" << endl;
                        count +=1;
                        cur_overlap.erase(elem);
                      }else{
                        cout << "this should appear once" << endl;
                        safe = elem;
                      }
                    }
                    cout << "count should be 2: " << to_string(count);
                    if (count == 2){
                      checked.insert(safe);
                      //return flag the non overlap in cur_overlap
                      vector<int> to_flag = *cur_overlap.begin();
                      return {FLAG, to_flag[0], to_flag[1]};
                    }
                  }
                }
              }

            }
          }
        }
      } //for loops closing brackets
    }


    for(int r = 0; r < row; r++){
      for(int c = 0; c < col; c++)
      {
        // ======================================================================
        // BEGINNING OF HARD CODED RULES
        // ======================================================================
        //check >=2 here
        // int adj8 [8][2] = {{-1, 1}, {0, 1}, {1 , 1},
        //           {-1, 0},           {1, 0},
        //           {-1, -1}, {0, -1}, {1, -1}};
        // vector<int> safe;
        // //if current tile has 3 uncovered AND effective >= 2:
        // if (effective >= 2 && getType(c, r, ".") == 3){
        //   //put all uncovered into overlap set
        //   set<vector<int>> cur_overlap = overlap;
        //   //iterate thru all tiles around to find number*
        //   for(int *n : adj8)
        //   {
        //     int new_x = c + n[1];
        //     int new_y = r + n[0];

        //     if((new_x < c && new_x >= 0) && (new_y < r && new_y >= 0))
        //     {
        //       if (!(board[new_y][new_x] == "." || board[new_y][new_x] == "-1")){
        //       //if effective of number* is > effective, and that num has 3 uncovered
        //       int cur_effective = std::stoi(board[new_y][new_x]) - getType(new_x, new_y, "-1");
        //       if ((cur_effective > effective) && (getType(new_x, new_y, ".") == 3)){
        //         //get a covered* set (of coordinates) of that number
        //         int count = 0;
        //         for (auto elem: overlap){ //note here that overlap is updated to contain covered of the new coord
        //           //check if there are 2 overlaps (or 2 elem in overlap also in cur_overlap)
        //           if (cur_overlap.find(elem) != cur_overlap.end()){
        //             count +=1;
        //             cur_overlap.erase(elem);
        //           }else{
        //             safe = elem;
        //           }
        //         }
        //         if (count == 2){
        //           checked.insert(safe);
        //           //return flag the non overlap in cur_overlap
        //           vector<int> to_flag = *cur_overlap.begin();
        //           return {FLAG, to_flag[0], to_flag[1]};
        //         }

        //         }
        //       }
        //     }
        //   }

        // checking basic HORIZONTAL 1 1 PATTERNS NEXT TO AN EDGE (l stands for left, r for right, u for up, d for down)
        bool l_h_edge_11_u = (board[r][c] == "1" && c == 0 && r+1 < row && board[r][c+1] == "1");
        if(l_h_edge_11_u)
        {
          if(board[r+1][c+2] == ".")
          {
            // cout << "horizontal edge 11 check" << endl;
            x = c+2;
            y = r+1;
            return {UNCOVER, x, y};
          }
        }

        bool l_h_edge_11_d = (board[r][c] == "1" && c == 0 && r-1 >= 0 && board[r][c+1] == "1");
        if(l_h_edge_11_d)
        {
          if(board[r-1][c+2] == ".")
          {
            // cout << "horizontal edge 11 check" << endl;
            x = c+2;
            y = r-1;
            return {UNCOVER, x, y};
          }
        }

        bool r_h_edge_11_u = (board[r][c] == "1" && c == col-1 && r+1 < row && board[r][c-1] == "1");
        if(r_h_edge_11_u)
        {
          if(board[r+1][c-2] == ".")
          {
            // cout << "horizontal edge 11 check" << endl;
            x = c-2;
            y = r+1;
            return {UNCOVER, x, y};
          }
        }

        bool r_h_edge_11_d = (board[r][c] == "1" && c == col-1 && r-1 >= 0 && board[r][c-1] == "1");
        if(r_h_edge_11_d)
        {
          if(board[r-1][c-2] == ".")
          {
            // cout << "horizontal edge 11 check" << endl;
            x = c-2;
            y = r-1;
            return {UNCOVER, x, y};
          }
        }

        // checking basic VERTICAL 1 1 PATTERNS NEXT TO AN EDGE (l stands for left, r for right, u for up, d for down)
        bool l_v_edge_11_d = (board[r][c] == "1" && r==row-1 && c-1>=0 && board[r-1][c]=="1");
        if(l_v_edge_11_d)
        {
          if(board[r-2][c-1] == ".")
          {
            // cout << "horizontal edge 11 check" << endl;
            x = c-1;
            y = r-2;
            return {UNCOVER, x, y};
          }
        }

        bool r_v_edge_11_d = (board[r][c] == "1" && r==row-1 && c+1<col && board[r-1][c]=="1");
        if(r_v_edge_11_d)
        {
          if(board[r-2][c+1] == ".")
          {
            // cout << "horizontal edge 11 check" << endl;
            x = c+1;
            y = r-2;
            return {UNCOVER, x, y};
          }
        }

        bool l_v_edge_11_u = (board[r][c] == "1" && r==0 && c-1>=0 && board[r+1][c]=="1");
        if(l_v_edge_11_u)
        {
          if(board[r+2][c-1] == ".")
          {
            // cout << "horizontal edge 11 check" << endl;
            x = c-1;
            y = r+2;
            return {UNCOVER, x, y};
          }
        }

        bool r_v_edge_11_u = (board[r][c] == "1" && r==0 && c+1<col && board[r+1][c]=="1");
        if(r_v_edge_11_u)
        {
          if(board[r+2][c+1] == ".")
          {
            // cout << "horizontal edge 11 check" << endl;
            x = c+1;
            y = r+2;
            return {UNCOVER, x, y};
          }
        }

        bool r_edge_v_11_u = (board[r][c] == "1" && c == col-1 && r+2<row && board[r+1][c]=="1");
        if (r_edge_v_11_u)
        {
          if(board[r+2][c-1] == ".")
          {
            x = c-1;
            y = r+2;
            return {UNCOVER, x, y};
          }
        }

        // special cases
        bool l_v_12_u = (board[r][c] == "1" && r+1<row && r+2<row && c-1>=0 && board[r+1][c]=="2" && (board[r+2][c]!="." && board[r+2][c]!="-1" ));
        if(l_v_12_u)
        {
            if(board[r+2][c-1] == ".")
            {
              //cout << "left vertical 1 2 up check" << endl;
              x = c-1;
              y = r+2;
              mines-=1;
              return {FLAG, x, y};
            }
        }

        // bool r_v_12_u = (board[r][c] == "1" && r+2<row && c+1 < col && board[r+1][c]=="2" && (board[r+2][c]!="-1" && board[r+2][c]!="." ));
        // if(r_v_12_u)
        // {
        //   if(board[r+2][c+1] == ".")
        //   {
        //     cout << "right vertical 1 2 up check" << endl;
        //     x = c+1;
        //     y = r+2;
        //     mines-=1;
        //     return {FLAG, x, y};
        //   }
        // }

        bool l_h_12_d = (board[r][c] == "1" && r-1>=0 && c+2<col && board[r][c+1]=="2" && (board[r][c+2]=="1" || board[r][c+2]=="2" )&& (board[r-1][c]=="." && board[r-1][c+1]=="." ));
        if(l_h_12_d)
        {
          if(board[r-1][c+2] == ".")
          {
            // cout << "left horizontal 1 2 down check" << endl;

            x = c+2;
            y = r-1;
            mines-=1;
            return {FLAG, x, y};
          }
        }

        bool r_h_12_d = (board[r][c] == "1" && r-1>=0 && c-2>=0 && board[r][c-1]=="2" && (board[r][c-2]!="." && board[r][c-2]!="-1" ));
        if(r_h_12_d)
        {
          if(board[r-1][c-2] == ".")
          {
            // cout << "right horizontal 1 2 down check" << endl;

            x = c-2;
            y = r-1;
            mines-=1;
            return {FLAG, x, y};
          }
        }

        bool r_h_12_u = (board[r][c] == "1" && r+1<row && c+2<col && c-1>=0 && board[r][c+1]=="2" && (board[r][c+2]!="." && board[r][c+2]!="-1")&&(board[r][c-1]!="." && board[r][c-1]!="-1"));
        if(r_h_12_u)
        {
          if(board[r+1][c+2] == ".")
          {
            // cout << "right horizontal 1 2 up check" << endl;

            x = c+2;
            y = r+1;
            mines-=1;
            return {FLAG, x, y};
          }
        }

        bool l_h_12_u = (board[r][c] == "1" && r+1<row && c-2>=0 && board[r][c-1]=="2" && (board[r][c-2]!="." && board[r][c-2]!="-1"));
        if(l_h_12_u)
        {
          if(board[r+1][c-2] == ".")
          {
            // cout << "left horizontal 1 2 up check" << endl;

            x = c-2;
            y = r+1;
            mines-=1;
            return {FLAG, x, y};
          }
        }

        // bool ledge_h_12_u = (board[r][c] == "1" && r+1<row && c==col-1 && board[r][c-1]=="2" && (board[r][c-2]!="." && board[r][c-2]!="-1"));
        // if(ledge_h_12_u)
        // {
        //   if(board[r+1][c-2] == ".")
        //   {
        //     x = c-2;
        //     y = r+1;
        //     mines-=1;
        //     return {FLAG, x, y};
        //   }
        // }

        // bool l_v_12_d = (board[r][c] == "1" && r-2>=0 && c-1>=0 && board[r-1][c]=="2" && (board[r-2][c]=="1" || board[r-2][c]=="2"));
        // if(l_v_12_d)
        // {
        //   if(board[r-2][c-1] == ".")
        //   {
        //     x = c-1;
        //     y = r-2;
        //     mines-=1;
        //     return {FLAG, x, y};
        //   }
        // }

        // bool d_h_12 = (board[r][c] == "1" && r-1>=0 && c+2<col && c-1>=0 && board[r][c+1]=="2" && (board[r][c-1] == "1" || board[r][c-1] != "2" )&& (board[r][c+1] == "1" || board[r][c+1] == "2"));
        // if (d_h_12)
        // {
        //   if(board[r-1][c+2] == ".")
        //   {
        //     x = c+2;
        //     y = r-1;
        //     mines-=1;
        //     return {FLAG, x, y};
        //   }
        // }

        bool r_v_121 = (board[r][c] == "1" && r+2<row && c+1<col && board[r+1][c] == "2" && board[r+2][c] == "1");
        if (r_v_121)
        {
          if(board[r+2][c+1] == ".")
          {
            // cout << "right vertical 121 check" << endl;
            x = c+1;
            y = r+2;
            mines-=1;
            return {FLAG, x, y};
          }
          if(board[r][c+1] == ".")
          {
            // cout << "right vertical 121 check" << endl;
            x = c+1;
            y = r;
            mines-=1;
            return {FLAG, x, y};
          }
        }

        // bool l_v_12_f = (board[r][c] == "1" && r+1<row && r+2<row && c-1>=0 && board[r+1][c]=="2" && (board[r+2][c]=="1" || board[r+2][c]=="2" ));
        // if (l_v_12_f)
        // {
        //   if(board[r+2][c-1] == "-1" && board[r][c-1] == ".")
        //   {
        //     cout << "left vertical 12 flag" << endl;
        //
        //     x = c-1;
        //     y = r;
        //     return {FLAG, x, y};
        //   }
        // }

        // ======================================================================
        // END OF HARD CODED RULES
        // ======================================================================


        // adding to frontier for guessing
        if(board[r][c] == ".")
        {
          int marked_num = 0;
          for(int i = -1; i < 9; i++)
          {
            marked_num += abs(i * getType(c, r, to_string(i)));
          }
          if(marked_num > 0)
          {
            //c is x, r is y, marked_num is priority in priority queue
            frontier.push({c, r, marked_num});
          }
        }

      }
      }//closing brackets of the nested for loops


    // if(frontier.size() > 0)
    // {
    //   int s = frontier.size();
    //   for(int i = 0; i < s; i++)
    //   {
    //     vector<int> f = frontier.top();
    //     frontier.pop();
    //     cout << "X: " << f[0] + 1 << " Y: " << f[1] + 1 << " priority: " << f[2] << endl;
    //   }
    // }
    while(frontier.size() > 0)
    {
      // cout << "CURRENTLY GUESSING!" << endl;
      vector<int> f = frontier.top();
      frontier.pop();
      if(board[f[1]][f[0]] == ".")
      {
        x = f[0];
        y = f[1];
        return {UNCOVER, x, y};
      }
    }

    
    // cout << "------------BOARD------------" << endl;
    // printBoard();
    return {LEAVE, -1 , -1};


    // pair<int, int> next = nextCov(x, y);
    // if(next.first != -1)
    // {
    //   x = next.first;
    //   y = next.second;
    //   return {UNCOVER, x, y};
    // }


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

pair<int, int> MyAI::nextCov(int target_x, int target_y)
{
  int adj8 [8][2] = {{-1, 1}, {0, 1}, {1 , 1},
                    {-1, 0},           {1, 0},
                    {-1, -1}, {0, -1}, {1, -1}};

  for(int *n : adj8)
  {
    int new_x = target_x + n[1];
    int new_y = target_y + n[0];

    // cout << "NEW_X: " << new_x << endl;
    // cout << "NEW_Y: " << new_y << endl;

    if((new_x < col && new_x >= 0) && (new_y < row && new_y >= 0))
    {
      if(isCovered(new_x, new_y))
      {
        return pair<int, int>(new_x, new_y);
      }
    }
  }
  return pair<int, int>(-1, -1);
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
  // cout << "------------BOARD------------" << endl;
  // printBoard();
}

int MyAI::getType(int x, int y, string type)
{
  int type_count = 0;
  int adj8 [8][2] = {{-1, 1}, {0, 1}, {1 , 1},
                    {-1, 0},           {1, 0},
                    {-1, -1}, {0, -1}, {1, -1}};
  overlap.clear();

  for(int *n : adj8)
  {
    int new_x = x + n[1];
    int new_y = y + n[0];

    if((new_x < col && new_x >= 0) && (new_y < row && new_y >= 0))
    {
      if(board[new_y][new_x] == type)
        type_count++;
        //add coord into coord set
        if (type == "."){
          vector<int> tile;
          tile.push_back(new_x);
          tile.push_back(new_y);
          overlap.insert(tile);
        }
    }
  }

  return type_count;
}


// ======================================================================
// YOUR CODE ENDS
// ======================================================================
