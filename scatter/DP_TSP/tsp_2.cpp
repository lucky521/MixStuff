#include<iostream>
#include<fstream>
#include<stdlib.h>
#include<Windows.h>
#include<math.h>

#define RAND_1 (rand()%5 + 15)		//�ص�Ĺ�ģ��15~20��֮��
#define RAND_NUM (rand()%20 + 1)	//��������ķ�Χ��1~20֮��
#define ROUND 100					//�����ִ�
#define MI(x,y)  ((int)pow((double)(x),(double)(y)))
using namespace std;

fstream fout("output.txt", ios::out);

void TSP()
{
	int i,j,k,min,temp;
	int n = RAND_1; //�ص����
	cout<<"�ص���Ϊ "<<n<<endl;
	fout<<"�ص���Ϊ "<<n<<endl;
	int b = MI(2, n-1);	//������λ��ʾδ�����ĵ�ļ���
	
	int **D = new int* [n]; //�洢�����ص�֮�����ľ���
	for(i=0;i<n;i++)
		D[i] = new int[n];
	//�����������
	for(i=0;i<n;i++)
	{
		for(j=0;j<n;j++)
		{
			D[i][j] = RAND_NUM;
			fout<<D[i][j]<<"\t";
		}
		fout<<endl;
	}

	//�����ά����F��M
	//i��ֵ��ʾ��ǰ���ڵص�ı�š�
	//j��ֵ����Ӧ�Ķ���������ÿһλ ����һ���ص�ķ��������1��ʾ�õص�δ�߹���0��ʾ�õص��߹������j�൱��һ�����ϡ�
	//��ؼ��Ķ�ά����F[i][j]�����壺��ǰλ��i������j����ʾ��Щ����Ҫȥ����ʱ�����ɴ˳���������Щ��ֱ���ص����������̾�����F[i][j]��
	//��ô��ؼ���״̬ת�Ʒ���Ϊ F[i][j����] = MIN{ D[i][k] + F[k][j�����а�k�ŵ���Ϊ���߹�] }   �൱����ǰ����һ����

	//�ص���Ϊ0~n-1�� ��0��λ~n-1��λ
	int ** F = new int* [n];//n��b�еĶ�ά���飬��Ž׶�����ֵ�����ɴ��ߵ��յ����̾��롣
	int ** M = new int* [n];//n��b�еĶ�ά���飬������Ų��ԣ�����һ�����ĸ��㡣
	for(i=0;i<n;i++)
	{
		F[i] = new int[b]; //ÿ����2��n-1�η���
		M[i] = new int[b];
	}

	//��ʼ��F[][]��M[][]
	for(i=0;i<b;i++)
		for(j=0;j<n;j++)
		{
			F[j][i] = -1;
			M[j][i] = -1;
		}
	
	//��F�ĵ�0�и���ֵ
	for(i=0;i<n;i++)
		F[i][0] = D[i][0]; //���壺��ǰ��i�ŵ㣬ȫ���ĵ㶼�������ˣ������ھ�ʣ���һ������i�ŵ�ص������㣬��D[i][0]��
		
	//���������
	for(i=1;i<b-1;i++) //���һ�в���ѭ�������
		for(j=1;j<n;j++)
		{
			//i�Ķ���������ʾ��ǰδ�߹��ĵص㣬j��ʾ��ǰλ��j�ŵ㡣
			//����Ҫ������������ǣ�����λ��j�ŵ㣬������i��Щ��ʱ�����·�����⡣
			if( (MI(2, j-1) & i) == 0) //���i���ϵ�j��λΪ0������ʾj�ŵ��Ѿ��߹�����ʵ���ڽ��£�ʱ����������������塣
			{
				min=100000;
				for(k=1;k<n;k++) //ѭ���ж�ÿ��δ�߹�����һ�����ҳ����������Ž�
					if( MI(2, k-1) & i ) //���k�ŵ��ڼ����У�����û�б�����k�ŵ�
					{
						temp = D[j][k] + F[k][i - MI(2,k-1)]; 
						if(temp < min)
						{
							min = temp;
							F[j][i] = min;//����׶�����ֵ
							M[j][i] = k;//�������ž���
						}
					}
		
			}
		}
		
	//���һ��b-1�У���������ֵ�ļ���
	min=100000;
	for(k=1;k<n;k++)
	{
		temp = D[0][k] + F[k][b-1 - MI(2, k-1)];
		if(temp < min)
		{
			min = temp;
			F[0][b-1] = min;//�����Ž�
			M[0][b-1] = k;
		}
	}
	fout<<"���·�����ȣ�"<<F[0][b-1]<<endl;

	//���ݲ��M������·��(���0~n-1)����Ϊ�ǻ�·�������㷨�ϰѳ�������Ϊ�ĸ��㶼һ���õ����·�̡�
	fout<<"���·��(���0��"<<n-1<<")��"<<"0";
	for(i=b-1,j=0; i>0; )
	{
		j = M[j][i]; //��һ���ߵ�j�ŵ�
		i = i - MI(2, j-1); //��i��ȥ��j�ŵ�
		fout<<"->"<<j;
	}
	fout<<"->0"<<endl;
	fout<<endl;


	//�ڴ�������
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
		cout<<"��"<<m+1<<"�Σ�";
		fout<<"��"<<m+1<<"�Σ�";
		start = GetTickCount();
		TSP();
		end = GetTickCount();
		cout<<"��ʱΪ"<<end - start<<"ms"<<endl;
		cout<<endl;
	}
	last_time = GetTickCount();
	cout<<"ƽ����ʱ"<<(last_time - first_time)/ROUND<<"ms"<<endl;
	system("pause");

}