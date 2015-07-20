import os
import psycopg2

illustPath = "c:\\1\\"

def main():
    conn = psycopg2.connect(''' host='172.26.179.184'
                    dbname='IND_MMI_2014Q4_0081_0001'
                    user='postgres' password='pset123456' ''')
    pgcur2 = conn.cursor()
    pgcur2.execute("select patternno, arrowno from spotguide_tbl")
    rows = pgcur2.fetchall()
    
    patternnoList = []
    arrowList = []
    datList = os.listdir(illustPath)
    for row in rows:
        patternnoList.append(row[0] + '.dat')
        arrowList.append(row[1] + '.dat')
    
    foundRoadList = set(patternnoList).intersection(set(datList))
    if(patternnoList == list(foundRoadList)):
        print '''all roads' illust ok.'''
    else:
        notFoundRoadList = set(patternnoList).difference(foundRoadList)
        for notFoundRoad in notFoundRoadList:
            print "road illust not found: " + notFoundRoad
     
    foundArrowList = set(arrowList).intersection(set(datList))
    if(arrowList == list(foundArrowList)):
        print '''all arrows' illust ok.'''
    else:
        notFoundArrowList = set(arrowList).difference(foundArrowList)
        for notFoundArrow in notFoundArrowList:
            print '''arrow illust not found: ''' + notFoundArrow

if __name__ == '__main__':
    main()
    pass
        

        