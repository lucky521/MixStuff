#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILENAMELENGTH 100

class CBeibao
{
public:
    int m_nNumber;        //物品的个数
    int m_nMaxWeight;    //背包的载重量
    int *m_pWeight;        //每个物品的重量
    int *m_pValue;        //每个物品的价值
    int *m_pCount;        //每个物品的选中情况
    int m_nMaxValue;	//最终的最大价值

public:
    CBeibao(const char *filename);
    ~CBeibao();
    
    int GetMaxValue();
    int GetMaxValue(int n,int m,int *w,int *v,int *c);
    void Display(int nMaxValue);
    void Display(int nMaxValue,const char *filename);
};


//初始化数据
CBeibao::CBeibao(const char *filename)
{
    FILE *fp=fopen(filename,"r");    
    if(fp==NULL)
    {
        printf("can not open file!");
        exit(0);
    }

    //读入物品数量和最大载重量
    fscanf(fp,"%d%d",&m_nNumber,&m_nMaxWeight);

    m_pWeight=new int[m_nNumber+1];
    m_pValue=new int[m_nNumber+1];

    
    //读入每个物品的重量
	m_pWeight[0]=0; //0位不用
    for(int i=1;i<=m_nNumber;i++)
        fscanf(fp,"%d",m_pWeight+i);

    
    //读入每个物品的价值
	m_pValue[0]=0; //0位不用
    for(int i=1;i<=m_nNumber;i++)
        fscanf(fp,"%d",m_pValue+i);

    //初始化每个物品的选中情况
    m_pCount=new int[m_nNumber+1];
    for(int i=0;i<=m_nNumber;i++)
        m_pCount[i]=0;

    fclose(fp);
}

CBeibao::~CBeibao()
{
    delete[] m_pWeight;
    m_pWeight=NULL;
    delete[] m_pValue;
    m_pValue=NULL;
    delete[] m_pCount;
    m_pCount=NULL;
}


int CBeibao::GetMaxValue(int n,int m,int *w,int *v,int *c)
{
    int i,j;
    int **value=new int*[n+1];

    for(i=0;i<n+1;i++)
        value[i]=new int[m+1];

    for(j=0;j<m+1;j++)
        value[0][j]=0;

    for(i=0;i<n+1;i++)
        value[i][0]=0;

    for(i=1;i<n+1;i++)
    {
        for(j=1;j<m+1;j++)
        {
            int temp=value[i-1][j-w[i]] + v[i];
            if(w[i]<=j && temp>value[i-1][j])
                value[i][j]=temp;
			else
				value[i][j]=value[i-1][j];
        }
    }

    
    for(i=n,j=m;i>0;i--)
    {
        if(value[i][j]>value[i-1][j])
        {
            c[i] = 1;
            j = j - w[i];
        }
    }

    //记录最大价值
    int nMaxVlaue=value[n][m];

    //释放该二维数组
    for(i=0;i<n+1;i++)
    {
        delete [m+1]value[i];
        value[i]=NULL;
    }
    delete[] value;
    value=NULL;

    return nMaxVlaue;
}

int CBeibao::GetMaxValue()
{
    m_nMaxValue = GetMaxValue(m_nNumber,m_nMaxWeight,m_pWeight,m_pValue,m_pCount);
    return m_nMaxValue;
}


void CBeibao::Display(int nMaxValue)
{
    printf("%d \n",nMaxValue);
    for(int i=1;i<=m_nNumber;i++)
    {
        if(m_pCount[i])
            printf("%d  %d \n",i,m_pCount[i]);
    }
    printf(" ");
}

void CBeibao::Display(int nMaxValue,const char *filename)
{
    FILE *fp=fopen(filename,"w");    
    if(fp==NULL)
    {
        printf("can not write file!");
        exit(0);
    }

    fprintf(fp,"%d \n",nMaxValue);
    for(int i=1;i<=m_nNumber;i++)
    {
        if(m_pCount[i])
            fprintf(fp,"%d  %d \n",i,m_pCount[i]);
    }
    fclose(fp);
}



void main()
{
    char sfilename[FILENAMELENGTH];
	printf("Please input a filename:");
	scanf("%s",sfilename);
	CBeibao beibao(sfilename);
	int nMaxValue=beibao.GetMaxValue();

	if(nMaxValue)
	{
		beibao.Display(nMaxValue);
		int nlen=strlen(sfilename);
		strcpy(sfilename+nlen-4, "_result.txt");
		beibao.Display(nMaxValue,sfilename);
		getchar();
		getchar();
		
	}
	else
		printf("error! please check the input data! ");
}