#include "../lib/RobotLib.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <fstream>
#include <string.h>
#include <cmath>
#include <vector>
#define PI 3.14159265358979323846


bool routine(RobotInfo* info, RobotUpdate* update)
{
  //update->closedLoop = true;
  //update->dist_target = 12;
  //update->gyroCorrect = true;
  //update->theta = 0;
  update->raw_left = 10;
  update->raw_right = 10;
  printf("Current: (%i,%i)\n", info->x, info->y);
  return false;
}


int main()
{
  RobotInit("10.0.8.250", routine);
}
