import psycopg2

def main():
    try:
        conn = psycopg2.connect(''' host='172.26.179.184'
                        dbname='IND_MMI_2014Q4_0081_0001'
                        user='postgres' password='pset123456' ''')
        pgcur2 = conn.cursor()
        pgcur2.execute("select patternno from spotguide_tbl")
        rows = pgcur2.fetchall()
        
        outFStream = open("C:\\My\\20150410_mmi_pic\\illust_pic\\all_sar_files_name.csv", "w")
        for row in rows:
            patternno = row[0]
            if("_sp_" in patternno):
                outFStream.write(patternno + "\n")
        
        outFStream.close()
    except Exception, e:
        print e
        
if __name__ == '__main__':
    main()
    pass
        

        