# -*- coding: cp936 -*-
import psycopg2

class IllustDataGenerator(object):
    def __init__(self, dbIP, dbName, userName, password):
        self.conn = psycopg2.connect(''' host='%s' dbname='%s' user='%s' password='%s' ''' % \
                                     (dbIP, dbName, userName, password))
        self.pg = self.conn.cursor()
    
    def generateSpotguidePointLatLon(self, outputCSV):
        sqlcmd = '''
                    select max(a) as max_lane_count
                    from
                    (
                        select unnest(from_list)::int as a
                        from temp_target_table
                    ) as t
                '''
        self.pg.execute(sqlcmd)
        # inlink的最大lane数动态确定
        maximum_lane_count = self.pg.fetchone()
        
        sqlcmd = '''
            select inlinkid, lat_lon, inlink_lane_count, tolink_list, from_list, to_list,
                    outlink_list, outlink_lanecount_list
            from temp_target_table
        '''
        self.pg.execute(sqlcmd)
        rows = self.pg.fetchall()
        line = []
        oFStream = open(outputCSV, 'w')
        for row in rows:
            inlinkid = row[0]
            if inlinkid is None:
                continue
            lat_lon = row[1]
            inlink_lane_count = row[2]
            tolink_list = row[3]
            from_list = row[4]
            to_list = row[5]
            outlink_list = row[6]
            outlink_lanecount_list = row[7]
            
            
            if inlink_lane_count is None:
                inlink_lane_count = 0
                for oneFrom in from_list:
                    if (int(oneFrom) > inlink_lane_count):
                        inlink_lane_count = int(oneFrom)
            line.append(inlink_lane_count) # 流入link的lane数
            laneDict = {}
            for i in range(1, maximum_lane_count+1):
                laneDict[i] = []          
            for i in range(0, len(tolink_list)): # tolink_list, from_list, to_list的长度必定相等。
                laneDict[int(from_list[i])].append('''%d/%s''' % (tolink_list[i], to_list[i]))
            
            tolinkDict = {}
            for i in range(0, len(outlink_list)):
                if outlink_lanecount_list[i] is not None:
                    tolinkDict[outlink_list[i]] = outlink_lanecount_list[i]
                else:
                    outlink_lanecount_list[i] = 0
                    for oneTo in to_list:
                        if(int(oneTo) > outlink_lanecount_list[i]):
                            outlink_lanecount_list[i] = int(oneTo)
                    tolinkDict[outlink_list[i]] = outlink_lanecount_list[i]
                
            line = []
            line.append('''%d''' % inlinkid)
            line.append(lat_lon)
            line.append('''%d''' % inlink_lane_count)
            for i in range(1, maximum_lane_count + 1):
                line.append('''"%s"''' % chr(0x0a).join(laneDict[i]))
            line.append('''%d''' % len(outlink_list))
            for tolinkid, tolinkLaneCount in tolinkDict.items():
                str1 = '''%d/%d''' % (tolinkid, tolinkLaneCount)
                line.append(str1)
            strLine = ','.join(line)
            oFStream.write(strLine + '\n')
        return
            
if __name__ == '__main__':
    dbIP = '''127.0.0.1'''
    dbName = '''AF_TOMTOM_201503_ORG'''
    userName = '''postgres'''
    password = '''pset123456'''
    datMaker = IllustDataGenerator(dbIP, dbName, userName, password)
    datMaker.generateSpotguidePointLatLon('c:\\my\\1.csv')
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    