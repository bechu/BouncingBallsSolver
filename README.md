Bouncing Balls Solver
=====================

This is a solver for the boucing balls game on facebook.

Required :
 - OpenCV 2
 - libX11

## How it's look like :

The green circle indicate ball detected. The red circle indicate ball detected but not used to compute the shoot.
The number on each ball indicate the score. 13, top left, indicate that 13 balls depends of it.
Finaly, the blue circle indicate the solution have to apply on the game.

![](assets/image.png?raw=true)


## How to compile

Be sure to run the binary in a same direcory of motif.png :

```
mkdir build
cd build
cmake ..
cd ..
build/BouncingBallsSolver .

```

