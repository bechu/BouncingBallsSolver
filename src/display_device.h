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

#ifndef BBS_DISPLAY_DEVICE_H
#define BBS_DISPLAY_DEVICE_H

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <opencv2/opencv.hpp>

namespace bbs
{

////////////////////////////////////////////////////////////
/// @brief take screenshot / control the mouse
/// 		works only on X11
////////////////////////////////////////////////////////////
class DisplayDevice
{
public:
  DisplayDevice();
  ~DisplayDevice();


  ////////////////////////////////////////////////////////////
  /// @brief capture all the screen
  ////////////////////////////////////////////////////////////
  cv::Mat capture();

  ////////////////////////////////////////////////////////////
  /// @brief capture a part of the scren
  ////////////////////////////////////////////////////////////
  cv::Mat capture(cv::Rect const& rect);

  ////////////////////////////////////////////////////////////
  /// @brief move the mouse to a position and left click
  ////////////////////////////////////////////////////////////
  void mouseMoveAndClick(int x, int y);
private:
  ////////////////////////////////////////////////////////////
  /// @brief capture a specified arear of the screen
  ////////////////////////////////////////////////////////////
  cv::Mat capture(int x, int y, int width, int height);

  ////////////////////////////////////////////////////////////
  /// @brief move the mouse to x, y
  ////////////////////////////////////////////////////////////
  void moveTo(int x, int y);

public:
  ////////////////////////////////////////////////////////////
  /// @brief left click
  ////////////////////////////////////////////////////////////
  void click();

  ///! pointer to the display
  Display * display_;
};

}

#endif // BB_DISPLAY_DEVICE_H
