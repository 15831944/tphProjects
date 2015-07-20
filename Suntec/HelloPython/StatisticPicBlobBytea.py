import psycopg2

def main():
    conn = psycopg2.connect(''' host='172.26.179.194'
                    dbname='ARG_RDF_2014Q4_0080_0002'
                    user='postgres' password='pset123456' ''')
    pgcur2 = conn.cursor()
    pgcur2.execute('''
                    select bit_length(data) 
                    from rdb_guideinfo_pic_blob_bytea as a
                    where a.gid in 
                    (select distinct pattern_id 
                     from rdb_guideinfo_spotguidepoint 
                     where type=5)
                    ''')
    rows = pgcur2.fetchall()
    totalLength = 0
    for row in rows:
        print row[0]
        totalLength += int(row[0])
    
    print totalLength
            
if __name__ == '__main__':
    main()
    pass
        