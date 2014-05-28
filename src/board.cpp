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

#include "board.h"
#include "util.h"

namespace bbs
{

Board::Board()
  : radius(15)
  , width(0)
  , height(0)
  , endGame(false)
  , ratio(0)
{
  clear();
}

Board::~Board()
{
}

Board::Ball::Ball(cv::Point const& point, unsigned char type)
  : point(point)
  , type(type)
  , disable(false)
  , count(0)
  , score(0)
  , up_left(0)
  , up_right(0)
  , right(0)
  , down_right(0)
  , down_left(0)
  , left(0)
{
}

Board::Ball::Ball()
  : Ball(cv::Point(), 0)
{
}

void Board::clear()
{
  all.clear();
  balls.clear();
  all_indices.clear();
  endGame = false;
  ratio = 0;
}

void Board::add(cv::Point const& point, unsigned char type)
{
  // naive approach, stack all balls for the future treatment
  all.push_back(Ball(point, type));
  all_indices.insert(point.y);
}

void Board::rearange()
{
  // resize the array with the number of row
  balls.resize(all_indices.size());
  // for each one
  for(auto &b : all)
  {
    // where the row location of this ball ?
    int index = std::distance(all_indices.begin(), all_indices.find(b.point.y));
    balls[index].push_back(&b);
  }

  // determine if the order of ball for each rows
  for(auto &b : balls)
  {
    std::sort(b.begin(), b.end(),
              [](Ball const* a, Ball const* b){return a->point.x < b->point.x;});
    if(b.size() > 1)
    {
      for(int i=0;i<b.size()-1;++i)
      {
        // this ball is close to the current ?
        if(distance(b[i]->point, b[i+1]->point) <= radius*2)
        {
          b[i]->right = b[i+1];
          b[i+1]->left = b[i];
        }
      }
    }
  }

  // create link with up/down balls (like an hexagon)
  for(int i=0;i<balls.size();++i)
  {
    Ball::PtrList *cur = &balls[i];
    if(i < balls.size()-1)
    {
      Ball::PtrList *next = &balls[i+1];
      for(auto ball : *cur)
      {
        for(auto ball_next : *next)
        {
          if(distance(ball_next->point, ball->point) <= radius*2)
          {
            if(ball_next->is_left_of(ball))
            {
              ball->down_left = ball_next;
            }
            else if(ball_next->is_right_of(ball))
            {
              ball->down_right = ball_next;
            }
          }
        }
      }
    }
    if(i > 0)
    {
      Ball::PtrList *previous = &balls[i-1];
      for(auto ball : *cur)
      {
        for(auto ball_next : *previous)
        {
          if(distance(ball_next->point, ball->point) <= radius*2)
          {
            if(ball_next->is_left_of(ball))
            {
              ball->up_left = ball_next;
            }
            else if(ball_next->is_right_of(ball))
            {
              ball->up_right = ball_next;
            }
          }
        }
      }
    }
  }

  for(auto & row : balls)
  {
    for(auto b : row)
    {
      // the ball is falling down ?
      b->disable = !hasTopParent(b);
      // am i in interessant place ?
      b->count = howSameBallInGroup(b->similar, b, b->type);
    }
  }
  Ball::PtrList done;
  for(auto & row : balls)
  {
    for(auto b : row)
    {
      if(std::find(done.begin(), done.end(), b) != done.end())
        continue ;
      if(b->disable) continue ;
      // compute the score
      b->score = calcScore(b->similar, b);
      for(auto p : b->similar)
      {
        p->score = b->score;
        done.push_back(p);
      }
    }
  }


  int ok = 0;
  for(auto & row : balls)
  {
    for(auto b : row)
    {
        if(b->count > 1)
            ok ++;
    }
  }
  ratio = double(ok) / double(all.size());
}

bool Board::allParent(Ball::PtrList const& sameballs, Ball::PtrList &done, Ball *ball)
{
  if(!ball) return true;
  if(ball->disable == true) return false;

  if(std::find(sameballs.begin(), sameballs.end(), ball) != sameballs.end())
    return true;
  if(std::find(done.begin(), done.end(), ball) != done.end())
    return true;

  done.push_back(ball);

  if(ball->point.y == *all_indices.begin())
    return false;

  return allParent(sameballs, done, ball->up_left)
      && allParent(sameballs, done, ball->up_right)
      && allParent(sameballs, done, ball->left)
      && allParent(sameballs, done, ball->right)
      && allParent(sameballs, done, ball->down_left)
      && allParent(sameballs, done, ball->down_right);
}

int Board::calcScore(const Ball::PtrList &sameballs, Ball *ball)
{
  if(!ball) return 0;
  if(ball->disable == true) return 0;

  int cpt = 0;
  for(auto & bb : balls)
  {
    for(auto b : bb)
    {
      Ball::PtrList done;
      if(allParent(sameballs, done, b))
      {
        cpt++;
      }
    }
  }
  return cpt;
}

void Board::drawBall(cv::Mat & game) const
{
  for(auto & bb : balls)
  {
    for(auto b : bb)
    {
      std::stringstream ss;
      ss << b->score;
      cv::putText(game, ss.str(), cv::Point(b->point.x-10, b->point.y+10), cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar());
      cv::Scalar color(100, 100, 230);
      if(b->disable == false)
        color = cv::Scalar(5, 200, 0);
      cv::circle(game, b->point, radius, color, 3);
    }
  }
}

int Board::howSameBallInGroup(Ball::PtrList &sameballs, Ball *ball, unsigned char type)
{
  if(!ball) return 0;
  if(ball->disable == true) return 0;
  if(std::find(sameballs.begin(), sameballs.end(), ball) != sameballs.end()) return 0;
  if(type != ball->type) return 0;

  sameballs.push_back(ball);

  return 1 + howSameBallInGroup(sameballs, ball->right, type)
      + howSameBallInGroup(sameballs, ball->left, type)
      + howSameBallInGroup(sameballs, ball->down_right, type)
      + howSameBallInGroup(sameballs, ball->down_left, type)
      + howSameBallInGroup(sameballs, ball->up_right, type)
      + howSameBallInGroup(sameballs, ball->up_left, type);
}

bool Board::find(cv::Point const& target, Ball::PtrList & myballs) const
{
  for(auto bb = balls.rbegin(); bb!=balls.rend();++bb)
  {
    for(auto b : *bb)
    {
      if(b->disable == true) continue ;
      if(circlesColliding(b->point.x, b->point.y, radius*0.6, target.x, target.y, radius*0.8))
      {
        myballs.push_back(b);
      }
    }
  }
  return myballs.size() > 0;
}

bool Board::hasTopParent(Board::Ball *ball, Board::Ball::PtrList previous)
{
  if(!ball) return false;
  // this ball is a top ball ;)
  if(ball->point.y == y_first_row())
    return true;
  // seen
  if(std::find(previous.begin(), previous.end(), ball) != previous.end())
    return false;
  previous.push_back(ball);
  return    hasTopParent(ball->up_left, previous)
      || hasTopParent(ball->up_right, previous)
      || hasTopParent(ball->left, previous)
      || hasTopParent(ball->right, previous)
      || hasTopParent(ball->down_left, previous)
          || hasTopParent(ball->down_right, previous);
}

}
