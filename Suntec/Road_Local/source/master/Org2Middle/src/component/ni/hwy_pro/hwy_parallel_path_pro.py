# -*- coding: UTF8 -*-
'''
Created on 2015-12-21

@author: hcz
'''
from component.rdf.hwy.hwy_parallel_path import PARALLEL_MULTIPLE
from component.rdf.hwy.hwy_parallel_path import PARALLEL_2KM, PARALLEL_DIST
from component.rdf.hwy.hwy_parallel_path import HwyParallelPath
PARALLEL_DIST_NI = PARALLEL_DIST / 2  # 0.005: about 0.5km
PARALLEL_MOD_NI = 0.04  # 平行系数(Hausdorff_distance)


class HwyParallelPathNiPro(HwyParallelPath):
    '''
    Parallel Line
    '''

    def __init__(self, ItemName='HwyParallelPathNiPro'):
        '''
        Constructor
        '''
        HwyParallelPath.__init__(self, ItemName)

    def _DoCreateTable(self):
        self.CreateTable2('hwy_parallel_road_code')

    def _Do(self):
        # 给线路添加geom, 把线路上node点的坐标做成线路的形状点
        self._add_geom_for_path()
        # 通过 豪斯多夫距离，求平行线路
        self._make_parallel_path_by_hausdorff()

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

    def _get_paralle_path_by_hausdorff(self):
        '''两条线路的最短距离不超过1Km, 豪斯多夫距离 小于 0.03'''
        sqlcmd = """
        SELECT a_road_code, b_road_code,
               s_dist, e_dist,
               a_length, b_length,
               HausdorffDistance
          FROM (
            SELECT a.road_code AS a_road_code, b.road_code as b_road_code,
                   ST_HausdorffDistance(a.the_geom,
                                        b.the_geom) as HausdorffDistance,
                   ST_Distance(a.the_geom,
                               ST_StartPoint(b.the_geom), true) as s_dist,
                   ST_Distance(a.the_geom,
                               ST_EndPoint(b.the_geom), true) as e_dist,
                   a.length as a_length,
                   b.length as b_length
              FROM road_code_the_geom AS a
              LEFT JOIN road_code_the_geom AS b
              ON ST_Dwithin(a.the_geom, b.the_geom, %s) and
                 a.road_code <> b.road_code and
                 ST_HausdorffDistance(a.the_geom, b.the_geom) < %s
                 --or
                 --ST_HausdorffDistance(get_closetline(a.the_geom,
                 --                                    b.the_geom),
                 --                     b.the_geom) < 0.03)
                 and a.road_code <> b.road_code
              ORDER BY a.road_code, HausdorffDistance
          ) as c
          WHERE b_road_code is not null
          ORDER BY a_road_code, HausdorffDistance
        """
        return self.get_batch_data(sqlcmd, (PARALLEL_DIST_NI,
                                            PARALLEL_MOD_NI))

    def _add_geom_for_path(self):
        '''给line添加geom, 把线路上node点的坐标做成线路的形状点'''
        self.CreateTable2('road_code_the_geom')
        sqlcmd = """
        INSERT INTO road_code_the_geom(road_code, updown, length, the_geom)
        (
        SELECT road_code, updown, sum(length) as length,
               ST_MakeLine(the_geom) as path_geom
        FROM (
            SELECT road_code,updown, a.node_id, seq_nm, length, b.the_geom
              FROM hwy_link_road_code_info as a
              LEFT JOIN node_tbl as b
              ON a.node_id = b.node_id
              LEFT JOIN link_tbl as c
              on a.link_id = c.link_id
              ORDER BY road_code, updown, seq_nm
        ) as c
        GROUP BY road_code, updown
        ORDER BY road_code, updown
        );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        self.CreateIndex2('road_code_the_geom_the_geom_idx')

    def _store_pallel_pathes(self, parallel_pathes):
        sqlcmd = """
        INSERT INTO hwy_parallel_road_code(road_code, p_road_code,
                                           s_dist, e_dist,
                                           hausdorff_dist)
            VALUES(%s, %s,
                   %s, %s,
                   %s);
        """
        for key, val in sorted(parallel_pathes.iteritems(),
                               cmp=lambda x, y: cmp(x[0][0], y[0][0])):
            self.pg.execute2(sqlcmd, (key + val))
        self.pg.commit2()
