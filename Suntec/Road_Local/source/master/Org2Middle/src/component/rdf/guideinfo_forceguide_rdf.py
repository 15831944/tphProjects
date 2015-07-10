# -*- coding: cp936 -*-
'''
Created on 2012-3-27

@author:
'''
import os
import component
import psycopg2

import component.default.guideinfo_forceguide
from common.common_func import GetPath
'''原数据来了之后我就会生成patch'''


class comp_guideinfo_forceguide_rdf(component.default.guideinfo_forceguide.com_guideinfo_forceguide):
    def __init__(self):
        '''
        Constructor
        '''
        component.default.guideinfo_forceguide.com_guideinfo_forceguide.__init__(self)

    def _DoCreateTable(self):
        if self.CreateTable2('force_guide_tbl') == -1:
            return -1
        if self.CreateTable2('new_force_guide_patch') == -1:
            return -1
        if self.CreateTable2('temp_patch_force_guide_tbl') == -1:
            return -1
        return 0

    def _DoCreateFunction(self):
        if self.CreateFunction2('mid_make_link_list_by_nodes') == -1:
            return -1
        if self.CreateFunction2('mid_findpasslinkbybothnodes') == -1:
            return -1
        return 0

    def _Do(self):
#         insert_sql = '''
#             INSERT INTO force_guide_tbl(
#             force_guide_id, nodeid, inlinkid, outlinkid, passlid, passlink_cnt,
#             guide_type, position_type)
#     VALUES (1, 1047712, 3200002761498, 3200002652092, '', 0,
#             2, 0),(2, 1111348, 3200003144648, 3200003144647, '', 0,
#             11, 5),(3, 1116030, 3200002774864, 3200002774860, '', 0,
#             0, 0);
#         '''
#         self.pg.execute2(insert_sql)
#         self.pg.commit2()
        '''导入patch'''
        forceguide_patch_full_path = GetPath('forceguide_patch_full_path')
        if forceguide_patch_full_path:
            import_patch(forceguide_patch_full_path, self.pg.pgcur2)
            self.pg.conn2.commit()
            '''通过patch中给出的node坐标序列，以0.03米搜索周围的点，确定本版数据中的点序列'''
            self._get_new_nodelist()
            '''根据搜到的点来确定link序列'''
            self._get_link_info()
            '''通过link序列，得到inlink、outlink、passlink等'''
            self._split_link_handler('temp_patch_force_guide_tbl', 'new_force_guide_patch')
            self._make_forceguide_tbl()
            
        self._make_mid_temp_force_guide_tbl()
        self._update_force_guide_tbl()
        return 0

    def _make_forceguide_tbl(self):
        insert_sql = '''
            INSERT INTO force_guide_tbl(
            force_guide_id, nodeid, inlinkid, outlinkid, passlid, passlink_cnt,
            guide_type, position_type)
            VALUES (%s, %s, %s, %s, %s, %s, %s, %s);
        '''
        sql_cmd = '''
            SELECT objectid, fromlinkid, tolinkid, midcount, midlinkid, guidetype
            FROM temp_patch_force_guide_tbl;
        '''
        self.pg.execute2(sql_cmd)
        rows = self.pg.fetchall2()
        for row in rows:
            force_guide_id = row[0]
            fromlinkid = row[1]
            tolink = row[2]
            midcount = row[3]
            midlinkid = row[4]
            guidetype = row[5]
            nodeid = 0
            node_type = 0
            if midcount:
                second_link = (midlinkid.split(','))[0]
                nodeid = self._get_connectnode_by_links(fromlinkid,
                                               second_link)
                node_type = self._get_node_type(fromlinkid, second_link)
            else:
                nodeid = self._get_connectnode_by_links(fromlinkid, tolink)
                node_type = self._get_node_type(fromlinkid, tolink)
            self.pg.execute2(insert_sql, (force_guide_id, nodeid, fromlinkid, tolink,
                                          '|'.join(midlinkid.split(',')), midcount,
                                          guidetype, node_type))
        self.pg.commit2()
        return

    def _get_node_type(self, inlink, outlink):
        node_type = 0
        sql_cmd = '''
            SELECT link_type, road_type
            FROM link_tbl
            where link_id = %s;
        '''
        self.pg.execute2(sql_cmd, (inlink,))
        inrow = self.pg.fetchone2()
        self.pg.execute2(sql_cmd, (outlink,))
        outrow = self.pg.fetchone2()
        if inrow[1] in (0, 1) and inrow[0] in (1, 2) and outrow[0] == 5:
            # 高速出口
            node_type = 5
        elif inrow[1] not in (0, 1) and outrow[0] == 5 and outrow[1] in (0, 1):
            # 高速入口
            node_type = 4
        elif inrow[1] in (0, 1) and inrow[0] in (1, 2) and outrow[1] in (0, 1) and outrow[0] == 3:
            # 高速本线分歧
            node_type = 6
        elif inrow[1] in (0, 1) and inrow[0] == 3 and outrow[1] in (0, 1) and outrow[0] in (1, 2):
            # 高速本线合流
            node_type = 7
        else:
            node_type = 0
        return node_type

    def _get_connectnode_by_links(self, inlink, outlink):
        sql_cmd = '''
            select a.start_node_id,a.end_node_id,b.link_id
            from
            (
            SELECT s_node as start_node_id, e_node as end_node_id
            FROM link_tbl
            where link_id = %s
            ) as a
            left join
            (        SELECT s_node as start_node_id, e_node as end_node_id,link_id
                    FROM link_tbl
                    where link_id = %s
            ) as b
            on a.start_node_id = b.end_node_id
                or a.start_node_id = b.start_node_id;
        '''
        self.pg.execute2(sql_cmd, (inlink, outlink))
        row = self.pg.fetchone2()
        if row[2]:
            return row[0]
        else:
            return row[1]

    def _get_new_nodelist(self):
        self.CreateFunction2('mid_getnode_by_goem')
        self.pg.callproc('mid_getnode_by_goem')
        self.pg.commit2()
        return 0
    
    def _get_link_info(self):
        sqlcmd = """
            insert into temp_patch_force_guide_tbl
            (
                select gid, link_array[1] as fromlinkid,
                    link_array[link_cnt] as tolinkid,
                    (link_cnt -2) as midcount,
                    array_to_string(link_array[2:link_cnt-1],',') as midlink,
                    guide_type as guidetype
                from
                (
                    select gid, link_array,
                        array_upper(link_array,1) as link_cnt, guide_type
                    from
                    (
                        select gid, mid_make_link_list_by_nodes(array[node1,node2,node3,node4,node5]) as link_array,guide_type
                        from new_force_guide_patch
                    ) as a
                ) as b
            )
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        return 0

    def _split_link_handler(self, temp_tbl_name, org_tbl_name):
        sqlcmd = '''
            SELECT objectid, guidetype,array[node1, node2, node3,  node4, node5]
            FROM {0} as a
            left join {1} as b
            on a.objectid = b.gid
            where a.fromlinkid is null or a.tolinkid is null;
        '''
        self.pg.execute2(sqlcmd.format(temp_tbl_name, org_tbl_name))
        c_rows = self.pg.fetchall2()
        for c_row in c_rows:
            all_linklib = ''
            objectid = c_row[0]
            nodes = c_row[2]
            for i in range(6):
                f_node = nodes[i]
                t_node = nodes[i + 1]
                if f_node and t_node:
                    self.pg.execute2('''
                        select mid_findpasslinkbybothnodes(%s,%s)
                    ''', (f_node, t_node))
                    sub_linklib = (self.pg.fetchone2())[0]
                    if sub_linklib:
                        all_linklib = all_linklib + sub_linklib + ','
            link_array = all_linklib.split(',')
            length = len(link_array)
            if length >= 3:
                midlinkid = ''
                j = 1
                while j < length - 2:
                    midlinkid = midlinkid + link_array[j] + ','
                    j = j + 1
                self.pg.execute2('''
                UPDATE {0}
                SET fromlinkid=%s, tolinkid=%s, midcount=%s,
                    midlinkid=%s
                 WHERE objectid = %s;
                '''.format(temp_tbl_name), (link_array[0], link_array[length - 2], length - 3,
                      midlinkid.rstrip(','), objectid))
            else:
                self.log.error('get route is fail!!!')
        self.pg.commit2()
        return
    
    def _make_mid_temp_force_guide_tbl(self):
        'use rdf_condition and rdf_nav_strand to make mid_temp_force_guide_tbl when rdf_condition.condition_type = 14(through route)'
        self.log.info('Now it is making mid_temp_force_guide_tbl...')
        if self.CreateTable2('mid_temp_force_guide_tbl') == -1:
            return -1
        self.CreateFunction2('rdf_update_mid_temp_force_guide_tbl')
        self.pg.callproc('rdf_update_mid_temp_force_guide_tbl')
        self.pg.commit2()
        self.log.info('making mid_temp_force_guide_tbl succeeded')
        return 0
    
    def _update_force_guide_tbl(self):
        
        sqlcmd = '''
            drop sequence if exists temp_link_forceguide_seq;
            create sequence temp_link_forceguide_seq;
            select setval('temp_link_forceguide_seq', cast(max_id as bigint))
            from
            (
                select max(force_guide_id) as max_id
                from force_guide_tbl
            )as a;
        '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2() 
        
        sqlcmd = """
                insert into force_guide_tbl (
                    force_guide_id,
                    nodeid, 
                    inlinkid,
                    outlinkid,
                    passlid,
                    passlink_cnt,
                    guide_type,
                    position_type
                )
                select 
                    nextval('temp_link_forceguide_seq') as force_guide_id,
                    nodeid, inlinkid, outlinkid, passlid, passlink_cnt,
                    0 as guide_type,
                    0 as position_type
                from (
                    select distinct a.nodeid, a.inlinkid, a.outlinkid, a.passlid, a.passlink_cnt
                    from mid_temp_force_guide_tbl a 
                    left join force_guide_tbl b
                        on 
                            b.nodeid = a.nodeid and
                            b.inlinkid = a.inlinkid and
                            b.outlinkid = a.outlinkid and
                            not(b.passlid is distinct from a.passlid) and
                            b.passlink_cnt = a.passlink_cnt
                    where b.gid IS NULL
                ) as c;

                --update force_guide_tbl set force_guide_id = gid where force_guide_id is null;
            """
        
        self.log.info('Now it is updating force_guide_tbl...')
        self.pg.do_big_insert2(sqlcmd)
        self.log.info('updating force_guide_tbl succeeded') 
        return 0


def import_patch(forceguide_patch_full_path, pgcur2):
    pgcur2.execute('''
            drop table if exists old_force_guide_patch;
            CREATE TABLE old_force_guide_patch
            (
              gid serial NOT NULL,
              guide_type smallint,
              node1_geom geometry,
              z1 smallint,
              node2_geom geometry,
              z2 smallint,
              node3_geom geometry,
              z3 smallint,
              node4_geom geometry,
              z4 smallint,
              node5_geom geometry,
              z5 smallint
            )
    ''')
    if os.path.exists(forceguide_patch_full_path):
        f_force_guide_patch = open(forceguide_patch_full_path, 'r')
        pgcur2.copy_from(f_force_guide_patch, 'old_force_guide_patch', '\t', "", 8192,
                        ('guide_type',
                       'node1_geom', 'z1',
                       'node2_geom', 'z2',
                       'node3_geom', 'z3',
                       'node4_geom', 'z4',
                       'node5_geom', 'z5'
                       ))
        f_force_guide_patch.close()
    return 0


def dump_patch(forceguide_patch_full_path, pgcur2):
    line_str = ''
    pgcur2.execute('''
        SELECT guide_type, b.the_geom, b.z,c.the_geom,
            c.z,d.the_geom, d.z,e.the_geom, e.z,f.the_geom, f.z
          FROM new_force_guide_patch as a
          left join temp_topo_node as b
          on a.node1 = b.node_id
          left join temp_topo_node as c
          on a.node2 = c.node_id
          left join temp_topo_node as d
          on a.node3 = d.node_id
          left join temp_topo_node as e
          on a.node4 = e.node_id
          left join temp_topo_node as f
          on a.node5 = f.node_id
    ''')
    rows = pgcur2.fetchall()
    f_force_guide_patch = open(os.path.join(forceguide_patch_full_path, 'text.csv'),
                                  'w')
    for row in rows:
        t_list = []
        for i in row:
            if i is not None:
                t_list.append(str(i))
            else:
                t_list.append('')
        f_force_guide_patch.write('\t'.join(t_list))
    f_force_guide_patch.close()
    return 0

# if __name__ == '__main__':
#     conn = psycopg2.connect(''' host='172.26.179.195'
#                         dbname='AP_NOSTRA_201401_0061_0103'
#                         user='postgres' password='pset123456' ''')
#     pgcur2 = conn.cursor()
#     forceguide_patch_full_path = 'C:\\shp\\forceguidepatch\\nostra\\temp_patch_force_guide_tbl.csv'
#     import_patch(forceguide_patch_full_path, pgcur2)
#     conn.commit()
    dump_patch(forceguide_patch_full_path, pgcur2)
    pass



