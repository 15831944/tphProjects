# -*- coding: UTF8 -*-
'''
Created on 2013-7

@author: zym
'''



import common
import component.component_base
import component.default.guideinfo_caution



class comp_guideinfo_caution_highaccident_rdf(component.default.guideinfo_caution.comp_guideinfo_caution):
    '''
    classdocs
    '''
    
    def __init__(self):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self, 'Guideinfo_Caution_highaccident')
    
    def _DoCreateTable(self):
        '创建表.'  
        
        return 0
    
    def _make_caution_from_origin(self):

        accident_flag = common.common_func.GetPath('accident_flag')

        if accident_flag == 'true':  
            # 危险案内          
            insert_sqlcmd = '''
                INSERT INTO caution_tbl(
                    inlinkid, nodeid, data_kind
                )
                VALUES (%s, %s, %s);
            '''
                 
            self._driver_alert_data(insert_sqlcmd)        
            self._high_accident_point_data(insert_sqlcmd)

            # 事故多发区，因数据保存在safety zone中，故不可将此处safety zone作成移动到guideinfo_safety zone中
            insert_sqlcmd = '''
                INSERT INTO safety_zone_tbl(
                    safetyzone_id, linkid, direction, safety_type
                )
                VALUES (%s, %s, %s, %s);
            ''' 
            
            self._high_accident_zone_data(insert_sqlcmd)
                        
        return 0
    
    def _driver_alert_data(self,insert_sqlcmd):
        
        self.log.info('Now it is updating temp_org_caution_link_tbl by driver alert data...')
        
        # gen_warning_sign_type---General Warning Sign Type = 4 – Accident Hazard is published only if \
        # the accident hazard sign exists as supplemental sign in reality
        # gen_warning_sign_type对应condition_type=17(traffic sign), it is applied to one link as origin \
        # and one connected node as the destination. 故关联link/node数都是1且不为空
        
        sqlcmd = '''
            SELECT nav_strand_id,array_agg(link_id),array_agg(seq_num),array_agg(node_id)
            from(
                select c.nav_strand_id,c.link_id,c.seq_num,c.node_id
                FROM (
                    select *
                    from rdf_condition
                    where condition_id in (
                        SELECT condition_id
                        FROM rdf_condition_driver_alert
                        where gen_warning_sign_type = 4
                    )
                ) as b
                left join rdf_nav_strand as c
                    on b.nav_strand_id = c.nav_strand_id
                order by c.nav_strand_id,c.seq_num
            ) as a
            group by nav_strand_id;
        '''
        self.pg.execute2(sqlcmd)
        rows = self.pg.fetchall2()
        for row in rows:
            linkids = row[1]
            nodeids = row[3]
            if ((len(linkids) != 1) or (len(linkids) == 1 and linkids[0] is None)) or \
                ((len(nodeids) != 1) or (len(nodeids) == 1 and nodeids[0] is None)):
                self.log.error('org data(driver alert) is disconsistent with logic')
                continue
            self.pg.execute2(insert_sqlcmd, (linkids[0], nodeids[0], 2))
            
        self.pg.commit2()
        
        self.log.info('updating temp_org_caution_link_tbl by driver alert data succeeded')
        
        return 0

    def _high_accident_point_data(self,insert_sqlcmd):
        
        self.log.info('Now it is updating temp_org_caution_link_tbl by high accident point data...')
        
        # Blackspot(condition_type=38) identifies intersections, points or stretches along a \
        # road with an unusual high number of accidents.
        # A Blackspot condition can be a Link – Node or a Link only condition. we only consider Link – Node \
        # (Involves one link as origin and one connected Node as the destination) below \
        # a link only condition 被作成事故多发区（safety zone）
        
        sqlcmd = '''
            SELECT nav_strand_id,array_agg(link_id),
                array_agg(seq_num),array_agg(direction),array_agg(node_id)
            from(
                select c.nav_strand_id,c.link_id,c.seq_num,a.direction,c.node_id
                FROM (
                    select *
                    from rdf_condition
                    where condition_type=38
                ) as b
                left join rdf_condition_blackspot as a
                    on b.condition_id = a.condition_id
                left join rdf_nav_strand as c
                    on b.nav_strand_id = c.nav_strand_id
                where c.node_id is not null
                order by c.nav_strand_id,c.seq_num
            ) as m
            group by nav_strand_id;
        '''
        self.pg.execute2(sqlcmd)
        
        # Link - Node: link/node数为1且不为空
        rows = self.pg.fetchall2()
        for row in rows:
            links = row[1]
            seq_nums = row[2]
            directions = row[3]
            node_ids = row[4]
            #if len(links) >1 or seq_nums[0] <> 0:
            if (len(links) != 1 or (len(links)==1 and links[0] is None)) or \
                (len(node_ids) != 1 or (len(node_ids) == 1 and node_ids[0] is None)):
                self.log.error('org data is disconsistent with logic')
                continue
            # Link-----Node
            link = links[0]
            node = node_ids[0]
            self.pg.execute2(insert_sqlcmd, (link, node, 2))
            
        self.pg.commit2()
        
        self.log.info('updating temp_org_caution_link_tbl by high accident point data succeeded')
        
        return 0
        
    def _high_accident_zone_data(self,insert_sqlcmd):
        
        self.log.info('Now it is updating safety_zone_tbl by high accident zone data...')
        
        # Create SafetyZone ID.
        self.CreateTable2('temp_highaccid_links')
        self.CreateIndex2('temp_highaccid_links_link_id_s_node_e_node_idx')
        self.CreateFunction2('mid_find_highaccid_links')
        self.CreateFunction2('mid_find_highaccid_links_in_one_direction')
        
        sqlcmd = '''
            drop table if exists temp_highaccid_links_path;
            CREATE TABLE temp_highaccid_links_path
            (
              link_id bigint,
              group_id int
            );
            
            drop table if exists temp_link_walked;
            CREATE TABLE temp_link_walked
            (
              link_id bigint
            );
            
            select mid_find_highaccid_links('temp_highaccid_links', 'temp_highaccid_links_path');        
        '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        # Insert into safety zone table.
        sqlcmd = '''    
            SELECT (a.group_id + (
                    select case when id is null then 0 else id end
                    from (
                        select max(safetyzone_id) as id
                        from safety_zone_tbl
                    ) max_id
                 )) as safetyzone_id
                ,a.link_id,b.dir 
            from temp_highaccid_links_path a
            left join temp_highaccid_links b
            on a.link_id = b.link_id;
        '''
        self.pg.execute2(sqlcmd)
        rows = self.pg.fetchall2()
        for row in rows:
            safetyzone_id = row[0]
            link = row[1]
            direction = row[2]               
            self.pg.execute2(insert_sqlcmd, (safetyzone_id,link,direction,2))
            
        self.pg.commit2()
        
        self.log.info('updating safety_zone_tbl by high accident zone data end.')
        
        return 0
    
    
    
    
    
    