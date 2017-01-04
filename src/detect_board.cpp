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

#include "detect_board.h"
#include "util.h"

namespace bbs
{

bool DetectBoard::run(cv::Mat const& screen_game, Board &board)
{
  //  convert to hsv !
  cv::Mat out(screen_game.size(), CV_8UC3);
  cv::cvtColor(screen_game, out, CV_RGB2HSV);

  // split out
  std::vector<cv::Mat> channel;
  cv::split(out, channel);

  // we only need hue (shortcut)
  cv::Mat hue = channel[0];

  // clear the board
  board.clear();
  board.width = screen_game.size().width;
  board.height = screen_game.size().height;

  // this is the player settings
  board.player.point.x = 210;
  board.player.point.y = 346;
  board.player.type = hue.at<unsigned char>(board.player.point.y, board.player.point.x);

  // looking for the first ball
  Candidate candidate;
  bool found = false;
  for(int y=0;y<screen_game.size().height && found == false;y+=20)
  {
    for(int x=0;x<screen_game.size().width && found == false;x+=20)
    {
      if(findBall(board, hue, candidate, x, y) == true)
      {
        if(good_candidate(board, hue, candidate.loc))
        {
          // found one, let's go !
          found = true;
        }
      }
    }
  }
  // noone found, return something bad
  if(! found )
  {
    return false;
  }

  // ok detect all the board based on the first ball
  detectBoard(board, hue, candidate);

  // reagange the ball :
  //  - sort
  //  - define parents/children links
  //  - define score for each ball
  board.rearange();

  return true;
}

double DetectBoard::getPlayerAngle(cv::Mat const& screen_game)
{
  //  convert to hsv !
  cv::Mat out(screen_game.size(), CV_8UC3);
  cv::cvtColor(screen_game, out, CV_RGB2HSV);

  // split out
  std::vector<cv::Mat> channel;
  cv::split(out, channel);

  // we only need hue (shortcut)
  cv::Mat hue = channel[0];

  double angle = 0;
  cv::Point player(211, 346);

  int count = 0;
  for(float k=kMinAngle;k<kMaxAngle;k+=kOffsetAngle)
  {
    cv::Point p;
    p.x = player.x + 50 * cos(k);
    p.y = player.y + 50 * sin(k);
    if(hue.at<unsigned char>(p.y, p.x) == 90)
      count++;
    if(count == 10)
    {
      angle = k;
      break;
    }
  }
  return angle;
}

void DetectBoard::detectBoard(Board & board, cv::Mat const& hue, Candidate const& candidate)
{
  // detect ball for the same row
  detectRow(board, hue, candidate);

  Candidate c = candidate;

  // for each balls below the current line ...
  while(c.loc.y < hue.size().height*0.9)
  {
    c.loc.y += (candidate.radius_y*2) * 0.87; // magic number :(
    c.loc.x += candidate.radius_x;
    if(detectRow(board, hue, c) == 0)
      break;
  }


  // for each balls upper the current line
  c.loc = candidate.loc;
  while(c.loc.y > 0)
  {
    c.loc.y -= (candidate.radius_y*2) * 0.86; // magic number :'(
    c.loc.x += candidate.radius_x;
    if(detectRow(board, hue, c) == 0)
      break;
  }

  Candidate mass;
  mass = findArea(hue, cv::Point(c.loc.x, c.loc.y));
  if(mass.radius_x > hue.size().width*0.4)
    board.endGame = true;
}

namespace
{

bool pixelon(cv::Mat const & hue, cv::Point const& p, unsigned char ref)
{
  return abs(hue.at<unsigned char>(p.y, p.x) - ref) < 2;
}

}

bool DetectBoard::good_candidate(Board const& board, cv::Mat const& hue, cv::Point const& point)
{
  // point in the picture
  if(point.x - board.radius < 0) return false;
  if(point.x + board.radius >= hue.size().width) return false;
  if(point.y - board.radius < 0) return false;
  if(point.y + board.radius >= hue.size().height) return false;

  unsigned char ref = hue.at<unsigned char>(point.y, point.x);

  // if not the ball player
  if(distance(point, board.player.point) < board.radius)
  {
    return false;
  }

  // test some strategic pixels to etablish the circle shape is present or not
  return pixelon(hue, cv::Point(point.x-board.radius/2, point.y-board.radius/2), ref)
      && pixelon(hue, cv::Point(point.x+board.radius/2, point.y+board.radius/2), ref)
      && pixelon(hue, cv::Point(point.x-board.radius/2, point.y+board.radius/2), ref)
      && pixelon(hue, cv::Point(point.x+board.radius/2, point.y-board.radius/2), ref)
      && !pixelon(hue, cv::Point(point.x-board.radius, point.y-board.radius+7), ref)
      && !pixelon(hue, cv::Point(point.x+board.radius, point.y-board.radius+7), ref)
      && !pixelon(hue, cv::Point(point.x-board.radius, point.y+board.radius-7), ref)
      && !pixelon(hue, cv::Point(point.x+board.radius, point.y+board.radius-7), ref);
}

int DetectBoard::detectRow(Board & board, cv::Mat const& hue, Candidate const& candidate)
{
  int how = 0;
  for(int x=candidate.loc.x;x<hue.size().width;x+=(candidate.radius_x*2))
  {
    if(good_candidate(board, hue, cv::Point(x, candidate.loc.y)))
    {
      board.add(cv::Point(x, candidate.loc.y), hue.at<unsigned char>(candidate.loc.y, x));
      how ++;
    }
  }

  for(int x=candidate.loc.x-candidate.radius_x*2;x>0;x-=(candidate.radius_x*2))
  {
    if(good_candidate(board, hue, cv::Point(x, candidate.loc.y)))
    {
      board.add(cv::Point(x, candidate.loc.y), hue.at<unsigned char>(candidate.loc.y, x));
      how++;
    }
  }
  return how;
}

bool DetectBoard::findBall(Board const& board, cv::Mat const& hue, Candidate & candidate, int x, int y)
{
  candidate = findArea(hue, cv::Point(x, y));
  int first_radius_y = candidate.radius_y;
  candidate = findArea(hue, candidate.loc);

  // maybe it's a special case : two ball with the same color side by side
  if(abs(candidate.radius_x - 2 * first_radius_y) < 5)
  {
    first_radius_y = candidate.radius_x / 2;
    cv::Point pt(candidate.loc.x-first_radius_y, candidate.loc.y);
    if(good_candidate(board, hue, pt))
    {
      candidate.loc.x = pt.x;
      candidate.radius_y = candidate.radius_x = first_radius_y;
    }
  }

  return (candidate.radius_x == candidate.radius_y
          && candidate.radius_x >= hue.size().width * kPercentMinRadius
          && candidate.radius_x < hue.size().width * kPercentMaxRadius);
}

DetectBoard::Candidate DetectBoard::findArea(cv::Mat const& hue, cv::Point const& origin)
{
  if(origin.x < 0 || origin.y < 0 || origin.x >= hue.size().width || origin.y >= hue.size().height)
    return Candidate();
  // it's not an elegant method ...
  unsigned char c = hue.at<unsigned char>(origin.y, origin.x);
  int minx = origin.x;
  int maxx = origin.x;
  int miny = origin.y;
  int maxy = origin.y;

  while(maxx < hue.size().width)
  {
    if(abs(hue.at<unsigned char>(origin.y, maxx) - c) > 2)
      break;
    ++maxx;
  }
  while(minx > 0)
  {
    if(abs(hue.at<unsigned char>(origin.y, minx) - c) > 2)
      break;
    --minx;
  }
  while(miny > 0)
  {
    if(abs(hue.at<unsigned char>(miny, origin.x) - c) > 2)
      break;
    --miny;
  }
  while(maxy < hue.size().height)
  {
    if(abs(hue.at<unsigned char>(maxy, origin.x) -c) > 2)
      break;
    ++maxy;
  }
  Candidate candidate;
  candidate.radius_x = (maxx-minx) / 2;
  candidate.radius_y = (maxy-miny) / 2;
  candidate.loc.x = minx + candidate.radius_x;
  candidate.loc.y = miny + candidate.radius_y;
  return candidate;
}

}
