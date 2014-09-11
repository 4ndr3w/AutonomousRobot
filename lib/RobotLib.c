#include "RobotLib.h"

void RobotInit(const char* robotAddress, bool routine(RobotInfo*, RobotUpdate*))
{
  RobotInfo robot;
  RobotUpdate newState;
  memset(&robot, 0, sizeof(robot));
  memset(&newState, 0, sizeof(newState));

  sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(1218);
  inet_aton(robotAddress, &addr.sin_addr);

  sockaddr_in bindaddr;
  bindaddr.sin_family = AF_INET;
  bindaddr.sin_port = htons(1218);
  bindaddr.sin_addr.s_addr = INADDR_ANY;

  int sock = socket(AF_INET, SOCK_DGRAM, 0);
//  fcntl(sock, F_SETFL, O_NONBLACK);
  bind(sock, (sockaddr*)&bindaddr, sizeof(bindaddr));
  bool shouldRun = true;
  newState.reset = 1;

  while ( shouldRun )
  {
    //printf("start\n");
    int bytes = sendto(sock, (void*)&newState, sizeof(newState), 0, (sockaddr*)&addr, sizeof(addr));
    newState.reset = 0;
    shouldRun = !routine(&robot, &newState);
    bytes = recv(sock, (void*)&robot, sizeof(robot), 0);
    //printf("recv %i\n", bytes);
  }

  newState.reset = 1;
  sendto(sock, (void*)&newState, sizeof(newState), 0, (sockaddr*)&addr, sizeof(addr));
  close(sock);
}
