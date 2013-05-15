#include<iostream>
#include<fstream>
#include<stdlib.h>
#include<Windows.h>
#include<math.h>

#define RAND_1 (rand()%5 + 15)		//地点的规模在15~20个之间
#define RAND_NUM (rand()%20 + 1)	//两点间距离的范围在1~20之间
#define ROUND 100					//运行轮次
#define MI(x,y)  ((int)pow((double)(x),(double)(y)))
using namespace std;

fstream fout("output.txt", ios::out);

void TSP()
{
	int i,j,k,min,temp;
	int n = RAND_1; //地点个数
	cout<<"地点数为 "<<n<<endl;
	fout<<"地点数为 "<<n<<endl;
	int b = MI(2, n-1);	//二进制位表示未遍历的点的集合
	
	int **D = new int* [n]; //存储各个地点之间距离的矩阵
	for(i=0;i<n;i++)
		D[i] = new int[n];
	//随机生成数据
	for(i=0;i<n;i++)
	{
		for(j=0;j<n;j++)
		{
			D[i][j] = RAND_NUM;
			fout<<D[i][j]<<"\t";
		}
		fout<<endl;
	}

	//申请二维数组F和M
	//i的值表示当前所在地点的编号。
	//j的值所对应的二进制数的每一位 代表一个地点的访问情况，1表示该地点未走过，0表示该地点走过。因此j相当于一个集合。
	//最关键的二维数组F[i][j]的意义：当前位于i点且有j所表示这些点需要去遍历时，则由此出发遍历这些点直到回到出发点的最短距离是F[i][j]。
	//那么最关键：状态转移方程为 F[i][j集合] = MIN{ D[i][k] + F[k][j集合中把k号点设为已走过] }   相当于向前走了一步。

	//地点编号为0~n-1， 即0号位~n-1号位
	int ** F = new int* [n];//n行b列的二维数组，存放阶段最优值，即由此走到终点的最短距离。
	int ** M = new int* [n];//n行b列的二维数组，存放最优策略，即下一步走哪个点。
	for(i=0;i<n;i++)
	{
		F[i] = new int[b]; //每行有2的n-1次方列
		M[i] = new int[b];
	}

	//初始化F[][]和M[][]
	for(i=0;i<b;i++)
		for(j=0;j<n;j++)
		{
			F[j][i] = -1;
			M[j][i] = -1;
		}
	
	//给F的第0列赋初值
	for(i=0;i<n;i++)
		F[i][0] = D[i][0]; //意义：当前在i号点，全部的点都遍历过了，那现在就剩最后一步，由i号点回到出发点，即D[i][0]。
		
	//遍历并填表
	for(i=1;i<b-1;i++) //最后一列不在循环里计算
		for(j=1;j<n;j++)
		{
			//i的二进制数表示当前未走过的地点，j表示当前位于j号点。
			//本轮要解决的子问题是：当我位于j号点，还需走i这些点时的最短路程问题。
			if( (MI(2, j-1) & i) == 0) //如果i集合的j号位为0，即表示j号点已经走过（其实就在脚下）时，该子问题才有意义。
			{
				min=100000;
				for(k=1;k<n;k++) //循环判断每个未走过的下一步，找出子问题最优解
					if( MI(2, k-1) & i ) //如果k号点在集合中，即还没有遍历过k号点
					{
						temp = D[j][k] + F[k][i - MI(2,k-1)]; 
						if(temp < min)
						{
							min = temp;
							F[j][i] = min;//保存阶段最优值
							M[j][i] = k;//保存最优决策
						}
					}
		
			}
		}
		
	//最后一列b-1列，即总最优值的计算
	min=100000;
	for(k=1;k<n;k++)
	{
		temp = D[0][k] + F[k][b-1 - MI(2, k-1)];
		if(temp < min)
		{
			min = temp;
			F[0][b-1] = min;//总最优解
			M[0][b-1] = k;
		}
	}
	fout<<"最短路径长度："<<F[0][b-1]<<endl;

	//回溯查表M输出最短路径(编号0~n-1)，因为是环路，所以算法上把出发点设为哪个点都一样得到最短路程。
	fout<<"最短路径(编号0—"<<n-1<<")："<<"0";
	for(i=b-1,j=0; i>0; )
	{
		j = M[j][i]; //下一步走到j号点
		i = i - MI(2, j-1); //从i中去掉j号点
		fout<<"->"<<j;
	}
	fout<<"->0"<<endl;
	fout<<endl;


	//内存分配回收
	for(i=0;i<n;i++)
	{
		delete [] F[i];
		delete [] M[i];
		delete [] D[i];
	}
	delete [] F;
	delete [] M;
	delete [] D;

}

void main()
{
	double first_time, last_time, start, end;
	first_time = GetTickCount();
	for(int m=0;m<ROUND;m++)
	{
		cout<<"第"<<m+1<<"次：";
		fout<<"第"<<m+1<<"次：";
		start = GetTickCount();
		TSP();
		end = GetTickCount();
		cout<<"耗时为"<<end - start<<"ms"<<endl;
		cout<<endl;
	}
	last_time = GetTickCount();
	cout<<"平均耗时"<<(last_time - first_time)/ROUND<<"ms"<<endl;
	system("pause");

}