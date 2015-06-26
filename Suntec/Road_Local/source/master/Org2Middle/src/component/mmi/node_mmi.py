# -*- coding: cp936 -*-
'''
Created on 2014-3-12

@author: zhangpeng
'''

import component.component_base

class comp_node_mmi(component.component_base.comp_base):
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
        return 0
    
    def _DoCreateIndex(self):
        'create index.'
        
        self.CreateIndex2('node_tbl_node_id_idx')
        self.CreateIndex2('node_tbl_the_geom_idx')
        return 0
    
    def _DoCreateFunction(self):
        return 0
    
    def _Do(self):

        self.CreateIndex2('org_city_jc_point_id_feattyp_jncttyp_idx')
        self.CreateIndex2('org_toll_booth_point_fm_edge_to_edge_idx')
        self.CreateIndex2('org_bifurcation_junctionid_idx')
           
        sqlcmd = """
                insert into node_tbl (node_id, kind, light_flag, toll_flag, bifurcation_flag, 
                    mainnodeid, node_lid, node_name, the_geom )
                select
                     jc.id, 
                     CASE when jncttyp = 3 then '1301' else null END as kind,
                     null    as light_flag,
                     tl.flag as toll_flag, 
                     case when bi.junctionid is not null then 1
                         else 0
                     end as bifurcation_flag,
                     0       as mainnodeid, 
                     nl.node_lid, 
                     null    as node_name, 
                     ST_GeometryN(jc.the_geom,1) as the_geom
                from (
                         select distinct id, feattyp, jncttyp, the_geom
                           from org_city_jc_point
                )  as jc
                join (
                      select nid, array_to_string( array_agg(lid), '|' ) as node_lid 
                        from (
                                select f_jnctid as nid, id as lid from org_city_nw_gc_polyline
                                union 
                                select t_jnctid as nid, id as lid from org_city_nw_gc_polyline
                              ) as n
                       group by nid
               ) as nl
               on jc.id = nl.nid
               left join (
                     select distinct CASE
                                        WHEN f.f_jnctid in ( t.f_jnctid, t.t_jnctid ) then f.f_jnctid
                                        WHEN f.t_jnctid in ( t.f_jnctid, t.t_jnctid ) then f.t_jnctid
                                        else null
                                      END as id, 1 as flag
                       from org_toll_booth_point     as n
                       join org_city_nw_gc_polyline  as f
                         on n.fm_edge = f.id
                       join org_city_nw_gc_polyline  as t
                         on n.to_edge = t.id
                ) as tl
                on jc.id = tl.id                
                left join (
                    select distinct junctionid from org_bifurcation
                ) as bi
                on jc.id = bi.junctionid 
        """
        self.log.info( 'Now it is inserting node_tbl...')
        self.pg.do_big_insert2(sqlcmd)
        self.log.info( 'Inserting node_tbl succeeded')    
        return 0
    
            