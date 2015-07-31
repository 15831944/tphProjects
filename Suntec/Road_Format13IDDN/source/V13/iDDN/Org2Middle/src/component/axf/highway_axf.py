# -*- coding: UTF8 -*-
'''
Created on 2013-4-23

@author: zhangyongmu
'''

import base

class comp_highway_axf(base.component_base.comp_base):
    '''
    high way class
    '''
    def __init__(self):
        '''
        Constructor
        '''
        base.component_base.comp_base.__init__(self, 'Highway')

    def _DoCreateTable(self):
        
        if self.CreateTable2('highway_facility_tbl') == -1:
            return -1
        if self.CreateTable2('highway_sa_pa_tbl') == -1:
            return -1
        self.CreateTable2('mid_all_highway_node')
        return 0
    
            
    def _DoCreateIndex(self):
        
        if self.CreateIndex2('highway_facility_tbl_facility_id_idx') == -1:
            return -1
        
        if self.CreateIndex2('highway_facility_tbl_node_idx') == -1:
            return -1
        
        if self.CreateIndex2('highway_sa_pa_tbl_sapa_id_idx') == -1:
            return -1
        
        
        if self.CreateIndex2('highway_sa_pa_tbl_node_id_idx') == -1:
            return -1
         
        return 0
        
    def _DoCreateFunction(self):
        if self.CreateFunction2('mid_get_toll_type') == -1:
            return -1
        if self.CreateFunction2('mid_get_ic_tpye') == -1:
            return -1 
        #找到ic对应的highway主线上的点    
        if self.CreateFunction2('mid_research_highway_ic_node') == -1:
            return -1   
        if self.CreateFunction2('mid_get_highway_ic_node') == -1:
            return -1  
        #判断icnode是否在highway主线上
        if self.CreateFunction2('mid_judge_highway_link') == -1:
            return -1  
        # 通过SA/JCT link判断SA/JCT出入口种别
        self.CreateFunction2('mid_get_hwy_in_out_type') 
        return 0
        
    def _Do(self):
        # 名称字典
        self._MakeDictionaryTemp()
        # Higway IC 转换点
        self.highway_facility_tbl_sql()
        # JCT 转换点
        self._MakeJCT_IC()
        # 点JCT 转换点
        self._MakeNodeJCT_IC()
        # 做成SAPA的IC点，及SAPA信息
        self._MakeSAPA()
        # 收费站
        self._MakeTollgate()
        return 0
    
    def highway_facility_tbl_sql(self):
        ''' 把highway ic做成主线上的ic'''
        
        self.CreateIndex2('org_interchange_ic_id_idx')
        self.CreateIndex2('org_interchange_ic_type_idx')
        self.CreateIndex2('org_interchange_mesh_node_idx')
        self.CreateIndex2('temp_node_mapping_new_node_idx')
        self.CreateIndex2('temp_node_mapping_node_idx')

        '''create two table to save path and icnode'''
        self.CreateTable2('mid_mainline_ic_node_info')
        self.CreateTable2('temp_from_ic_to_highwaymainline_path')
        
        self.MakeIcInfo()
        
        sqlcmd = """
                Insert into highway_facility_tbl(facility_id, node_id, name_id, 
                        separation, junction, pa, sa, ic, ramp, smart_ic, tollgate, 
                        dummy_tollgate, tollgate_type, service_info_flag, in_out_type)
                (
                    select icArray[1], mainline_node, new_name_id, 2, 0, 0, 0, 1, 0, 0, 0, 
                                       0, 0, 0, ic_type
                      from(
                        SELECT  array_agg(a.ic_id) as icArray,  c.new_name_id,
                                a.ic_node, a.mainline_node, a.ic_type
                          FROM mid_mainline_ic_node_info as a
                          left join org_interchange as b
                          on a.ic_id = b.ic
                          left join temp_interchange_name as c
                          on b.gid=c.gid
                          group by c.new_name_id, a.ic_node, a.mainline_node, a.ic_type
                      ) as d  
                );
                """  
        self.log.info('Now it is inserting to highway_facility_tbl...')
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()
            self.log.info('Inserting highway_facility_tbl succeeded')
    
    def MakeIcInfo(self):
        ''' 做成临时表，这个临时表为中间表服务'''
        sqlcmd = """select mid_get_highway_ic_node() """  
        self.log.info('Now it is inserting to mid_mainline_ic_node_info and temp_from_ic_to_highwaymainline_path...')
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()
            self.log.info('Inserting mid_mainline_ic_node_info and temp_from_ic_to_highwaymainline_path succeeded')
        return 0
    def _MakeTollgate(self):
        '''做成收费站。'''
        self.log.info('Start Make Tollgate Information.')
        # 由于没有原始表RoadNodeTollgate，数据收费站先从node_tbl取得。
        # 缺点是收费站的种别没有， 默认[1]
        
        sqlcmd = """
            insert into highway_facility_tbl(facility_id, node_id, name_id, separation, junction, pa, 
                        sa, ic, ramp, smart_ic, tollgate, dummy_tollgate, tollgate_type, 
                        service_info_flag, in_out_type)
            (
            SELECT gid, node_id, name_id, 2 as separation, 0 as jct, 0 as pa, 
                   0 as sa, 0 as ic, 0 as ramp, 0 as smart_ic, toll_flag, 0 as dummy_tollgate, 
                   1 as tollgate_type, 0 as service_info_flag, 0 as in_out_type
              FROM node_tbl
              where toll_flag = 1
            );
            """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        self.log.info('End Make Tollgate Information.')
        return 0
          
    def _MakeJCT_IC(self):
        '''做成JCT转换点。'''
        # 名称是[XXX收费站]的不做
        
        sqlcmd = """
insert into highway_facility_tbl(facility_id, node_id, name_id, separation, junction, pa, 
            sa, ic, ramp, smart_ic, tollgate, dummy_tollgate, tollgate_type, 
            service_info_flag, in_out_type)
(
SELECT a.ic, c.new_node, new_name_id, 2 as separation, 1 as juction, 
       0 as pa, 0 as sa, 0 as ic, 0 as ramp, 0 as smart_ic, 
       0 as tollgate, 0 as dummy_tollgate, 0 as tollgate_type, 
       1 as service_info_flag, 
       mid_get_hwy_in_out_type(c.new_node, 1::smallint, 0::smallint) as in_out_type
  from org_interchange as a
  left join mesh_mapping_tbl as b 
  on a.mesh = b.meshid_str 
  left join temp_node_mapping as c
  on globl_mesh_id = c.meshid and a.node = c.node
  left join temp_interchange_name as d
  on a.gid=d.gid 
  where  a.ic_type = 4 and name_chn not like '%收费站'
);
"""
        self.log.info('Start Make JCT InterChange...')
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()
            self.log.info('End Make JCT InterChange.')   
    
    def _MakeNodeJCT_IC(self):
        '''做成点JCT转换点。'''
        # 注：1. 点JCT的类型都做成:[out]型; 2. 名称是[XXX收费站]的不做
        
        sqlcmd = """
insert into highway_facility_tbl(facility_id, node_id, name_id, separation, junction, pa, 
            sa, ic, ramp, smart_ic, tollgate, dummy_tollgate, tollgate_type, 
            service_info_flag, in_out_type)
(
SELECT a.ic, c.new_node, new_name_id, 2 as separation, 1 as juction, 
       0 as pa, 0 as sa, 0 as ic, 0 as ramp, 0 as smart_ic, 
       0 as tollgate, 0 as dummy_tollgate, 0 as tollgate_type, 
       1 as service_info_flag, 
       1 as out_type
  from org_interchange as a
  left join mesh_mapping_tbl as b 
  on a.mesh = b.meshid_str 
  left join temp_node_mapping as c
  on globl_mesh_id = c.meshid and a.node = c.node
  left join temp_interchange_name as d
  on a.gid=d.gid 
  where  a.ic_type = 32 and name_chn not like '%收费站'
);
"""
        self.log.info('Start Make JCT InterChange...')
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()
            self.log.info('End Make JCT InterChange.')   
            
    def _MakeSAPA(self):
        '''做成SAPA的IC点，及SAPA信息.'''
        self.log.info('Start Make SAPA.')
        
        self.CreateIndex2('org_sapa_sapa_idx')
        self.CreateIndex2('org_interchange_ic_idx')
        
        ###############################################
        ## 第一步、做SAPA进入node和ic node相同,  ##
        ###############################################
        self._MakeSAPA_IC_SAME()
        
        ###############################################
        ## 第二步、做SAPA进入node和ic node不相同, ##
        ###############################################
        self._MakeSAPA_IC_NOT_SAME()
        
        ###############################################
        ## 第三步、连接名称表把结果插入highway_facility_tbl##
        ###############################################
        self._InsertIntoSAPA_IC()
        
        ###############################################
        ## 第四步、把服务区信息插入highway_sa_pa_tbl##
        ###############################################
        self._InsertIntoSAPA()
        
        ###############################################
        ## 第五步、把SAPA出入口并列IC, JCT找出，并做成。##
        ## 注：第二步的SAPA，很多是IC, JCT的并列设施。   ##
        ## 所以，IC和JCT要加上这些点。  ##
        ###############################################
        # 这个技能，暂时还是实现。
        # ... ...
        self.log.info('End Make SAPA.')
        return 0
    
    def _MakeSAPA_IC_SAME(self):
        """做SAPA进入node和ic node相同"""
        #######################################
        ## 第一步、做SAPA进入node和ic node相同,  ##
        ########################################
        self.log.info('Start Make SAPA, which Enter Node is same as IC Node.')
        # 临时存放IC和SAPA的关系表(点位置相同)
        self.CreateTable2('mid_temp_sapa_ic_same')    
        # 方法：
        # 1. 用InterChange中SAPA对应的node点，和SAPA表中的node_entr进行连接（sapa表有对应node点的，就是sapa出口）
        # 2. 再按ic_no, ic排序, 并去掉SAPA无对应点的组合;
        # 3. 最后按顺序检查，
        #    1）sapa表有对应node点的，就是sapa出口；
        #    2) 下个sapa入口前，或者是本组（同一个ic_no）的最后一个点，就是sapa入口，
        #    3）介于SAPA出入之间的点，就不要了。
        #    4）对孤点，直接收录。
        self.CreateFunction2('mid_make_sapa_same')
        self.CreateFunction2('mid_get_next_sapa_idx')
        
        self.pg.callproc('mid_make_sapa_same')
        return 0
    
    def _MakeSAPA_IC_NOT_SAME(self): 
        """做SAPA进入node和ic node不相同。"""
        #######################################
        ## 第二步、做SAPA进入node和ic node不相同,  ##
        ########################################
        self.log.info('Start Make SAPA, which Enter Node is not same as IC Node.')
        # 临时存放IC和SAPA的关系表(点位置不相同)
        self.CreateTable2('mid_temp_sapa_ic_not_same_all')
        # 方法:
        # 1. 从IC点出发，探索路径到SAPA所在点or入口点；
        # 2. 如果探索不到，用距离搜索，从IC点出发，周围2公里；
        # 3. 以上方面都找不到SAPA的，不做成(如: IC = 10493)。
        
        # 通过探索路径找查SAPA
        self.CreateFunction2('mid_find_sapa_by_path')
        # 搜索周边SAPA, 距离大约 1700米
        self.CreateFunction2('mid_find_sapa_by_distance')
        # 通过node IC (SAPA所在node or SAPA入口node)，取得SAPA番号
        self.CreateFunction2('mid_get_sapa_user_id')
        
        # 将第二步，找到的IC和SAPA对应关系，插入表mid_temp_sapa_ic_not_same_all
        # 注：这表里包含了无SAPA的ic, 其他SAPA值为[0] (如: IC = 10493).
        sqlcmd = """
            insert into mid_temp_sapa_ic_not_same_all(ic_no, ic, in_out_type, sapa)
            (
                select ic_no, ic, sapa_path[2] as in_out_type,
                    (case when sapa_path[1] is not null then sapa_path[1] 
                         else mid_find_sapa_by_distance(ic, 1700) end 
                    ) as sapa
                  from (
                    SELECT ic_no, ic_array[seq_num] as ic, mid_find_sapa_by_path(ic_array[seq_num]) as sapa_path
                      from (
                        select ic_no, ic_array, generate_series(1,array_upper(ic_array,1)) as seq_num
                        FROM (
                        SELECT ic_no, array_agg(ic)::integer[] as ic_array, 
                               array_agg(sapa_out_flag) AS sapa_out_flag_array
                          from (
                            SELECT a.ic_no, ic, ic_type, a.mesh, a.node, 
                                   (sapa is null)::integer as sapa_out_flag
                              FROM org_interchange as a
                              left join org_sapa as b
                              on a.mesh = b.mesh and a.node = b.node_entr
                              where ic_type = 2 
                              order by ic_no, ic
                          ) as b
                          group by ic_no
                        ) AS c
                        WHERE 0 <> all (sapa_out_flag_array) 
                      ) as c
                ) as d
                order by ic_no, ic
            );
        """ 
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        # 1. 去掉无SAPA的
        # 2. 去掉SAPA第一个入口和最后一个出口之间的SAPA出入口
        self.CreateTable2('mid_temp_sapa_ic_not_same')
        self.CreateFunction2('mid_make_sapa_not_same')
        self.pg.callproc('mid_make_sapa_not_same')
        
        return 0
    
    def _InsertIntoSAPA_IC(self):
        """连接名称表把结果插入highway_facility_tbl"""
        ###############################################
        ## 第三步、连接名称表把结果插入highway_facility_tbl##
        ## 注: SAPA都用org_sapaname的名称, 不用org_interchage的名称
        ###############################################
        self.log.info('Insert SAPA_IC Information.')
        
        # 1. SAPA进入node和ic node相同
        sqlcmd = """
insert into highway_facility_tbl(facility_id, node_id, name_id, separation, junction, pa, 
            sa, ic, ramp, smart_ic, tollgate, dummy_tollgate, tollgate_type, 
            service_info_flag, in_out_type)
(
SELECT a.ic, d.new_node, new_name_id, 2 as separation, 0 as juction, 
       (case when type_flag = 2 then 1 else 0 end) as pa,
       (case when type_flag in (0, 1) then 1 else 0 end) as sa, 
       0 as ic, 0 as ramp, 0 as smart_ic, 
       0 as tollgate, 0 as dummy_tollgate, 0 as tollgate_type, 
       1 as service_info_flag, in_out_type
  FROM mid_temp_sapa_ic_same as a
  left join org_interchange as b
  on a.ic = b.ic
  left join mesh_mapping_tbl as c
  on b.mesh = c.meshid_str 
  left join temp_node_mapping as d
  on globl_mesh_id = d.meshid and b.node = d.node

  left join org_sapa as e
  on a.sapa = e.sapa
  left join temp_sapa_name as f
  on e.name = f.name
  order by a.ic, a.sapa 
);
"""
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        # 2. SAPA进入node和ic node不相同
        sqlcmd = """
insert into highway_facility_tbl(facility_id, node_id, name_id, separation, junction, pa, 
            sa, ic, ramp, smart_ic, tollgate, dummy_tollgate, tollgate_type, 
            service_info_flag, in_out_type)
(
SELECT a.ic, d.new_node, new_name_id, 2 as separation, 0 as juction, 
       (case when type_flag = 2 then 1 else 0 end) as pa,
       (case when type_flag in (0, 1) then 1 else 0 end) as sa, 
       0 as ic, 0 as ramp, 0 as smart_ic, 
       0 as tollgate, 0 as dummy_tollgate, 0 as tollgate_type, 
       1 as service_info_flag, in_out_type
  FROM mid_temp_sapa_ic_not_same as a
  
  left join org_interchange as b
  on a.ic = b.ic
  left join mesh_mapping_tbl as c
  on b.mesh = c.meshid_str 
  left join temp_node_mapping as d
  on globl_mesh_id = d.meshid and b.node = d.node

  left join org_sapa as e
  on a.sapa = e.sapa
  left join temp_sapa_name as f
  on e.name = f.name 
  order by a.ic, a.sapa 
);
"""
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        return 0
    
    def _InsertIntoSAPA(self):
        """把服务区信息插入highway_sa_pa_tbl"""
        self.log.info('Insert SAPA Information.')
        # 注：这里做成的sapa_id很可能不唯一，主要是因为一个SAPA，对应多个有node_entr
        sqlcmd = """
INSERT INTO highway_sa_pa_tbl(
            sapa_id, node_id, name_id, public_telephone, vending_machine, 
            handicapped_telephone, toilet, handicapped_toilet, gas_station, 
            natural_gas, nap_rest_area, rest_area, nursing_room, dinning, 
            repair, shop, launderette, fax_service, coffee, post, 
            hwy_infor_terminal, hwy_tot_springs, shower, image_id)
(
SELECT  a.sapa, d.new_node, new_name_id, phonebooth, automat,
        0 as handicapped_telephone, wc, physical as handicapped_toilet, gasstation, 
        naturalgas, resting, resting, 0 as nursing_room, dining, 
        repair, shop, 0 as launderette, 0 as fax_service, coffee, 0 as post,
        0 as hwy_infor_terminal, 0 as hwy_tot_springs, shower, -1 as image_id
  FROM (
    select distinct ic, sapa
      from (
        select *
          from mid_temp_sapa_ic_same
          where in_out_type = 1
      union 
        select *
          from mid_temp_sapa_ic_not_same
          where in_out_type = 1
      ) as sa
    ) as a
  
  left join org_interchange as b
  on a.ic = b.ic
  left join mesh_mapping_tbl as c
  on b.mesh = c.meshid_str 
  left join temp_node_mapping as d
  on globl_mesh_id = d.meshid and b.node = d.node
  
  left join org_sapa as e
  on a.sapa = e.sapa
  left join temp_sapa_name as f
  on e.name = f.name 
  order by a.ic, a.sapa
);       
"""
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        return 0

    def _MakeDictionaryTemp(self):
        '''临时'''
        import dictionary_axf
        dict_axf = dictionary_axf.comp_dictionary_axf()
        # IC名称
        dict_axf.MakeHighWayInterChangeName()
        # SAPA名称
        dict_axf.MakeHighWaySAPAName()
        return 0
