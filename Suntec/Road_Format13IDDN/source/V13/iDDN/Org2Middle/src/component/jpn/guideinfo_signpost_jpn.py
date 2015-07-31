# -*- coding: cp936 -*-
'''
Created on 2012-3-27

@author: liuxinxing
'''
from base import comp_base

class comp_guideinfo_signpost_jpn(comp_base):
    def __init__(self):
        '''
        Constructor
        '''
        comp_base.__init__(self, 'GuideInfo_SignPost')
    
    def _DoCreateTable(self):
        self.CreateTable2('temp_signpost_tbl')
        self.CreateTable2('signpost_tbl')
        
    def _Do(self):
        self.log.info('Begin convert SignPost from road_dir...')
        
        self.CreateFunction2('mid_convert_signpost')
        
        self.pg.callproc('mid_convert_signpost')
        
        self.pg.commit2()
        
        # ɾ������ļ�¼(����or��)
        self._DeleteUnnecessaryRouteNumber()
        
        sqlcmd = """
                
            insert into signpost_tbl(
                id,nodeid,inlinkid,outlinkid,passlid
                ,passlink_cnt,direction,guideattr
                ,namekind,guideclass,name_id
                ,patternno,arrowno)
            (
                select id,nodeid,inlinkid,outlinkid,passlid
                    ,passlink_cnt,direction,guideattr
                    ,namekind,guideclass,name_id
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
        """ɾ������ļ�¼(����or��)
           1. ͬ�����з��棬�Ͳ�Ҫ����
           2. ͬ�����ж�����ţ�ֻ����һ���������м�¼(gid��С��)
           3. ���з����ж��Ƿ���or��
        """
        # ͬ�����з��棬�Ͳ�Ҫ����
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
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        # ͬ�����ж�����ţ�ֻ����һ���������м�¼(gid��С��)
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
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        # ���з����ж��Ƿ���or��
        sqlcmd = """
                delete from temp_signpost_tbl
                 where gid in (
                    select a.gid
                      from (
                        SELECT gid, id, nodeid, inlinkid, outlinkid, passlid, passlink_cnt, 
                               direction, guideattr, namekind, guideclass, name_id, patternno, 
                               arrowno
                          FROM temp_signpost_tbl
                          where name_id is null
                    ) as a
                    left join (
                        SELECT gid, id, nodeid, inlinkid, outlinkid, passlid, passlink_cnt, 
                               direction, guideattr, namekind, guideclass, name_id, patternno, 
                               arrowno
                          FROM temp_signpost_tbl
                          where name_id is not null
                    ) as b
                    on a.nodeid = b.nodeid and a.inlinkid = b.inlinkid
                    where b.name_id is  null
                 );
                 """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
 
        return 0
    