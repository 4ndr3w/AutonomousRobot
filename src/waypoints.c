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

std::vector< std::vector<int> > waypoints;

void LoadWaypoints(const char* f)
{
  std::ifstream file(f);
  while ( true )
  {
    std::vector<int> lineWaypoints;
    std::string line;
    std::getline(file, line);
    if ( line.length() == 0 )
      break;

    char* data = strtok((char*)line.c_str(), ",");
    while ( data != NULL )
    {
      lineWaypoints.push_back(atoi(data));
      data = strtok(NULL, ",");
    }
    waypoints.push_back(lineWaypoints);
  }
}


int current = 0;

bool routine(RobotInfo* info, RobotUpdate* update)
{
  update->closedLoop = false;
  update->gyroCorrect = true;
  if ( current < waypoints.size() )
  {
  std::vector<int> waypoint = waypoints[current];

  double dx = info->x - waypoint.at(0);
  double dy = abs(info->y) - waypoint.at(1);

  update->theta = (atan2(dy,dx)*(180/PI)+90)*-1;

 // update->raw_left = 40;
 // update->raw_right = 40;

  double dist = sqrt(pow(dx,2)+pow(dy,2));
  if ( dist < 2 )
  {
    update->theta = info->theta;
    current++;
  }
  printf("Current (%i, %i) Theta: %i Commanded Theta: %i\n",info->x, info->y, info->theta, update->theta);

   if ( abs(update->theta-info->theta) > 10 )
   {
     update->raw_left = 0;
     update->raw_right = 0;
   }
   else
   {
   	update->raw_left = dist*5;
   	update->raw_right = dist*5;
   }
   return false;
  }
  else return true;
}


int main()
{
  LoadWaypoints("waypoints.txt");
  RobotInit("10.0.8.250", routine);
}
