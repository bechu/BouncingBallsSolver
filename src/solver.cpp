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

#include "solver.h"
#include "util.h"

namespace bbs
{

Solver::Solution Solver::run(Board const& board)
{
  discoverSolution(board);
  return  takeDecision(board);
}

void Solver::discoverSolution(Board const& board)
{
  cv::Point origin = board.player.point;
  solutions_.clear();

  for(float k=kMinAngle;k<kMaxAngle;k+=kOffsetAngle)
  {
    Solution solution(k);
    if(testTrajectory(origin, k, board, solution))
      solutions_.push_back(solution);
  }
}

Solver::Solution Solver::onlyStrike(Board const& board)
{
  int score = std::numeric_limits<int>::min();
  Solution best = solutions_[0];

  for(auto solution : solutions_)
  {
    if(solution.score == score && solution.rebound == 0)
    {
      best = solution;
      score = solution.score;
    }
    else if(solution.score > score)
    {
      best = solution;
      score = solution.score;
    }
  }
  return best;
}

Solver::Solution Solver::endTheGame(Board const& board)
{
  {
    int score = std::numeric_limits<int>::min();
    int count = std::numeric_limits<int>::min();
    Solution best;
    for(auto solution : solutions_)
    {
      if(solution.score >= score
         && solution.rebound == 0
         && solution.balls.size() > count
         && solution.balls.size() > 1)
      {
        best = solution;
        score = solution.score;
        count = solution.balls.size();
      }
    }
    if(score != std::numeric_limits<int>::min())
    {
      return best;
    }
  }

  return onlyStrike(board);
}

Solver::Solution Solver::takeDecision(Board const& board)
{
  // if no solution, return a random action
  if(solutions_.size() == 0)
    return Solution(cv::theRNG().uniform(kMinAngle, kMaxAngle));

  if(board.endGame == false || board.count_ball() >= 10)
  {
    return onlyStrike(board);
  }

  return endTheGame(board);
}

// check for ball collision every positons each offset on a line
bool Solver::collision(const Board &board,
                       cv::Point const& origin,
                       Solver::Solution & solution,
                       float angle,
                       cv::Point & result)
{
  int x = 0;
  cv::Point p(origin.x + (board.radius/2) * x * cos(angle), origin.y + (board.radius/2) * x * sin(angle));
  while(p.x > 0 && p.y > 0 && p.x < board.width && p.y < board.height)
  {
    if(board.find(p, solution.balls))
    {
      solution.score = 0;

      for(auto b : solution.balls)
      {
        if(b->point.y < solution.y)
        {
          solution.y = b->point.y;
        }
        if(b->type == board.player.type)
        {
          // get the local maximum
          if(b->score > solution.score)
            solution.score = b->score;
        }
      }

      if(solution.y > board.height * 0.9)
        solution.score = -100;

      result = p;
      return true;
    }
    p.x = origin.x + (board.radius/2) * x * std::cos(angle);
    p.y = origin.y + (board.radius/2) * x * std::sin(angle);
    x++;
  }
  return false;
}

// test a particular trajectory (angle)
bool Solver::testTrajectory(cv::Point const& origin, float angle, const Board &board, Solver::Solution & solution, cv::Mat *game)
{
  if(origin.x < 0 || origin.y < 0 || origin.x > board.width || origin.y > board.height)
    return false;
  if(solution.rebound > 1)
    return false;

  cv::Point limit;
  cv::Point dest(origin.x + board.radius * cos(angle),
                 origin.y + board.radius * sin(angle));

  bool tobe_continued = false;
  if(dest.x <= origin.x)
  {
    tobe_continued = intersection(cv::Point(board.radius+5, 0),
                                  cv::Point(board.radius+5, board.height),
                                  origin, dest, limit);
  }
  else
  {
    tobe_continued = intersection(cv::Point(board.width - board.radius - 1, 0),
                                  cv::Point(board.width - board.radius - 1, board.height),
                                  origin, dest, limit);
  }
  if(game)
    cv::line(*game, origin, limit, cv::Scalar(255, 255, 255), 1);
  cv::Point p;
  if(collision(board, origin, solution, angle, p))
  {
    if(game)
    {
      cv::circle(*game, p, board.radius, cv::Scalar(255, 25, 5), 2);
      std::stringstream ss;
      ss << solution.score;
      cv::putText(*game, ss.str(), cv::Point(p.x-5, p.y+5), cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar(255, 255, 255));
    }
    return true;
  }
  if(tobe_continued)
  {
    angle = std::atan2(origin.y-limit.y, origin.x-limit.x);
    angle = M_PI*2 - angle;

    solution.rebound ++;
    return testTrajectory(limit, angle, board, solution, game);
  }
  return false;
}

void Solver::draw(cv::Mat &game, Solution const& solution, const Board &board)
{
  cv::Point origin = board.player.point;
  Solution s;
  testTrajectory(origin, solution.angle, board, s, &game);
}

}
