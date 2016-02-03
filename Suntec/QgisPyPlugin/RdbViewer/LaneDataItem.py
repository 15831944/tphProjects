#encoding=utf-8

class LaneDataItem(object):
    arrowImagesMap_highlight = {
            0: ":/lane/0.png",
            2**0 : ":/lane/1.png",
            2**1 : ":/lane/2.png",
            2**2 : ":/lane/4.png",
            2**3 : ":/lane/8.png",
            2**4 : ":/lane/16.png",
            2**5 : ":/lane/32.png",
            2**6 : ":/lane/64.png",
            2**7 : ":/lane/128.png",
            2**8 : ":/lane/256.png",
            2**9 : ":/lane/512.png",
            2**10 : ":/lane/1024.png",
            2**11 : ":/lane/2048.png",
            2**12 : ":/lane/4096.png",
            2**13 : ":/lane/8192.png"
            }

    arrowImagesMap_gray = {
            0: ":/lane/0_gray.png",
            2**0 : ":/lane/1_gray.png",
            2**1 : ":/lane/2_gray.png",
            2**2 : ":/lane/4_gray.png",
            2**3 : ":/lane/8_gray.png",
            2**4 : ":/lane/16_gray.png",
            2**5 : ":/lane/32_gray.png",
            2**6 : ":/lane/64_gray.png",
            2**7 : ":/lane/128_gray.png",
            2**8 : ":/lane/256_gray.png",
            2**9 : ":/lane/512_gray.png",
            2**10 : ":/lane/1024_gray.png",
            2**11 : ":/lane/2048_gray.png",
            2**12 : ":/lane/4096_gray.png",
            2**13 : ":/lane/8192_gray.png"
            }

    def __init__(self, record):
        self.guideinfo_id = record[0]
        self.in_link_id = record[1]
        self.in_link_id_t = record[2]
        self.node_id = record[3]
        self.node_id_t = record[4]
        self.out_link_id = record[5]
        self.out_link_id_t = record[6]
        self.lane_num = record[7]
        self.lane_info = record[8]
        self.arrow_info = record[9]
        self.lane_num_l = record[10]
        self.lane_num_r = record[11]
        self.passlink_count = record[12]
        self.exclusive = record[13]
        self.order_id = record[14]
        return

    def toString(self):
        strTemp = """field count: 15\n"""
        strTemp += """guideinfo_id: %s\n""" % self.guideinfo_id
        strTemp += """in_link_id: %s\n""" % self.in_link_id
        strTemp += """in_link_id_t: %s\n""" % self.in_link_id_t
        strTemp += """node_id: %s\n""" % self.node_id
        strTemp += """node_id_t: %s\n""" % self.node_id_t
        strTemp += """out_link_id: %s\n""" % self.out_link_id
        strTemp += """out_link_id_t: %s\n""" % self.out_link_id_t
        strTemp += """lane_num: %s\n""" % self.lane_num
        strTemp += """lane_info: %s\n""" % self.lane_info
        strTemp += """arrow_info: %s\n""" % self.arrow_info
        strTemp += """lane_num_l: %s\n""" % self.lane_num_l
        strTemp += """lane_num_r: %s\n""" % self.lane_num_r
        strTemp += """passlink_count: %s\n""" % self.passlink_count
        strTemp += """exclusive: %s\n""" % self.exclusive
        strTemp += """order_id: %s\n""" % self.order_id
        return strTemp

    def getKeyString(self):
        return """inlink:%s, node:%s, outlink:%s""" % (self.in_link_id, self.node_id, self.out_link_id)

    def getPicPath_gray(self):
        picPathList = []
        if self.arrow_info == 0:
            picPathList.append(LaneDataItem.arrowImagesMap_gray[0])
        else:
            for oneKey, oneImagePath in LaneDataItem.arrowImagesMap_gray.items():
                if self.arrow_info & oneKey:
                    picPathList.append(oneImagePath)
        return picPathList

    def getPicPath_highlight(self):
        picPathList = []
        if self.arrow_info == 0:
            picPathList.append(LaneDataItem.arrowImagesMap_gray[0])
        else:
            for oneKey, oneImagePath in LaneDataItem.arrowImagesMap_highlight.items():
                if self.arrow_info & oneKey:
                    picPathList.append(oneImagePath)
        return picPathList

    # returns:
    # inlinkid, nodeid, outlinkid
    @staticmethod
    def parseKeyString(errMsg, strKeyString):
        if strKeyString.find('inlink:') == -1 or \
            strKeyString.find('node:') == -1 or \
            strKeyString.find('outlink:') == -1:
            errMsg[0] = """input key string is not valid."""
            return None, None, None
        
        strTemp = strKeyString.replace('inlink:', '')
        strTemp = strTemp.replace('node:', '')
        strTemp = strTemp.replace('outlink:', '')

        strSplit = strTemp.split(',')
        return int(strSplit[0]), int(strSplit[1]), int(strSplit[2])




