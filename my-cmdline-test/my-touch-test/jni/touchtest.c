#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int mFd;
/********************************************************************
函数功能：移动光标。
入口参数：x：x轴坐标；y：y轴坐标
返    回：无。
备    注：无。
********************************************************************/
void MoveTo(int x, int y)
{
 //需要返回的5字节报告的缓冲
 //Buf[0]的D0就是左键，D1就是右键，D2就是中键
 //Buf[1]为X轴低字节，Buf[2]为X轴高字节，
 //Buf[3]为Y轴低字节，Buf[4]为Y轴高字节，
 char Buf[9]={0,0,0,0,0};
 Buf[0] = 0x00;
 Buf[1] = x & 0xFF;
 Buf[2] = (x >> 8) & 0xFF;
 Buf[3] = y & 0xFF;
 Buf[4] = (y >> 8) & 0xFF;
 if (write(mFd, Buf, 9) != 9) {
     return;
  }
  usleep(50000);
}
////////////////////////End of function//////////////////////////////

/********************************************************************
函数功能：画线段的函数。
入口参数：x：x轴坐标；y：y轴坐标
返    回：无。
备    注：无。
********************************************************************/
void LineTo(int x, int y)
{
 //需要返回的5字节报告的缓冲
 //Buf[0]的D0就是左键，D1就是右键，D2就是中键
 //Buf[1]为X轴低字节，Buf[2]为X轴高字节，
 //Buf[3]为Y轴低字节，Buf[4]为Y轴高字节，
 char Buf[9]={0,0,0,0,0};
 Buf[0] = 0x01; //左键按下
 Buf[1] = x & 0xFF;
 Buf[2] = (x >> 8) & 0xFF;
 Buf[3] = y & 0xFF;
 Buf[4] = (y >> 8) & 0xFF;
 if (write(mFd, Buf, 9) != 9) {
     return;
  }
  usleep(50000);
}
////////////////////////End of function//////////////////////////////

/********************************************************************
函数功能：画圆的函数。
入口参数：x：x轴坐标；y：y轴坐标；r：半径
返    回：无。
备    注：无。
********************************************************************/
void DrawCircle(int x, int y, int r)
{
 int i, px, py;
 MoveTo(x + r, y);
 LineTo(x + r, y);
 for(i = 0; i < 360; i ++)
 {
  px = x + r * cos((i * 1.0) / 180 * 3.1415926);
  py = y + r * sin((i * 1.0) / 180 * 3.1415926);
  LineTo(px, py);
 }
 MoveTo(x + r, y);
}
////////////////////////End of function//////////////////////////////


/********************************************************************
函数功能：画正弦曲线的函数。
入口参数：无。
返    回：无。
备    注：无。
********************************************************************/
void DrawSinCurve(void)
{
 int px, py;
 MoveTo(4000, 2000);
 LineTo(4000, 2000);
 LineTo(400, 2000);
 for(px = 400; px <= 4000; px += 10)
 {
  py = 2000 - 1000 * sin(((px - 400) / 2.5) / 180 * 3.1415926);
  LineTo(px, py);
 }
 MoveTo(px, py);
}
////////////////////////End of function//////////////////////////////

/********************************************************************
函数功能：根据按键情况返回报告的函数。
入口参数：无。
返    回：无。
备    注：无。
********************************************************************/
void SendReport(void)
{
#if 0     
 if(KeyDown & (KEY1))
 {
  MoveTo(4096 / 2, 4096 /2); //移动到屏幕中心
 }
 if(KeyDown & (KEY2)) //画直线
 {
  MoveTo(1000, 1000); //移动到（1000，1000）
  LineTo(1000, 1000); //开始画线
  LineTo(3000, 3000); //画线到（3000，3000）
  MoveTo(3000, 3000); //松开鼠标左键
 }
 if(KeyDown & (KEY3)) //画三角形
 {
  MoveTo(2000, 1000); //移动到（2000，1000）
  LineTo(2000, 1000); //开始画线
  LineTo(1000, 3000); //画线到（1000，3000）
  LineTo(3000, 3000); //画线到（3000，3000）
  LineTo(2000, 1000); //画线到（2000，1000）
  MoveTo(2000, 1000); //松开鼠标左键
 }
 if(KeyDown & (KEY4)) //画正方形
 {
  MoveTo(1000, 1000); //移动到（1000，1000）
  LineTo(1000, 1000); //开始画线
  LineTo(1000, 3000); //画线到（1000，3000）
  LineTo(3000, 3000); //画线到（3000，3000）
  LineTo(3000, 1000); //画线到（3000，1000）
  LineTo(1000, 1000); //画线到（1000，1000）
  MoveTo(1000, 1000); //松开鼠标左键
 }
 if(KeyDown & (KEY5))
 {
  DrawCircle(2000, 2000, 1000); //画一个圆心在（2000，2000），半径为1000的圆
 }
 if(KeyDown & (KEY6))
 {
  DrawSinCurve(); //画正弦曲线
 }
 //记得清除KeyUp和KeyDown
 KeyUp=0;
 KeyDown=0;
#endif
}
////////////////////////End of function//////////////////////////////
//zhudm, use this to replace gets
static void get_string(char  * cmd)
{
    int i;
    for (i = 0; i < 10; i ++) {
        int ch = getchar();
        if (ch == 10)
        {
            break;
        }
        cmd[i] = ch;
    }

    cmd[i]= 0;
}

int main(int argc, const char *argv[])
{
	const char *filename = NULL;
	//int fd = 0;
	char report[8];
	int count;
	int i;
    char cmd[25];

	/* filename = argv[1]; */
	filename = "/dev/hidg2";

	if ((mFd = open(filename, O_RDWR, 0666)) == -1) {
		perror(filename);
		return 3;
	}

    while(1)
    {
	    printf("\n===========================\n");
        printf("q: Stop\n");
        printf("a: Draw A Tri-angle\n");  
        printf("l: Draw A Line\n");
        printf("s: Draw A Square\n");
        printf("c: Draw A Circle\n");
        printf("x: Draw A Sine\n");
        printf("===========================\n\n");
        get_string(cmd);
        if(strstr(cmd, "q") != NULL)
        {
            break;
        }
        else if(strstr(cmd, "a") != NULL)
        {
            printf("Draw Tri-angle begin\n");
            MoveTo(2000, 1000); //移动到（2000，1000）
            LineTo(2000, 1000); //开始画线
            LineTo(1000, 3000); //画线到（1000，3000）
            LineTo(3000, 3000); //画线到（3000，3000）
            LineTo(2000, 1000); //画线到（2000，1000）
            MoveTo(2000, 1000); //松开鼠标左键
            printf("Draw Tri-angle end\n");
        }
        else if(strstr(cmd, "l") != NULL)
        {
            printf("Draw line begin\n");
            MoveTo(1000, 1000); //移动到（1000，1000）
            LineTo(1000, 1000); //开始画线
            LineTo(3000, 3000); //画线到（3000，3000）
            MoveTo(3000, 3000); //松开鼠标左键
            printf("Draw line end\n");
        }
        else if(strstr(cmd, "s") != NULL)
        {
            MoveTo(1000, 1000); //移动到（1000，1000）
            LineTo(1000, 1000); //开始画线
            LineTo(1000, 3000); //画线到（1000，3000）
            LineTo(3000, 3000); //画线到（3000，3000）
            LineTo(3000, 1000); //画线到（3000，1000）
            LineTo(1000, 1000); //画线到（1000，1000）
            MoveTo(1000, 1000); //松开鼠标左键
        }
        else if(strstr(cmd, "c") != NULL)
        {
            DrawCircle(2000, 2000, 1000); //画一个圆心在（2000，2000），半径为1000的圆
        }
        else if(strstr(cmd, "x") != NULL)
        {
            DrawSinCurve(); //画正弦曲线
        }
    }
    
	close(mFd);
	return 0;
}
