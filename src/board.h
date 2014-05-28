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

#ifndef BBS_BOARD_H
#define BBS_BOARD_H

#include <opencv2/opencv.hpp>
#include <iomanip>
#include <set>

namespace bbs
{

////////////////////////////////////////////////////////////
/// @brief board definition (list of balls sorted and evaluated)
////////////////////////////////////////////////////////////
class Board
{
public:
  struct Ball
  {
    typedef std::vector<Ball> List;
    typedef std::vector<Ball*> PtrList;
    Ball();
    Ball(cv::Point const& point, unsigned char type);

    inline bool is_left_of(Ball const* ball)
    {
      return ball->point.x > point.x;
    }

    inline bool is_right_of(Ball const* ball)
    {
      return ball->point.x < point.x;
    }

    ///! x, y pixel coordinate
    cv::Point point;
    ///! ball is enable ?
    bool disable;
    ///! ball type (from hue)
    unsigned char type;
    ///! how many same ball was in my group ?
    int count;
    ///! what's the score ?
    int score;
    ///! pointer to my neighbors
    Ball *up_left;
    Ball *up_right;
    Ball *right;
    Ball *down_right;
    Ball *down_left;
    Ball *left;
    ///! list of same ball in my group
    PtrList similar;
  };

  Board();
  ~Board();

  ////////////////////////////////////////////////////////////
  /// @brief reset the board
  ////////////////////////////////////////////////////////////
  void clear();

  ////////////////////////////////////////////////////////////
  /// @brief add a new ball
  ////////////////////////////////////////////////////////////
  void add(cv::Point const& point, unsigned char type);

  ////////////////////////////////////////////////////////////
  /// @brief sort & evaluate
  ////////////////////////////////////////////////////////////
  void rearange();

  ////////////////////////////////////////////////////////////
  /// @brief debug method print circle on ball position
  ////////////////////////////////////////////////////////////
  void drawBall(cv::Mat & game) const;

  ////////////////////////////////////////////////////////////
  /// @brief return the first y position corresponding of the first row
  ////////////////////////////////////////////////////////////
  int y_first_row() const;

  ////////////////////////////////////////////////////////////
  /// @brief lookfor balls around the target position
  ////////////////////////////////////////////////////////////
  bool find(cv::Point const& target, std::vector<Ball *> &balls) const;

  ////////////////////////////////////////////////////////////
  /// @brief return the number of ball
  ////////////////////////////////////////////////////////////
  int count_ball() const;

  ///! contains useful information about the player
  Ball player;

  ///! a radius for everyone !
  int radius;
  ///! a width for everyone !
  int width;
  ///! a height for everyone !
  int height;
  ///! to know if the mass is appear or not
  bool endGame;

  double ratio;

private:
  ////////////////////////////////////////////////////////////
  /// @brief test if a ball is link to the board
  ////////////////////////////////////////////////////////////
  bool hasTopParent(Ball *ball, Ball::PtrList previous=Ball::PtrList());

  ////////////////////////////////////////////////////////////
  /// @brief return the number of identic ball found in the current group
  ////////////////////////////////////////////////////////////
  int howSameBallInGroup(Ball::PtrList &sameballs, Ball *ball, unsigned char type);

  ////////////////////////////////////////////////////////////
  /// @brief calculate the score balls
  ///        how many balls depends on me ?
  ////////////////////////////////////////////////////////////
  int calcScore(Ball::PtrList const& sameballs, Ball *ball);

  ////////////////////////////////////////////////////////////
  /// @brief return true if all parents of ball is contains in sameballs vector
  ////////////////////////////////////////////////////////////
  bool allParent(const Ball::PtrList &sameballs, Ball::PtrList &done, Ball *ball);

  ///! contains all detected balls
  Ball::List all;

  ///! sorted array of balls
  std::vector<Ball::PtrList> balls;

  ///! contains y positions of balls
  std::set<int> all_indices;
};

inline int Board::y_first_row() const
{
  return *all_indices.begin();
}

inline int Board::count_ball() const
{
  return all.size();
}

}

#endif //  BB_BOARD_H
