#!/usr/local/bin/python
#coding:GBK
import re
fopen=open(r'c:\my\1.txt','r')
f=fopen.read().strip()

print "1.txt�ļ�����"
print f
print "---------------------------------------"
print "ƥ����"
#for x in re.findall(r"{font-family: \"[\w\W]*?\"",f,re.S):
for x in re.findall(r"rdb_link[\S]*", f, re.S | re.I):
    print x
print "---------------------------------------"


