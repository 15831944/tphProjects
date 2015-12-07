# -*- coding: UTF8 -*-
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
        
        # 作成表单regulation_relation_tbl记录道路规制情报（进入link、脱出link、交通规制类型等）\
        # 若规制是单条link规制，则表单regulation_relation_tbl中脱出link为空 \
        # 若规制是多条link规制，则表单regulation_relation_tbl中脱出link为规制最终脱出link，对应的经过link会记录在表单regulation_item_tbl
        
        # 作成表单regulation_item_tbl记录道路规制元素表（即道路规制涉及的点、线元素） \
        # 若规制是单条link规制时，当前规制id仅对应一条link, node为空，seq_num=1
        # 若规制是多条link规制时，当前规制id对应多条link，node不为空，link顺序通过seq_num保证
        
        self.CreateTable2('regulation_relation_tbl')
        self.CreateTable2('regulation_item_tbl')
        
        self.__convert_regulation_oneway_link()
        self.__convert_regulation_through_link()
        self.__convert_regulation_access_link()
        self.__make_linklist_for_linkdir()
        
        self.__convert_regulation_linkrow()
        self.__convert_regulation_nation_boundary()
        self.__convert_regulation_pdm()
        self.__prepare_admin_link_node()
        self.__convert_regulation_awr()
        self.__make_linklist_for_awr()
        
        self.__delete_dummy_regulation()
        
        return 0
    
    def __convert_condition_table(self):
        
        self.log.info('Begin convert condition_regulation_tbl...')
        
        # 作成表单temp_condition_regulation_tbl记录道路规制id与道路数据的对照关系
        # 表单temp_condition_regulation_tbl使用方法：道路首先根据对照关系找到对应的道路规制id，再根据id在表单condition_regulation_tbl查询\
        # 道路规制条件情报(规制id为-1代表没有规制；为空代表全时规制；>0代表规制有时间、车辆类型限制)
        # 作成表单condition_regulation_tbl记录道路规制的条件情报（包括时间、车辆类型限制）
        
        self.CreateTable2('temp_condition_regulation_tbl')
        self.CreateTable2('condition_regulation_tbl')
        
        self.CreateFunction2('mid_get_access')
        self.CreateFunction2('mid_convert_condition_regulation_tbl')
        
        self.pg.callproc('mid_convert_condition_regulation_tbl')
        self.pg.commit2()
        
        self.log.info('End convert condition_regulation_tbl.')
        
    def __convert_regulation_oneway_link(self):
        
        self.log.info('Begin convert regulation for oneway link...')
        
        # 更新表单regulation_relation_tbl/regulation_item_tbl---单条link一方通行规制（rdf_condition.condition_type=5）
        # Direction of Travel (CONDITION_TYPE = 5) condition indicates if vehicles can travel in a direction other than \
        # the general link or lane travel direction. The Direction of Travel condition describes the \
        # direction of travel on a link or lane for specific time periods and for specific vehicles.
        # 交通规制类型作成：
        #  travel_direction bearing    regulation
        #       F（正向）                         1（正向）                 不作成规制
        #       F（正向）                         2（反向）                 作成两条规制：（1）对应车型和时间条件下，该Link正方向不可通行 （2）对应车型和时间条件下，该Link反方向才可通行
        #       F（正向）                         3（双向）                 作成规制：（1）对应车型和时间条件下，该Link反方向才可通行
        #       T（反向）                         1（正向）                 作成两条规制：（1）对应车型和时间条件下，该Link正方向才可通行 （2）对应车型和时间条件下，该Link反方向不可通行
        #       T（反向）                         2（反向）                 不作成规制
        #       T（反向）                         3（双向）                作成规制：（1）对应车型和时间条件下，该Link正方向才可通行
        #       B（双向）                         1（正向）                 作成规制：（1）对应车型和时间条件下，该Link反方向不可通行
        #       B（双向）                         2（反向）                 作成规制：（1）对应车型和时间条件下，该Link正方向不可通行
        #       B（双向）                         3（双向）                  不作成规制
        
        self.CreateFunction2('mid_convert_regulation_oneway_link')
        
        self.pg.callproc('mid_convert_regulation_oneway_link')
        self.pg.commit2()
        
        self.log.info('End convert regulation for oneway link.')
        
    def __convert_regulation_through_link(self):
        
        self.log.info('Begin convert regulation for through link...')
        
        # 更新表单regulation_relation_tbl/regulation_item_tbl---单条link双向禁止规制（rdf_condition.condition_type=8）
        # Access Restriction (CONDITION_TYPE = 8) conditions identify situations where specified types of \
        # vehicles are prohibited from travelling on the road at specific times.
        
        self.CreateFunction2('mid_convert_regulation_through_link')
        
        self.pg.callproc('mid_convert_regulation_through_link')
        self.pg.commit2()
        
        self.log.info('End convert regulation for through link.')
        
    def __convert_regulation_access_link(self):
        
        self.log.info('Begin convert regulation for access link...')
        
        # 更新表单regulation_relation_tbl/regulation_item_tbl---单条link双向禁止规制（针对在当前link不能通行的车辆类型）
        # 根据RDF_NAV_LINK.ACCESS_ID（记录当前link可通行的车辆类型）找到当前link不可通行车辆类型，作成针对上述车辆的link双向禁止规制
        
        self.CreateFunction2('mid_convert_regulation_access_link')
        
        self.pg.callproc('mid_convert_regulation_access_link')
        self.pg.commit2()
        
        self.log.info('End convert regulation for access link.')
    
    def __convert_regulation_linkrow(self):
        
        self.log.info('Begin convert regulation for linkrow...')
        
        # 更新表单regulation_relation_tbl/regulation_item_tbl---多条link之间禁止通行规制
        # 作成数据来源：
        # 1、Restricted Driving Manoeuvre (RDM) (CONDITION_TYPE = 7) describes a manoeuvre from one link \
        #   to another that is prohibited.
        # 2、RDF_CONDITION_DIVIDER：a turn restriction from the originating link (FROM_LINK_ID) to the \
        #   restricted link (TO_LINK_ID). 无规制条件信息，默认全年全天全时规制
        
        self.CreateFunction2('mid_convert_regulation_linkrow')
        
        self.pg.callproc('mid_convert_regulation_linkrow')
        self.pg.commit2()
        
        self.log.info('End convert regulation for linkrow.')
    
    def __convert_regulation_nation_boundary(self):
        
        self.log.info('Begin convert regulation for national boundary...')
        
        # 作成表单temp_node_nation_boundary记录在国家边界上的node信息
        
        self.CreateTable2('temp_node_nation_boundary')
        self.CreateIndex2('temp_node_nation_boundary_node_id_idx')
        self.pg.commit2()
        
        # 更新表单regulation_relation_tbl/regulation_item_tbl---跨国规制
        # 两条Link所属国别（iso_country_code）不同，且按照交通流可通行
        
        self.CreateFunction2('mid_convert_regulation_nation_boundary')
        self.pg.callproc('mid_convert_regulation_nation_boundary')
        self.pg.commit2()
        
        self.log.info('End convert regulation for national boundary.')
    
    def __convert_regulation_pdm(self):
        
        self.log.info('Begin convert regulation for pdm...')
        
        # 更新表单regulation_relation_tbl/regulation_item_tbl---U-Turn允许通行
        # Permitted Driving Manoeuvre conditions (PDM) (CONDITION_TYPE = 39) indicate if a U-turn is allowed \
        # in areas where administrative wide U- turn restrictions exist.
        
        # 因path算路时不会出现在一条link上的u turn情形，所以仅mainnode u-turn, and one link u-turn is thrown away
        
        self.CreateFunction2('mid_convert_regulation_pdm')
        
        self.pg.callproc('mid_convert_regulation_pdm')
        self.pg.commit2()
        
        self.log.info('End convert regulation for pdm.')
    
    def __prepare_admin_link_node(self):
        
        self.log.info('Begin prepare admin link&node for make awr...')
        self.CreateIndex2('rdf_admin_hierarchy_admin_place_id_idx')
        self.CreateIndex2('rdf_admin_hierarchy_country_id_idx')
        self.CreateIndex2('rdf_admin_hierarchy_order1_id_idx')
        self.CreateIndex2('rdf_admin_hierarchy_order2_id_idx')
        self.CreateIndex2('rdf_admin_hierarchy_order8_id_idx')
        self.CreateIndex2('rdf_admin_hierarchy_builtup_id_idx')
        
        # 作成表单temp_regulation_admin记录存在U-turns are restricted throughout the entire administrative area的admin place
        
        self.CreateTable2('temp_regulation_admin')
        self.CreateIndex2('temp_regulation_admin_admin_place_id_idx')
        self.pg.commit2()
        
        # 创建表单temp_regulation_admin_link记录存在Admin Wide Regulation的行政区域内的所有link信息
        # 创建表单temp_regulation_admin_node记录存在Admin Wide Regulation的行政区域内的所有link关联的Node信息
        
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
        
        self.log.info('End prepare admin link&node for make awr.')
        
    def __convert_regulation_awr(self):
        
        self.log.info('Begin convert regulation for awr...')
        
        # find mainnode related to admin-wide-regulation
        # 一个复杂路口（典型的“井”字路口）或者一个简单路口（必须包含inner link，典型的“工”字路口）作成一个main node \
        # main node是一个逻辑上的概念，代表一个路口。实际应用为描述复杂路口，如“井”字路口，本身就是一个main node，为容易 \ 
        # 描述其交通流，将其拆分为多条inner link组成的路口信息。因此在数据中，main node本身是不存在的，取而代之的是路口内的 \
        # sub link and sub node
        # 创建表单temp_awr_mainnode_sublink记录一个main node内的sub link（可以有多条）信息
        # 创建表单temp_awr_mainnode_subnode记录一个main node内的sub node（可以有多个）信息
        # 创建表单temp_awr_mainnode记录main node与其关联的sub node的对照关系
        
        self.log.info('Begin find mainnode related to admin-wide-regulation...')
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
        # 创建表单temp_awr_mainnode_uturn记录进入main node以及从main node脱出的link信息
        # 创建表单temp_awr_node_uturn记录存在Admin Wide Regulation的行政区域内的node信息以及进入该node \
        # 从该node脱出的link信息（link均是双线化 and 非inner link and 进入link与脱出link存在夹角）
        # 上述两表作成原则如下：
        # 由于PATH算法问题需数据特别支持，特作临时对应：以上PDM允许通行规制和pdm_flag仍然作成；另外，对于有ADM规制的行政区域，区域内所有的无条件UTURN禁止，作成通行禁止规制数据。
        # UTURN禁止Link列的作成方法：
        # 对于复合路口：
        # （1）    进入路口的Link和退出路口的Link都是双向化道路，道路名称相同，且夹角小于85度；
        # （2）    进入路口的Link和退出路口的Link道路都是双向化道路，都没有道路名称，且夹角小于60度；
        # （3）    进入路口的Link和退出路口的Link道路是同一条单向化的Link，且能在复合路口内完成inner link掉头算路
        # 对于普通路口：
        # （1）    进入路口的Link和退出路口的Link都是双向化道路，道路名称相同，且夹角小于85度；
        # （2）    进入路口的Link和退出路口的Link道路都是双向化道路，都没有道路名称，且夹角小于60度
        self.CreateFunction2('rdb_get_angle_diff')
        self.CreateTable2('temp_awr_mainnode_uturn')
        self.CreateIndex2('temp_awr_mainnode_uturn_mainnode_id_idx')
        self.CreateTable2('temp_awr_node_uturn')
        self.CreateIndex2('temp_awr_node_uturn_node_id_idx')
        self.log.info('End find mainnode related to admin-wide-regulation.')
        
        # 创建表单temp_awr_pdm_linkrow记录Permitted Driving Manoeuvre (CONDITION_TYPE = 39)对应的规制信息（包含规制id、link序列）
        self.CreateTable2('temp_awr_pdm_linkrow')
        self.CreateIndex2('temp_awr_pdm_linkrow_regulation_id_idx')
        self.CreateIndex2('temp_awr_pdm_linkrow_first_link_idx')
        self.CreateFunction2('rdb_contains')
        self.CreateFunction2('mid_find_awr_inner_path')
        self.CreateTable2('temp_node_uturn_linkrow')
        self.CreateIndex2('temp_node_uturn_linkrow_linkids_idx')
        self.CreateTable2('temp_forbid_entry_regulation_linkrow')
        self.CreateIndex2('temp_forbid_entry_regulation_linkrow_linkids_idx')
        self.CreateFunction2('mid_convert_regulation_awr')
        self.pg.callproc('mid_convert_regulation_awr')
        self.pg.commit2()
        
        self.log.info('End convert regulation for awr.')
    
    def __make_linklist_for_awr(self):
        
        self.log.info('Begin make linklist for admin wide regulation...')
        # 创建表单temp_link_regulation_pdm_flag记录存在Admin Wide Regulation的link
        # 后续该信息会补充到link_tbl的字段extend_flag中
        self.CreateIndex2('regulation_relation_tbl_inlinkid_idx')
        self.CreateIndex2('regulation_relation_tbl_outlinkid_idx')
        self.CreateTable2('temp_link_regulation_pdm_flag')
        self.CreateIndex2('temp_link_regulation_pdm_flag_link_id_idx')
        self.pg.commit2()
        self.log.info('End make linklist for admin wide regulation.')
    
    def __make_linklist_for_linkdir(self):
        
        self.log.info('Begin make linklist for linkdir...')
        
        # 因无条件单条Link禁止通行规制需修改Link交通流为双向禁止，作成表单temp_link_regulation_forbid_traffic
        
        self.CreateTable2('temp_link_regulation_forbid_traffic')
        self.CreateIndex2('temp_link_regulation_forbid_traffic_link_id_idx')
        self.pg.commit2()
        
        # 因单条Link一方通行规制中的Link方向需补充至Link交通流，作成表单temp_link_regulation_permit_traffic
        
        self.CreateTable2('temp_link_regulation_permit_traffic')
        self.CreateIndex2('temp_link_regulation_permit_traffic_link_id_idx')
        self.pg.commit2()
        
        # 因作成跨国规制需要用到link的通行方向，作成表单temp_rdf_nav_link_traffic记录link的基本信息、通行方向
        # 此处所得一方通行与表单link_tbl_bak_splitting（link分割前数据）中一方通行存在差异，因此处作成时未考虑在建道路的情况（本应作成双向禁止）
        # 此处可能作成双向/正向/反向可通行，但对于规制数据作成本着宁滥勿缺的原则，故把在建道路的规制也做进去
        
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
        # 当进入link是双向禁止的，再做同一条件下的进入禁止信息就显得多余，需要删除
        
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
        # 当规制类别是顺行通行、逆行通行、双向禁止、U-Turn允许通行时，若对应的条件为空，即全时规制，应删除
        # 因上述信息可以通过link的交通流方向即可判定，无需增加规制信息
        
        sqlcmd = """
                delete from regulation_relation_tbl
                where condtype in (2,3,4,10) and cond_id is null;
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        # update regulation_item_tbl
        # 删除道路规制信息与道路规制要素信息不匹配的情形
        
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
    