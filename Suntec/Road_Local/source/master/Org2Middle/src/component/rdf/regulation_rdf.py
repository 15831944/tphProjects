# -*- coding: cp936 -*-
'''
Created on 2012-3-27

@author: liuxinxing
'''
import component.component_base

class comp_regulation_rdf(component.component_base.comp_base):
    def __init__(self):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self, 'Regulation')
        
    def _Do(self):
        self.__convert_condition_table()
        
        self.CreateTable2('regulation_relation_tbl')
        self.CreateTable2('regulation_item_tbl')
        
        self.__convert_regulation_oneway_link()
        self.__convert_regulation_through_link()
        self.__convert_regulation_access_link()
        self.__make_linklist_for_linkdir()
        
        self.__convert_regulation_linkrow()
        self.__convert_regulation_nation_boundary()
        self.__convert_regulation_pdm()
        self.__convert_regulation_awr()
        self.__make_linklist_for_awr()
        
        self.__delete_dummy_regulation()
        
        return 0
    
    def __convert_condition_table(self):
        self.log.info('Begin convert condition_regulation_tbl...')
        
        self.CreateTable2('temp_condition_regulation_tbl')
        self.CreateTable2('condition_regulation_tbl')
        
        self.CreateFunction2('mid_get_access')
        self.CreateFunction2('mid_convert_condition_regulation_tbl')
        
        self.pg.callproc('mid_convert_condition_regulation_tbl')
        self.pg.commit2()
        
        self.log.info('End convert condition_regulation_tbl.')
        
    def __convert_regulation_oneway_link(self):
        self.log.info('Begin convert regulation for oneway link...')
        
        self.CreateFunction2('mid_convert_regulation_oneway_link')
        
        self.pg.callproc('mid_convert_regulation_oneway_link')
        self.pg.commit2()
        
        self.log.info('End convert regulation for oneway link.')
        
    def __convert_regulation_through_link(self):
        self.log.info('Begin convert regulation for through link...')
        
        self.CreateFunction2('mid_convert_regulation_through_link')
        
        self.pg.callproc('mid_convert_regulation_through_link')
        self.pg.commit2()
        
        self.log.info('End convert regulation for through link.')
        
    def __convert_regulation_access_link(self):
        self.log.info('Begin convert regulation for access link...')
        
        self.CreateFunction2('mid_convert_regulation_access_link')
        
        self.pg.callproc('mid_convert_regulation_access_link')
        self.pg.commit2()
        
        self.log.info('End convert regulation for access link.')
    
    def __convert_regulation_linkrow(self):
        self.log.info('Begin convert regulation for linkrow...')
        
        self.CreateFunction2('mid_convert_regulation_linkrow')
        
        self.pg.callproc('mid_convert_regulation_linkrow')
        self.pg.commit2()
        
        self.log.info('End convert regulation for linkrow.')
    
    def __convert_regulation_nation_boundary(self):
        self.log.info('Begin convert regulation for national boundary...')
        
        self.CreateTable2('temp_node_nation_boundary')
        self.CreateIndex2('temp_node_nation_boundary_node_id_idx')
        self.pg.commit2()
        
        self.CreateFunction2('mid_convert_regulation_nation_boundary')
        self.pg.callproc('mid_convert_regulation_nation_boundary')
        self.pg.commit2()
        
        self.log.info('End convert regulation for national boundary.')
    
    def __convert_regulation_pdm(self):
        self.log.info('Begin convert regulation for pdm...')
        
        self.CreateFunction2('mid_convert_regulation_pdm')
        
        self.pg.callproc('mid_convert_regulation_pdm')
        self.pg.commit2()
        
        self.log.info('End convert regulation for pdm.')
    
    def __convert_regulation_awr(self):
        self.log.info('Begin convert regulation for awr...')
        
        # find links related to admin-wide-regulation
        self.log.info('find links related to admin-wide-regulation...')
        self.CreateIndex2('rdf_admin_hierarchy_admin_place_id_idx')
        self.CreateIndex2('rdf_admin_hierarchy_country_id_idx')
        self.CreateIndex2('rdf_admin_hierarchy_order1_id_idx')
        self.CreateIndex2('rdf_admin_hierarchy_order2_id_idx')
        self.CreateIndex2('rdf_admin_hierarchy_order8_id_idx')
        self.CreateIndex2('rdf_admin_hierarchy_builtup_id_idx')
        
        self.CreateTable2('temp_regulation_admin')
        self.CreateIndex2('temp_regulation_admin_admin_place_id_idx')
        self.pg.commit2()
        
        self.CreateIndex2('temp_rdf_nav_link_left_admin_place_id_idx')
        self.CreateIndex2('temp_rdf_nav_link_right_admin_place_id_idx')
        self.CreateFunction2('rdb_get_link_angle')
        self.CreateTable2('temp_regulation_admin_link')
        self.CreateIndex2('temp_regulation_admin_link_link_id_idx')
        self.CreateIndex2('temp_regulation_admin_link_s_node_idx')
        self.CreateIndex2('temp_regulation_admin_link_e_node_idx')
        self.CreateTable2('temp_regulation_admin_node')
        self.CreateIndex2('temp_regulation_admin_node_node_id_idx')
        self.pg.commit2()
        
        # find mainnode related to admin-wide-regulation
        self.log.info('find mainnode related to admin-wide-regulation...')
        self.CreateTable2('temp_awr_mainnode_sublink')
        self.CreateTable2('temp_awr_mainnode_subnode')
        self.CreateIndex2('temp_awr_mainnode_sublink_sublink_idx')
        self.CreateIndex2('temp_awr_mainnode_subnode_subnode_idx')
        
        self.CreateFunction2('mid_find_awr_mainnode')
        self.pg.callproc('mid_find_awr_mainnode')
        self.CreateTable2('temp_awr_mainnode')
        self.CreateIndex2('temp_awr_mainnode_mainnode_id_idx')
        self.pg.commit2()
        
        # find regulation
        self.CreateFunction2('rdb_get_angle_diff')
        self.CreateTable2('temp_awr_mainnode_uturn')
        self.CreateIndex2('temp_awr_mainnode_uturn_mainnode_id_idx')
        self.CreateTable2('temp_awr_node_uturn')
        self.CreateIndex2('temp_awr_node_uturn_node_id_idx')
        self.CreateTable2('temp_awr_pdm_linkrow')
        self.CreateIndex2('temp_awr_pdm_linkrow_regulation_id_idx')
        self.CreateIndex2('temp_awr_pdm_linkrow_first_link_idx')
        self.CreateFunction2('rdb_contains')
        self.CreateFunction2('mid_find_awr_inner_path')
        self.CreateFunction2('mid_convert_regulation_awr')
        self.pg.callproc('mid_convert_regulation_awr')
        self.pg.commit2()
        
        self.log.info('End convert regulation for awr.')
    
    def __make_linklist_for_awr(self):
        self.log.info('Begin make linklist for admin wide regulation...')
        
        self.CreateIndex2('rdf_admin_hierarchy_admin_place_id_idx')
        self.CreateIndex2('rdf_admin_hierarchy_country_id_idx')
        self.CreateIndex2('rdf_admin_hierarchy_order1_id_idx')
        self.CreateIndex2('rdf_admin_hierarchy_order2_id_idx')
        self.CreateIndex2('rdf_admin_hierarchy_order8_id_idx')
        self.CreateIndex2('rdf_admin_hierarchy_builtup_id_idx')
        
        self.CreateTable2('temp_regulation_admin')
        self.CreateIndex2('temp_regulation_admin_admin_place_id_idx')
        self.pg.commit2()
        
        self.CreateIndex2('temp_rdf_nav_link_left_admin_place_id_idx')
        self.CreateIndex2('temp_rdf_nav_link_right_admin_place_id_idx')
        self.CreateTable2('temp_regulation_admin_link')
        self.CreateIndex2('temp_regulation_admin_link_link_id_idx')
        self.pg.commit2()
        
        self.CreateIndex2('regulation_relation_tbl_inlinkid_idx')
        self.CreateIndex2('regulation_relation_tbl_outlinkid_idx')
        self.CreateTable2('temp_link_regulation_pdm_flag')
        self.CreateIndex2('temp_link_regulation_pdm_flag_link_id_idx')
        self.pg.commit2()
        
        self.log.info('End make linklist for admin wide regulation.')
    
    def __make_linklist_for_linkdir(self):
        self.log.info('Begin make linklist for linkdir...')
        
        self.CreateTable2('temp_link_regulation_forbid_traffic')
        self.CreateIndex2('temp_link_regulation_forbid_traffic_link_id_idx')
        self.pg.commit2()
        
        self.CreateTable2('temp_link_regulation_permit_traffic')
        self.CreateIndex2('temp_link_regulation_permit_traffic_link_id_idx')
        self.pg.commit2()
        
        self.CreateTable2('temp_rdf_nav_link_traffic')
        self.CreateIndex2('temp_rdf_nav_link_traffic_link_id_idx')
        self.CreateIndex2('temp_rdf_nav_link_traffic_ref_node_id_idx')
        self.CreateIndex2('temp_rdf_nav_link_traffic_nonref_node_id_idx')
        self.pg.commit2()
        
        self.log.info('End make linklist for linkdir.')
        
    def __delete_dummy_regulation(self):
        self.log.info('Begin deleting dummy regulation...')
        
        # backup
        sqlcmd = """
                drop table if exists regulation_relation_tbl_bak_dummy;
                create table regulation_relation_tbl_bak_dummy
                as 
                select * from regulation_relation_tbl;
                
                drop table if exists regulation_item_tbl_bak_dummy;
                create table regulation_item_tbl_bak_dummy
                as 
                select * from regulation_item_tbl;
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        # update regulation_relation_tbl
        sqlcmd = """
                delete from regulation_relation_tbl as x
                using
                (
                    select b.gid, b.regulation_id, b.condtype, b.cond_id
                    from regulation_relation_tbl as a
                    inner join regulation_relation_tbl as b
                    on  (a.condtype = 4 and b.condtype = 1)
                        and 
                        (a.inlinkid in (b.inlinkid, b.outlinkid))
                        and 
                        (a.cond_id is not distinct from b.cond_id)
                )as y
                where x.gid = y.gid
                ;
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        # update regulation_relation_tbl
        sqlcmd = """
                delete from regulation_relation_tbl
                where condtype in (2,3,4,10) and cond_id is null;
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        # update regulation_item_tbl
        sqlcmd = """
                delete from regulation_item_tbl as x
                using
                (
                    select distinct a.regulation_id
                    from
                    (
                        select distinct regulation_id
                        from regulation_item_tbl
                    )as a
                    left join regulation_relation_tbl as b
                    on a.regulation_id = b.regulation_id
                    where b.regulation_id is null
                )as y
                where x.regulation_id = y.regulation_id
                ;
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        self.log.info('End deleting dummy regulation.')
    
    def _DoCheckLogic(self):
        self.log.info('Check regulation...')
        
        self.CreateFunction2('mid_check_regulation_item')
        self.pg.callproc('mid_check_regulation_item')
        
        self.CreateFunction2('mid_check_regulation_condition')
        self.pg.callproc('mid_check_regulation_condition')
        
        self.log.info('Check regulation end.')
        return 0
    