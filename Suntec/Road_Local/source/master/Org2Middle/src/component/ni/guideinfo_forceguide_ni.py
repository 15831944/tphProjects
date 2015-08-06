# -*- coding: UTF8 -*-
'''
Created on 2015-5-4

@author:
'''



import component.default.guideinfo_forceguide

class comp_guideinfo_forceguide_ni(component.default.guideinfo_forceguide.com_guideinfo_forceguide):
    def __init__(self):
        '''
        Constructor
        '''
        component.default.guideinfo_forceguide.com_guideinfo_forceguide.__init__(self)
        
    def _DoCreateTable(self):
        
        self.CreateTable2('force_guide_tbl')
        
        return 0
    
    def _Do(self):
        
        self._deal_temp_patch_force_guide_tbl()        
        self._make_mid_temp_force_guide_tbl()
        self._update_force_guide_tbl()
        
        return 0
    
    def _make_mid_temp_force_guide_tbl(self):
        
        self.log.info('Now it is making mid_temp_force_guide_tbl...')
        
        # 作成表单mid_temp_force_guide_tbl记录原始数据提供的force guide信息
        self.CreateIndex2('org_cond_mapid_condid_idx')
        self.CreateIndex2('org_cnl_mapid_condid_idx')
        self.CreateTable2('mid_temp_force_guide_tbl')
        
        self.CreateFunction2('ni_update_mid_temp_force_guide_tbl')
        self.pg.callproc('ni_update_mid_temp_force_guide_tbl')
        self.pg.commit2()
        
        self.log.info('making mid_temp_force_guide_tbl succeeded')
        return 0
    
    def _update_force_guide_tbl(self):
        
        self.log.info('Now it is updating force_guide_tbl...')
        
        # 更新表单force_guide_tbl
        # 表单force_guide_tbl可能已存储配置文件提供的force guide数据，因此force guide id要从当前表单最大的id开始计数
        sqlcmd = '''
            drop sequence if exists temp_force_guide_tbl_seq;
            create sequence temp_force_guide_tbl_seq;
            select setval('temp_force_guide_tbl_seq', cast(max_id as bigint))
            from
            (
                select max(force_guide_id) as max_id
                from force_guide_tbl
            )as a;
        '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2() 
        
        sqlcmd = """
                insert into force_guide_tbl (
                    force_guide_id, nodeid, inlinkid, outlinkid,
                    passlid, passlink_cnt, guide_type, position_type
                )
                select nextval('temp_force_guide_tbl_seq') as force_guide_id,
                    nodeid, inlinkid, outlinkid, passlid, passlink_cnt,
                    0 as guide_type, 0 as position_type
                from (
                    select distinct a.nodeid, a.inlinkid, a.outlinkid, a.passlid, a.passlink_cnt
                    from mid_temp_force_guide_tbl a 
                    left join force_guide_tbl b
                        on 
                            b.nodeid = a.nodeid and
                            b.inlinkid = a.inlinkid and
                            b.outlinkid = a.outlinkid and
                            not(b.passlid is distinct from a.passlid) and
                            b.passlink_cnt = a.passlink_cnt
                    where b.gid is null
                ) as c
            """
            
        self.pg.do_big_insert2(sqlcmd)
        
        self.log.info('updating force_guide_tbl succeeded') 
        return 0