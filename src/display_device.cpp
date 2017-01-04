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

#include <unistd.h>

#include "display_device.h"

namespace bbs
{

#define VARIABLES_DECLARATION \
    unsigned char _rshift, _gshift, _bshift, _ashift; \
    unsigned int  _rmask,  _gmask,  _bmask,  _amask; \
    unsigned char _rwidth, _gwidth, _bwidth, _awidth

#define InitRGBShiftsAndMasks(rs,rw,gs,gw,bs,bw,as,aw) \
  do { \
  _rshift = (rs);              \
  _rmask  = (1<<(rw)) -1;      \
  _rwidth = (rw);              \
  _gshift = (gs);              \
  _gmask  = (1<<(gw)) -1;      \
  _gwidth = (gw);              \
  _bshift = (bs);              \
  _bmask  = (1<<(bw)) -1;      \
  _bwidth = (bw);              \
  _amask  = (1<<(aw)) -1;      \
  _ashift = (as);              \
  _awidth = (aw);              \
} while (0)


#define PixelToRGB(pixel,r,g,b)  \
  do { \
  (r) = (pixel >> _rshift) & _rmask; \
  (g) = (pixel >> _gshift) & _gmask; \
  (b) = (pixel >> _bshift) & _bmask; \
} while (0)

DisplayDevice::DisplayDevice()
{
  display_ = XOpenDisplay( NULL );
}

DisplayDevice::~DisplayDevice()
{
  XCloseDisplay(display_);
}

cv::Mat DisplayDevice::capture()
{
  int width = XDisplayWidth(display_, 0);
  int height = XDisplayHeight(display_, 0);
  return capture(0, 0, width, height);
}

cv::Mat DisplayDevice::capture(cv::Rect const& rect)
{
  return capture(rect.x, rect.y, rect.width, rect.height);
}

cv::Mat DisplayDevice::capture(int x, int y, int width, int height)
{
  XImage *img = XGetImage(display_, DefaultRootWindow(display_), x, y, width, height, AllPlanes, ZPixmap);
  VARIABLES_DECLARATION;
  InitRGBShiftsAndMasks(16,8,8,8,0,8,0,8);

  unsigned long  pixel;
  unsigned	sr, sg, sb;

  cv::Mat capture(height, width, CV_8UC3);
  for(int i=0;i<width;++i)
  {
    for(int j=0;j<height;++j)
    {
      pixel = XGetPixel( img, i, j);
      PixelToRGB(pixel, sr, sg, sb);
      capture.at<cv::Vec3b>(j, i)[0] = sb;
      capture.at<cv::Vec3b>(j, i)[1] = sg;
      capture.at<cv::Vec3b>(j, i)[2] = sr;
    }
  }

  // very important, don't forget to release memory ;)
  XDestroyImage(img);

  return capture;
}

void DisplayDevice::mouseMoveAndClick(int x, int y)
{
  int width = XDisplayWidth(display_, 0);
  int height = XDisplayHeight(display_, 0);
  if(x > 0 && y > 0 && x < width && y < height)
  {
    moveTo(x, y);
  }
}

void DisplayDevice::moveTo(int x, int y)
{
  XWarpPointer(display_, None, DefaultRootWindow(display_), 0, 0, 0, 0, x, y);
  XFlush(display_);
  /*while(true)
  {
    XEvent event;
    memset(&event, 0x00, sizeof(event));
    XQueryPointer(display_, RootWindow(display_, DefaultScreen(display_)), &event.xbutton.root, &event.xbutton.window, &event.xbutton.x_root, &event.xbutton.y_root, &event.xbutton.x, &event.xbutton.y, &event.xbutton.state);
    if(event.xbutton.x == x && event.xbutton.y == y)
      break;
    usleep(10);
  }*/
}

void DisplayDevice::click()
{
  XEvent event;

  memset(&event, 0x00, sizeof(event));

  event.type = ButtonPress;
  event.xbutton.button = 1;
  event.xbutton.same_screen = True;

  XQueryPointer(display_, RootWindow(display_, DefaultScreen(display_)), &event.xbutton.root, &event.xbutton.window, &event.xbutton.x_root, &event.xbutton.y_root, &event.xbutton.x, &event.xbutton.y, &event.xbutton.state);

  event.xbutton.subwindow = event.xbutton.window;

  while(event.xbutton.subwindow)
  {
    event.xbutton.window = event.xbutton.subwindow;

    XQueryPointer(display_, event.xbutton.window, &event.xbutton.root, &event.xbutton.subwindow, &event.xbutton.x_root, &event.xbutton.y_root, &event.xbutton.x, &event.xbutton.y, &event.xbutton.state);
  }

  if(XSendEvent(display_, PointerWindow, True, 0xfff, &event) == 0)
    return ;

  XFlush(display_);

  usleep(100000);

  event.type = ButtonRelease;
  event.xbutton.state = 0x100;

  if(XSendEvent(display_, PointerWindow, True, 0xfff, &event) == 0)
    return ;

  XFlush(display_);
}

}
