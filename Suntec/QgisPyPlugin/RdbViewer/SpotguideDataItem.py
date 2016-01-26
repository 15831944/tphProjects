#encoding=utf-8

class SpotguideDataItem(object):
    def __init(self, record):
        self.gid = record[0]
        self.in_link_id = record[1]
        self.in_link_id_t = record[2]
        self.node_id = record[3]
        self.node_id_t = record[4]
        self.out_link_id = record[5]
        self.out_link_id_t = record[6]
        self.type = record[7]
        self.passlink_count = record[8]
        self.pattern_id = record[9]
        self.arrow_id = record[10]
        self.point_list = record[11]
        self.is_exist_sar = record[12]
        self.order_id = record[13]
        self.patternDat = record[14]
        self.arrowDat = record[15]
        return

    def getKeyString(self):
        return """%s(%s)""" % (self.out_link_id, self.gid)

    def toString(self):
        return """xxxxxxxxxxx"""


