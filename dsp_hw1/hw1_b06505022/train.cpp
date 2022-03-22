#include<iostream>
#include<string.h>
#include<fstream>
#include "hmm.h"
using namespace std;

void train(HMM *hmm,int iteration,int data[10000][50])
{
    cout<<"Training\n";
    for(int i=0;i<iteration;i++)
    {
        double all_y[50][6]={0};
        double obs_y[6][6]={0};
        double state_y[6]={0};
        double all_e[6][6]={0};        
        cout<<"Iteration="<<i<<endl;
        for(int j=0;j<10000;j++)
        {
            double a[50][6]={0};
            double b[50][6]={0};
            double y[50][6]={0};
            double e[49][6][6]={0};
            for(int k=0;k<6;k++)
            {
                int ob=data[j][0];
                a[0][k]=hmm->initial[k]*hmm->observation[ob][k];
            }
            for(int k=0;k<49;k++)
            {
                int ob=data[j][k+1];
                for(int l=0;l<6;l++)
                {
                    for(int m=0;m<6;m++)
                    {
                        a[k+1][l]+=hmm->observation[ob][l]*a[k][m]*hmm->transition[m][l];
                    }
                }
            }
            for(int k=0;k<6;k++)
            {
                b[49][k]=1;
            }
            for(int k=48;k>=0;k--)
            {
                int ob=data[j][k+1];
                for(int l=0;l<6;l++)
                {
                    for(int m=0;m<6;m++)
                    {
                        b[k][l]+=hmm->observation[ob][m]*b[k+1][m]*hmm->transition[l][m];
                    }
                }
            }
            for(int k=0;k<50;k++)
            {
                double sum=0;
                int ob=data[j][k];
                for(int l=0;l<6;l++)
                {
                    sum+=a[k][l]*b[k][l];
                }
                for(int l=0;l<6;l++)
                {
                    y[k][l]=a[k][l]*b[k][l]/sum;
                    all_y[k][l]+=y[k][l];
                    obs_y[ob][l]+=y[k][l];
                }
            }
            for(int k=0;k<49;k++)
            {
                double sum=0;
                int ob=data[j][k+1];
                for(int l=0;l<6;l++)
                {
                    for(int m=0;m<6;m++)
                    {
                        double v=a[k][l]*hmm->transition[l][m]*hmm->observation[ob][m]*b[k+1][m];
                        e[k][l][m]=v;
                        sum+=v;
                    }
                }
                for(int l=0;l<6;l++)
                {
                    for(int m=0;m<6;m++)
                    {
                        e[k][l][m]/=sum;
                    }
                }
            }
            for(int k=0;k<49;k++)
            {
                for(int l=0;l<6;l++)
                {
                    for(int m=0;m<6;m++)
                    {
                        all_e[l][m]+=e[k][l][m];
                    }
                }
            }
        }
        for(int j=0;j<50;j++)
        {
            for(int k=0;k<6;k++)
            {
                state_y[k]+=all_y[j][k];
            }
        }
        for(int j=0;j<6;j++)
        {
            hmm->initial[j]=all_y[0][j]/10000.0;
        }
        for(int j=0;j<6;j++)
        {
            for(int k=0;k<6;k++)
            {
                hmm->transition[j][k]=all_e[j][k]/state_y[j];
            }
        }
        for(int j=0;j<6;j++)
        {
            for(int k=0;k<6;k++)
            {
                hmm->observation[k][j]=obs_y[k][j]/state_y[j];
            }
        }
    }   
}

int main(int argc,char *argv[])
{
    int iteration=atoi(argv[1]);
    char *init=argv[2];
    char *c_data=argv[3];
    char *output=argv[4];
    HMM model;
    int data[10000][50]={0};

    string line;
    ifstream train_file(c_data);
    if(train_file.is_open())
    {
        int i=0;
        while(getline(train_file,line))
        {
            char temp[51]="\0";
            strcpy(temp,line.c_str());
            for(int j=0;j<50;j++)
            {
                data[i][j]=int(temp[j]-'A');
            }
            ++i;
        }
        train_file.close();
    }
    else
    {
        cout<<"can't open\n";
    }
    
    loadHMM(&model,init);
    train(&model,iteration,data);
    FILE *out=open_or_die(output,"w");
    dumpHMM(out,&model);
    fclose(out);
    return 0;
}