# -*- coding: UTF8 -*-
'''
Created on 2015-7-30

@author: hcz
'''
import component.component_base
PARALLEL_MOD = 0.03  # 平行系数(Hausdorff_distance)
PARALLEL_DIST = 0.009  # About 1Km
PARALLEL_MULTIPLE = 5
PARALLEL_2KM = 2000


class HwyParallelPath(component.component_base.comp_base):
    '''
    Parallel Line
    '''

    def __init__(self, ItemName='HwyParallelline'):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self,
                                                    ItemName)

    def _DoCreateTable(self):
        self.CreateTable2('mid_temp_hwy_parallel_path')

    def _Do(self):
        # 给线路添加geom, 把线路上node点的坐标做成线路的形状点
        self._add_geom_for_path()
        # 通过 豪斯多夫距离，求平行线路
        self._make_parallel_path_by_hausdorff()
        pass

    def _make_parallel_path_by_hausdorff(self):
        '''通过 豪斯多夫距离，求平行线路'''
        # 取得 两条线路的最短距离不超过1Km, 豪斯多夫距离 小于 0.03
        parallel_pathes = dict()
        for parallel_info in self._get_paralle_path_by_hausdorff():
            a_path_id, b_path_id = parallel_info[0:2]
            s_dist = parallel_info[2]  # 线路b起点到线路a的最小距离
            e_dist = parallel_info[3]  # 线路b终点到线路a的最小距离
            a_lengh = parallel_info[4]
            b_length = parallel_info[5]
            hausdorff_dist = parallel_info[6]
            if (b_path_id, a_path_id) in parallel_pathes:
                # 保存平行，随便更新s_dist, e_dist,hausdorff_dist
                parallel_pathes[(a_path_id, b_path_id)] = (s_dist, e_dist,
                                                           hausdorff_dist)
            else:
                longer_dist = max([s_dist, e_dist])
                min_len = min([a_lengh, b_length])
                # 某一头偏离的比较大, 超过线路长的五分之一
                if (s_dist + e_dist) * PARALLEL_MULTIPLE >= min_len:
                    self.log.warning('distance > length/5. '
                                     'distance=%s,a_path_id=%s,b_path_id=%s'
                                     % (longer_dist, a_path_id, b_path_id))
                    continue
                if longer_dist > PARALLEL_2KM:  # 超过2Km
                    self.log.warning('longer distance > 2km. '
                                     'distance=%s,a_path_id=%s,b_path_id=%s'
                                     % (longer_dist, a_path_id, b_path_id))
                parallel_pathes[(a_path_id, b_path_id)] = (s_dist, e_dist,
                                                           hausdorff_dist)
                parallel_pathes[(b_path_id, a_path_id)] = (s_dist, e_dist,
                                                           hausdorff_dist)
        self._store_pallel_pathes(parallel_pathes)
        self.CreateIndex2('mid_temp_hwy_parallel_path_path_id_idx')

    def _get_paralle_path_by_hausdorff(self):
        '''两条线路的最短距离不超过1Km, 豪斯多夫距离 小于 0.03'''
        sqlcmd = """
        SELECT a_path_id, b_path_id,
               s_dist, e_dist,
               a_length, b_length,
               HausdorffDistance
          FROM (
            SELECT a.path_id AS a_path_id, b.path_id as b_path_id,
                   ST_HausdorffDistance(a.the_geom,
                                        b.the_geom) as HausdorffDistance,
                   ST_Distance(a.the_geom,
                               ST_StartPoint(b.the_geom), true) as s_dist,
                   ST_Distance(a.the_geom,
                               ST_EndPoint(b.the_geom), true) as e_dist,
                   a.length as a_length,
                   b.length as b_length,
                   a.sort_name as a_sort_name,
                   b.sort_name as b_sort_name,
                   a.sort_number as a_sort_number,
                   b.sort_number as b_sort_number
              FROM mid_temp_hwy_main_path_attr AS a
              LEFT JOIN mid_temp_hwy_main_path_attr AS b
              ON ST_Dwithin(a.the_geom, b.the_geom, %s) and -- 0.009: about 1km
                 a.delete_flag = 0 and
                 b.delete_flag = 0 and
                 (ST_HausdorffDistance(a.the_geom, b.the_geom) < %s or
                  ST_HausdorffDistance(get_closetline(a.the_geom, b.the_geom),
                                        b.the_geom) < %s) and
                 a.path_id <> b.path_id
              WHERE a.delete_flag = 0 --and a.path_id = 7 and b.path_id = 10
              ORDER BY a.path_id, HausdorffDistance
          ) as c
          WHERE b_path_id is not null
          ORDER BY a_path_id, HausdorffDistance
        """
        return self.get_batch_data(sqlcmd, (PARALLEL_DIST,
                                            PARALLEL_MOD,
                                            PARALLEL_MOD))

    def _add_geom_for_path(self):
        '''给line添加geom, 把线路上node点的坐标做成线路的形状点'''
        sqlcmd = """
        SELECT AddGeometryColumn('','mid_temp_hwy_main_path_attr',
                                 'the_geom', '4326', 'LINESTRING', 2);
        """
        try:
            self.pg.execute2(sqlcmd)
            self.pg.commit2()
        except Exception, ex:
            self.log.warning('%s:%s' % (Exception, ex))
        finally:
            self.pg.conn2.rollback()
        sqlcmd = """
        UPDATE mid_temp_hwy_main_path_attr as p SET the_geom = path_geom
          FROM (
            SELECT path_id, ST_MakeLine(the_geom) as path_geom
              FROM (
                SELECT path_id, a.node_id, link_id, seq_nm, b.the_geom
                  FROM mid_temp_hwy_main_path as a
                  LEFT JOIN node_tbl as b
                  ON a.node_id = b.node_id
                  ORDER BY path_id, seq_nm
              ) as c
              GROUP BY path_id
          ) as p_g
          where p.path_id = p_g.path_id;
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        self.CreateIndex2('mid_temp_hwy_main_path_attr_the_geom_idx')

    def _store_pallel_pathes(self, parallel_pathes):
        sqlcmd = """
        INSERT INTO mid_temp_hwy_parallel_path(path_id, p_path_id,
                                               s_dist, e_dist, hausdorff_dist)
            VALUES(%s, %s,
                   %s, %s, %s);
        """
        for key, val in sorted(parallel_pathes.iteritems(),
                               cmp=lambda x, y: cmp(x[0][0], y[0][0])):
            self.pg.execute2(sqlcmd, (key + val))
        self.pg.commit2()
