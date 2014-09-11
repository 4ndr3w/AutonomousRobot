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
  update->reset = 1;
  return true;
}


int main()
{
  RobotInit("10.0.8.250", routine);
}
