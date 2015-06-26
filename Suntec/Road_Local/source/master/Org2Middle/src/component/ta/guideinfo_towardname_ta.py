# -*- coding: cp936 -*-
'''
Created on 2012-6-21

@author: hongchenzai
'''
import component.default.guideinfo_towardname

class comp_guideinfo_towardname_ta(component.default.guideinfo_towardname.comp_guideinfo_towardname):
    '''方面名称
    '''

    def __init__(self):
        '''
        Constructor
        '''
        component.default.guideinfo_towardname.comp_guideinfo_towardname.__init__(self)
    
    def _DoCreateTable(self):
        
        component.default.guideinfo_towardname.comp_guideinfo_towardname._DoCreateTable(self)
        
        return 0
        
    def _Do(self):
        # 引索
        self.CreateIndex2('sg_id_idx')
        
        # TowardName所在的link序
        # 不在调用，只要使用 SignPost做好的link序就可以了。
        self.__MakeTowardnamePathLinks()
        
        # TowardName和SignPost的方面名称，使用相同的数据（表：temp_mid_signpost_name）

        sqlcmd = """
                INSERT INTO towardname_tbl(
                    id, nodeid, inlinkid, outlinkid, passlid, passlink_cnt, 
                    direction, guideattr, namekind, name_id, "type")
                (
                    SELECT a.id, b.jnctid as node_id, in_link_id, out_link_id, passlid, passlink_cnt,
                           0 as direction, 0 as guideattr, 0 as namekind, c.signpost_name, 0 as type
                      FROM temp_towardname_path_links as a
                      left outer join sg as b
                      on a.id = b.id
                      left join temp_mid_signpost_name as c
                      on a.id = c.id
                      where signpost_name is not null
                      order by a.id, in_link_id, node_id, out_link_id, passlid
                );
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        return 0
    
    def __MakeTowardnamePathLinks(self):
        "找到TowardName对应的link序 (如：in_link_id, out_link_id, passlid, passlid_cnt)"
        # 同一个id中，
        # seqnr = 1的， 作为in_link_id
        # seqnr最大的，作为out_link_id
        # 处于以上两者之间的，作为pass link
        self.CreateTable2('temp_towardname_path_links')
        self.CreateFunction2('mid_make_towardname_path_links')
        self.pg.callproc('mid_make_towardname_path_links')
        self.pg.commit2()
        
        return 0
   
        
    
    