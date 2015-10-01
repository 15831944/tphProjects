# -*- coding: UTF8 -*-
'''
Created on 2015年5月11日

@author: PC_ZH
'''
import platform.TestCase
import json


class CCheckSapa(platform.TestCase.CTestCase):
    '''高速上的SAPA Link都在表mid_temp_hwy_ic_path里，且设施种别是SAPA'''
    def _do(self):
        ''' '''
        return True
        self.pg.CreateTable_ByName('temp_hwy_check_link')
        self.pg.CreateFunction_ByName('check_is_hwy_sapa_link')
        self.pg.CreateFunction_ByName('check_sapa_link')
        sqlcmd = '''
        SELECT check_sapa_link();
        '''
        self.pg.execute(sqlcmd)
        sqlcmd = '''
        SELECT c.link_id AS check_link_id , a.link_id AS ic_link_id
        FROM temp_hwy_check_link as c
        LEFT JOIN
        (
        SELECT  link_id
           FROM
            (
            SELECT regexp_split_to_table(link_lid, E'\\,+') as link_id
              FROM mid_temp_hwy_ic_path
              WHERE facilcls_c in (1,2)
              GROUP BY road_code, road_seq, facilcls_c, link_id
              ORDER BY link_id
            )AS b
        ) AS a
        ON c.link_id::bigint  = a.link_id::bigint
        WHERE a.link_id is null
        '''
        flag = True
        for row in self.pg.get_batch_data(sqlcmd):
            link_id = row[0]
            ic_link = row[1]
            if not ic_link:
                self.pg.log.error('CCheckSapa:no link_id %d' % link_id)
                flag = False
        if not flag:
            return False
        return True


#-----------------------------------------------------#
#
#
#-----------------------------------------------------#
class CCheckSapaPath(platform.TestCase.CTestCase):
    '''SAPA设施路径里，至少有一条link是SAPA link '''
    def _do(self):
        ''' '''
        sqlcmd = '''
        SELECT gid, road_code,
               array_to_string(array_agg(link.link_id),',')AS link_id_org ,
               array_to_string(array_agg(ic_link.link_id),',')AS link_id_check
        FROM(
            SELECT gid, road_code, link_id
            FROM(
                SELECT gid, road_code,
                       regexp_split_to_table(link_lid, E'\\,+') as link_id
                FROM mid_temp_hwy_ic_path
                WHERE facilcls_c IN (1, 2)
                )AS a
            GROUP BY gid, road_code, link_id
            ORDER BY road_code
            )AS link
        LEFT JOIN
            (SELECT link_id
             FROM link_tbl
             WHERE link_type = 7
             )AS ic_link
        ON link.link_id::bigint = ic_link.link_id::bigint
        GROUP BY gid, road_code
        ORDER BY gid, road_code
        '''
        flag = True
        for row in self.pg.get_batch_data(sqlcmd):
            link_id_org = row[2]
            link_id_check = row[3]
            if not link_id_org:
                self.pg.log.info('CCheckSapaPath:no link id %s' % link_id_org)
            if link_id_check == "":
                self.pg.log.error('CCheckSapaPath:no sapa link, link_id %s'
                                   % link_id_org)
                flag = False
                continue
        if not flag:
            return False
        return True


#-----------------------------------------------------#
#
#
#-----------------------------------------------------#
class CCheckJctPath(platform.TestCase.CTestCase):
    '''JCT路径，不能有一般道 '''
    def _do(self):
        ''' '''
        sqlcmd = '''
        SELECT road_code, ic_link.link_id, ic_link.road_type
        FROM(
          SELECT road_code, link_id
          FROM(
            SELECT road_code,
                   regexp_split_to_table(link_lid, E'\\,+') as link_id
            FROM mid_temp_hwy_ic_path
            WHERE facilcls_c = 3  AND link_lid <>''
            )AS a
          GROUP BY road_code, link_id
          ORDER BY road_code
          )AS link
        LEFT JOIN link_tbl AS ic_link
        ON link.link_id::bigint = ic_link.link_id::bigint
        WHERE ic_link.road_type <> 0 AND link_type not in (4, 7, 8)
        ORDER BY  road_code
        '''
        flag = True
        for row in self.pg.get_batch_data(sqlcmd):
            link_id = row[1]
            road_type = row[2]
            if road_type != 0:
                self.pg.log.error('CCheckJctPath:not hwy link_id %s' % link_id)
                continue
        if not flag:
            return False
        return True


#-----------------------------------------------------#
#
#
#-----------------------------------------------------#
class CCheckIcPath(platform.TestCase.CTestCase):
    '''检查表中不符合规制路径 '''
    def _do(self):
        ''' '''
        sqlcmd = '''
        SELECT *
        FROM
        (
        SELECT link_lid , link_id,
               (CASE WHEN ic_path.inout_c = 1
            THEN ic_path.link_lid||',' || road.link_id
            ELSE road.link_id|| ',' || ic_path.link_lid
            END  )AS link
        FROM mid_temp_hwy_ic_path AS ic_path
        LEFT JOIN hwy_link_road_code_info AS road
        ON  (ic_path.inout_c = 2 AND ic_path.node_id = road.node_id) OR
            (IC_path.inout_c = 1 AND ic_path.to_node_id = road.node_id)
            --1:entry ;  2:exit;
        WHERE link_lid <> ''
        )AS path
        INNER JOIN
        (
        SELECT regulation_id ,
               array_to_string(array_agg(linkid),',') AS link_id,
               array_to_string(array_agg(nodeid),',') AS node_id,
               array_to_string(array_agg(seq_num),',') AS seq_num
        FROM (
            SELECT item.regulation_id,
                   item.linkid,
                   item.nodeid,
                   seq_num
            FROM regulation_item_tbl AS item
            INNER JOIN regulation_relation_tbl AS relation
            ON item.regulation_id = relation.regulation_id
            WHERE linkid is not null  AND condtype = 1
            ORDER BY item.regulation_id, seq_num
                )AS a
        GROUP BY regulation_id
        )AS b
        ON path.link_lid LIKE b.link_id
        WHERE path.link <> ''
        '''
        self.pg.execute(sqlcmd)
        row = self.pg.fetchone()
        if row:
            self.pg.log.error('CCheckIcPath:regulation path in ic path')
            return False
        return True

#=========================================================================
# CCheckIcNodeSapa
#若一个点有多个sapa设施，其设施类别是否一致
#=========================================================================
class CCheckIcNodeSapa(platform.TestCase.CTestCase):
    ''' '''
    def _do(self):
        sqlcmd = '''
        select road_code, updown_c, node_id ,
               array_agg(facilcls_c) as facil_c
        from mid_temp_hwy_ic_path
        where facilcls_c in (1,2)
        group by road_code, updown_c, node_id
        '''
        for row in self.pg.get_batch_data(sqlcmd):
            road_code = row[0]
            updown = row[1]
            node_id = row[2]
            facil_c_array = row[3]
            if len(set(facil_c_array)) > 1:
                return False
        return True
#===============================================================================
# CCheckFacilityNode
#检查所有收录node（非起始点）均为分歧点
#即判断流出node的link数大于等于2
#===============================================================================
# class CCheckFacilityNode(platform.TestCase.CTestCase):
#     '''检查所有收录node均为分歧点'''
#     def _do(self):
#         sqlcmd = '''
#         select path_id, node_id,
#                array_agg(outlink_path)as outlink_path_list, array_agg(outlink_id) as outlink_id_list,
#                array_agg(inlink_path )as inlink_path_list, array_agg(inlink_id)as inlink_id_list
#          from (
#          select c.path_id, c.node_id,
#                case
#                  when d.path_id is null then 0
#                  else d.path_id 
#                end as outlink_path,
#                d.link_id as outlink_id,
#                case
#                  when e.path_id is null then 0
#                  else e.path_id 
#                end  as inlink_path,
#                e.link_id as inlink_id
#         from mid_hwy_org_facility_node as c
#         left join
#         (
#             select b.road_code as path_id, a.link_id, s_node, e_node
#             from (
#              select *
#              from link_tbl
#              where road_type =0
#             )as a
#             left join  hwy_link_road_code_info as b
#             on a.link_id = b.link_id
#         )as d
#         on c.node_id = d.s_node
#         left join
#         (
#             select b.road_code as path_id, a.link_id, s_node, e_node
#             from (
#              select *
#              from link_tbl
#              where road_type =0
#             )as a
#             left join  hwy_link_road_code_info as b
#             on a.link_id = b.link_id
#         )as e
#         on c.node_id = e.e_node
#         order by path_id, node_id
#         )as f
#         group by  path_id, node_id
#         order by path_id, node_id
#         '''
#         for row in self.pg.get_batch_data(sqlcmd):
#             node_path = row[0]
#             node_id = row[1]
#             outlink_path = row[2]
#             outlink_id = row[3]
#             inlink_path = row[4]
#             inlink_id = row[5]
# 
#             for path in outlink_path:
#                 if path != node_path:
#                     break
# 
#             for path in inlink_path:
#                 if path != node_path:
#                     break
#             self.log.error('node id :%s' % node_id)
#             return False
#         return True
