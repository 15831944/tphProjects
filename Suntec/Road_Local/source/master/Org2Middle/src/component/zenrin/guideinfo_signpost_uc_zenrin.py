# -*- coding: UTF8 -*-
#!/usr/bin/python
'''
Created on 2015-6-29

@author: wushengbing
'''
from component.component_base import comp_base
import common.cache_file
import component.default.multi_lang_name


class comp_guideinfo_signpost_uc_zenrin(comp_base):
    
    def __init__(self):
        comp_base.__init__(self, 'guideinfo_signpost_uc')

    
    def _Do(self):
        self.log.info("start make signpost_tbl ")
        self.__make_temp_signpost()
        self.__make_sp_name()
        self.__update_temp_signpost_after_delete_dummy_link()
        self.__make_signpost_uc()
        self.log.info("end make signpost_tbl ")
        return 0
  
    
    def __make_temp_signpost(self):
        
        self.CreateFunction2('zenrin_join')
        self.log.info("make temp_signpost... ")
        
        sqlcmd = '''
        drop table if exists temp_signpost;
        create table temp_signpost
        as
        (
            select  b.id, tlk1.link_id as inlink, tlk2.link_id as outlink, tnd1.node_id as tnode,
                    tnd2.node_id as enode,
                    zenrin_join(name_array) as name,
                    zenrin_join(reading_array) as reading
            from 
            (
                 SELECT  row_number() over(order by meshcode, stotlinkno, ttoelinkno, tnodeno,enodeno ) as id,
                         meshcode, stotlinkno, ttoelinkno, tnodeno, enodeno,
                         array_agg(name) as name_array, 
                         array_agg(reading_pym) as reading_array
                 FROM 
                 (
                    select * from org_dest_guide
                    order by gid, meshcode, destnmno, stotlinkno, ttoelinkno, snodeno, tnodeno,enodeno, nextflg
                  ) as a
                group by meshcode, stotlinkno, ttoelinkno, tnodeno,enodeno
                
            ) as b
            
            left join temp_link_mapping as tlk1
            on tlk1.meshcode = b.meshcode and tlk1.linkno = b.stotlinkno
            left join temp_link_mapping as tlk2
            on tlk2.meshcode = b.meshcode and tlk2.linkno = b.ttoelinkno
            left join temp_node_mapping as tnd1
            on tnd1.meshcode = b.meshcode and tnd1.nodeno = b.tnodeno
            left join temp_node_mapping as tnd2
            on tnd2.meshcode = b.meshcode and tnd2.nodeno = b.enodeno
        
       ); 
        '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        self.CreateIndex2('temp_signpost_id_idx')
        self.CreateIndex2('temp_signpost_inlink_idx')
        
        
    def __make_sp_name(self):
        self.log.info("make temp_sp_name... ")
        self.CreateTable2('temp_sp_name')
        
        if not component.default.multi_lang_name.MultiLangName.is_initialized():
            component.default.multi_lang_name.MultiLangName.initialize()
        
        sqlcmd = '''
            select  id,
                    array_agg(name_id) as name_id_array,
                    array_agg(name_type) as name_type_array,
                    array_agg(language) as language_code_array,
                    array_agg(name) as name_array,
                    array_agg(phoneme_lang) as phonetic_language_array,
                    array_agg(phoneme) as phoneme_array
            from
            (
                select  a.id,
                        a.name as name,
                        'CHT'::varchar as language,
                        1 as name_id,
                        'office_name'::varchar as name_type,
                        'BPM'::varchar as phoneme_lang,
                        a.reading as phoneme
                from temp_signpost as a  
             ) as b
            group by id
            order by id;
        '''
        recs = self.pg.get_batch_data2(sqlcmd)
        
        temp_file_obj = common.cache_file.open('sp_name')
        for rec in recs:
            gid = rec[0]
            json_name = component.default.multi_lang_name.MultiLangName.name_array_2_json_string_multi_phon(rec[1], 
                                                                                                            rec[2], 
                                                                                                            rec[3], 
                                                                                                            rec[4],
                                                                                                            rec[5], 
                                                                                                            rec[6])
            temp_file_obj.write('%d\t%s\n' % (int(gid), json_name))
        
        temp_file_obj.seek(0)
        self.pg.copy_from2(temp_file_obj, 'temp_sp_name')
        self.pg.commit2()
        common.cache_file.close(temp_file_obj,True)
        self.CreateIndex2('temp_sp_name_id_idx') 
    
    
    def __update_temp_signpost_after_delete_dummy_link(self):
        self.log.info("update temp_signpost  after deal with dummy link and inner link... ")
        self.CreateFunction2('zenrin_update_inlink_to_no_dummy_link')
        self.CreateTable2('temp_signpost_uc_deal_with_dummy_link')
        sqlcmd = '''
            drop table if exists temp_signpost_uc_path_after_deal_with_dummy_link;
            create table temp_signpost_uc_path_after_deal_with_dummy_link
            as
            (
                select a.id, a.inlink, a.outlink, a.tnode
                from temp_signpost as a
                left join link_tbl as b
                on a.inlink = b.link_id
                where a.inlink not in (select link_id from temp_dummy_todelete) and  b.link_type <> 4
                
                union
                
                select c.id ,unnest(zenrin_update_inlink_to_no_dummy_link(c.tnode) ) as inlink, c.outlink, c.tnode
                from
                (
                    select a.*
                    from temp_signpost as a
                    left join link_tbl as b
                    on a.inlink = b.link_id
                    where a.inlink in (select link_id from temp_dummy_todelete) or b.link_type = 4
                ) as c
            )
            
        ''' 
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        self.CreateIndex2('temp_signpost_uc_path_after_deal_with_dummy_link_id_idx') 
        
        sqlcmd = '''
            insert into temp_signpost_uc_deal_with_dummy_link(nodeid, inlinkid, 
                                                            outlinkid, passlid, passlink_cnt, sp_name)
            select distinct a.tnode as nodeid, a.inlink as inlinkid, a.outlink as outlinkid,
                    '' as passlid, 0 as passlink_cnt, b.sp_name as sp_name
            from temp_signpost_uc_path_after_deal_with_dummy_link as a
            left join temp_sp_name as b
            on b.id = a.id 
        ''' 
        self.pg.execute2(sqlcmd)
        self.pg.commit2()          
    
    def __make_signpost_uc(self):
        self.log.info("make signpost_uc... ")
        #self.CreateFunction2('mid_findpasslinkbybothnodes')
        #self.CreateFunction2('zenrin_findpasslink_count')
        self.CreateTable2('signpost_uc_tbl')
        
        sqlcmd = '''
            insert into signpost_uc_tbl(id, nodeid, inlinkid, outlinkid, passlid, passlink_cnt, sp_name )
            select a.gid,
                   a.nodeid,
                   a.inlinkid,
                   a.outlinkid,
                   a.passlid,
                   a.passlink_cnt,
                   a.sp_name
            from temp_signpost_uc_deal_with_dummy_link as a
                     
          '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        self.CreateIndex2('signpost_uc_tbl_node_id_idx')
        self.CreateIndex2('signpost_uc_tbl_inlinkid_nodeid_outlinkid_idx')

            
        
        
        
        


