#encoding=utf-8
from DatParser import DatParser

class SignpostDataItem(object):
    def __init__(self, row):
        self.gid = row[0]
        self.in_link_id = row[1]
        self.in_link_id_t = row[2]
        self.node_id = row[3]
        self.node_id_t = row[4]
        self.out_link_id = row[5]
        self.out_link_id_t = row[6]
        self.sp_name = row[7]
        self.passlink_count = row[8]
        self.is_pattern = row[9]
        self.pattern_id = row[10]
        self.arrow_id = row[11]
        self.order_id = row[12]
        self.pattern_name = row[13]
        self.arrow_name = row[14]
        self.pattern_data = row[15]
        self.arrow_data = row[16]
        return

    def getKeyString(self):
        return """%s(%s)""" % (self.out_link_id, self.gid)

    def toString(self):
        strTemp = """field count: 15\n"""
        strTemp += """gid: %s\n""" % self.gid
        strTemp += """in_link_id: %s\n""" % self.in_link_id
        strTemp += """in_link_id_t: %s\n""" % self.in_link_id_t
        strTemp += """node_id: %s\n""" % self.node_id
        strTemp += """node_id_t: %s\n""" % self.node_id_t
        strTemp += """out_link_id: %s\n""" % self.out_link_id
        strTemp += """out_link_id_t: %s\n""" % self.out_link_id_t
        strTemp += """sp_name: %s\n""" % self.sp_name
        strTemp += """passlink_count: %s\n""" % self.passlink_count
        strTemp += """is_pattern: %s\n""" % self.is_pattern
        strTemp += """pattern_id: %s\n""" % self.pattern_id
        strTemp += """arrow_id: %s\n""" % self.arrow_id
        strTemp += """arrow_id: %s\n""" % self.order_id
        strTemp += """pattern_name: %s\n""" % self.pattern_name
        strTemp += """arrow_name: %s\n""" % self.arrow_name
        return strTemp

    def getPatternPicture(self, errMsg):
        if self.pattern_data == None:
            errMsg[0] = """pattern dat is null."""
            return None
        patternDatParser = DatParser()
        patternDatParser.initFromMemory(errMsg, self.pattern_data)
        return patternDatParser.getDatContentByIndex(errMsg, 0)

    def getArrowPicture(self, errMsg):
        if self.arrow_data == None:
            errMsg[0] = """arrow dat is null."""
            return None
        arrowDatParser = DatParser()
        arrowDatParser.initFromMemory(errMsg, self.arrow_data)
        return arrowDatParser.getDatContentByIndex(errMsg, 0)