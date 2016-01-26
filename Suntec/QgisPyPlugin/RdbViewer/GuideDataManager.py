#encoding=utf-8

class GuideDataManager(object):
    def __init__(self, pg, nodeidList):
        self.pg = pg
        self.nodeidList = nodeidList
        self.spotguideDataItemDict = {}
        self.laneDataItemDict = {}
        return

    def initData(self):
        sqlcmd = """
select a.*, b.image_id, b.data, c.image_id, c.data 
from 
rdb_guideinfo_spotguidepoint as a
left join rdb_guideinfo_pic_blob_bytea as b
on a.pattern_id=b.gid
left join rdb_guideinfo_pic_blob_bytea as c
on a.arrow_id=b.gid
where a.node_id in (%s)
""" % self.nodeidList
        self.pg.execute(sqlcmd)
        rows = self.pg.fetchall()
        for row in rows:
            spotguideDataItem = SpotguideDataItem(row)
            self.spotguideDataItemDict[spotguideDataItem.getKeyString()] = spotguideDataItem
        return


