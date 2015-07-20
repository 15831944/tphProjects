# -*- coding: UTF8 -*-
'''
Created on 2015-6-29

@author: PC_ZH
'''

from component.component_base import comp_base


class HwyPrepareZenrin(comp_base):
    '''
    Hwy Prepare
    '''

    def __init__(self, itemname='HwyPrepareZenrin'):
        '''
        Constructor
        '''
        comp_base.__init__(self, itemname)

    def _DoCreateTable(self):
        return 0

    def _DoCreateIndex(self):
        return 0

    def _Do(self):
        self._make_prepare_node()
#         self._make_all_hwy_node()
        return 0

    def _make_all_hwy_node(self):
        return 0

    def _make_prepare_node(self):
        # 高速特征点(高速出入口点和高速设施所在点也即不能被合并的点)
        self.log.info('start make prepare node')
        self.CreateTable2('temp_prepare_node')
        sqlcmd = '''
        SELECT pathid, dirflag,
               array_agg(seq)AS seq_array,
               array_agg(array_to_string(prepntid,'|')) AS preptnid_array,
               array_agg(array_to_string(nextpntid,'|')) AS nextpntid_array,
               array_agg(s_node) AS s_node_array,
               array_agg(e_node) AS e_node_array
        FROM(
                SELECT pathid, dirflag, seq,
                       n1.node_id as s_node, n2.node_id as e_node,
                       array_agg(prepntid) as prepntid,
                       array_agg(nextpntid) as nextpntid
                FROM org_highwaypath as hwy_p
                left joIn org_road_hiway1 as r
                ON hwy_p.meshcode = r.meshcode::BIGINT and
                   hwy_p.linkno = r.linkno
                left join temp_node_mapping as n1   -- Start Node
                on r.meshcode = n1.meshcode and r.snodeno = n1.nodeno
                left join temp_node_mapping as n2   -- End Node
                on r.meshcode = n2.meshcode and r.enodeno = n2.nodeno
                GROUP BY pathid, seq, dirflag, s_node, e_node
        ORDER BY pathid, dirflag, seq
        )AS g
        GROUP BY pathid, dirflag
        ORDER BY pathid, dirflag;
        '''
        for rec in self.get_batch_data(sqlcmd):
            if not rec:
                self.log.error('no path')
            path_id = rec[0]
#             dirflag = rec[1]
            seq_array = rec[2]
            pre_point_array = rec[3]
            next_point_array = rec[4]
            s_node_array = rec[5]
            e_node_array = rec[6]

            path_info_array = zip(seq_array, pre_point_array,
                                  next_point_array,
                                  s_node_array, e_node_array)
            for index in range(0, len(path_info_array)):
                curr_point_no_prec = path_info_array[index][1]
                curr_point_no_next = path_info_array[index][2]  # 最后一个设施绑定
                curr_s_node = path_info_array[index][3]
                curr_e_node = path_info_array[index][4]

                if index != (len(path_info_array) - 1):
#                     next_point_no_prec = path_info_array[index + 1][1]
                    next_point_no_next = path_info_array[index + 1][2]
                    next_s_node = path_info_array[index + 1][3]
                    next_e_node = path_info_array[index + 1][4]
                else:
                    pass

                if index == 0:
                    start_node_no = self._get_start_node(curr_s_node,
                                                         curr_e_node,
                                                         next_s_node,
                                                         next_e_node)
                    if start_node_no:
                        self._store_prepare_node(path_id, curr_point_no_prec,
                                                 start_node_no)
                    else:
                        self.log.error('no start node')
                        continue
                if index == (len(path_info_array) - 1):
                    prev_s_node = path_info_array[index - 1][3]
                    prev_e_node = path_info_array[index - 1][4]
                    end_node_no = self._get_end_node(prev_s_node,
                                                     prev_e_node,
                                                     curr_s_node,
                                                     curr_e_node)

                    self._store_prepare_node(path_id, curr_point_no_next,
                                             end_node_no)
                    continue
                if next_point_no_next != curr_point_no_next:
                    node_no = self._get_inserction_node(curr_s_node,
                                                        curr_e_node,
                                                        next_s_node,
                                                        next_e_node)
                    if node_no:
                        self._store_prepare_node(path_id, curr_point_no_next,
                                                 node_no)
        self.pg.commit2()
        self.log.info('end make prepare node')

    def _store_prepare_node(self, path_id, ic_no, node_id):
        self.log.info('store prepare node')
        sqlcmd = '''
        INSERT INTO temp_prepare_node(path_id, ic_no, node_id)
        VALUES(%s, %s, %s);
        '''
        ic_no_array = set(ic_no.split('|'))
        for ic_id in ic_no_array:
            self.pg.execute2(sqlcmd, (path_id, ic_id, node_id))
        return 0

    def _store_point_node(self, ic_no, node_id):
        self.log.info('store point node')
        sqlcmd = '''
        INSERT INTO temp_ic_node(ic_no, node_id)
        VALUES(%s, %s)
        '''
        ic_no_array = ic_no.split('|')
        for ic_id in ic_no_array:
            self.pg.execute2(sqlcmd, (ic_id, node_id))
        return 0

    def _get_inserction_node(self, curr_s_node, curr_e_node,
                             next_s_node, next_e_node):
        if curr_s_node in (next_s_node, next_e_node):
            return curr_s_node
        elif curr_e_node in (next_s_node, next_e_node):
            return curr_e_node
        else:
            self.log.error('path is not continued')
            return 0

    def _get_start_node(self, curr_s_node, curr_e_node,
                        next_s_node, next_e_node):
        if curr_s_node in (next_s_node, next_e_node):
            return curr_e_node
        elif curr_e_node in (next_s_node, next_e_node):
            return curr_s_node
        else:
            self.log.error('path is not continued')
            return 0

    def _get_end_node(self, last_s_node, last_e_node,
                      curr_s_node, curr_e_node):
        if curr_s_node in (last_s_node, last_e_node):
            return curr_e_node
        elif curr_e_node in (last_s_node, last_e_node):
            return curr_s_node
        else:
            self.log.error('path is not continued')
            return 0
