# -*- coding: cp936 -*-
'''
Created on 2015-5-6
@author: 
'''
from common import common_func
from component.default.guideinfo_spotguide import comp_guideinfo_spotguide
from component.default.guideinfo_spotguide import roadType2SpotguideTypeMap

spotguide_tbl_insert_sqlcmd = '''
            insert into spotguide_tbl(nodeid, inlinkid, outlinkid,
                                      passlid, passlink_cnt, direction,
                                      guideattr, namekind, guideclass,
                                      patternno, arrowno, type,
                                      is_exist_sar)
            values( %s, %s, %s,
                    %s, %s, %s,
                    %s, %s, %s,
                    %s, %s, %s,
                    %s)'''

class comp_guideinfo_spotguide_ni(comp_guideinfo_spotguide):
    def __init__(self):
        comp_guideinfo_spotguide.__init__(self)

    def _DoCreateTable(self):
        comp_guideinfo_spotguide._DoCreateTable(self)
        return 0

    def _DoCreatIndex(self):
        return 0

    def _DoCreateFunction(self):
        return 0

    def _Do(self):
        self.log.info("ni generate spotguide_tbl begin.")
        self._CreateTempSpotguideLinkTbl()
        self._GenerateSpotguideTblFromBr()
        self._GenerateSpotguideTblFromDm()
        comp_guideinfo_spotguide._GenerateSpotguideTblForTollStation(self)
        self.log.info("ni generate spotguide_tbl end.")
        return 0

    # �Ż���������ʱ����ѯlink���˽ڵ�ʱʹ�ã���߲�ѯ�ٶ�
    def _CreateTempSpotguideLinkTbl(self):
        sqlcmd = '''
                    drop table if exists temp_spotguide_link_ni;
                    
                    select a.* 
                    into temp_spotguide_link_ni
                    from link_tbl as a
                    left join org_br as b
                    on a.link_id=b.inlinkid::bigint 
                    left join org_br as c
                    on a.link_id=c.outlinkid::bigint
                    left join org_br as d
                    on cast(a.link_id as text)=split_part(d.passlid, '|', 1)
                    left join org_dm as e
                    on a.link_id=e.inlinkid::bigint
                    left join org_dm as f
                    on a.link_id=f.outlinkid::bigint
                    left join org_dm as g
                    on cast(a.link_id as text)=split_part(g.passlid, '|', 1)
                    where b.inlinkid is not null
                    or c.outlinkid is not null
                    or d.passlid is not null
                    or e.inlinkid is not null
                    or f.outlinkid is not null
                    or g.passlid is not null;
                    
                    drop index if exists temp_spotguide_link_ni_id_idx;
                    create index temp_spotguide_link_ni_id_idx
                    on temp_spotguide_link_ni
                    using btree
                    (link_id);
                 ''' 
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        return        

    # �����ģʽͼ��·�ڷŴ�ͼ�ĵ����Ϣ
    def _GenerateSpotguideTblFromBr(self):
        # ��ά���ݸ���nodeid��·�����㣬��ʹ�ã�nodeid���������
        # spotguide��������Ҫ����inlink��road_type����˲�ʹ����ά���ݸ�jv_type��
        org_br_query_sqlcmd = '''
                    SELECT a.inlinkid, a.outlinkid, a.direction, a.patternno, a.arrowno, 
                           a.guidattr, a.namekind, a.passlid, a.passlid2, b.road_type
                    FROM org_br as a
                    LEFT JOIN link_tbl as b
                    ON a.inlinkid=b.link_id
                    WHERE patternno is not null and arrowno is not null and "type" <> '6'
                    ORDER BY gid
                '''
        rows = self.get_batch_data(org_br_query_sqlcmd)
        for row in rows:
            inlinkid = row[0]
            outlinkid = row[1]
            direction = row[2]
            patternno = row[3]
            arrowno = row[4]
            guidattr = row[5]
            namekind = row[6]
            passlid = row[7]
            passlid2 = row[8]
            inlinkRoadType = row[9]
                        
            totalPasslid = passlid
            if(passlid and passlid2):
                totalPasslid = totalPasslid + '|' + passlid2
            totalPasslidCount = common_func.CountPassLink(totalPasslid)
            
            # ����·��ʱ��ά���������nodeid��·�����㣬RDB��Ҫ����ǽ���inlink�ĵ�һ��node�㣬�����Լ����nodeid
            nodeid = None
            if totalPasslid == '':
                nodeid = common_func._GetNodeBetweenLinks(self.pg, inlinkid, outlinkid, 
                                                          "temp_spotguide_link_ni")
            else:
                nodeid = common_func._GetNodeBetweenLinks(self.pg, inlinkid, totalPasslid.split("|")[0], 
                                                          "temp_spotguide_link_ni")
            if nodeid is None:
                self.log.error('one org_br record can\'t find correct nodeid. inlinkid: %s, '\
                               'outlinkid: %s, passlid: %s' % (inlinkid, outlinkid, totalPasslid))
                # �Ҳ������ʵ�nodeid��������������
                continue
            
            # �й�������û��SAR
            isExistSar = False 
            
            # ����inlink��road_typeȷ��spotguide���͡�
            spotguideType = roadType2SpotguideTypeMap[inlinkRoadType]
            
            self.pg.execute2(spotguide_tbl_insert_sqlcmd, 
                             (nodeid, inlinkid, outlinkid, 
                              totalPasslid, totalPasslidCount, direction,
                              guidattr, namekind, 0,
                              patternno, arrowno, spotguideType, isExistSar))
        self.pg.commit2()
    
    # ��3D�����ģʽͼ�ŵĵ����Ϣ����spotguide_tbl    
    def _GenerateSpotguideTblFromDm(self):        
        rows = self.get_batch_data( '''
                                    SELECT nodeid, inlinkid, outlinkid, patternno, 
                                           arrowno, passlid, folder
                                    FROM org_dm
                                    order by gid
                                    ''' )
        for row in rows:        
            #nodeid = row[0]
            inlinkid = row[1]
            outlinkid = row[2]
            patternno = row[3] 
            arrowno = row[4]
            totalPasslid = row[5]
                         
            passlidCount = common_func.CountPassLink(totalPasslid)
            
            # ����·��ʱ��ά���������nodeid��·�����㣬RDB��Ҫ����ǽ���inlink�ĵ�һ��node�㣬�����Լ����nodeid
            nodeid = None
            if totalPasslid == '':
                nodeid = common_func._GetNodeBetweenLinks(self.pg, inlinkid, outlinkid, 
                                                          "temp_spotguide_link_ni")
            else:
                nodeid = common_func._GetNodeBetweenLinks(self.pg, inlinkid, totalPasslid.split("|")[0], 
                                                          "temp_spotguide_link_ni")
            if nodeid is None:
                self.log.error('one org_dm record can\'t find correct nodeid. inlinkid: %s, '\
                               'outlinkid: %s, passlid: %s' % (inlinkid, outlinkid, totalPasslid))
                # �Ҳ������ʵ�nodeid��������������
                continue
            
            # �й�������û��SAR
            isExistSar = False
            
            # 3D�����ģʽͼ����Ӧ����Ϊ7
            jv_type = 7
            
            self.pg.execute2(spotguide_tbl_insert_sqlcmd, 
                             (nodeid, inlinkid, outlinkid, 
                              totalPasslid, passlidCount, 0,
                              0, 0, 0,
                              patternno, arrowno, jv_type, isExistSar))
        self.pg.commit2()
        return

            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
        
        
        