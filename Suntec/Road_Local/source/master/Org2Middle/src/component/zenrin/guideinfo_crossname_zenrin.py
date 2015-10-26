# -*- coding: UTF8 -*-
#!/usr/bin/python
'''
Created on 2015-6-30

@author: wushengbing
'''
import component.component_base
import common.cache_file
import component.default.multi_lang_name


class comp_guideinfo_crossname_zenrin(component.component_base.comp_base):

    def __init__(self):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self, 'CrossName')
        
        
    def _Do(self):
        self.log.info("start make crossname_tbl ")
        self._make_temp_crossname()
        self._make_intersection_name()
        self._make_crossname()
        self.log.info("end make crossname_tbl ")
    
    
    
    
    def _make_temp_crossname(self):
        self.log.info("make temp_crossname...")
        self.CreateFunction2('zenrin_join')
        self.CreateFunction2('zenrin_find_tnode')
        
        sqlcmd = '''
            drop table if exists temp_crossname;
            create table temp_crossname
            as
            (
                select  b.id,
                        tnd.node_id as nodeid,
                        tlk.link_id as inlink,
                        zenrin_join(name_array) as name,
                        zenrin_join(reading_array) as reading
                from 
                (
                    SELECT  row_number() over(order by meshcode,tonodeno, fromnodeno ) as id,
                            meshcode, 
                            --(zenrin_find_tnode(a.meshcode,a.fromnodeno,a.tonodeno))[1] as fromnodeno, 
                            --(zenrin_find_tnode(a.meshcode,a.fromnodeno,a.tonodeno))[2] as tonodeno,
                            fromnodeno,
                            tonodeno, 
                            array_agg(nextflg) as flg_array,
                            array_agg(name) as name_array, 
                            array_agg(reading_pym) as reading_array
                    FROM 
                    (
                        select * from org_cross_name
                        order by gid, meshcode, crossnmno, tonodeno, fromnodeno,nextflg 
                    ) as a
                    group by meshcode,tonodeno, fromnodeno
                
                ) as b
    
                left join org_road as r
                on  ( r.snodeno = b.tonodeno and r.enodeno = b.fromnodeno
                      or 
                      r.enodeno = b.tonodeno and r.snodeno = b.fromnodeno
                    )
                    and r.meshcode = b.meshcode
                 
                left join temp_link_mapping as tlk
                on tlk.meshcode = b.meshcode and tlk.linkno = r.linkno
            
                left join temp_node_mapping as tnd
                on tnd.meshcode = b.meshcode and tnd.nodeno = b.tonodeno   
            
            );
            
            '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        self.CreateIndex2('temp_crossname_id_idx')
        
  
        
    def _make_intersection_name(self):
        
        self.log.info("make temp_intersection_name...")
        self.CreateTable2('temp_intersection_name')
        
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
                from temp_crossname as a  
             ) as b
            group by id
            order by id;
        '''
        recs = self.pg.get_batch_data2(sqlcmd)
        
        temp_file_obj = common.cache_file.open('intersection_name')
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
        self.pg.copy_from2(temp_file_obj, 'temp_intersection_name')
        self.pg.commit2()
        common.cache_file.close(temp_file_obj,True)
        self.CreateIndex2('temp_intersection_name_id_idx')                 
        
        
        
    def _make_crossname(self):
        self.log.info("make crossname...")
        self.CreateTable2('crossname_tbl')
        
        sqlcmd = '''
            insert into crossname_tbl(inlinkid, nodeid, outlinkid, passlid, passlink_cnt, namestr)
            select  a.inlink, 
                    a.nodeid,
                    null as outlinkid,
                    '' as passlid,
                    0 as passlink_cnt,
                    b.intersection_name as namestr            
            from temp_crossname as a
            left join temp_intersection_name as b
            on a.id = b.id
        
            '''
        
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        