# -*- coding: cp936 -*-
'''
Created on 2012-11-13

@author: zhangliang
'''
import component.component_base

class comp_vics_jpn(component.component_base.comp_base):
    '''
    vics tmc middle table
    '''

    def __init__(self):
        '''
        Constructors
        '''
        component.component_base.comp_base.__init__(self, 'vics tmc info')
        
    def _DoCreateTable(self):
        # 道路名称与字典关系表
        self.CreateTable2('vics_ipc_link_tbl')
        
        self.CreateTable2('vics_link_tbl')
        
    def _DoCreateIndex(self):
        self.CreateIndex2('vics_link_tbl_linkid_dir_idx')  
        self.CreateIndex2('vics_ipc_link_tbl_linkid_dir_idx')  
        return 0 
       
    def _DoCreateFunction(self):
        return 0
    
    def _Do(self):
        sqlcmd = """
            insert into vics_link_tbl(vicsid ,link_id , dir)
            (
                select A.vicsid, B.tile_link_id, A.linkdir from road_vic as A
                left join middle_tile_link as B
                on A.linkid = B.old_link_id
            )
        """
        self.log.info(self.ItemName + ': Now it is inserting vics_link_tbl...')
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()
            
        self.log.info(self.ItemName + ': end of inserting vics_link_tbl...')
        
        sqlcmd = """
            insert into vics_ipc_link_tbl(vicsid_ipc ,link_id, dir)
            (
                select vicslink_id, C.tile_link_id, dir from ipc_vics as A
                left join road_rlk as B
                on A.startnode_id = B.fromnodeid and A.endnode_id = B.tonodeid
                left join middle_tile_link as C
                on B.objectid = C.old_link_id
            )
        """
        self.log.info(self.ItemName + ': Now it is inserting vics_ipc_link_tbl...')
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()
            
        self.log.info(self.ItemName + ': end of inserting vics_ipc_link_tbl...')
        
        return 0
    
