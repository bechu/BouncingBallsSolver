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

#include <unistd.h> // usleep

#include "display_device.h"
#include "detect_game.h"
#include "detect_board.h"
#include "solver.h"

int main()
{
  // initialize the opencv random seed
  cv::theRNG().state = time(NULL);
  // instance use to take screenshot and control the mouse
  bbs::DisplayDevice display_device;
  // use to detect the area of the game
  bbs::DetectGame game_detector;
  try
  {
      game_detector.loadMotif("motif.png");
  }
  catch( ... )
  {
      std::cerr<<"motif.png not found ..."<<std::endl;
  }

  // convert the image to a board instance
  bbs::DetectBoard board_detector;
  // take decisions
  bbs::Solver solver;
  // the current board
  bbs::Board board;
  bbs::Solver::Solution solution;

  while(1)
  {
    // take full desktop screenshot
    cv::Mat screenshot = display_device.capture();
    cv::Rect game_rect = game_detector.run(screenshot);

    // found something ...
    if(game_rect.area() > 0)
    {
      while(1)
      {

        cv::Mat screenshot = display_device.capture(game_rect);
        cv::imwrite("/home/jerome/test.png", screenshot);

        if(board_detector.run(screenshot, board) == false)
        {
          break;
        }
        // take a decision !
        solution = solver.run(board);

        //// and apply
        display_device.mouseMoveAndClick(
              game_rect.x + board.player.point.x + 100 * std::cos(solution.angle),
              game_rect.y + board.player.point.y + 100 * std::sin(solution.angle));

              usleep(10000);
       screenshot = display_device.capture(game_rect);
       display_device.click();

       board.drawBall(screenshot);
       solver.draw(screenshot, solution, board);
       cv::imshow("game", screenshot);
       cv::waitKey(1);
       //// cv::imwrite(ss.str(), screenshot);
        //int overloop = 0;
        //while(1)
        //{
        //    screenshot = display_device.capture(game_rect);
        //  double angle = board_detector.getPlayerAngle(screenshot);
       //// std::stringstream ss;
       //// ss << "./out/"<<counter++<<".png";
       //// board.drawBall(screenshot);
       //// solver.draw(screenshot, solution, board);
       //// cv::imwrite(ss.str(), screenshot);
        //  if(std::fabs(solution.angle-angle) < 0.1)
        //    break;
        //  usleep(20);
        //  if(overloop++ > 15)
        //  {
        //    std::cerr<<"oups error angle ..."<<std::endl;
        //    std::cerr<<"solution angle : "<<solution.angle<<" and target angle :"<<angle<<std::endl;
        //    break;
        //  }
        //}
        //
      }
    }
    cv::waitKey(1000);
  }
  return 0;
}

//  int c = 0;
//  while(0)
//  {
//    cv::Mat src;
//    std::stringstream ss;
//    //if(board.run(screenshot, 0) == false) break;
//    ss << "./out/" << c << ".png";
//    src = cv::imread(ss.str());
//    std::cout<<ss.str()<<std::endl;
//    board_detector.run(src, board);
//    //bbs::Solver::Solution solution = solver.run(board);
//    board.drawBall(src);
//    //solver.draw(src, solution, board);
//    cv::imshow("board", src);
//    int u = cv::waitKey(0);
//    if(u == 1113937)
//      c--;
//    if(u == 1113939)
//      c++;
//    if(u == 1048603)
//      return 0;
//  }
