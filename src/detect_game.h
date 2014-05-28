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

#ifndef BBS_DETECT_GAME_H
#define BBS_DETECT_GAME_H

#include <opencv2/opencv.hpp>

namespace bbs
{

////////////////////////////////////////////////////////////
/// @brief detect the game zone based on a motif
///      take an area of the game (top left corner)
///      and try to found it in the screenshot
///      if success return the real game square
////////////////////////////////////////////////////////////
class DetectGame
{
public:
    ////////////////////////////////////////////////////////////
    /// @brief the motif if the part of the game board
    ////////////////////////////////////////////////////////////
    DetectGame(std::string const& motif);

    ////////////////////////////////////////////////////////////
    /// @brief try to find a source_image similar zone
    ////////////////////////////////////////////////////////////
    cv::Rect run(cv::Mat const& screenshot);
private:
    ///! threshold
    constexpr static int kThreshold = 20;
    ///! game width
    constexpr static int kGameWidth = 422;
    ///! game height
    constexpr static int kGameHeight = 260;

    ///! instance to keep the motif
    cv::Mat tmpl_;
};

}

#endif // BB_DETECT_GAME_H
