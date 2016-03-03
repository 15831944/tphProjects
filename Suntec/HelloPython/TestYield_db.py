#encoding=utf-8
import psycopg2

class TestYield(object):
    def __init__(self, dbIP, dbName, userName, password):
        self.conn = psycopg2.connect(''' host='%s' dbname='%s' user='%s' password='%s' ''' % \
                                     (dbIP, dbName, userName, password))
        self.pg = self.conn.cursor()
         
    def func1(self):
        try:
            self.pg.arraysize = 1024
            self.pg.execute("select * from rdb_link limit 1024*1024")
            while True:
                rows = self.pg.fetchone()
                if not rows:
                    break
                yield rows
        except psycopg2.DatabaseError as e:
            raise
        finally:
            pass
        return
    
    def func2(self):
        row_data = list()
        for rows in self.func1():
            for row in rows:
                if not row:
                    break
                row_data = row
                yield row_data
         
if __name__ == '__main__':
    dbIP = '''192.168.10.14'''
    dbNameList = ['post13_ASE_RDF_CI']
    userName = '''postgres'''
    password = '''pset123456'''
    for dbName in dbNameList:
        try:
            testYield = TestYield(dbIP, dbName, userName, password)
            for x in testYield.func2():
                print x
        except Exception, ex:
            print '''%s/%s.\nmsg:\t%s\n''' % (dbIP, dbName, ex)
    

    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    