#include "stdafx.h"

#include <stdio.h>
static const int citycount = 10;

int ok(int matrix[citycount][citycount], int color_list[citycount], int current)
{
    for(int j=1; j<current; j++)
    {
        if(matrix[current][j]==1 && color_list[j]==color_list[current])
        {
            return 0;
        }
    }
    return 1;
}

void go(int matrix[citycount][citycount], int r_color[citycount], int sum, int current)
{
    if(current<=sum)
    {
        for(int i=1;i<=4;i++)
        {
            r_color[current] = i;
            if(ok(matrix,r_color,current))
            {
                go(matrix,r_color,sum,current+1);
                return;
            }
        }
    }
}

void color(int matrix[citycount][citycount],int result[citycount])
{
    for(int i=0; i<citycount; i++)
    {
        result[i] = 0;
        for(int k=0; k<i; k++)
        {
            if(matrix[i][k]==1 && result[k] == result[i])
            {
                result[i]++;
                k=0;
            }
        }
    }
}

int _tmain(int argc, _TCHAR* argv[])
{
    int color_list[citycount]={0};
    int i;
    int matrix[citycount][citycount]={
    {0,1,1,0,0,1,1,0,0,1},
    {1,0,1,1,1,0,0,0,0,0},
    {1,1,0,1,0,1,0,0,0,0},
    {0,1,1,0,1,1,0,0,0,0},
    {0,1,0,1,0,1,0,0,0,0},
    {1,0,1,1,1,0,1,1,1,0},
    {1,0,0,0,0,1,0,1,0,1},
    {0,0,0,0,0,1,1,0,1,1},
    {0,0,0,0,0,1,0,1,0,1},
    {1,0,0,0,0,0,1,1,1,0},
    };
    color(matrix, color_list);

    for(i=0; i<citycount; i++)
    {
        printf("%3d", color_list[i]);
    }
    return 0;
}

