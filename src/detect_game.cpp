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

#include "detect_game.h"

namespace bbs
{

DetectGame::DetectGame()
{
}

bool DetectGame::loadMotif(std::string const& motif)
{
  tmpl_ = cv::imread(motif);
  return tmpl_.data;
}

cv::Rect DetectGame::run(cv::Mat const& screenshot)
{
  if(!screenshot.data)
    return cv::Rect();
  if(!tmpl_.data)
    return cv::Rect();

  cv::Mat result;

  cv::matchTemplate(screenshot, tmpl_, result, CV_TM_SQDIFF_NORMED);

  cv::normalize(result, result, 0, 1, cv::NORM_MINMAX, -1, cv::Mat() );

  double minVal; double maxVal; cv::Point minLoc; cv::Point maxLoc;
  cv::Point matchLoc;

  cv::minMaxLoc( result, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat() );

  matchLoc = minLoc;

  cv::Mat sub(screenshot,  cv::Rect(matchLoc,
                                    cv::Point( matchLoc.x + tmpl_.cols , matchLoc.y + tmpl_.rows )));

  for(int x=0;x<tmpl_.size().width;++x)
  {
    for(int y=0;y<tmpl_.size().height;++y)
    {
      cv::Vec3b diff = sub.at<cv::Vec3b>(y, x)-tmpl_.at<cv::Vec3b>(y, x);
      if(abs(diff[0]) < kThreshold && abs(diff[1]) < kThreshold && abs(diff[2]) < kThreshold)
        continue;
      return cv::Rect();
    }
  }

  return cv::Rect(
        cv::Point(matchLoc.x+tmpl_.cols, matchLoc.y),
        cv::Point(matchLoc.x + tmpl_.cols + kGameWidth , matchLoc.y + tmpl_.rows + kGameHeight ));
}

}
