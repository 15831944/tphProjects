# -*- coding: cp936 -*-
'''

'''
import component.component_base

class comp_guideinfo_point_adjust(component.component_base.comp_base):
    '''
    Node Update for guideinfo table
    '''

    def __init__(self):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self, 'Guidence_Point_Adjust')
        
    def _Do(self):
        table_list=[
                    'signpost_tbl',
                    'lane_tbl',
                    'spotguide_tbl',
                    'towardname_tbl',
                    'signpost_uc_tbl'
                    ]
        
        self.CreateFunction2('mid_adjust_guidence_node')
        sqlcmd='''
                drop table if exists temp_adjust_[table_name]_backup;
                create table temp_adjust_[table_name]_backup
                as
                (
                    select a.*
                    from [table_name] as a
                    join node_tbl as b
                    on a.nodeid = b.node_id
                    where (a.passlink_cnt > 0) and array_upper(string_to_array(node_lid,'|'),1) < 3
                );

                drop table if exists temp_adjust_[table_name]_update;
                create table temp_adjust_[table_name]_update
                as
                select *
                from
                (
                    select  *,
                            (mid_adjust_guidence_node(nodeid, link_array, '[table_name]')).* 
                    from
                    (
                        select  *,
                                array[inlinkid] || (string_to_array(passlid, '|'))::bigint[] || array[outlinkid] as link_array
                        from temp_adjust_[table_name]_backup
                    ) as t
                ) as t
                where nodeid != new_nodeid
                ;
                create index temp_adjust_[table_name]_update_gid_idx
                    on temp_adjust_[table_name]_update
                    using btree
                    (gid);

                update [table_name] as a
                set inlinkid = b.new_inlinkid, nodeid = b.new_nodeid, passlid = b.new_passlid, 
                    passlink_cnt = (
                                    case when b.new_passlid = '' or b.new_passlid is null then 0 
                                         else array_upper(string_to_array(b.new_passlid, '|'), 1)
                                    end
                                    )
                from temp_adjust_[table_name]_update as b
                where a.gid = b.gid;
               '''
        
        for table_name in table_list:
            self.pg.execute2(sqlcmd.replace('[table_name]',table_name))
            self.pg.commit2()
            