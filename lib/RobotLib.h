#ifndef RobotLib_h
#define RobotLib_h

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

struct RobotInfo
{
  int16_t x;
  int16_t y;

  int16_t theta;
  int16_t error;
  int16_t avgDistance;
};

struct RobotUpdate
{
  int8_t closedLoop;

  int8_t gyroCorrect;
  int16_t theta;
  int16_t dist_target;

  int16_t raw_left;
  int16_t raw_right;

  int8_t reset;
};

void RobotInit(const char* robotAddress, bool routine(RobotInfo*, RobotUpdate*));

#endif
