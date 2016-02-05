#encoding=utf-8
from NodeDataItem import NodeDataItem
from LaneDataItem import LaneDataItem
from SignpostDataItem import SignpostDataItem
from SpotguideDataItem import SpotguideDataItem

class GuideDataManager(object):
    def __init__(self, pg, nodeDataItemList):
        self.pg = pg
        self.nodeDataItemList = nodeDataItemList
        self.spotguideDataItemDict = {}
        self.laneDataItemDict = {}
        self.signpostDataItemDict = {}
        return

    def initData(self):
        self.initSpotguide()
        self.initSignpost()
        self.initLane()
        return

    def initSpotguide(self):
        strNodeIdList = ','.join(str(x.node_id).strip('L') for x in self.nodeDataItemList)
        sqlcmd = """
SET bytea_output TO escape;
select a.gid, a.in_link_id, a.in_link_id_t, a.node_id, a.node_id_t,
       a.out_link_id, a.out_link_id_t, a.type, a.passlink_count, a.pattern_id,
       a.arrow_id, a.is_exist_sar, a.order_id, b.image_id, b.data,
       c.image_id, c.data
from 
rdb_guideinfo_spotguidepoint as a
left join rdb_guideinfo_pic_blob_bytea as b
on a.pattern_id=b.gid
left join rdb_guideinfo_pic_blob_bytea as c
on a.arrow_id=c.gid
where a.node_id in (%s)
""" % (strNodeIdList)
        self.pg.execute(sqlcmd)
        rows = self.pg.fetchall()
        for row in rows:
            assert(len(row) >= 14)
            spotguideDataItem = SpotguideDataItem(row)
            self.spotguideDataItemDict[spotguideDataItem.getKeyString()] = spotguideDataItem
        return

    def initLane(self):
        strNodeIdList = ','.join(str(x.node_id).strip('L') for x in self.nodeDataItemList)
        sqlcmd = """
select a.guideinfo_id, a.in_link_id, a.in_link_id_t, a.node_id, a.node_id_t,
       a.out_link_id, a.out_link_id_t, a.lane_num, a.lane_info, a.arrow_info,
       a.lane_num_l, a.lane_num_r, a.passlink_count, a.exclusive, a.order_id
from 
rdb_guideinfo_lane as a
where a.node_id in (%s)
""" % (strNodeIdList)
        self.pg.execute(sqlcmd)
        rows = self.pg.fetchall()
        for row in rows:
            laneDataItem = LaneDataItem(row)
            if self.laneDataItemDict.has_key(laneDataItem.in_link_id) == False:
                self.laneDataItemDict[laneDataItem.in_link_id] = {}
            if self.laneDataItemDict[laneDataItem.in_link_id].has_key(laneDataItem.out_link_id) == False:
                self.laneDataItemDict[laneDataItem.in_link_id][laneDataItem.out_link_id] = []
            self.laneDataItemDict[laneDataItem.in_link_id][laneDataItem.out_link_id].append(laneDataItem)
        return

    def initSignpost(self):
        strNodeIdList = ','.join(str(x.node_id).strip('L') for x in self.nodeDataItemList)
        sqlcmd = """
SET bytea_output TO escape;
select a.gid, a.in_link_id, a.in_link_id_t, a.node_id, a.node_id_t, a.out_link_id, 
       a.out_link_id_t, a.sp_name, a.passlink_count, a.is_pattern, a.pattern_id, 
       a.arrow_id, a.order_id, b.image_id as pattern_name, c.image_id as arrow_name, 
       b.data as pattern_data, c.data as arrow_data
from
rdb_guideinfo_signpost as a
left join rdb_guideinfo_pic_blob_bytea as b
on a.pattern_id=b.gid
left join rdb_guideinfo_pic_blob_bytea as c
on a.arrow_id=c.gid
where a.node_id in (%s)
""" % (strNodeIdList)
        self.pg.execute(sqlcmd)
        rows = self.pg.fetchall()
        for row in rows:
            signpostDataItem = SignpostDataItem(row)
            self.signpostDataItemDict[signpostDataItem.getKeyString()] = signpostDataItem


