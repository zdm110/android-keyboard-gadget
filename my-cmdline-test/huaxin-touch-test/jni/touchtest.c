#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define FACTOR 8
#define PI 3.1415926
#define MAX_TOUCH_POINT 10
#define REPORTID_MTOUCH 2

int mFd;
/********************************************************************
函数功能：移动光标。
入口参数：x：x轴坐标；y：y轴坐标
返    回：无。
备    注：无。
********************************************************************/
void MoveTo(int x, int y)
{
 char Buf[8]={0,0,0,0,0,0,0,0};
 Buf[0] = 1; //single touch
 Buf[1] = 0; //no button
 Buf[2] = x & 0xFF;
 Buf[3] = (x >> 8) & 0xFF;
 Buf[4] = y & 0xFF;
 Buf[5] = (y >> 8) & 0xFF;
 Buf[6] = 0; //touch
 Buf[7] = 0; //wheel
 if (write(mFd, Buf, 8) != 8) {
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
 char Buf[6]={0,0,0,0,0,0,0,0};
 Buf[0] = 1; //single touch
 Buf[1] = 0x01; //左键按下
 Buf[2] = x & 0xFF;
 Buf[3] = (x >> 8) & 0xFF;
 Buf[4] = y & 0xFF;
 Buf[5] = (y >> 8) & 0xFF;
 Buf[6] = 1; //touch down
 Buf[7] = 0; //wheel
 if (write(mFd, Buf, 8) != 8) {
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
函数功能：产生多点触摸事件。
入口参数：x：x轴坐标数组；y：y轴坐标数组；
          s：状态数组，例如是否触摸，是否有效；n：触摸的点数。
返    回：无。
备    注：无。
********************************************************************/
void MultiPointReport(int *x, int *y, char *s, char n)
{
 char i;
 char ReportBuf[62]; //总共为14字节，第1字节为报告ID。
 //第2字节为第一点状态，第3字节为第一点的触摸ID号；
 //第4、5字节为第一点x轴，第6、7字节为第一点y轴；
 //第8字节为第二点状态，第9字节为第二点的触摸ID号；
 //第10、11字节为第二点x轴，第12、13字节为第二点y轴；
 //第14字节为当前触摸的点数。我们定义的报告中，每次只能
 //发送2个点，如果超过两个点，则另外再增加额外的报告，
 //这时额外的报告的触摸点数都要设置为0。
 if(n == 0) return;
 memset(ReportBuf, 0, 62);
 ReportBuf[0] = 2;
 ReportBuf[1] = s[0];
 for(i=0; i<n; i++)
 {
     ReportBuf[2+i*10] = i;
     ReportBuf[3+i*10] = x[i] & 0xFF;
     ReportBuf[4+i*10] = x[i] >> 8;
     ReportBuf[5+i*10] = y[i] & 0xFF;
     ReportBuf[6+i*10] = y[i] >> 8;
     //ReportBuf[7+i*10] = 0xFF;
 }
// ReportBuf[8] = 0x00;
// ReportBuf[9] = 0xC6;
// ReportBuf[10] = 0x01;
// ReportBuf[11] = 0x00;
//
 if(n<6)
 {
     for(i=n; i<6; i++)
     {
         ReportBuf[2+i*10] = i;
     }
 }
 ReportBuf[61] = 0x01;


 if (write(mFd, ReportBuf, 62) != 62) {
     return;
 }
 
}
////////////////////////End of function//////////////////////////////

/********************************************************************
函数功能：多点模式点击屏幕中央。
入口参数：touch: 0表示松开，1表示触摸。
返    回：无。
备    注：无。
********************************************************************/
void MultiPointGoToCenter(char touch)
{
 int x, y;
 char s;
 x = 4096 / 2 * FACTOR;
 y = 4096 / 2 * FACTOR;
 if(touch)
 {
  s = 0x07; //点击
 }
 else
 {
  s = 0x04; //松开
 }
 MultiPointReport(&x, &y, &s, 1);
}
////////////////////////End of function//////////////////////////////

/********************************************************************
函数功能：多点模式画一条线段。
入口参数：无。
返    回：无。
备    注：无。
********************************************************************/
void MultiPointDrawLine()
{
 int x, y;
 char s;
 x = 1000*FACTOR;
 y = 1000*FACTOR;
 s = 0x06; //数据有效，但是无点击
 MultiPointReport(&x, &y, &s, 1); //移动到(1000, 1000)
 s = 0x07; //数据有效，且有点击
 MultiPointReport(&x, &y, &s, 1); //开始画线
 x = 3000*FACTOR;
 y = 3000*FACTOR;
 MultiPointReport(&x, &y, &s, 1); //画到(3000, 3000)
 s = 0x04; //停止触摸
 MultiPointReport(&x, &y, &s, 1);
}
////////////////////////End of function//////////////////////////////

/********************************************************************
函数功能：同时画二个三角形。
入口参数：无。
返    回：无。
备    注：无。
********************************************************************/
void MultiPointDrawTriangle()
{
 int x[2], y[2];
 char s[2];
 x[0] = 2000;
 y[0] = 1000;
 s[0] = 0x06;
 x[1] = x[0];
 y[1] = y[0] + 500;
 s[1] = 0x06;
 MultiPointReport(x, y, s, 2); //移动到起点
 s[0] = 0x07;
 s[1] = 0x07;
 MultiPointReport(x, y, s, 2); //开始画线
 x[0] = 1000;
 y[0] = 3000;
 x[1] = x[0] + 500 * cos(30 * PI / 180);
 y[1] = y[0] - 500 * sin(30 * PI / 180);
 MultiPointReport(x, y, s, 2); //画左边两条线
 x[0] = 3000;
 y[0] = 3000;
 x[1] = x[0] - 500 * cos(30 * PI / 180);
 y[1] = y[0] - 500 * sin(30 * PI / 180);
 MultiPointReport(x, y, s, 2); //画底边两条线
 x[0] = 2000;
 y[0] = 1000;
 x[1] = x[0];
 y[1] = y[0] + 500;
 MultiPointReport(x, y, s, 2); //画右边两条线
 s[0] = 0;
 s[1] = 0;
 MultiPointReport(x, y, s, 2); //停止触摸
}
////////////////////////End of function//////////////////////////////
void MultiPointDrawTwoline()
{
 int x[2], y[2];
 char s[2];
 x[0] = 2000;
 y[0] = 1000;
 s[0] = 0x06;
 x[1] = x[0];
 y[1] = y[0] + 500;
 s[1] = 0x06;
 MultiPointReport(x, y, s, 2); //移动到起点
 s[0] = 0x07;
 s[1] = 0x07;
 MultiPointReport(x, y, s, 2); //开始画线
 x[0] = 1000;
 y[0] = 3000;
 x[1] = x[0] + 500 * cos(30 * PI / 180);
 y[1] = y[0] - 500 * sin(30 * PI / 180);
 MultiPointReport(x, y, s, 2); //画左边两条线
 x[0] = 3000;
 s[0] = 0;
 s[1] = 0;
 MultiPointReport(x, y, s, 2); //停止触摸
}

/********************************************************************
函数功能：同时画四个正方形。
入口参数：无。
返    回：无。
备    注：无。
********************************************************************/
void MultiPointDrawSquare()
{
 int x[4], y[4];
 char s[4];
 char i; 
 x[0] = 1000;
 y[0] = 1000;
 s[0] = 0x06;
 for(i = 1; i < 4; i ++)
 {
  x[i] = x[i - 1] + 100;
  y[i] = y[i - 1] + 100;
  s[i] = s[0];
 }
 MultiPointReport(x, y, s, 4); //移动到起点
 for(i = 0; i < 4; i ++)
 {
  s[i] = 0x07;
 }
 MultiPointReport(x, y, s, 4); //开始画线
 y[0] = 3000;
 y[1] = 3000 - 100;
 y[2] = 3000 - 200;
 y[3] = 3000 - 300;
 MultiPointReport(x, y, s, 4); //画左边4条线
 x[0] = 3000;
 x[1] = 3000 - 100;
 x[2] = 3000 - 200;
 x[3] = 3000 - 300;
 MultiPointReport(x, y, s, 4); //画左边4条线
 y[0] = 1000;
 y[1] = 1000 + 100;
 y[2] = 1000 + 200;
 y[3] = 1000 + 300;
 MultiPointReport(x, y, s, 4); //画右边4条线
 x[0] = 1000;
 x[1] = 1000 + 100;
 x[2] = 1000 + 200;
 x[3] = 1000 + 300;
 MultiPointReport(x, y, s, 4); //画上边4条线
 for(i = 0; i < 4; i ++)
 {
  s[i] = 0;
 }
 MultiPointReport(x, y, s, 4); //停止触摸
}
////////////////////////End of function//////////////////////////////

/********************************************************************
函数功能：同时画三个圆。
入口参数：无。
返    回：无。
备    注：无。
********************************************************************/
void MultiPointDrawCircle(void)
{
 #define CENTER_X 2000
 #define CENTER_Y 2000
 #define R0       1000
 #define R1       800
 #define R2       600
 
 int x[3], y[3];
 char s[3];
 int i;
 float vsin, vcos;
 
 x[0] = CENTER_X + R0;
 x[1] = CENTER_X + R1;
 x[2] = CENTER_X + R2;
 for(i = 0; i < 3; i ++)
 {
  y[i] = CENTER_Y;
  s[i] = 0x06;
 }
 MultiPointReport(x, y, s, 3); //移动到起点
 for(i = 0; i < 3; i ++)
 {
  s[i] = 0x07;
 }
 MultiPointReport(x, y, s, 3); //开始画线
 for(i = 0; i < 360; i ++)
 {
  vsin = sin((i * 1.0) / 180 * PI);
  vcos = cos((i * 1.0) / 180 * PI);
  x[0] = CENTER_X + R0 * vcos;
  y[0] = CENTER_Y + R0 * vsin;
  x[1] = CENTER_X + R1 * vcos;
  y[1] = CENTER_Y + R1 * vsin;
  x[2] = CENTER_X + R2 * vcos;
  y[2] = CENTER_Y + R2 * vsin;
  MultiPointReport(x, y, s, 3); //画线
 }
 for(i = 0; i < 3; i ++)
 {
  s[i] = 0x00;
 }
 MultiPointReport(x, y, s, 3); //停止触摸
}
////////////////////////End of function//////////////////////////////

/********************************************************************
函数功能：同时画正弦、余弦曲线。
入口参数：无。
返    回：无。
备    注：无。
********************************************************************/
void MultiPointDrawSinCosCurve()
{
 int x[2], y[2];
 char s[2];
 int i;
 x[0] = 400;
 y[0] = 2000;
 x[1] = 400;
 y[1] = 2000;
 s[0] = 0x06;
 s[1] = 0x06;
 MultiPointReport(x, y, s, 2); //移动到起点
 s[0] = 0x07;
 s[1] = 0x07;
 MultiPointReport(x, y, s, 2); //开始画线
 for(i = 400; i <= 4000; i += 10)
 {
  x[0] = i;
  x[1] = i;
  y[0] = 2000 - 1000 * sin(((i - 400) / 2.5) / 180 * PI);
  y[1] = 2000 - 1000 * cos(((i - 400) / 2.5) / 180 * PI);
  MultiPointReport(x, y, s, 2); //画线
 }
 s[0] = 0x00;
 s[1] = 0x00;
 MultiPointReport(x, y, s, 2); //停止触摸
}
////////////////////////End of function//////////////////////////////

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
	filename = "/dev/hidg0";

	if ((mFd = open(filename, O_RDWR, 0666)) == -1) {
		perror(filename);
		return 3;
	}

    while(1)
    {
	    printf("\n===========================\n");
        printf("q: Stop\n");
        printf("a: Multi-touch: Goto Center\n");  
        printf("l: Multi-touch: Draw A Line\n");
        printf("s: Multi-touch: Draw Two Triangle\n");
        printf("c: Multi-touch: Draw Two square\n");
        printf("e: Multi-touch: Draw Two line\n");
        printf("k: Single-touch: Goto Center\n");  
        printf("v: Single-touch: Draw A Line\n");
        printf("x: Draw A Sine\n");
        printf("===========================\n\n");
        get_string(cmd);
        if(strstr(cmd, "q") != NULL)
        {
            break;
        }
        else if(strstr(cmd, "a") != NULL)
        {
            printf("start goto center\n");
            MultiPointGoToCenter(1);
            usleep(50000);
            MultiPointGoToCenter(0);
            printf("stop goto center\n");
        }
        else if(strstr(cmd, "l") != NULL)
        {
            printf("Draw line begin\n");
            MultiPointDrawLine();
            printf("Draw line end\n");
        }
        else if(strstr(cmd, "s") != NULL)
        {
            MultiPointDrawTriangle();
        }
        else if(strstr(cmd, "c") != NULL)
        {
            MultiPointDrawSquare();
        }
        else if(strstr(cmd, "e") != NULL)
        {
            MultiPointDrawTwoline();
        }
        else if(strstr(cmd, "x") != NULL)
        {
            DrawSinCurve(); //画正弦曲线
        }
        else if(strstr(cmd, "k") != NULL)
        {
            MoveTo(4096*FACTOR/2, 4096*FACTOR/2);
        }
        else if(strstr(cmd, "v") != NULL)
        {
            MoveTo(1000*FACTOR, 1000*FACTOR);
            LineTo(1000*FACTOR, 1000*FACTOR);
            MoveTo(3000*FACTOR, 3000*FACTOR);
            LineTo(3000*FACTOR, 3000*FACTOR);
        }
    }
    
	close(mFd);
	return 0;
}
