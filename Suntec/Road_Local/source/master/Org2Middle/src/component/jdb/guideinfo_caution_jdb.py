# -*- coding: UTF8 -*-
'''
Created on 2012-2-23

@author: sunyifeng
'''

import io
import os

import component.component_base
import common.common_func
from common import cache_file
from component.default.multi_lang_name import MultiLangName
from component.default.multi_lang_name import NAME_TYPE_OFFICIAL
from component.default.multi_lang_name import NAME_TYPE_SHIELD
from component.default.multi_lang_name import NAME_TYPE_ALTER


class comp_guideinfo_caution_jdb(component.component_base.comp_base):

    def __init__(self):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self, 'Guideinfo Caution')

    def _DoCreateTable(self):

        self.CreateTable2('caution_tbl')
        self.CreateTable2('temp_boundary')
        sqlcmd = '''
            Alter table temp_boundary drop CONSTRAINT enforce_geotype_the_geom;
        '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        self.CreateTable2('temp_hlink')
        self.CreateTable2('temp_inode')
        sqlcmd = '''
            Alter table temp_inode drop CONSTRAINT enforce_geotype_the_geom;
        '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        self.CreateTable2('temp_bnode')
        self.CreateTable2('temp_guideinfo_boundary')
        self.CreateTable2('temp_admin_wavid')
        self.CreateTable2('temp_admin_wavid_strtts')
        self.CreateTable2('temp_admin_wavid_strtts_json')
        return 0

    def _DoCreateFunction(self):

        self.CreateFunction2('mid_get_inout_link')
        return 0

    def _Do(self):
        self.log.info('making guideinfo caution info ...')
        self.log.info('making guideinfo caution boundary info ...')
        self._DoBoundary()
        self.log.info('making guideinfo caution info OK.')
        self.log.info('making guideinfo caution boundary info OK.')
        self._check_passlink()
        
        # add safety caution guidance
        self.log.info('making guideinfo safety caution info ...')
        self._do_safety_caution()
        self.log.info('making guideinfo safety caution info OK.')

    def _DoBoundary(self):
        pg = self.pg
        pgcur = self.pg.pgcur2
        admin_wav_path = common.common_func.GetPath('admin_wav')
        f = io.open(admin_wav_path, 'r', 8192, 'utf8')
        pgcur.copy_from(f, 'temp_admin_wavid', ',', "", 8192, None)        
        pg.commit2()
        f.close()
        #tts text
        admin_wav_strTTS_path = admin_wav_path[0:admin_wav_path.rfind('\\')+1] + 'admin_wav_strTTS.txt'
        f = io.open(admin_wav_strTTS_path, 'r', 8192, 'utf8')
        pgcur.copy_from(f, 'temp_admin_wavid_strtts', ',', "", 8192, None)        
        pg.commit2()
        f.close()
        #tts json table
        sqlcmd = '''
                select wav_id,strTTS
                from temp_admin_wavid_strtts;
                '''
        temp_file_obj = cache_file.open('temp_admin_wavid_strTTS_name_new')
        rows = self.get_batch_data(sqlcmd)
        for row in rows:
            json_name = self._name_to_json(row[1])
            if not json_name:
                self.log.error('Json Name is null. id=%d' % id)
            else:
                self.__store_name_to_temp_file(temp_file_obj, row[0], json_name)
                
        temp_file_obj.seek(0)
        self.pg.copy_from2(temp_file_obj, 'temp_admin_wavid_strtts_json')
        self.pg.commit2()
        cache_file.close(temp_file_obj,True)
        
        sqlcmd = """
            INSERT into temp_boundary(ad_cd, the_geom)
                SELECT ad_cd, ST_Boundary(the_geom) as the_geom
                  FROM
                  (
                      select ad_code as ad_cd,the_geom
                      from mid_admin_zone
                      where ad_order = 1
                  ) as a;
        """

        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        sqlcmd = """
            INSERT INTO
                temp_hlink(link_id, s_node, e_node, one_way_code, the_geom)
                SELECT link_id, s_node, e_node, one_way_code, the_geom
                  FROM link_tbl
                  where toll = 1;
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        self.CreateIndex2('temp_boundary_the_geom_idx')
        self.CreateIndex2('temp_hlink_the_geom_idx')
        sqlcmd = """
            INSERT INTO
                temp_inode(link_id, s_node, e_node, one_way_code, the_geom)
                SELECT distinct a.link_id, a.s_node, a.e_node, a.one_way_code,
                        ST_Intersection(a.the_geom, b.the_geom) as the_geom
                    FROM temp_hlink as a
                    inner join temp_boundary as b
                    on ST_Intersects(a.the_geom, b.the_geom) = TRUE
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        sqlcmd = """
            INSERT INTO temp_bnode(
                            link_id, s_node, e_node, one_way_code, bnode,
                            link_id2, s_node2, e_node2, one_way_code2, the_geom
                            )
                SELECT t1.link_id, t1.s_node, t1.e_node,
                        t1.one_way_code, t1.bnode,
                        t2.link_id, t2.s_node, t2.e_node,
                        t2.one_way_code, t1.the_geom
                    FROM
                    (
                        SELECT link_id, s_node, e_node, one_way_code,
                            case when slocate < 0.5 and slocate < 1 - elocate then s_node else e_node end as bnode,
                            case when slocate < 0.5 and slocate < 1 - elocate then sp else ep end as the_geom,
                            slocate,
                            elocate
                        FROM
                        (
                        SELECT a.link_id, a.s_node, a.e_node, a.one_way_code,
                            ST_Line_Locate_Point(the_geom, sbp) as slocate,
                            ST_Line_Locate_Point(the_geom, ebp) as elocate,
                            sp,
                            ep
                            FROM
                              (
                              select m.link_id, m.s_node, m.e_node, m.one_way_code, n.the_geom, 
                                    ST_ClosestPoint(m.the_geom, ST_StartPoint(n.the_geom)) as sbp,
                                    ST_ClosestPoint(m.the_geom, ST_EndPoint(n.the_geom)) as ebp,
                                    ST_StartPoint(n.the_geom) as sp,
                                    ST_EndPoint(n.the_geom) as ep
                                from temp_inode as m
                                inner join link_tbl as n
                                on m.link_id = n.link_id
                              ) as a
                        ) as t
                    ) as t1
                    inner join link_tbl as t2
                    on t1.bnode = t2.s_node or t1.bnode = t2.e_node
                    where t1.link_id != t2.link_id;
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        sqlcmd = """
            INSERT INTO temp_guideinfo_boundary(
                inlinkid, nodeid, outlinkid, innode, outnode, out_adcd
                )
                SELECT distinct inlinkid, bnode, outlinkid, innode,
                    outnode, b.ad_cd
                from
                (
                    SELECT links[1] as inlinkid, bnode, links[2] as outlinkid, 
                        links[3] as innode, links[4] as outnode
                    from
                    (
                    SELECT
                        bnode,
                        mid_get_inout_link(
                                link_id, s_node, e_node, one_way_code,
                                link_id2, s_node2, e_node2, one_way_code2
                                ) as links
                        from temp_bnode
                    ) as t
                    where links is not null
                ) as a
                inner join node_tbl as c
                on a.outnode = c.node_id
                inner join (
                    select ad_code as ad_cd,the_geom
                    from mid_admin_zone
                    where ad_order = 1
                    )as b
                on ST_Intersects(c.the_geom, b.the_geom) = TRUE;
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        sqlcmd = """
            INSERT INTO caution_tbl(
                inlinkid, nodeid, outlinkid, passlid,
                passlink_cnt, data_kind, voice_id, strTTS, image_id
            )
                select inlinkid, nodeid, outlinkid, null, 0, 4, b.wav_id,c.strTTS_json,0
                    from temp_guideinfo_boundary as a
                    left join temp_admin_wavid as b
                    on a.out_adcd = b.ad_cd
                    left join temp_admin_wavid_strtts_json as c
                    on b.wav_id = c.wav_id
                    order by inlinkid, nodeid, outlinkid;
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        return 0
    
    def __store_name_to_temp_file(self, file_obj, strid, json_name):
        if file_obj:
            file_obj.write('%s\t%s\n' % (strid, json_name))
        return 0
    
    def _name_to_json(self,strTTS):
        
        code_dict = dict()
        code_dict['JPN'] = None
        MultiLangName.set_language_code(code_dict)
        ml_name = MultiLangName('JPN', strTTS, NAME_TYPE_OFFICIAL)
        strTTS_json = ml_name.json_format_dump()
        
        return strTTS_json

    def _check_passlink(self):
        self.CreateFunction2('mid_check_guide_item_new')
        check_sql = '''
            select mid_check_guide_item_new({0});
        '''
#         try:
        self.pg.execute2(check_sql.format("\'caution_tbl\'"))
        row = self.pg.fetchone2()
        if row[0]:
            self.log.warning('passlink is not continuous,num=%s!!!', row[0])
            pass
#         except Exception:
#             self.log.error('passlink is not continuous!!!')
#             self.pg.commit2()
        return
    
    def _do_safety_caution(self):
        #self.CreateFunction2('mid_findpasslinkbybothnodes_caution')
		self.CreateFunction2('mid_findpasslinkbybothnodes')
        
        # step one: get all inlinks which flow to the node 
        sqlcmd = '''
        drop table if exists temp_caution_inlink_with_innode;
        
        create table temp_caution_inlink_with_innode
        as
        (
            with temp_node as
            (
                select in_node_id, b.tile_node_id 
                from
                (
                    select distinct in_node_id
                    from road_guidance_caution_tbl
                ) as a
                left join middle_tile_node as b
                on a.in_node_id = b.old_node_id
            )
            select link_id, in_node_id, link_type
            from
            (
                select link_id, s_node as in_node_id, link_type
                from link_tbl as a
                where s_node in 
                (
                    select tile_node_id
                    from temp_node
                ) and one_way_code in (1,3)
                union
                select link_id, e_node as in_node_id, link_type
                from link_tbl as a
                where e_node in 
                (
                select tile_node_id
                from temp_node
                ) and one_way_code in (1,2)
            ) as t
        );
        '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        # step two: get all pathlinks from in_node to out_node
        sqlcmd = '''
        drop table if exists temp_caution_full_path;
        
        create table temp_caution_full_path
        as
        select 
			in_node_id, out_node_id, 
			--mid_findpasslinkbybothnodes_caution(in_node_id, out_node_id, 30) as all_path_links, 
			mid_findpasslinkbybothnodes(in_node_id, out_node_id, 30) as all_path_links, 
			safety_type
        from
        (
            select b.tile_node_id as in_node_id, c.tile_node_id as out_node_id, 
                safety_type
            from road_guidance_caution_tbl as a
            left join middle_tile_node as b
            on a.in_node_id = b.old_node_id
            left join middle_tile_node as c
            on a.out_node_id = c.old_node_id
        ) as m;
        '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        # step three: insert into caution to middle table
        # please pay attention that if you want to adjust the text of tts contents,
        # you should change the mid_get_caution_info function
        #self.CreateFunction2('mid_get_caution_info')
        
        sqlcmd = '''
                create or replace function mid_get_caution_info(safety_type varchar)
                  RETURNS varchar[]
                LANGUAGE plpgsql volatile
                AS $$
                DECLARE
                BEGIN
                    return case safety_type when '1' then array['81','この先カーブが続きます。ご注意ください']
                         when '4' then array['82','ここからおよそ1km先までは、充分運転に注意してください']
                         when '5' then array['83','ここからおよそ2km先までは、充分運転に注意してください']
                         when '6' then array['84','ここからおよそ3km先までは、充分運転に注意してください']
                         when '7' then array['85','ここからおよそ4km先までは、充分運転に注意してください']
                         when '8' then array['86','ここからおよそ5km先までは、充分運転に注意してください']
                         when '9' then array['87','ここからおよそ6km先までは、充分運転に注意してください']
                         when 'A' then array['88','ここからおよそ7km先までは、充分運転に注意してください']
                         when 'B' then array['89','ここからおよそ8km先までは、充分運転に注意してください']
                         when 'C' then array['90','ここからおよそ9km先までは、充分運転に注意してください']
                         when 'D' then array['91','ここからおよそ10km先までは、充分運転に注意してください']
                        END;    
                END;
                $$;
                '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        
        sqlcmd = """
            select link_id as in_link_id,in_node_id as node_id, out_link_id ,
                null, 0,data_kind,voice_id,strTTS,image_id
                from
                (
                    select link_id, in_node_id, link_type, all_path_links,(regexp_split_to_array(all_path_links,E'\\\,+'))[1]::bigint as out_link_id,
                        case when safety_type = '1' then 1 
                        when safety_type <> '1' and safety_type <> '2' and safety_type <> '3' then 2 end as data_kind,
                        voice_tts[1]::int as voice_id,
                        voice_tts[2] as strTTS,
                        0 as image_id
                    from
                    (
                        select a.link_id, a.in_node_id, a.link_type,
                         b.all_path_links, b.safety_type, mid_get_caution_info(b.safety_type) as voice_tts
                        from temp_caution_inlink_with_innode as a
                        left join temp_caution_full_path as b
                        on a.in_node_id = b.in_node_id
                        where all_path_links is not null
                    ) as m
                ) as n
                where link_id <> out_link_id
        """
        self.pg.execute2(sqlcmd)
        rows = self.pg.fetchall2()
        for row in rows:
            in_link_id = row[0]
            node_id = row[1]
            out_link_id = row[2]
            passlids = None
            passcnt = 0
            data_kind = row[5]
            voice_id = row[6]
            tts_contents_to_jason = self._name_to_json(row[7])
            image_id = row[8]
            
            insert_sql = """
                INSERT INTO caution_tbl(
                    inlinkid, nodeid, outlinkid, passlid,
                    passlink_cnt, data_kind, voice_id,strTTS ,image_id
                )
                values
				(
                    %s, %s, %s, %s, %s, %s, %s, %s, %s
                );

            """
            
            self.pg.execute2(insert_sql, (in_link_id, node_id, out_link_id,
                                          passlids, passcnt, data_kind,
                                          voice_id, tts_contents_to_jason, image_id))
        
        self.pg.commit2()
        return 0
