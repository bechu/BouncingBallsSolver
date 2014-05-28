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

#ifndef BBS_DETECT_BOARD_H
#define BBS_DETECT_BOARD_H

#include <opencv2/opencv.hpp>

#include "board.h"

namespace bbs
{

////////////////////////////////////////////////////////////
/// @brief this class take a game image and detect the board
////////////////////////////////////////////////////////////
class DetectBoard
{
public:
  ////////////////////////////////////////////////////////////
  /// @brief take image and look for balls
  /// @param screen_game the screen of the game
  /// @param board the meta structure of the baord
  ////////////////////////////////////////////////////////////
  bool run(const cv::Mat &screen_game, Board &board);
  double getPlayerAngle(cv::Mat const& screen_game);
private:
  ///! min angle for shooting
  constexpr static float kMinAngle = -M_PI+0.2;
  ///! max angle for shooting
  constexpr static float kMaxAngle = -0.2;
  ///! what is the offset ?
  constexpr static float kOffsetAngle = 0.01;
  ///! percent of image, minimum radius of a ball
  static constexpr float kPercentMinRadius = 0.03;
  ///! percent of image, maximum radius of a ball
  static constexpr float kPercentMaxRadius = 0.05;

  ///! a private structure to locate ball candidate
  struct Candidate
  {
    cv::Point loc;
    int radius_x;
    int radius_y;
  };

  ////////////////////////////////////////////////////////////
  /// @brief try to find a ball about the x, y position
  ////////////////////////////////////////////////////////////
  bool findBall(const Board &board, const cv::Mat &hue, Candidate & candidate, int x, int y);

  ////////////////////////////////////////////////////////////
  /// @brief find continous pixel identic
  ////////////////////////////////////////////////////////////
  Candidate findArea(const cv::Mat &hue, cv::Point const& origin);

  ////////////////////////////////////////////////////////////
  /// @brief detect all ball on a line
  ////////////////////////////////////////////////////////////
  int detectRow(Board &board, const cv::Mat &hue, Candidate const& candidate);

  ////////////////////////////////////////////////////////////
  /// @brief detect the board (treat columns)
  ////////////////////////////////////////////////////////////
  void detectBoard(Board &board, const cv::Mat &hue, Candidate const& candidate);

  ////////////////////////////////////////////////////////////
  /// @brief return true if the point if the center of a ball
  ////////////////////////////////////////////////////////////
  bool good_candidate(const Board &board, cv::Mat const& hue, cv::Point const& point);
};

}

#endif // BB_DETECT_BOARD_H
