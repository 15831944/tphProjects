#encoding=utf-8

class RdbLinkObject(object):
    speed_roadtype_toll = [
        [ 80. * 0.277778, 80. * 0.277778 ], #///< road kind = 0
        [ 60. * 0.277778, 65. * 0.277778 ], #///< 1
        [ 40. * 0.277778, 60. * 0.277778 ], #///< 2
        [ 35. * 0.277778, 45. * 0.277778 ], #///< 3
        [ 28. * 0.277778, 35. * 0.277778 ], #///< 4
        [ 25. * 0.277778, 30. * 0.277778 ], #///< 5
        [ 20. * 0.277778, 25. * 0.277778 ], #///< 6
        [ 20. * 0.277778, 20. * 0.277778 ], #///< 7
        [ 15. * 0.277778, 15. * 0.277778 ], #///< 8
        [ 15. * 0.277778, 15. * 0.277778 ], #///< 9
        [ 30. * 0.277778, 30. * 0.277778 ], #///< 10
        [ 30. * 0.277778, 30. * 0.277778 ], #///< 11
        [ 30. * 0.277778, 30. * 0.277778 ], #///< 12
        [ 30. * 0.277778, 30. * 0.277778 ], #///< 13
        [ 30. * 0.277778, 30. * 0.277778 ], #///< 14
        [ 30. * 0.277778, 30. * 0.277778 ], #///< 15
    ]

    # width, one_way
    speed_width = [
        [0.4, 0.54],
        [0.66, 0.83],
        [1.05, 1.14],
        [1.13, 1.18],
        [1.22, 1.31]
    ]
    
    cost_turn = [ 
        200, 200, 200, 200, 200, 200, 200, 100, 100, #///< 0 ~ 45
        80, 80, 80, 15, 15, 15, 15, 15, 15, #///< 45 ~ 90
        13, 13, 13, 10, 10, 10, 8, 8, 8, #///< 90 ~ 135
        6, 6, 6, 0, 0, 0, 0, 0, 0, #///< 135 ~ 180
        0, 0, 0, 0, 0, 0, 5, 5, 5, #///< 180 ~ 225
        8, 8, 8, 8, 8, 8, 10, 10, 10, #///< 225 ~ 270
        10, 10, 10, 10, 10, 10, 80, 80, 80, #///< 270 ~ 315
        100, 100, 200, 200, 200, 200, 200, 200, 200, #///< 315 ~ 360
        0, #///< delta angle equals to 360 degree
    ]

    def __init__(self):
        self.link_id = -1
        self.road_type = -1
        self.toll = -1
        self.link_length = -1
        self.one_way = -1
        self.start_node_id = -1
        self.end_node_id = -1
        self.fazm = -1
        self.tazm = -1
        return
    
    def initFromRecord_11(self, errMsg, record):
        if len(record) <> 9:
            tmpStr = "invalid input argument record: "
            tmpStr += self.getRecordString(record)
            errMsg[0] = "tmpStr"
            return
        
        self.link_id = record[0]
        self.road_type = record[1]
        self.toll = record[2]
        self.link_length = record[3]
        self.one_way = record[4]
        self.start_node_id = record[5]
        self.end_node_id = record[6]
        self.fazm = record[7]
        self.tazm = record[8]
    
    def getArgDist_11(self):
        dict = {}
        dict['link_id'] = self.link_id
        dict['weight'] = 1
        return dict
       
    def getRecordString(self, record):
        tmpStr = "length=%s, ["
        for i in range(0, len(record)):
            tmpStr += str(record[i]) + ", "
        tmpStr = tmpStr.strip(", ")+"]"
        return tmpStr

    def getCost(self):
        if self.toll != 1:
            self.toll = 0
        
        if self.width == 4:
            self.width = 2
        
        if self.one_way in (2,3):
            self.one_way = 1
        else:
            self.one_way = 0
        
        in_angle = 100
        out_angle = 100
        if in_angle is None:
            angle = 360
        else:
            in_angle = (in_angle + 32768) * 360.0 / 65536.0
            out_angle = (out_angle + 32768) * 360.0 / 65536.0
            angle = int(out_angle - in_angle)
            if angle <= 0:
                angle += 360
            if angle > 360:
                angle -= 360
        angle = int(angle / 5)
        
        speed = RdbLinkObject.speed_roadtype_toll[road_type][toll] * \
                RdbLinkObject.speed_width[width][one_way]
        cost = link_length / speed + RdbLinkObject.cost_turn[angle]
        return cost

    @staticmethod
    def isMyFeature(theFeature):
        return True
















