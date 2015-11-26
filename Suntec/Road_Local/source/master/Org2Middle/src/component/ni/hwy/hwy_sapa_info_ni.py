# -*- coding: UTF8 -*-
'''
Created on 2015-5-26

@author: hcz
'''
from component.ni.hwy.hwy_exit_enter_poi_name_ni import HwyExitEnterNameNi
from common import cache_file
SAPA_TYPE_DICT = {'8380': 'SA', '8381': 'PA'}


class HwySaPaInfoNi(HwyExitEnterNameNi):
    '''
    SAPA情报
    '''

    def __init__(self, item_name='HwySaPaInfoNi'):
        '''
        Constructor
        '''
        HwyExitEnterNameNi.__init__(self, item_name)

    def _DoCreateTable(self):
        self.CreateTable2('mid_temp_poi_link')
        self.CreateTable2('mid_temp_hwy_sapa_name')
        self.CreateTable2('mid_temp_sapa_store_info')
        self.CreateTable2('mid_temp_poi_closest_link')
        return 0

    def _DoCreateIndex(self):
        return 0

    def _Do(self):
        # POI关联link
        # 11年
#         self._make_poi_link_11()
#         self._make_hwy_sapa_store_info_11()
#         self._make_hwy_sapa_name_11()
#         self._make_hwy_store_name_11()
        # 14年
        self._make_poi_link()
        self._make_ni_temp_poi_category()
        # POI最近的link(由于切割，Entry link可能有多条)
        self._make_hwy_poi_closest_link()
        self._make_hwy_sapa_store_info()
        self._make_hwy_sapa_name()
        self._make_hwy_store_name()
        return 0

    def _make_ni_temp_poi_category(self):
        self.log.info('start make ni_temp_poi_category')
        self.pg.CreateTable2_ByName('ni_temp_poi_category')
        sqlcmd = '''
        INSERT INTO ni_temp_poi_category(per_code, is_brand, gen1, gen2,
                                         gen3, level, name, genre_type,
                                         org_code1, org_code2, chaincode)
        (
        select a.per_code, a.genre_is_brand, a.gen1, a.gen2, a.gen3, a.level,
                b.name, a.genre_type, a.org_code1, a.org_code2, a.chaincode
        from (
            select distinct per_code, level, genre_type, gen1, gen2, gen3,
                   genre_is_brand, org_code1, org_code2, chaincode
            from temp_poi_category_code
            order by level, genre_is_brand, gen1, gen2, gen3
             )as a
        left join
             (
            select distinct per_code, name
            from temp_poi_category_name
            order by per_code, name
             )as b
        on a.per_code = b.per_code
        )
        '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        self.log.info('end make ni_temp_poi_category')
        return 0

    def _make_poi_link(self):
        '''SAPA Poi所在linkid
        '''
        self.log.info('Start Make SAPA link.')
        # self.CreateIndex2('org_poi_linkid_idx')
        self.CreateIndex2('org_poi_linkid_idx2')
        self.CreateIndex2('org_poi_poi_id_idx2')
        self.CreateIndex2('org_pname_featid_nametype_idx1')
        # 8380: 高速服务区, 8381: 高速停车区
        # 4101: 室内停车场, 4102: 室外停车场
        sqlcmd = '''
        INSERT INTO mid_temp_poi_link(poi_id, link_id)
        (
        SELECT distinct poi_id::bigint, m.link_id
          FROM org_poi AS p
          LEFT JOIN mid_link_mapping as m
          ON p.linkid::bigint = m.org_link_id
          left join link_tbl
          on m.link_id = link_tbl.link_id
          --where p.kind IN ('8380', '8381')
          where road_type = 0
        );
        '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        self.CreateIndex2('mid_temp_poi_link_poi_id_idx')
        self.CreateIndex2('mid_temp_poi_link_link_id_idx')
        return 0

    def _make_hwy_poi_closest_link(self):
        self.log.info('Make Poi closest link.')
        sqlcmd = """
        INSERT INTO mid_temp_poi_closest_link(
                                             poi_id, link_id, dist,
                                             s_length, e_length)
        (
        SELECT poi_id, link_id, dist,
               length *  point as s_length,
               length * (1 - point) as e_length
          FROM (
              SELECT poi_id,
                     (array_agg(link_id))[1] as link_id,  -- closed link
                     (array_agg(dist))[1] as dist,
                     (array_agg(length))[1] as length,
                     (array_agg(point))[1] as point
              FROM (
                    SELECT a.poi_id, a.link_id,
                           ST_Distance(b.the_geom, link_tbl.the_geom) as dist,
                           length,
                           ST_Line_Locate_Point(link_tbl.the_geom,
                                                b.the_geom) as point
                      FROM mid_temp_poi_link as a
                      LEFT JOIN org_poi as b
                      ON a.poi_id = b.poi_id::bigint
                      INNER JOIN link_tbl
                      ON a.link_id = link_tbl.link_id and road_type = 0
                      ORDER BY a.poi_id, dist, a.link_id
              ) AS d
              GROUP BY poi_id
          ) as c
        );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

    def _make_hwy_sapa_name(self):
        '''SAPA名称'''
        self.log.info('start make exit sapa name...')
        temp_file_obj = cache_file.open('sapa_name')
        for nameinfo in self._get_sapa_name():
            poi_id = nameinfo[0]
            poi_names = nameinfo[1]
            lang_codes = nameinfo[2]
            if not poi_id:
                self.log.error('No poi_id')
                continue
            # kind = SAPA_TYPE_DICT.get(poi_kind)
            json_name = self._get_json_name(lang_codes, poi_names)
            if not json_name:
                self.log.error('No json name. poi_id is %s' % poi_id)
            self._store_name_to_temp_file(temp_file_obj, poi_id, json_name)
        temp_file_obj.seek(0)
        self.pg.copy_from2(temp_file_obj,
                           'mid_temp_hwy_sapa_name')
        self.pg.commit2()
        cache_file.close(temp_file_obj, True)
        self.log.info('end make exit sapa name...')
        return 0

    def _get_sapa_name(self):
        sqlcmd = '''
        SELECT poi_id,
               array_to_string(array_agg(name),'|') as names,
               array_to_string(array_agg(language),'|')as lang_codes
          FROM (
                SELECT distinct a.poi_id, name, language
                FROM org_poi as a
                INNER JOIN mid_temp_poi_closest_link AS b
                ON a.poi_id::bigint = b.poi_id
                LEFT JOIN org_pname
                ON a.poi_id::bigint = featid::bigint and nametype = '9'  -- 9: POI
                -- 8380: SA, 8381: PA
                WHERE a.kind IN ('8380', '8381', '4101', '4102')
                ORDER BY poi_id, language
          ) AS A
          GROUP BY poi_id;;
        '''
        return self.get_batch_data(sqlcmd)

    def _check_temp_poi_category(self):
        '''check temp_poi_category'''
        sqlcmd = '''
        select count(*)
        from (
            select distinct chaincode, org_code1, array_agg(per_code)
            from ni_temp_poi_category
            where chaincode <>'' and chaincode is not null
            group by chaincode, org_code1
            having count(per_code) > 1
        )as a
        '''
        self.pg.execute2(sqlcmd)
        row = self.pg.fetchone2()
        if row and row[0] == 0:
            return True
        return False

    def _make_hwy_store_name(self):
        '''Store or Chain Name'''
        self.CreateTable2('hwy_chain_name')
        if not self._check_temp_poi_category():
            self.log.error('table temp_poi_category error')
        sqlcmd = """
        INSERT INTO hwy_chain_name(u_code, cat_id, sub_cat,
                                   chain_id, chain_name, language_code)
        (
        SELECT per_code as u_code, a.kind, '' as subcat,
               a.chaincode, b.name as chain_name, 'CHI' as language_code
          FROM org_poi as a
          LEFT JOIN ni_temp_poi_category as b
          ON a.chaincode = b.chaincode and a.kind = b.org_code1
          where a.chaincode <> '' and a.chaincode is not null and
                per_code is not null

        union
         SELECT distinct per_code as u_code, a.kind, '' as subcat,
               a.chaincode, b.name as chain_name, 'CHI' as language_code
          FROM org_poi as a
          LEFT JOIN ni_temp_poi_category as b
          ON a.chaincode = b.chaincode and a.kind = b.org_code2
          where a.chaincode <> '' and a.chaincode is not null and
                per_code is not null and is_brand = 'Y'
        );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

    def _store_name_to_temp_file(self, file_obj, poi_id, json_name):
        ''' '''
        if file_obj:
            file_obj.write('%s\t%s\n' % (poi_id, json_name))
        return 0

    def _make_hwy_sapa_store_info(self):
        '''SAPA的店铺'''
        self.log.info('Start Make sapa store info.')
        self.CreateIndex2('org_poi_relation_poi_id1_idx')
        self.CreateIndex2('org_poi_relation_poi_id2_idx')
        sqlcmd = '''
        INSERT INTO mid_temp_sapa_store_info(poi_id, child_poi_id,
                                             chain_id, cat_id)
        (
        SELECT a.poi_id::bigint, c.poi_id::bigint as child_poi_id,
               c.chaincode as chain_id, c.kind as cat_id
          FROM org_poi as a
          INNER JOIN org_poi_relation as b
          ON a.poi_id = b.poi_id1
          LEFT JOIN org_poi as c
          ON b.poi_id2 = c.poi_id
          INNER JOIN mid_temp_poi_link AS d
          ON a.poi_id::bigint = d.poi_id
          where a.kind IN ('8380', '8381', '4101', '4102')
          ORDER BY a.poi_id
        );
        '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        return 0

    def _make_poi_link_11(self):
        '''SAPA POI所在linkid(11年数据)
        '''
        self.log.info('Start Make SAPA link.')
        sqlcmd = """
        -----------------------------------------------------
        DROP INDEX if exists org_p_linkid_idx;
        CREATE INDEX org_p_linkid_idx
          ON org_p
          USING btree
          (linkid);
        -----------------------------------------------------
        DROP INDEX if exists org_p_linkid_idx2;
        CREATE INDEX org_p_linkid_idx2
          ON org_p
          USING btree
          ((linkid::bigint));
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        sqlcmd = '''
        INSERT INTO mid_temp_poi_link(poi_id, link_id)
        (
        SELECT distinct poi_id::bigint, m.link_id
          FROM org_p AS p
          LEFT JOIN mid_link_mapping as m
          ON p.linkid::bigint = m.org_link_id
          where p.kind IN ('8380', '8381')
        );
        '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        return 0

    def _make_hwy_sapa_store_info_11(self):
        '''SAPA的店铺'''
        self.log.info('Start Make sapa store info.')
        sqlcmd = """
        DROP INDEX if exists org_p_poi_id_idx;
        CREATE INDEX org_p_poi_id_idx
          ON org_p
          USING btree
          (poi_id);

        DROP INDEX if exists org_p_pa_poi_id_idx;
        CREATE INDEX org_p_pa_poi_id_idx
          ON org_p
          USING btree
          (pa_poi_id);
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

        sqlcmd = '''
        INSERT INTO mid_temp_sapa_store_info(poi_id, child_poi_id,
                                             chain_id, cat_id)
        (
        SELECT a.poi_id::bigint,  b.poi_id::bigint as child_poi_id,
               b.chaincode as chain_id, b.kind as cat_id
          FROM org_p as a
          INNER JOIN org_p as b
          ON a.poi_id = b.pa_poi_id
          where a.kind IN ('8380', '8381')
          ORDER BY a.poi_id
        );
        '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        return 0

    def _make_hwy_sapa_name_11(self):
        return 0

    def _make_hwy_store_name_11(self):
        '''Store or Chain Name'''
        self.CreateTable2('hwy_chain_name')
