/////////////////////////////////////////////////////////////////////////
/// BouncingBallsSolver
/// Copyright (C) 2014 Jérôme Béchu
///
/// This program is free software: you can redistribute it and/or modify
/// it under the terms of the GNU General Public License as published by
/// the Free Software Foundation, either version 3 of the License, or
/// (at your option) any later version.
///
/// This program is distributed in the hope that it will be useful,
/// but WITHOUT ANY WARRANTY; without even the implied warranty of
/// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
/// GNU General Public License for more details.
///
/// You should have received a copy of the GNU General Public License
/// along with this program.  If not, see <http://www.gnu.org/licenses/>.
/////////////////////////////////////////////////////////////////////////

#ifndef BBS_SOLVER_H
#define BBS_SOLVER_H

#include "board.h"

namespace bbs
{

////////////////////////////////////////////////////////////
/// @brief BouncingBall Solver :
///      - list all possible solution depending of the context
///      - choose the better option (in fact try to do this)
////////////////////////////////////////////////////////////
class Solver
{
public:

  ///! an effective solution
  struct Solution
  {
    Solution(float angle=0)
      : rebound(0)
      , angle(angle)
      , score(-1)
      , y(std::numeric_limits<int>::max())
    {
    }

    ///! this is the list of balls concerned by this solution
    Board::Ball::PtrList balls;
    ///! how many rebound have to use for this solution ?
    int rebound;
    ///! what is the angle
    float angle;
    ///! what it the score
    int score;
    ///! what is my final y ?
    int y;
  };

  ////////////////////////////////////////////////////////////
  /// @brief list solutions, and choose one
  ////////////////////////////////////////////////////////////
  Solution run(Board const& board);

  ////////////////////////////////////////////////////////////
  /// @brief draw the solution on the picture ** debug **
  ////////////////////////////////////////////////////////////
  void draw(cv::Mat &game, Solution const & solution, Board const& board);

private:
  ///! min angle for shooting
  constexpr static float kMinAngle = -M_PI+0.2;
  ///! max angle for shooting
  constexpr static float kMaxAngle = -0.2;
  ///! what is the offset ?
  constexpr static float kOffsetAngle = 0.01;

  ////////////////////////////////////////////////////////////
  /// @brief test a particular trajectory
  ////////////////////////////////////////////////////////////
  bool testTrajectory(cv::Point const& origin, float angle, const Board &board, Solver::Solution & solution, cv::Mat *game=0);

  ////////////////////////////////////////////////////////////
  /// @brief test a collision with balls from board
  ////////////////////////////////////////////////////////////
  bool collision(Board const& board, cv::Point const& origin, Solver::Solution & solution, float angle, cv::Point & result);

  ////////////////////////////////////////////////////////////
  /// @brief list possible solution
  ////////////////////////////////////////////////////////////
  void discoverSolution(Board const& board);

  Solver::Solution onlyStrike(Board const& board);

  Solver::Solution endTheGame(Board const& board);

  ////////////////////////////////////////////////////////////
  /// @brief select the one
  ////////////////////////////////////////////////////////////
  Solver::Solution takeDecision(const Board &board);

  ///! internal list of solution
  std::vector<Solution> solutions_;
};

}

#endif // BB_SOLVER_H
