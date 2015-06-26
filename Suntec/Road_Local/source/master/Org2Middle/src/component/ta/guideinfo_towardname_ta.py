# -*- coding: cp936 -*-
'''
Created on 2012-6-21

@author: hongchenzai
'''
import component.default.guideinfo_towardname

class comp_guideinfo_towardname_ta(component.default.guideinfo_towardname.comp_guideinfo_towardname):
    '''��������
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
        # ����
        self.CreateIndex2('sg_id_idx')
        
        # TowardName���ڵ�link��
        # ���ڵ��ã�ֻҪʹ�� SignPost���õ�link��Ϳ����ˡ�
        self.__MakeTowardnamePathLinks()
        
        # TowardName��SignPost�ķ������ƣ�ʹ����ͬ�����ݣ���temp_mid_signpost_name��

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
        "�ҵ�TowardName��Ӧ��link�� (�磺in_link_id, out_link_id, passlid, passlid_cnt)"
        # ͬһ��id�У�
        # seqnr = 1�ģ� ��Ϊin_link_id
        # seqnr���ģ���Ϊout_link_id
        # ������������֮��ģ���Ϊpass link
        self.CreateTable2('temp_towardname_path_links')
        self.CreateFunction2('mid_make_towardname_path_links')
        self.pg.callproc('mid_make_towardname_path_links')
        self.pg.commit2()
        
        return 0
   
        
    
    