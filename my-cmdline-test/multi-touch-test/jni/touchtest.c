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
 //需要返回的5字节报告的缓冲
 //Buf[0]的D0就是左键，D1就是右键，D2就是中键
 //Buf[1]为X轴低字节，Buf[2]为X轴高字节，
 //Buf[3]为Y轴低字节，Buf[4]为Y轴高字节，
 char Buf[5]={0,0,0,0,0};
 Buf[0] = 0x00;
 Buf[1] = x & 0xFF;
 Buf[2] = (x >> 8) & 0xFF;
 Buf[3] = y & 0xFF;
 Buf[4] = (y >> 8) & 0xFF;
 if (write(mFd, Buf, 5) != 5) {
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
 char Buf[5]={0,0,0,0,0};
 Buf[0] = 0x01; //左键按下
 Buf[1] = x & 0xFF;
 Buf[2] = (x >> 8) & 0xFF;
 Buf[3] = y & 0xFF;
 Buf[4] = (y >> 8) & 0xFF;
 if (write(mFd, Buf, 5) != 5) {
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
 char ReportBuf[14]; //总共为14字节，第1字节为报告ID。
 //第2字节为第一点状态，第3字节为第一点的触摸ID号；
 //第4、5字节为第一点x轴，第6、7字节为第一点y轴；
 //第8字节为第二点状态，第9字节为第二点的触摸ID号；
 //第10、11字节为第二点x轴，第12、13字节为第二点y轴；
 //第14字节为当前触摸的点数。我们定义的报告中，每次只能
 //发送2个点，如果超过两个点，则另外再增加额外的报告，
 //这时额外的报告的触摸点数都要设置为0。
 if(n == 0) return;
 if(n > MAX_TOUCH_POINT) //如果超过最大支持的点数，则只发送最多点数
 {
  n = MAX_TOUCH_POINT;
 }
 ReportBuf[0] = REPORTID_MTOUCH; //多点报告的报告ID为REPORTID_MTOUCH
 for(i = 0; i < n;) //分别发送各个点
 {
  ReportBuf[1] = s[i]; //状态
  ReportBuf[2] = i + 1; //ID号
  ReportBuf[3] = x[i] & 0xFF; //X轴低8位
  ReportBuf[4] = (x[i] >> 8) & 0xFF; //X轴高8位
  ReportBuf[5] = y[i] & 0xFF; //Y轴低8位
  ReportBuf[6] = (y[i] >> 8) & 0xFF; //Y轴高8位
  if(i == 0) //第一个包
  {
   ReportBuf[13] = n; //触摸的点数
  }
  else //其它包，设置为0
  {
   ReportBuf[13] = 0;
  }
  i ++;
  if(i < n) //还有数据需要发送
  {
   ReportBuf[7] = s[i]; //状态
   ReportBuf[8] = i + 1; //ID号
   ReportBuf[9] = x[i] & 0xFF; //X轴低8位
   ReportBuf[10] = (x[i] >> 8) & 0xFF; //X轴高8位
   ReportBuf[11] = y[i] & 0xFF; //Y轴低8位
   ReportBuf[12] = (y[i] >> 8) & 0xFF; //Y轴高8位
   i ++;
  }
  else //没有更多的数据需要发送，后面的清0
  {
   char j;
   for(j = 7; j < 13; j++)
   {
    ReportBuf[j] = 0;
   }
  }
 if (write(mFd, ReportBuf, 14) != 14) {
     return;
 }
#if 0
  while(Ep1InIsBusy) //等待之前的数据发送完毕
  {
   ProcessInterrupt();  //处理中断
  }
  if(ConfigValue == 0) return;
  //报告准备好了，通过端点1返回，长度为14字节。
  D12WriteEndpointBuffer(3, 14, ReportBuf);
  Ep1InIsBusy=1;  //设置端点忙标志。
#endif
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
 x = 4096 / 2;
 y = 4096 / 2;
 if(touch)
 {
  s = 0x07; //点击
 }
 else
 {
  s = 0x00; //松开
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
 x = 1000;
 y = 1000;
 s = 0x06; //数据有效，但是无点击
 MultiPointReport(&x, &y, &s, 1); //移动到(1000, 1000)
 s = 0x07; //数据有效，且有点击
 MultiPointReport(&x, &y, &s, 1); //开始画线
 x = 3000;
 y = 3000;
 MultiPointReport(&x, &y, &s, 1); //画到(3000, 3000)
 s = 0x00; //停止触摸
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
    }
    
	close(mFd);
	return 0;
}
