# -*- coding: cp936 -*-
'''
Created on 2012-3-27

@author: liuxinxing
'''
import component.component_base

class comp_guideinfo_signpost_jdb(component.component_base.comp_base):
    def __init__(self):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self, 'GuideInfo_SignPost')
    
    def _DoCreateTable(self):
        #self.CreateTable2('road_dir_t')
        self.CreateTable2('temp_signpost_tbl')
        self.CreateTable2('signpost_tbl')
        
    def _Do(self):
        self.log.info('Begin convert SignPost from road_dir...')
        
        self.CreateFunction2('mid_getSplitLinkID_dir')
        self.CreateFunction2('mid_get_new_passlid_dir')
        self.CreateFunction2('mid_getSplitLinkIDarray_dir')
        self.CreateFunction2('mid_getConnectionNodeID')
        
        self.CreateFunction2('mid_convert_signpost')
        
        self.pg.callproc('mid_convert_signpost')
        
        self.pg.commit2()
        
        # 删除多余的记录(番号or空)
        self._DeleteUnnecessaryRouteNumber()
        
        sqlcmd = """
                
            insert into signpost_tbl(
                id,nodeid,inlinkid,outlinkid,passlid
                ,passlink_cnt,direction,guideattr
                ,namekind,guideclass,sp_name
                ,patternno,arrowno)
            (
                select id,nodeid,inlinkid,outlinkid,passlid
                    ,passlink_cnt,direction,guideattr
                    ,namekind,guideclass,sp_name
                    ,patternno,arrowno
                  from temp_signpost_tbl
                  order by gid
            );
            """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        
        self.log.info('End convert SignPost from road_dir.')
        
        return 0
    
    def _DeleteUnnecessaryRouteNumber(self):
        """删除多余的记录(番号or空)
           #1. 同方向有方面，就不要番号(无需再做)
           #2. 同方向有多个番号，只保留一条空文字列记录(gid最小的)(无需再做)
           3. 所有方向有都是番号or空
        """
        # 同方向有方面，就不要番号
        sqlcmd = """
                delete from temp_signpost_tbl
                 where gid in (
                    select distinct a.gid
                      from (
                        SELECT gid, id, nodeid, inlinkid, outlinkid, passlid, passlink_cnt, 
                               direction, guideattr, namekind, guideclass, name_id, patternno, 
                               arrowno
                          FROM temp_signpost_tbl
                          where guideclass <> -1 and name_id is null
                    ) as a
                    left join temp_signpost_tbl as b
                    on a.nodeid = b.nodeid and a.inlinkid = b.inlinkid and a.outlinkid = b.outlinkid 
                      and a.patternno = b.patternno and a.arrowno = b.arrowno and a.gid <> b.gid
                    where b.name_id is not null
                );
                """
        '''self.pg.execute2(sqlcmd)
        self.pg.commit2()'''
        
        # 同方向有多个番号，只保留一条空文字列记录(gid最小的)
        sqlcmd = """
                delete from temp_signpost_tbl
                 where gid in (
                    SELECT gid
                      FROM temp_signpost_tbl
                      where guideclass <> -1 and name_id is null and gid not in (
                        SELECT min(gid)
                          FROM temp_signpost_tbl
                          where guideclass <> -1 and name_id is null
                          group by nodeid, inlinkid, outlinkid, passlid, passlink_cnt, patternno, arrowno
                      )
                 );
                 """
        '''self.pg.execute2(sqlcmd)
        self.pg.commit2()'''
        
        # 所有方向有都是番号or空
        sqlcmd = """
                delete from temp_signpost_tbl
                 where gid in (
                    select a.gid
                      from (
                        SELECT gid, id, nodeid, inlinkid, outlinkid, passlid, passlink_cnt, 
                               direction, guideattr, namekind, guideclass, sp_name, patternno, 
                               arrowno
                          FROM temp_signpost_tbl
                          where sp_name is null
                    ) as a
                    left join (
                        SELECT gid, id, nodeid, inlinkid, outlinkid, passlid, passlink_cnt, 
                               direction, guideattr, namekind, guideclass, sp_name, patternno, 
                               arrowno
                          FROM temp_signpost_tbl
                          where sp_name is not null
                    ) as b
                    on a.nodeid = b.nodeid and a.inlinkid = b.inlinkid
                    where b.sp_name is  null
                 );
                 """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
 
        return 0
    def _DoCheckValues(self):
        self.log.info('begin Check_signpost_Values..')
        sqlcmd = '''
                 select ad_code
                 from mid_admin_zone
                 where ad_order not in(0,1,8)
                 '''
        '''rows = self.__GetRows(sqlcmd)
        if rows:
            self.log.error('ad_order is not in(0,1,8)')
            return -1'''
        self.log.info('end Check_signpost_Values..')
        return 0
    
