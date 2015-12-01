# -*- coding: cp936 -*-
import os
import psycopg2

class org_junctionview_fiexe_update1(object):
    def __init__(self, dbIP, dbName, userName, password):
        self.conn = psycopg2.connect(''' host='%s' dbname='%s' user='%s' password='%s' ''' % \
                                     (dbIP, dbName, userName, password))
        self.pg = self.conn.cursor()
        self.focusLineList = []
         
    # 从手工制作的文件读取修改项。
    def readFromFile(self):
        strFilePath = r'C:\Users\hexin\Desktop\20151021_check_nostra\log2_fixed_ok.txt'
        with open(strFilePath, 'r') as iFStream:
            lineList = iFStream.readlines()
            
            for oneLine in lineList:
                if oneLine.find('detail: walked_linkid_list') != -1:
                    self.focusLineList.append(oneLine)
            
            
            sqlcmdTemp = """update org_junctionview_fixed set new_arc2=%s where arc1=%s and arc2=%s;"""
            for oneLine in self.focusLineList:
                oneLine = oneLine.replace('INFO:  detail: walked_linkid_list: ', '')
                oneLine = oneLine.replace('\n', '')
                linkIdList = oneLine.split('|')
                arc1 = linkIdList[0]
                arc2 = linkIdList[1]
                new_arc2 = linkIdList[-1]
                sqlcmd = sqlcmdTemp%(new_arc2, arc1, arc2)
                print sqlcmd
                #self.pg.execute(sqlcmd)
                #self.conn.commit()
        return
    
    
    def updateDB1(self):
        strFilePath = r'C:\Users\hexin\Desktop\20151023\20151023checked.txt'
        with open(strFilePath, 'r') as iFStream:
            lineList = iFStream.readlines()
            for i in range(0, len(lineList), 17):
                feature_count = lineList[i].split(':')[1].strip()
                field_count = lineList[i+1].split(':')[1].strip()
                gid = lineList[i+2].split(':')[1].strip()
                arc1 = lineList[i+3].split(':')[1].strip()
                arc2 = lineList[i+4].split(':')[1].strip()
                day_pic = lineList[i+5].split(':')[1].strip()
                night_pic = lineList[i+6].split(':')[1].strip()
                arrowimg = lineList[i+7].split(':')[1].strip()
                lon = lineList[i+8].split(':')[1].strip()
                lat = lineList[i+9].split(':')[1].strip()
                new_arc1 = lineList[i+10].split(':')[1].strip()
                new_arc2 = lineList[i+11].split(':')[1].strip()
                oneway = lineList[i+12].split(':')[1].strip()
                fw = lineList[i+13].split(':')[1].strip()
                id = lineList[i+14].split(':')[1].strip()
                fixed_new_arc2 = lineList[i+15].split(':')[1].strip()
                splitter = lineList[i+16]
                
                sqlcmd= """update org_junctionview_fixed set new_arc2=%s where gid=%s and arc1=%s and arc2=%s;"""\
                        %(fixed_new_arc2, gid, arc1, arc2)
                print sqlcmd
                
            
            
            sqlcmdTemp = """update org_junctionview_fixed set new_arc2=%s where arc1=%s and arc2=%s;"""
            for oneLine in self.focusLineList:
                oneLine = oneLine.replace('INFO:  detail: walked_linkid_list: ', '')
                oneLine = oneLine.replace('\n', '')
                linkIdList = oneLine.split('|')
                arc1 = linkIdList[0]
                arc2 = linkIdList[1]
                new_arc2 = linkIdList[-1]
                sqlcmd = sqlcmdTemp%(new_arc2, arc1, arc2)
                print sqlcmd
                #self.pg.execute(sqlcmd)
                #self.conn.commit()
        return
        return
        
         
if __name__ == '__main__':
    dbIP = '''192.168.10.20'''
    dbNameList = ['AP_NOSTRA_201530_ORG']
    userName = '''postgres'''
    password = '''pset123456'''
    for dbName in dbNameList:
        try:
            datMaker = org_junctionview_fiexe_update1(dbIP, dbName, userName, password)
            #datMaker.readFromFile()
            datMaker.updateDB1()
        except Exception, ex:
            print '''%s/%s.\nmsg:\t%s\n''' % (dbIP, dbName, ex)

    
    
    
    
    
    
    
    
    
    
    
    
    
    
