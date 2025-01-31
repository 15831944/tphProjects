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
        self.mMDPSPicSet = set() # 连续分歧illust图片列表

    def _DoCreateTable(self):
        comp_guideinfo_spotguide._DoCreateTable(self)
        return 0

    def _DoCreatIndex(self):
        return 0

    def _DoCreateFunction(self):
        return 0

    def _Do(self):
        self.log.info("ni generate spotguide_tbl begin.")
        self.initMDPSPicList(self.mMDPSPicSet)
        self._CreateTempSpotguideLinkTbl()
        self._GenerateSpotguideTblFromBr()
        self._GenerateSpotguideTblFromDm()
        self._GenerateSpotguideTblFromTollPattern()
        #tollIllustName = common_func.GetPath('toll_station_illust')
        #comp_guideinfo_spotguide._GenerateSpotguideTblForTollStation(self, tollIllustName)
        self.log.info("ni generate spotguide_tbl end.")
        return 0
    
    # guide组要求，需要将连续分歧illust图单独区分成spotguide类型13.
    # 需要对数据进行预处理，人工对四维提供的原始图片进行识别，把所有连续分歧illust图的名字列出至文件mdps_pic_list中。
    # 将分歧illust都列入mMDPSPicSet中。
    def initMDPSPicList(self, MDPSPicSet):
        mdps_pic_list = common_func.GetPath('mdps_pic_list')
        if mdps_pic_list == '':
            self.log.error("get attribute failed: %s" % mdps_pic_list)
            return
    
        with open(mdps_pic_list, 'r') as iFStream:
            listline = iFStream.readlines()
            for line in listline:
                MDPSPicSet.add(line.lower().split('.')[0])
        
    # 优化：建立临时表，查询link两端节点时使用，提高查询速度
    def _CreateTempSpotguideLinkTbl(self):
        self.log.info("generate temp_spotguide_link_ni...")
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

    # 分歧点模式图、路口放大图的点的信息
    def _GenerateSpotguideTblFromBr(self):
        self.log.info("generate spotguide from br...")
        org_br_query_sqlcmd = '''
                    SELECT nodeid, inlinkid, outlinkid, direction, patternno, arrowno, 
                           guidattr, namekind, passlid, passlid2, "type", folder
                    FROM org_br
                    where patternno is not null and arrowno is not null and "type" <> '6'
                    order by gid
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
                        
            totalPasslid = passlid
            if(passlid and passlid2):
                totalPasslid = totalPasslid + '|' + passlid2
            totalPasslidCount = common_func.CountPassLink(totalPasslid)
            
            # 交叉路口时四维数据里给的nodeid是路口主点，RDB里要求的是紧邻inlink的第一个node点，故需自己求出nodeid
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
                # 找不到合适的nodeid，丢弃此条数据
                continue
            
            # 中国的数据没有SAR
            isExistSar = False 
            
            # 连续分歧illust图，类型为13
            if patternno in self.mMDPSPicSet:
                jv_type = 13
            
            self.pg.execute2(spotguide_tbl_insert_sqlcmd, 
                             (nodeid, inlinkid, outlinkid, 
                              totalPasslid, totalPasslidCount, direction,
                              guidattr, namekind, 0,
                              patternno, arrowno, jv_type, isExistSar))
        self.pg.commit2()
    
    # 将3D交叉点模式图号的点的信息插入spotguide_tbl    
    def _GenerateSpotguideTblFromDm(self):        
        self.log.info("generate spotguide from dm...")
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
            
            # 交叉路口时四维数据里给的nodeid是路口主点，RDB里要求的是紧邻inlink的第一个node点，故需自己求出nodeid
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
                # 找不到合适的nodeid，丢弃此条数据
                continue
            
            # 中国的数据没有SAR
            isExistSar = False
            
            # 3D交叉点模式图，对应类型为7
            jv_type = 7
            # 连续分歧illust图，类型为13
            if patternno in self.mMDPSPicSet:
                jv_type = 13
            
            self.pg.execute2(spotguide_tbl_insert_sqlcmd, 
                             (nodeid, inlinkid, outlinkid, 
                              totalPasslid, passlidCount, 0,
                              0, 0, 0,
                              patternno, arrowno, jv_type, isExistSar))
        self.pg.commit2()
        return

    def _GenerateSpotguideTblFromTollPattern(self):
        if self.pg.IsExistTable('org_toll_pattern'):
            self.log.info("generate spotguide from toll pattern...")
            sqlcmd = """
                    insert into spotguide_tbl(
                                              nodeid, inlinkid, outlinkid, passlid, passlink_cnt, 
                                              direction, guideattr, namekind, guideclass,
                                              patternno, arrowno, type, is_exist_sar
                                              )
                    select  (
                            case 
                                when c.new_node_id is null then a.id::bigint
                                else c.new_node_id::bigint 
                            end
                            ) as ndoeid,
                            a.inlinkid::bigint,
                            a.outlinkid::bigint,
                            null as passlid,
                            0 as passlink_cnt,
                            0, 0, 0, 0,
                            b.patternno,
                            b.arrowno,
                            12 as type,
                            false as is_exist_sar
                    from org_c as a
                    inner join org_toll_pattern as b
                    on a.condid = b.condid
                    left join temp_node_mapping as c
                    on a.id = c.old_node_id
                    where a.condtype = '3'
                    """
            self.pg.execute2(sqlcmd)
            self.pg.commit2()
        