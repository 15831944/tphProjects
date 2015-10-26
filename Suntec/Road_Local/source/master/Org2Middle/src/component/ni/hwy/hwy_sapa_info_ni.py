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
        return 0

    def _DoCreateIndex(self):
        self.CreateIndex2('mid_temp_poi_link_poi_id_idx')
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
        self._make_hwy_sapa_store_info()
        self._make_hwy_sapa_name()
        self._make_hwy_store_name()
        return 0

    def _make_poi_link(self):
        '''SAPA Poi所在linkid
        '''
        self.log.info('Start Make SAPA link.')
        self.CreateIndex2('org_poi_linkid_idx')
        self.CreateIndex2('org_poi_linkid_idx2')
        sqlcmd = '''
        INSERT INTO mid_temp_poi_link(poi_id, link_id)
        (
        SELECT distinct poi_id::bigint, m.link_id
          FROM org_poi AS p
          LEFT JOIN mid_link_mapping as m
          ON p.linkid::bigint = m.org_link_id
          where p.kind IN ('8380', '8381')
        );
        '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        return 0

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
                SELECT poi_id, name, language
                FROM org_poi
                LEFT JOIN org_pname
                ON poi_id = featid
                -- 8380: SA, 8381: PA
                WHERE org_poi.kind IN ('8380', '8381')
                      and nametype = '9'  -- 9: POI
                ORDER BY poi_id, language
          ) AS A
          GROUP BY poi_id;
        '''
        return self.get_batch_data(sqlcmd)

    def _make_hwy_store_name(self):
        '''Store or Chain Name'''
        self.CreateTable2('hwy_chain_name')
        sqlcmd = """
        INSERT INTO hwy_chain_name(u_code, cat_id, sub_cat,
                                   chain_id, chain_name, language_code)
        (
        SELECT per_code as u_code, a.kind, '' as subcat,
               a.chaincode, '' as chain_name, 'CHI' as language_code
          FROM org_poi as a
          LEFT JOIN temp_poi_category as b
          ON a.chaincode = b.chaincode and a.kind = b.org_code1
          where a.chaincode <> '' and per_code is not null

        union
         SELECT distinct per_code as u_code, a.kind, '' as subcat,
               a.chaincode, '' as chain_name, 'CHI' as language_code
          FROM org_poi as a
          LEFT JOIN temp_poi_category as b
          ON a.chaincode = b.chaincode and a.kind = b.org_code2
          where a.chaincode <> '' and per_code is not null
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
          LEFT JOIN org_poi as C
          ON b.poi_id2 = c.poi_id
          where a.kind IN ('8380', '8381')
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
