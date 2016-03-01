#encoding=utf-8

class RdbLinkRegulationObject(object):
    def __init__(self):
        self.record_no = -1
        self.regulation_id = -1
        self.regulation_type = -1
        self.is_seasonal = -1
        self.key_string = ''
        self.car_type = -1
        self.start_date = -1
        self.end_date = -1
        self.month_flag = -1
        self.day_flag = -1
        self.week_flag = -1
        self.start_time = -1
        self.end_time = -1
        self.exclude_date = -1
        return
    
    def initFromRecord_14(self, errMsg, record):
        if len(record) <> 14:
            tmpStr = "invalid input argument record: "
            tmpStr += self.getRecordString(record)
            errMsg[0] = "tmpStr"
            return
        
        self.record_no = record[0]
        self.regulation_id = record[1]
        self.regulation_type = record[2]
        self.is_seasonal = record[3]
        self.key_string = record[4]
        self.car_type = record[5]
        self.start_date = record[6]
        self.end_date = record[7]
        self.month_flag = record[8]
        self.day_flag = record[9]
        self.week_flag = record[10]
        self.start_time = record[11]
        self.end_time = record[12]
        self.exclude_date = record[13]
    
    def getArgDist_14(self):
        dict = {}
        dict['record_no'] = self.record_no
        dict['regulation_id'] = self.regulation_id
        dict['regulation_type'] = self.regulation_type
        dict['is_seasonal'] = self.is_seasonal
        dict['key_string'] = self.key_string
        dict['car_type'] = self.car_type
        dict['start_date'] = self.start_date
        dict['end_date'] = self.end_date
        dict['month_flag'] = self.month_flag
        dict['day_flag'] = self.day_flag
        dict['week_flag'] = self.week_flag
        dict['start_time'] = self.start_time
        dict['end_time'] = self.end_time
        dict['exclude_date'] = self.exclude_date
        return dict
       
    def getRecordString(self, record):
        tmpStr = "length=%s, ["
        for i in range(0, len(record)):
            tmpStr += str(record[i]) + ", "
        tmpStr = tmpStr.strip(", ")+"]"
        return tmpStr
    
    def getLinkList(self):
        if self.key_string == '':
            return []
        return list(int(x) for x in self.key_string.split(','))
    
    @staticmethod
    def getRegulationRecordByTileID(errMsg, pg_, tileID):
        sqlcmd = """
select a.record_no, a.regulation_id, a.regulation_type, a.is_seasonal, a.key_string, 
       c.car_type, c.start_date, c.end_date, c.month_flag, c.day_flag, 
       c.week_flag, c.start_time, c.end_time, c.exclude_date
from 
rdb_link_regulation as a
left join rdb_link_cond_regulation as c
on a.regulation_id=c.regulation_id
where first_link_id_t=ANY(array[%s]) 
or last_link_id_t=ANY(array[%s])
"""
        sqlcmd = sqlcmd % (tileID, tileID)
        pg_.execute(sqlcmd)
        return pg_.fetchall()
    
    @staticmethod
    def getRegulationRecordByTileIDList(errMsg, pg_, tileIDList):
        sqlcmd = """
select a.record_no, a.regulation_id, a.regulation_type, a.is_seasonal, a.key_string, 
       c.car_type, c.start_date, c.end_date, c.month_flag, c.day_flag, 
       c.week_flag, c.start_time, c.end_time, c.exclude_date
from 
rdb_link_regulation as a
left join rdb_link_cond_regulation as c
on a.regulation_id=c.regulation_id
where first_link_id_t=ANY(array[%s]) 
or last_link_id_t=ANY(array[%s])
"""
        sqlcmd = sqlcmd % (tileIDList, tileIDList)
        pg_.execute(sqlcmd)
        return pg_.fetchall()
    
    @staticmethod
    def mergeRegulationDictionary(regulationDict1, regulationDict2):
        regulationDict1.update(regulationDict2)
        return













