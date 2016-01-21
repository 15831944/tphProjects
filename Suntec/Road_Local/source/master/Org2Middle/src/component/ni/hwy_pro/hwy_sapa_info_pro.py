# -*- coding: UTF8 -*-
'''
Created on 2015年12月14日

@author: hcz
'''
import codecs
import common
from component.rdf.hwy.hwy_exit_poi_name_rdf import HwyPoiExitNameRDF


class HwySaPaInfoNiPro(HwyPoiExitNameRDF):
    '''
    SAPA情报
    '''

    def __init__(self, item_name='HwySaPaInfoNi'):
        '''
        Constructor
        '''
        HwyPoiExitNameRDF.__init__(self, item_name)

    def _DoCreateTable(self):
        self.CreateTable2('mid_temp_poi_link')
        self.CreateTable2('mid_temp_hwy_sapa_name')
        self.CreateTable2('mid_temp_sapa_store_info')
        self.CreateTable2('mid_temp_poi_closest_link')
        return 0

    def _DoCreateIndex(self):
        return 0

    def _Do(self):
        # 14年
        self._make_poi_link()
        # 添加扩展poi
        self._load_extend_poi()
        self._make_hwy_poi_closest_link()
        if self._check_duplicate_poi_id():
            self._updata_poi_link()
        self._make_hwy_store_name()
        return 0

    def _check_duplicate_poi_id(self):
        '''检查扩展poi_id是否与原始poi_id冲突'''
        sqlcmd = '''
        select distinct a.poi_id
        from hwy_extend_sapa_poi_ni as a
        inner join org_poi as b
        on a.poi_id = b.poi_id::bigint
        '''
        dup_flag = True
        for row in self.pg.get_batch_data2(sqlcmd):
            poi_id = row[0]
            dup_flag = False
            self.log.error('duplicated poi id:%s' % poi_id)
        return dup_flag

    def _updata_poi_link(self):
        '''将扩展poi更新进mid_temp_poi_link表'''
        if not self.pg.IsExistTable('mid_temp_poi_link'):
            self.log.error('no table mid_temp_poi_link')
            return
        sqlcmd = '''
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
                           ST_Distance(a.the_geom, link_tbl.the_geom) as dist,
                           length,
                           ST_Line_Locate_Point(link_tbl.the_geom,
                                                a.the_geom) as point
                      FROM hwy_extend_sapa_poi_ni as a
                      INNER JOIN link_tbl
                      ON a.link_id = link_tbl.link_id and road_type = 0
                      ORDER BY a.poi_id, dist, a.link_id
              ) AS d
              GROUP BY poi_id
          ) as c
        );
        '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        return 0

    def _load_extend_poi(self):
        obj_config = common.config.CConfig.instance()
        file_path = obj_config.getPara('hwy_extend_sapa_poi')
        self.CreateTable2('hwy_extend_sapa_poi_ni')
        try:
            file_obj = codecs.open(file_path, 'r', 'utf-8')
            file_obj.readline()  # skip first line
            self.pg.copy_from2(file_object=file_obj,
                               table='hwy_extend_sapa_poi_ni',
                               columns=('poi_id', 'link_id', 'x', 'y'))
            self.pg.commit2()
        except:
            self.log.error('can not open file')
        sqlcmd = """
        UPDATE hwy_extend_sapa_poi_ni
          SET the_geom = ST_SetSRID(ST_MakePoint(x, y), 4326);
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

    def _make_poi_link(self):
        '''SAPA Poi所在linkid
        '''
        self.log.info('Start Make SAPA link.')
        self.CreateIndex2('org_poi_linkid_idx2')
        self.CreateIndex2('org_poi_poi_id_idx2')
        self.CreateIndex2('org_pname_featid_nametype_idx1')
        # 230206: 高速服务区, 230207: 高速停车区
        # 230211: 室内停车场, 230212: 地上露天停车场
        # 230224: 地上非露天停车场, 230225: 地下停车场
        sqlcmd = '''
        INSERT INTO mid_temp_poi_link(poi_id, link_id)
        (
        SELECT distinct poi_id::bigint, m.link_id
          FROM org_poi AS p
          LEFT JOIN mid_link_mapping as m
          ON p.linkid::bigint = m.org_link_id
          left join link_tbl
          on m.link_id = link_tbl.link_id
          where p.kind IN ('230206', '230207', '230211',
                           '230212', '230224', '230225') and
                road_type = 0
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

    def _make_hwy_store_name(self):
        '''Store or Chain Name'''
        self.CreateTable2('hwy_chain_name')
        sqlcmd = """
        INSERT INTO hwy_chain_name(u_code, cat_id, sub_cat,
                                   chain_id, chain_name, language_code)
        (
        select per_code as u_code, org_code1 as cat_id, '' as subcat,
               chaincode, name as chain_name, 'CHI' as language_code
          from temp_poi_category
          where chaincode is not null and chaincode <> '' and
                org_code1 is not null and is_brand = 'Y'
        );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
