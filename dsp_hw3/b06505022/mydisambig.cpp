#include<iostream>
#include<fstream>
#include<vector>
#include"stdio.h"
#include"Ngram.h"
#include"Vocab.h"
#include<set>
#include<map>
using namespace std;

void input_mapping(const char* filename,map<string,set<string>>& mapping)
{
    fstream f;
    string line;
    f.open(filename,ios::in|ios::binary);
    while(getline(f,line))
    {
        string word=line.substr(0,2);
        set<string>& current_set1=mapping[word];
        line=line.substr(3);
        string new_line;
        for(int i=0;i<line.size();i++)
        {
            if(line[i]!=' ')
            {
                new_line+=line[i];
            }
        }
        for(int i=0;i<(new_line.size()-1);i+=2)
        {
            current_set1.insert(new_line.substr(i,2));
        }
    }
    f.close();
    string end="</s>";
    set<string>& current_set2=mapping[end];
    current_set2.insert(end);
}

void input_testdata(const char* filename,vector<string>& testdata)
{
    fstream f;
    string input_line;
    f.open(filename,ios::in|ios::binary);
    while(getline(f,input_line))
    {
        string line;
        for(int i=0;i<input_line.length();i++)
        {
            if(input_line[i]!=' ')
            {
                line+=input_line[i];
            }
        }
        testdata.push_back(line);
    }
    f.close();
}

double bigram_prob(const char* w1,const char* w2,Vocab& voc,Ngram& lm)
{
    VocabIndex wid1=voc.getIndex(w1);
    VocabIndex wid2=voc.getIndex(w2);
    if(wid1==Vocab_None)
    {
        wid1=voc.getIndex(Vocab_Unknown);
    }
    if(wid2==Vocab_None)
    {
        wid2=voc.getIndex(Vocab_Unknown);
    }
    VocabIndex context[]={wid1,Vocab_None};
    return lm.wordProb(wid2,context);
}

vector<string> viterbi(string& input,map<string,set<string>>& mapping,Vocab& voc,Ngram& lm)
{
    vector<vector<string>> words(input.size()/2+1);
    vector<vector<double>> log_prob(input.size()/2+1);
    vector<vector<int>> pre_index(input.size()/2+1);
    for(int i=0;i<log_prob.size();i++)
    {
        string word;
        if(i==log_prob.size()-1)
        {
            word="</s>";
        }
        else
        {
            word=input.substr(i*2,2);
        }
        log_prob[i].resize(mapping[word].size());
        words[i].resize(mapping[word].size());
        pre_index[i].resize(mapping[word].size());
        set<string>::iterator it=mapping[word].begin();
        for(int j=0;j<words[i].size();j++)
        {
            words[i][j]=*it;
            it++;
        } 
    } 
    for(int i=0;i<log_prob[0].size();i++)
    {
        log_prob[0][i]=bigram_prob("<s>",words[0][i].c_str(),voc,lm);
        pre_index[0][i] = 0;
    }
    for(int i=1;i<log_prob.size();i++)
    {
        for(int j=0;j<log_prob[i].size();j++)
        {
            log_prob[i][j]=-1e9;
            for(int k=0;k<log_prob[i-1].size();k++)
            {
                double prob;
                prob=bigram_prob(words[i-1][k].c_str(),words[i][j].c_str(),voc, lm);
                prob+=log_prob[i-1][k];
                if(prob>log_prob[i][j])
                {
                    log_prob[i][j]=prob;
                    pre_index[i][j]=k;
                }
            }
        }
    }
    double max_prob=-1e9;
    int max_index=0;
    for(int i=0;i<log_prob.back().size();i++)
    {
        if(log_prob.back()[i]>max_prob)
        {
            max_prob=log_prob.back()[i];
            max_index=i;
        }
    }
    vector<string> output(log_prob.size());
    vector<string>::iterator it;
    for(int i=output.size()-1;i>=0;i--)
    {
        output[i]=words[i][max_index];
        max_index=pre_index[i][max_index];
    }
    it=output.begin();
    it=output.insert(it,"<s>");
    return output;
}

int main(int argc,char* argv[])
{
    int ngram_order=2;
    Vocab voc;
    Ngram lm(voc,ngram_order);
    vector<string> testdata;
    map<string,set<string>> mapping;
    File lmFile(argv[6],"r");
    lm.read(lmFile);
    lmFile.close();
    input_mapping(argv[4],mapping);
    input_testdata(argv[2],testdata);
    for(int i=0;i<testdata.size();i++)
    {
        vector<string> ans=viterbi(testdata[i],mapping,voc,lm);
        for(int j=0;j<ans.size();j++)
        {
            if(j!=ans.size()-1)
            {
                cout<<ans[j]<<" ";
            }
            else
            {
                cout<<ans[j];
            }
        }
        cout<<"\n";
    }
}