#encoding=utf-8
'''
Created on 2015-5-6
@author: tangpinghui
'''
from common import common_func
from component.component_base import comp_base

class comp_guideinfo_signpost_ni(comp_base):
    def __init__(self):
        comp_base.__init__(self, 'GuideInfo_SignPost')
        return

    def _DoCreateTable(self):
        self.CreateTable2('signpost_tbl')
        return

    def _DoCreatIndex(self):
        return 0

    def _DoCreateFunction(self):
        return 0

    def _Do(self):
        self.log.info("ni generate signpost_tbl begin.")
        self._CreateTempSignboardLinkTbl()
        self._GenerateSignpostTblFromOriSignboard()
        self.log.info("ni generate signpost_tbl end.")
        return 0
    
    def _GenerateSignpostTblFromOriSignboard(self):
        signpost_tbl_insert_sqlcmd = '''
                                        insert into signpost_tbl(nodeid, inlinkid, outlinkid,
                                                                 passlid, passlink_cnt, patternno, 
                                                                 arrowno, is_pattern)
                                        VALUES (%s, %s, %s,
                                                %s, %s, %s, 
                                                %s, %s)
                                    '''
        # 四维提供的协议所做的demo版本。
        # 由于没有元数据表，具体还需要等signpost协议到了以后进行调整
        rows = self.get_batch_data('''
                                    SELECT nodeid, inlinkid, outlinkid, patternno, 
                                           arrowno, passlid, passlid2, folder
                                    FROM org_signboard
                                    order by gid
                                   ''')
        for row in rows:     
            nodeid = row[0]
            inlinkid = row[1]
            outlinkid = row[2]
            patternno = row[3]
            arrowno = row[4]
            passlid = row[5]
            passlid2 = row[6]
            
            totalPasslid = passlid
            if(passlid and passlid2):
                totalPasslid = totalPasslid + '|' + passlid2
            totalPasslidCount = common_func.CountPassLink(totalPasslid)

            # 交叉路口时四维数据里给的nodeid是路口主点，RDB里要求的是紧邻inlink的第一个node点，故需自己求出nodeid
            nodeid = None
            if totalPasslid == '':
                nodeid = common_func._GetNodeBetweenLinks(self.pg, inlinkid, outlinkid, 
                                                          "temp_signboard_link_ni")
            else:
                nodeid = common_func._GetNodeBetweenLinks(self.pg, inlinkid, totalPasslid.split("|")[0], 
                                                          "temp_signboard_link_ni")
            if nodeid is None:
                self.log.error('one org_signboard record can\'t find correct nodeid. inlinkid: %s, ' \
                               'outlinkid: %s, passlid: %s' % (inlinkid, outlinkid, totalPasslid))
                # 找不到合适的nodeid，丢弃此条数据
                continue
                         
            self.pg.execute2(signpost_tbl_insert_sqlcmd, 
                             (nodeid, inlinkid, outlinkid,
                              totalPasslid, totalPasslidCount, patternno, 
                              arrowno, True))
        self.pg.commit2()
        return
    
    # 优化：建立临时表，查询link两端节点时使用，提高查询速度
    def _CreateTempSignboardLinkTbl(self):
        sqlcmd = '''
                    drop table if exists temp_signboard_link_ni;
                    
                    select a.* 
                    into temp_signboard_link_ni
                    from link_tbl as a
                    left join org_signboard as b
                    on a.link_id=b.inlinkid::bigint
                    left join org_signboard as c
                    on a.link_id=c.outlinkid::bigint
                    left join org_signboard as d
                    on cast(a.link_id as text)=split_part(d.passlid, '|', 1)
                    where b.inlinkid is not null
                    or c.outlinkid is not null
                    or d.passlid is not null;
                    
                    drop index if exists temp_signboard_link_ni_id_idx;
                    create index temp_signboard_link_ni_id_idx
                    on temp_signboard_link_ni
                    using btree
                    (link_id);
                '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        return
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        