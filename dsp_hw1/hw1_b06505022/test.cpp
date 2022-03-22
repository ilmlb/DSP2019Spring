#include<iostream>
#include<math.h>
#include<fstream>
#include "hmm.h"
using namespace std;

int main(int argc,char *argv[])
{
    char *model=argv[1];
    char *test=argv[2];
    char *result=argv[3];
    HMM hmm[5];
    int data[2500][50]={0};
    load_models(model,hmm,5);

    string line;
    ifstream test_file(test);
    if(test_file.is_open())
    {
        int i=0;
        while(getline(test_file,line))
        {
            char temp[51]="\0";
            strcpy(temp,line.c_str());
            for(int j=0;j<50;j++)
            {
                data[i][j]=int(temp[j]-'A');
            }
            ++i;
        }
        test_file.close();
    }
    else
    {
        cout<<"can't open\n";
    }

    int ans[2500]={0};
    double similarity[2500]={0};
    for(int i=0;i<2500;i++)
    {
        int an=0;
        double p_max=0;
        for(int j=0;j<5;j++)
        {
            double d[50][6]={0};
            int ob=data[i][0];
            for(int k=0;k<6;k++)
            {
                d[0][k]=hmm[j].initial[k]*hmm[j].observation[ob][k];
            }
            for(int k=1;k<50;k++)
            {
                ob=data[i][k];
                for(int l=0;l<6;l++)
                {
                    double v_max=0;
                    for(int m=0;m<6;m++)
                    {
                        double v=d[k-1][m]*hmm[j].transition[m][l];
                        if(v>v_max)
                        {
                            v_max=v;
                        }
                    }
                    d[k][l]=v_max*hmm[j].observation[ob][l];
                }
            }
            double p=0;
            for(int k=0;k<6;k++)
            {
                if(d[49][k]>p)
                {
                    p=d[49][k];
                }
            }
            if(p>p_max)
            {
                p_max=p;
                an=j;
            }
        }
        ans[i]=an+1;
        similarity[i]=p_max;
    }

    ofstream result_file(result);
    if(result_file.is_open())
    {
        for(int i=0;i<2500;i++)
        {
            result_file<<"model_0"<<ans[i]<<".txt "<<similarity[i]<<endl;
        }
        result_file.close();
    }
    else
    {
        cout<<"can't open\n";
    }
    return 0;
}
