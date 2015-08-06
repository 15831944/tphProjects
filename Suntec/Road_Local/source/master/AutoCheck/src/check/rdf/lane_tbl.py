# -*- coding: UTF8 -*-

import platform.TestCase

# rdf_lane_nav_strand中算出的每条诱导link链中的link数必须不少于2条。
class CCheckAllLaneStrandsContainMoreThan2Lanes(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
                    select count(*)
                    from
                    (
                        select lane_nav_strand_id, array_agg(lane_id) as lane_list
                        from rdf_lane_nav_strand
                        group by lane_nav_strand_id
                    ) as t
                    where array_upper(lane_list, 1) < 2;
                '''
        return self.pg.getOnlyQueryResult(sqlcmd) == 0

# rdf_lane中某条link所有T方向的车线必须是并列的，没有被F方向的车线隔开。
class CCheckAllTLanesNotSeperated(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
                    select link_id, array_agg(lane_number) as T_lane_num_list
                    from
                    (
                        select * from rdf_lane 
                        where lane_travel_direction='T'
                        order by link_id, lane_number
                    ) as t
                    group by link_id
                '''
        self.pg.execute(sqlcmd)
        rows = self.pg.fetchall()
        for row in rows:
            T_lane_num_list = row[1]
            for i in range(0, len(T_lane_num_list)-1):
                if T_lane_num_list[i+1]-T_lane_num_list[i] != 1:
                    return False
        return True

# rdf_lane中某条link所有F方向的车线必须是并列的，没有被T方向的车线隔开。
class CCheckAllFLanesNotSeperated(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
                    select link_id, array_agg(lane_number) as F_lane_num_list
                    from
                    (
                        select * from rdf_lane 
                        where lane_travel_direction='F'
                        order by link_id, lane_number
                    ) as t
                    group by link_id
                '''
        self.pg.execute(sqlcmd)
        rows = self.pg.fetchall()
        for row in rows:
            F_lane_num_list = row[1]
            for i in range(0, len(F_lane_num_list)-1):
                if F_lane_num_list[i+1]-F_lane_num_list[i] != 1:
                    return False
        return True
    
# 由rdf_lane_nav_strand中算出的诱导link链必须是一一连接的。
class CCheckAllLaneNavStrandAreConnected(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
                    select lane_nav_strand_id, 
                           array_agg(link_id) as link_id_list,
                           array_agg(ref_node_id) as ref_node_id_list, 
                           array_agg(nonref_node_id) as nonref_node_id_list
                    from
                    (
                    select a.lane_nav_strand_id, c.link_id, c.ref_node_id, c.nonref_node_id 
                    from 
                    rdf_lane_nav_strand as a
                    left join rdf_lane as b
                    on a.lane_id=b.lane_id
                    left join rdf_link as c
                    on b.link_id=c.link_id
                    order by lane_nav_strand_id, seq_num
                    ) as t
                    group by lane_nav_strand_id;
                '''
        self.pg.execute(sqlcmd)
        rows = self.pg.fetchall()
        for row in rows:
            ref_node_id_list = row[2]
            nonref_node_id_list = row[3]
            tempZip = zip(ref_node_id_list, nonref_node_id_list)
            for i in range(0, len(tempZip)-1):
                if tempZip[i][0] != tempZip[i+1][0] and \
                   tempZip[i][0] != tempZip[i+1][1] and \
                   tempZip[i][1] != tempZip[i+1][0] and \
                   tempZip[i][1] != tempZip[i+1][1]:
                    return False
        return True

# 关于车线的方向，协议中写明了4种：'B','N','T','F'。
# 目前所有仕向地均未发现有'B'和'N'，即只有'T'与'F'。
# 主处理函数中未对'B'和'N'的车线进行处理，而是直接丢弃。此处添加check，若存在'B'和'N'的车线，则发出警报。
class CCheckAllLaneDirShouldBeTorF(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
                    select * from rdf_lane 
                    where lane_travel_direction = 'N' 
                          or lane_travel_direction = 'B';
                '''
        return self.pg.getOnlyQueryResult(sqlcmd) == 0
























