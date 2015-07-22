# -*- coding: cp936 -*-
'''
Created on 2012-6-3

@author: hongchenzai
'''

import component.component_base

tollStationImageName = 'toll_station_image' # ��ͼ�ɻ�����ָ����
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
    
    # ������վ����spotguide����䵽�м��spotguide_tbl����
    def _GenerateSpotguideTblForTollStation(self):
        # ��node_tbl�ѹγ�toll_flag=1�ĵ㡣
        # һЩ������ṩ��toll station���ݼ�Ԫ����ͼƬ����Щ������o2m��ʱ���ѱ�����spotguide_tbl���С�
        # Ϊ��ֹ����Щtoll station���ظ�������ʹ��spotguide_tbl���ѳ���toll_station�б�����˹��ˡ�
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
          
        # arrowno�ֶ����գ�������rdb_guideinfo_spotguidepoint�������Ƿ����arrowͼ��
        # toll station ����spotguide��ʱtype��Ϊ12��
        # toll station Ĭ��û��sar
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
            sNodeLinkList = row[1] # �Դ�nodeΪs_node��link�б�
            sOneWayList = row[2] # ��Ӧ��one_way_code�б�
            eNodeLinkList = row[3] # �Դ�nodeΪe_node��link�б�
            eOneWayList = row[4] # ��Ӧ��one_way_code�б�
            
            inLinkList = []
            outLinkList = []
            # ����s_node�б������Ӧ��one_way_code�ҳ���node��inlink�б��outlink�б�
            for x in zip(sNodeLinkList, sOneWayList):
                if x[1] == 1: # ˫��˫�������ͨ��
                    inLinkList.append(x[0])
                    outLinkList.append(x[0])
                elif x[1] == 2: # ����ͨ��
                    outLinkList.append(x[0])
                elif x[1] == 3: # ����ͨ��
                    inLinkList.append(x[0])
                else:
                    continue
                
            # ����e_node�б������Ӧ��one_way_code�ҳ���node��inlink�б��outlink�б�
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
            
            # ���������inlink�б��outlink�б�����inlink --> node --> outlinkֵ�ԡ� 
            for oneInLink in inLinkList:
                for oneOutLink in outLinkList:
                    if oneInLink != oneOutLink:
                        if self.pg.execute2(spotguide_tbl_insert_str%\
                                            (node_id, oneInLink, oneOutLink, tollStationImageName)) == -1:
                            return -1
        
        self.pg.commit2()
        return 0
    

    