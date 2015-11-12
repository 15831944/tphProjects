# -*- coding: cp936 -*-
'''
Created on 2012-3-23

@author: sunyifeng
'''

import common.database
import component.component_base

class comp_node_ta(component.component_base.comp_base):
    '''
    classdocs
    '''


    def __init__(self):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self, 'Node')
        
    def _DoCreateTable(self):
        self.CreateTable2('node_tbl')
        self.CreateTable2('temp_jc')
        self.CreateTable2('temp_node_lid')
#        self.CreateTable2('temp_node_lid_ref')
#        self.CreateTable2('temp_node_lid_nonref')
        
        return 0
    
    def _DoCreateIndex(self):
        'create index.'
        
        self.CreateIndex2('node_tbl_node_id_idx')
        self.CreateIndex2('node_tbl_the_geom_idx')

        return 0
    
    def _DoCreateFunction(self):
        
        self.CreateFunction2('mid_cnv_node_kind')

        return 0
    
    def _Do(self):
        
        self.CreateIndex2('nw_f_jnctid_idx')
        self.CreateIndex2('nw_t_jnctid_idx')

        sqlcmd = """
            insert into temp_jc(id, feattyp, jncttyp, elev, geom)
                select distinct id, feattyp, jncttyp, elev, the_geom 
                from org_jc where feattyp = 4120;
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        self.CreateIndex2('temp_jc_id_idx')
        
        # Create traffic light.
        self._MakeTrafficLight() 
                
        # 合成node相连的所有link
        self._MakeNodeLid() 
            
        sqlcmd = """
                insert into node_tbl (
                    node_id, kind, light_flag, toll_flag, stopsign_flag, bifurcation_flag, 
                    mainnodeid, node_lid, node_name, the_geom, feature_string, feature_key 
                    )
                select 
                    a.id, 
                    mid_cnv_node_kind(jncttyp) as kind,
                    case when c.node_id is not null then 1
                        else 0
                    end as light_flag,
                    null as toll_flag,
                    case when ss.node_id is not null then 1
                        else 0
                    end as stopsign_flag,                    
                    case when a.jncttyp = 2 then 1
                        else 0
                    end as bifurcation_flag,
                    0 as mainnodeid, 
                    b.lid as node_lid, 
                    null as node_name, 
                    geom,
                    b.lid::varchar as feature_string,
                    md5(b.lid::varchar) as feature_key
                from temp_jc as a
                left outer join temp_node_lid as b
                on a.id = b.node_id
                left join temp_node_trafficlight c
                on a.id = c.node_id
                left join (
                    select distinct node_id from stopsign_tbl
                ) as ss
                on a.id = ss.node_id
                ;
        """
        
        self.log.info(self.ItemName + ': Now it is inserting node_tbl...')
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
            
        self.log.info(self.ItemName + ': end of inserting node_tbl...')
                  
        return 0
    
    
    def _MakeNodeLid(self):
        "合成node相连的所有link"
        threads = []
        
        threads.append(node_child_thread(1))  # 起点
        threads.append(node_child_thread(2))  # 终点
        
        # 启动线程
        for t in threads:
            t.start()
           
        # 等待子线程结束
        for t in threads:
            t.join()
            
        # 把起点和终点的所有link合并
        sqlcmd = """
            insert into temp_node_lid(node_id, lid)
            (
                select a.id,  array_to_string(b.lid || c.lid, '|')
                  from temp_jc as a
                  LEFT JOIN temp_node_lid_ref as b
                  ON a.id = b.node_id
                  LEFT JOIN temp_node_lid_nonref as c
                  ON a.id = c.node_id
            );
            
            create index temp_node_lid_node_id_idx
                on temp_node_lid
                using btree
                (node_id);
            
            analyze temp_node_lid;
            """
        
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else :
            self.pg.commit2()

    def _MakeTrafficLight(self):
        
        # Fetch traffic light data from POI with feattyp = 7230.
        
        self.CreateIndex2('org_jc_geom_idx')
        self.CreateIndex2('org_jc_elev_idx')
        self.CreateIndex2('org_mn_pi_geom_idx')
        self.CreateIndex2('org_mn_pi_feattyp_idx')
        self.CreateIndex2('org_mnpoi_pi_geom_idx')
        self.CreateIndex2('org_mnpoi_pi_feattyp_idx')
                
        sqlcmd = """
            drop table if exists temp_node_trafficlight;
            create table temp_node_trafficlight as
            select a.id as poi_id,b.id as node_id 
            from (
                select id,the_geom from org_mn_pi where feattyp = 7230
                union
                select id,the_geom from org_mnpoi_pi where feattyp = 7230
            ) a
            left join org_jc b
            on ST_DWithin(b.the_geom, a.the_geom, 0.000000694945852358566745)
            where b.elev = 0 and b.the_geom is not null
        """
        
        self.pg.execute2(sqlcmd)        

#####################################################################################                 
# node的子线程类
#####################################################################################                 

import threading
import common  
class node_child_thread(threading.Thread):
    "多线程处理."
    def __init__(self, task_type):
        threading.Thread.__init__(self)
        self.task_type = task_type
        self.pg        = common.database.pg_client()
        
    def run(self):
        self.pg.connect2()
        
        if self.task_type == 1:
            # 起点
            print "Start _MakeNodeLidRef."
            self._MakeNodeLidRef()
            print "End _MakeNodeLidRef."
        else:
            # 终点
            print "Start _MakeNodeLidRef."
            self._MakeNodeLidNonRef()
            print "End _MakeNodeLidRef."
        self.pg.close2()
        return 0
    
    def _MakeNodeLidRef(self):
        # 起点
        self.pg.CreateTable2_ByName('temp_node_lid_ref')
        
        sqlcmd = """
            insert into temp_node_lid_ref(node_id, lid)
            (
                select f_jnctid, array_agg(id) 
                from
                (
                    select f_jnctid, id
                    from org_nw
                    where frc != -1
                    order by f_jnctid, id
                )as t
                group by f_jnctid
            );
        """
        
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else :
            self.pg.commit2()
            
    def _MakeNodeLidNonRef(self):
        # 终点
        self.pg.CreateTable2_ByName('temp_node_lid_nonref')
        
        sqlcmd = """
            insert into temp_node_lid_nonref(node_id, lid)
            (
                select t_jnctid, array_agg(id) 
                from
                (
                    select t_jnctid, id
                    from org_nw
                    where frc != -1
                    order by t_jnctid, id
                )as t
                group by t_jnctid
            );
        """
        
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else :
            self.pg.commit2()
        
    
