//朋友选修课的作业 好吧 我知道这很逊   qwq~
//貌似可以用了   
//第三次修改  纯手工打造  没学过数据结构和算法  c语言看到指针就学不下去了  死磕出真理

#include<stdio.h>
#include <stdlib.h>   //提供清屏支持
#define N 5   //宏定义  定义学生总数

void input(float x[N][6],int numtmp[N])     //录入函数 录入四科成绩同时初始化学号数组   
{
	int i,j;
	float sum=0;
	printf("\n-------请输入成绩 格式：  英语   数学    化学    C语言-------\n\n");
	for (j=0;j<N;j++)
	{
		numtmp[j]=j+1;
		printf("请输入学号为%d学生的成绩\n",numtmp[j]);
		for (i=0;i<4;i++)                  //同一个学生的四科成绩储存在[0][0] --[0][3]
		{
		scanf("%f",&x[j][i]);
		}
		for (sum=0,i=0;i<4;i++)
		{
		sum+=x[j][i];
		}
		x[j][4]=sum;
		x[j][5]=sum/4;
		
	
	}
}

void output(float x[N][6],char strtmp[N][20],int numtmp[N])  //打印成绩表部分
{
	printf("      学号      姓名      英语       数学       化学       C语言       总分      平均分\n");
	int i,j;
	for (j=0;j<N;j++)
	{
		printf("      %d",numtmp[j]);                        //打印成绩表学号部分
		printf("      %s",strtmp[j]);                         //打印成绩表姓名部分
		for (i=0;i<6;i++)
		{
			
			printf("      %.2f",x[j][i]);
		}
		printf("\n");
	}
}

void Sort(float x[N][6],char strtmp[N][20],int numtmp[N])    //排序函数 按总分排序  //三个数组的顺序均需要调整
{ 
	int m,n,i,j,tmpint;
	char tmpchar;
	float tmpfloat;
	for (j=0;j<N-1;j++)
	{
		for (m=0,n=m+1;m<N-1-j;m++,n++)
		{
			if (x[m][4]<x[n][4])    //冒泡法
				{
		            //j=n;
					for (tmpfloat=0,i=0;i<6;i++)    //成绩数组 重新排序
					{
						tmpfloat=x[m][i];
						x[m][i]=x[n][i];
						x[n][i]=tmpfloat;
					}
					for (tmpchar=0,i=0;i<20;i++)  //  姓名数组 重新排序
					{
						tmpchar=strtmp[m][i];
						strtmp[m][i]=strtmp[n][i];
						strtmp[n][i]=tmpchar;
					}
					tmpint=numtmp[m];    //  学号数组 重新排序
					numtmp[m]=numtmp[n];
					numtmp[n]=tmpint;
				}
		}
	}
}


void Low (float x[][6],char strtmp[N][20])  //得出最低分并打印低分者名字
{
	int i,j,num=0;
	float tmp;
	printf ("\n            最低分者");
	for (i=0;i<4;i++)
	{
		for (tmp=x[0][i],j=0;j<N;j++)
		{
			if (tmp>x[j][i]) 
			{
				tmp=x[j][i];
				num=j;
			}
		}
	printf("     %s",strtmp[num]);
	}
	printf("\n");
}	

void High (float x[][6],char strtmp[N][20])  //得出最高分并打印高分者名字
{
	int i,j,num=0;
	float tmp;
	printf ("            最高分者");
	for (i=0;i<4;i++)
	{
		for (tmp=x[0][i],num=0,j=0;j<N;j++)
		{
			if (tmp<x[j][i]) {tmp=x[j][i];num=j;}
		}
	printf("     %s",strtmp[num]);
	}
	printf("\n");
}	
 
void menu (float x[N][6],char strtmp[N][20],int numtmp[N]) //菜单函数 调用其他子函数
{
	int tmp;
	while (1)    
		{
		
			printf("\n\n-------------操作选项-----------\n");
			printf("1:打印成绩不显示最高分最低分者 \n");
			printf("2:打印成绩和各科最高分最低分者 \n");
			printf("--------------------------------\n");
			printf("按数字键选择要执行的操作: ");
			scanf("%d",&tmp);
			printf("\n");
			system("cls");
			switch(tmp)
			{
				case 1:  output(x,strtmp,numtmp);break;    
				case 2:  output(x,strtmp,numtmp);Low(x,strtmp);High(x,strtmp);break;     
				default:          //数字输入错误，跳出siwtch语句，进入下一次循环
					printf("输入的数字不正确\n");
				break;
				}
		}
	}

int main()
{   int numb[N];                                                       //定义学号数组
	char str[N][20]={"李小琳","张飞瀛","宁  可","王明云","朱鸿儒"};   //定义字符串数组  储存姓名  
	float a[N][6];  	   //定义成绩表数组
	input(a,numb);         //开始录入并求出总分和平均分
	Sort(a,str,numb);      //开始按总分排序
	menu(a,str,numb);      //显示菜单
	}

	
