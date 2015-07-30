# -*- coding: cp936 -*-
import psycopg2
import json

class Signpost_uc_checker(object):
    def __init__(self, dbIP, dbName, userName, password):
        self.conn = psycopg2.connect(''' host='%s' dbname='%s' user='%s' password='%s' ''' % \
                                     (dbIP, dbName, userName, password))
        self.pg = self.conn.cursor()
         
    def checkBegin(self):

        sqlcmd = """
                    select distinct exit_no
                    from rdb_guideinfo_signpost_uc 
                    where exit_no is not null;
                """
        self.pg.execute(sqlcmd)
        rows = self.pg.fetchall() 
        for row in rows:
            exit_no=row[0]
            dictListFromJson = json.loads(exit_no)
            langAndCountDict = {}
            for oneDictList in dictListFromJson:
                for oneDict in oneDictList:
                    if langAndCountDict.has_key(oneDict.get('lang')):
                        langAndCountDict[oneDict.get('lang')] += 1
                    else:
                        langAndCountDict[oneDict.get('lang')] = 1
            for lang, count in langAndCountDict.items():
                if count > 1:
                    intx = 1
                    intx += 1        
        return
            
if __name__ == '__main__':
    dbIP = '''172.26.179.184'''
    dbNameList = ['post13_twn_rdf_ci',
                  'post13_ASE_RDF_CI',
                  'post13_MEA8_RDF_CI',
                  'post13_NI_CHINA_CI',
                  'post13_SAF_TOMTOM_201312',
                  'post13_TOMTOM_SAF_CI',
                  'post13_ZENRIN_TWN_CI',
                  'post13_twn_rdf_ci',
                  'TWN_ZENRIN_2014Q4_0082_0001',
                  'TWN_ZENRIN_14Q4_ZH_TEST',
                  'MEA8_RDF_2015Q1_0065_0010_french',
                  'MAC_RDF_2014Q4_0081_0001',
                  'HKG_RDF_2014Q4_0081_0001']
    userName = '''postgres'''
    password = '''pset123456'''
    for dbName in dbNameList:
        try:
            datMaker = Signpost_uc_checker(dbIP, dbName, userName, password)
            datMaker.checkBegin()
        except Exception, ex:
            print '''db:\t%s.\nmsg:\t%s\n''' % (dbName, ex)
            
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    