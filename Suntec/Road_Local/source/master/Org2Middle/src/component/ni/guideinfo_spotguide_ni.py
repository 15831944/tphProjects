# -*- coding: cp936 -*-
'''
Created on 2015-5-6
@author: 
'''
from common import common_func
from component.default.guideinfo_spotguide import comp_guideinfo_spotguide

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
        self.log.info("ni generate spotguide_tbl end.")
        return 0

    # 优化：建立临时表，查询link两端节点时使用，提高查询速度
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
                    
                    select * from temp_spotguide_link_ni;
                    
                    drop index if exists temp_spotguide_link_ni_id_idx;
                    create index temp_spotguide_link_ni_id_idx
                    on temp_spotguide_link_ni
                    using btree
                    (link_id);
                 ''' 
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        return        

    # 分歧点模式图、路口放大图的点的信息
    def _GenerateSpotguideTblFromBr(self):
        org_br_query_sqlcmd = '''
                    SELECT nodeid, inlinkid, outlinkid, direction, patternno, arrowno, 
                           guidattr, namekind, passlid, passlid2, "type", folder
                    FROM org_br
                    where patternno is not null and arrowno is not null and "type" <> '6'
                    order by inlinkid::bigint
                '''
        rows = self.get_batch_data(org_br_query_sqlcmd)
        for row in rows:     
            #nodeid = row[0]
            inlinkid = row[1]
            outlinkid = row[2]
            direction = row[3]
            patternno = row[4]
            arrowno = row[5]
            guidattr = row[6]
            namekind = row[7]
            passlid = row[8]
            passlid2 = row[9]
            jv_type = row[10]
            folder = row[11]
                        
            totalPasslid = passlid
            if(passlid and passlid2):
                totalPasslid = totalPasslid + '|' + passlid2
            totalPasslidCount = common_func.CountPassLink(totalPasslid)
            
            # 交叉路口时四维数据里给的nodeid是路口主点，RDB里要求的是紧邻inlink的第一个node点，故需自己求出nodeid
            nodeid = common_func._GetNodeAfterInlinkid(self.pg, inlinkid, outlinkid, 
                                                       totalPasslid, "temp_spotguide_link_ni")
            if nodeid is None:
                self.log.error('one org_br record can\'t find correct nodeid. inlinkid: %s, '\
                               'outlinkid: %s, passlid: %s' % (inlinkid, outlinkid, totalPasslid))
                # 找不到合适的nodeid，丢弃此条数据
                continue
            
            # 经过查证，各个folder下可能存在同名的patternno或arrowno图片。
            # 例如20150508所做的四维中国数据中存在以下情况：
            # pattern图有：beijing\pattern\40210371.png 和 guangzhou\pattern\40210371.png两张同名图片
            # arrow图有：beijing\arrow\224019.png 和 guangzhou\arrow\224019。png两张同名图片
            # 目前情况（肉眼判断）：两个同名pattern图和arrow图是相同的。
            # 为了预防它们有可能存在不同，在转数据时把folder加成后缀，制作插图时必须遵循此规则。
            fixedPatternno = patternno + '_' + folder
            fixedArrowno = arrowno + '_' + folder
            
            # 中国的数据没有SAR
            isExistSar = False 
            
            self.pg.execute2(spotguide_tbl_insert_sqlcmd, 
                             (nodeid, inlinkid, outlinkid, 
                              totalPasslid, totalPasslidCount, direction,
                              guidattr, namekind, 0,
                              fixedPatternno, fixedArrowno, jv_type, isExistSar))
        self.pg.commit2()
    
    # 将3D交叉点模式图号的点的信息插入spotguide_tbl    
    def _GenerateSpotguideTblFromDm(self):        
        rows = self.get_batch_data( '''
                                    SELECT nodeid, inlinkid, outlinkid, patternno, 
                                           arrowno, passlid, folder
                                    FROM org_dm
                                    order by inlinkid::bigint
                                    ''' )
        for row in rows:        
            #nodeid = row[0]
            inlinkid = row[1]
            outlinkid = row[2]
            patternno = row[3] 
            arrowno = row[4]
            passlid = row[5]
            folder = row[6]
                         
            passlidCount = common_func.CountPassLink(passlid)
            
            # 经过查证，各个folder下可能存在同名的patternno和arrowno图片。
            # 例如20150508所做的四维中国数据中存在以下情况：
            # pattern图有：beijing\pattern\40210371.png 和 guangzhou\pattern\40210371.png两张同名图片
            # arrow图有：beijing\arrow\224019.png 和 guangzhou\arrow\224019。png两张同名图片
            # 目前情况（肉眼判断）：两个同名pattern图和arrow图是相同的。
            # 为了预防它们有可能存在不同，在转数据时把folder加成后缀，制作插图时必须遵循此规则。
            fixedPatternno = patternno + '_' + folder
            fixedArrowno = arrowno + '_' + folder    
            
            # 交叉路口时四维数据里给的nodeid是路口主点，RDB里要求的是紧邻inlink的第一个node点，故需自己求出nodeid
            nodeid = common_func._GetNodeAfterInlinkid(self.pg, inlinkid, outlinkid, 
                                                       passlid, "temp_spotguide_link_ni")
            if nodeid is None:
                self.log.error('one org_dm record can\'t find correct nodeid. inlinkid: %s, '\
                               'outlinkid: %s, passlid: %s' % (inlinkid, outlinkid, passlid))
                # 找不到合适的nodeid，丢弃此条数据
                continue
            
            # 中国的数据没有SAR
            isExistSar = False
            
            # 3D交叉点模式图，对应类型为7
            jv_type = 7
            
            self.pg.execute2(spotguide_tbl_insert_sqlcmd, 
                             (nodeid, inlinkid, outlinkid, 
                              passlid, passlidCount, 0,
                              0, 0, 0,
                              fixedPatternno, fixedArrowno, jv_type, isExistSar))
        self.pg.commit2()
        return


            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
        
        
        