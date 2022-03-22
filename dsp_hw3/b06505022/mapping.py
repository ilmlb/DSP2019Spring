#!/usr/bin/python3

import sys,re

with open(sys.argv[1],"r",encoding='BIG5HKSCS') as infile:
    testdata=infile.read()

testdata=testdata.split("\n")

mapping={}

for line in testdata[:-1]:
    line=re.findall(r"[\w']+",line)
    word=line[0]
    if word not in mapping:
        mapping[word]=set()
        mapping[word].add(word)
    for a in line[1:]:
        if a[0] not in mapping:
            mapping[a[0]]=set()
            mapping[a[0]].add(word)
        else:
            mapping[a[0]].add(word)

with open(sys.argv[2],"w",encoding='BIG5HKSCS') as outfile:
    for ZhuYin in mapping:
        print("{}\t{}".format(ZhuYin," ".join(str(a) for a in mapping[ZhuYin])),file=outfile)