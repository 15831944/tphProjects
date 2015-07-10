# -*- coding: cp936 -*-
import psycopg2

maximum_lane_count = 10
class IllustDataGenerator(object):
    def __init__(self, dbIP, dbName, userName, password):
        self.conn = psycopg2.connect(''' host='%s' dbname='%s' user='%s' password='%s' ''' % \
                                     (dbIP, dbName, userName, password))
        self.pg = self.conn.cursor()
    
    def generateSpotguidePointLatLon(self, outputCSV):
        sqlcmd = '''
                    select inlinkid, lat_lon, inlink_lane_count, tolink_list, from_list, to_list,
                            outlinkid_list
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
            outlinkid_list = row[6]
            
            
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
            for oneOutlinkid in outlinkid_list:
                tolinkDict[oneOutlinkid] = []
            for i in range(0, len(tolink_list)): # tolink_list, from_list, to_list的长度必定相等。
                tolinkDict[tolink_list[i]].append(to_list[i])
                
            line = []
            line.append('''%d''' % inlinkid)
            line.append(lat_lon)
            line.append('''%d''' % inlink_lane_count)
            for i in range(1, maximum_lane_count + 1):
                line.append('''"%s"''' % chr(0x0a).join(laneDict[i]))
            line.append('''%d''' % len(outlinkid_list))
            for tolinkid, tolinkLanelist in tolinkDict.items():
                str1 = '''%d/%s''' % (tolinkid, '-'.join(tolinkLanelist))
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
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    