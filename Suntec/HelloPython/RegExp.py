#!/usr/local/bin/python
#coding:GBK
import re
fopen=open(r'c:\my\1.txt','r')
f=fopen.read().strip()

print "1.txt文件内容"
print f
print "---------------------------------------"
print "匹配结果"
#for x in re.findall(r"{font-family: \"[\w\W]*?\"",f,re.S):
for x in re.findall(r"rdb_link[\S]*", f, re.S | re.I):
    print x
print "---------------------------------------"


