#encoding=utf-8
from DatParser import DatParser

class SpotguideDataItem(object):
    def __init__(self, record):
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
        self.is_exist_sar = record[11]
        self.order_id = record[12]
        self.patternName = record[13]
        self.patternDat = record[14]
        self.arrowName = record[15]
        self.arrowDat = record[16]
        return

    def getKeyString(self):
        return """%s(%s)""" % (self.out_link_id, self.gid)

    def toString(self):
        strTemp = """field count: 17\n"""
        strTemp += """gid: %s\n""" % self.gid
        strTemp += """in_link_id: %s\n""" % self.in_link_id
        strTemp += """in_link_id_t: %s\n""" % self.in_link_id_t
        strTemp += """node_id: %s\n""" % self.node_id
        strTemp += """node_id_t: %s\n""" % self.node_id_t
        strTemp += """out_link_id: %s\n""" % self.out_link_id
        strTemp += """out_link_id_t: %s\n""" % self.out_link_id_t
        strTemp += """type: %s\n""" % self.type
        strTemp += """passlink_count: %s\n""" % self.passlink_count
        strTemp += """pattern_id: %s\n""" % self.pattern_id
        strTemp += """arrow_id: %s\n""" % self.arrow_id
        strTemp += """is_exist_sar: %s\n""" % self.is_exist_sar
        strTemp += """order_id: %s\n""" % self.order_id
        strTemp += """patternName: %s\n""" % self.patternName
        strTemp += """arrowName: %s\n""" % self.arrowName
        return strTemp

    def getPatternPicture(self, errMsg):
        if self.patternDat == None:
            errMsg[0] = """pattern dat is null."""
            return None
        patternDatParser = DatParser()
        patternDatParser.initFromMemory(errMsg, self.patternDat)
        return patternDatParser.getDatContentByIndex(errMsg, 0)

    def getArrowPicture(self, errMsg):
        if self.arrowDat == None:
            errMsg[0] = """arrow dat is null."""
            return None
        arrowDatParser = DatParser()
        arrowDatParser.initFromMemory(errMsg, self.arrowDat)
        return arrowDatParser.getDatContentByIndex(errMsg, 0)


