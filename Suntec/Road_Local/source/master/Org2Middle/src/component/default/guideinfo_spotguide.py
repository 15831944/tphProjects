# -*- coding: cp936 -*-
'''
Created on 2012-6-3

@author: hongchenzai
'''

import component.component_base

tollStationImageName = 'toll_station_image' # 此图由机能组指定。
roadType2SpotguideTypeMap = {0:1, 1:1, 2:4,
                             3:4, 4:4, 5:4,
                             6:4, 7:4, 8:4,
                             9:4, 10:4, 11:4,
                             12:4, 13:4, 14:4,
                             15:4}
class comp_guideinfo_spotguide(component.component_base.comp_base):
    def __init__(self):
        component.component_base.comp_base.__init__(self, 'spotguide_tbl')
        
    def _DoCreateTable(self):
        self.CreateTable2('spotguide_tbl')
        return 0
    
    def _DoCreateIndex(self):
        self.CreateIndex2('spotguide_tbl_node_id_idx')
        return 0
    
    # 将加油站作成spotguide点填充到中间表spotguide_tbl表中
    def _GenerateSpotguideTblForTollStation(self):
        # 从node_tbl搜刮出toll_flag=1的点。
        # 一些仕向地提供了toll station数据及元数据图片，这些数据在o2m的时候已被做到spotguide_tbl表中。
        # 为防止与这些toll station点重复，这里使用spotguide_tbl对搜出的toll_station列表进行了过滤。
        sqlcmd = """
                select a.node_id, array_agg(b.link_id) as slinkid, array_agg(b.one_way_code) as soneway, 
                array_agg(c.link_id) as elinkid, array_agg(c.one_way_code) as eoneway
                from
                node_tbl as a
                left join link_tbl as b
                on a.node_id=b.s_node
                left join link_tbl as c
                on a.node_id=c.e_node
                left join spotguide_tbl as d
                on a.node_id=d.nodeid
                where a.toll_flag=1
                and d.nodeid is null
                group by a.node_id
          """
          
        # arrowno字段留空，在作成rdb_guideinfo_spotguidepoint不考虑是否存在arrow图。
        # toll station 做成spotguide点时type定为12。
        # toll station 默认没有sar
        spotguide_tbl_insert_str = '''
                insert into spotguide_tbl(nodeid, inlinkid, outlinkid,
                                          passlid, passlink_cnt, direction,
                                          guideattr, namekind, guideclass,
                                          patternno, type,
                                          is_exist_sar)
                  values(%d, %d, %d,
                         '', 0, 0,
                         0, 0, 0,
                         '%s', 12,
                         false)
            '''
        self.pg.execute2(sqlcmd)
        rows = self.pg.fetchall2()
        for row in rows:
            node_id=row[0]
            sNodeLinkList = row[1] # 以此node为s_node的link列表
            sOneWayList = row[2] # 对应的one_way_code列表
            eNodeLinkList = row[3] # 以此node为e_node的link列表
            eOneWayList = row[4] # 对应的one_way_code列表
            
            inLinkList = []
            outLinkList = []
            # 根据s_node列表和它对应的one_way_code找出该node的inlink列表和outlink列表。
            for x in zip(sNodeLinkList, sOneWayList):
                if x[1] == 1: # 双向：双方向可以通行
                    inLinkList.append(x[0])
                    outLinkList.append(x[0])
                elif x[1] == 2: # 正向通行
                    outLinkList.append(x[0])
                elif x[1] == 3: # 反向通行
                    inLinkList.append(x[0])
                else:
                    continue
                
            # 根据e_node列表和它对应的one_way_code找出该node的inlink列表和outlink列表。
            for x in zip(eNodeLinkList, eOneWayList):
                if x[1] == 1:
                    inLinkList.append(x[0])
                    outLinkList.append(x[0])
                elif x[1] == 2:
                    inLinkList.append(x[0])
                elif x[1] == 3:
                    outLinkList.append(x[0])
                else:
                    continue
            
            # 根据求出的inlink列表和outlink列表作成inlink --> node --> outlink值对。 
            for oneInLink in inLinkList:
                for oneOutLink in outLinkList:
                    if oneInLink != oneOutLink:
                        if self.pg.execute2(spotguide_tbl_insert_str%\
                                            (node_id, oneInLink, oneOutLink, tollStationImageName)) == -1:
                            return -1
        
        self.pg.commit2()
        return 0
    

    