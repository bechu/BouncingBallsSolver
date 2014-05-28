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

#ifndef BBS_UTIL_H
#define BBS_UTIL_H

#include <opencv2/opencv.hpp>
#include <cmath>

namespace bbs
{

////////////////////////////////////////////////////////////
/// @brief compute euclidian distance
////////////////////////////////////////////////////////////
inline int distance(cv::Point const& pt1, cv::Point const& pt2)
{
    return std::sqrt((pt2.x-pt1.x)*(pt2.x-pt1.x) + (pt2.y-pt1.y)*(pt2.y-pt1.y));
}

////////////////////////////////////////////////////////////
/// @brief check if two liens have an intersectio
////////////////////////////////////////////////////////////
inline bool intersection(cv::Point const& o1, cv::Point const& p1, cv::Point const& o2, cv::Point const& p2,
                  cv::Point &r)
{
    cv::Point x = o2 - o1;
    cv::Point d1 = p1 - o1;
    cv::Point d2 = p2 - o2;

    float cross = d1.x*d2.y - d1.y*d2.x;
    if (abs(cross) < /*EPS*/1e-8)
        return false;

    double t1 = (x.x * d2.y - x.y * d2.x)/cross;
    r = o1 + d1 * t1;
    return true;
}

////////////////////////////////////////////////////////////
/// @brief check circle collision
////////////////////////////////////////////////////////////
inline bool circlesColliding(int x1,int y1,int radius1,int x2,int y2,int radius2)
{
    //compare the distance to combined radii
    int dx = x2 - x1;
    int dy = y2 - y1;
    int radii = radius1 + radius2;
    if ( ( dx * dx )  + ( dy * dy ) < radii * radii )
    {
        return true;
    }
    return false;
}

}

#endif  // BBS_UTIL_H
