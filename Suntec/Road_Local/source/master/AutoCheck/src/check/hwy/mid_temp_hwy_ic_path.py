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
        self.pg.CreateTable_ByName('check_hwy_sapa_link')
        self.pg.CreateFunction_ByName('check_is_hwy_sapa_link')
        self.pg.CreateFunction_ByName('check_sapa_link')
        sqlcmd = '''
        SELECT check_hwy_sapa_link();
        '''
        self.pg.execute(sqlcmd)
        sqlcmd = '''
        SELECT c.link_id AS check_link_id , a.link_id AS ic_link_id
        FROM check_hwy_sapa_link as c
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
                self.pg.log.error('no link_id %d' % link_id)
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
            GROUP BY gid,road_code, link_id
            ORDER BY road_code)AS link
        LEFT JOIN
            (SELECT *
             FROM mid_temp_hwy_ic_link
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
                self.log.info('no link id %d' % link_id_org)
            if link_id_check == "":
                self.log.error('no sapa link, link_id %d' % link_id_org)
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
        WHERE ic_link.road_type <> 0 AND link_type <> 7
        ORDER BY  road_code
        '''
        flag = True
        for row in self.pg.get_batch_data(sqlcmd):
            link_id = row[1]
            road_type = row[2]
            if road_type != 0:
                self.log.error('not hwy link_id %d' % link_id)
                continue
        if not flag:
            return False
        return True


#-----------------------------------------------------#
#
#
#-----------------------------------------------------#
class CCheckIcPath(platform.TestCase.CTestCase):
    ''' '''
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
            self.pg.log.error('regulation path in ic path')
            return False
        return True


#-----------------------------------------------------#
#
#
#-----------------------------------------------------#
class CCheckICName(platform.TestCase.CTestCase):
    '''当同个设施有多个名称时，判断里面的名称有无重复(语种+val都相同)'''
    def _do(self):
        ''' '''
        sqlcmd = '''
        SELECT ic_no, name
        FROM rdb_highway_ic_info
        ORDER BY  ic_no
        '''
        for row in self.pg.get_batch_data(sqlcmd):
            ic_no = row[0]
            multi_name = row[1]
            if not multi_name:
                continue
            if self._is_name_repeat(multi_name):
                self.pg.log.error('name repeat in ic_no=%s:' % ic_no)
                return False
        return True

    def _is_name_repeat(self, multi_name):
        multi_name_list = self._get_json_name(multi_name)
        if not multi_name_list:
            self.pg.log.error('no ic name ')
            return True
        name_list = list()
        for multi_names in multi_name_list:
            for name_dict in multi_names:
                name_list.append((name_dict.get('lang'),
                                  name_dict.get('val'))
                                )
        name_set = set(name_list)
        if len(name_list) != len(name_set):
            return True
        return False

    def _get_json_name(self, json_name):
        ''' '''
        get_multi_name_list = list()
        if not json_name:
            return get_multi_name_list
        for multi_names in json.loads(json_name):
            get_names_list = list()
            for name_dict in multi_names:
                if(name_dict.get("tts_type") == "not_tts" and
                           name_dict.get("type") != "route_num"):
                    get_names_list.append(name_dict)
            get_multi_name_list.append(get_names_list)

        return get_multi_name_list
