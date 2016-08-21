/* Includes ------------------------------------------------------------------*/
#include "_typedef.h"
#include "../Algorithm/_mecanum.h"
#include "../Algorithm/_math.h"
#include "../Algorithm/_pid.h"
#include "../Algorithm/_pathpoint.h"
#include "../Algorithm/_gopath.h"
#include "_delay.h"
u8 Route_Num=1;
int RouteIndex[]={0,474};
u8 RouteFinish;

u8 GoRoute(u8 Model)
{
	uint8_t i = 0;

	SetPointPath(RouteIndex[0], RouteIndex[1]);
	while (1)
	{
		GoPointPath(Pid_List[1],Pid_List[2]); 
		if(Point_NowNum>Point_EndNum-2)
		{
			RouteFinish=0;
			SPEED_STOP;
			break;
		}
		delay_ms(5);
	}

	return 1;
}