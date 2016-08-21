#ifndef __GOPATH_H
#define __GOPATH_H
#include "_pid.h"
void SetPointPath(int sta_num,int end_num);
void GoPointPath(struct PID Pid_ang,struct PID Pid_dis);
extern int Point_NowNum;
extern int Point_StartNum;
extern int Point_EndNum;
#endif