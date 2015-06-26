# -*- coding: cp936 -*-
'''
Created on 2012-3-26

@author: hongchenzai
'''
import component.component_base

class comp_split_link_jdb(component.component_base.comp_base):
    
    '''按道路结构(隧道、踏切)重新分割Link.
    '''

    def __init__(self):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self, 'Split_Link')
        
    def _DoCreateTable(self):
        self.CreateTable2('temp_split_link')
        self.CreateTable2('temp_new_link')
        self.CreateTable2('temp_road_node')
        
    def _DoCreateFunction(self):
        self.CreateFunction2('rdb_split_a_link_by_attr') 
        self.CreateFunction2('rdb_line_reverse')
        self.CreateFunction2('rdb_min')
        self.CreateFunction2('rdb_max') 
        self.CreateFunction2('rdb_line_substring')
        self.CreateFunction2('mid_get_fraction')
        self.CreateFunction2('rdb_split_link_by_attr')
        self.CreateFunction2('rdb_make_new_link_by_attr')
        self.CreateFunction2('rdb_make_new_node_link')

    def _Do(self):
        self.__updateAttrValue()
        self.__SplitLinkByAttr()          
        self.__CopyOldNode()  
        self.__CreateTempRoadLink()
        self.__CopyOldLink()
        self.__AddNewNodeAndLink()            
        return 0
    
    def __updateAttrValue(self):
        
        # Since 14Autumn, code of tunnel & railway crossing have been refined.
        # tunnel: 2 -> 2,100,101  ;  railway crossing: 4 -> 4,102
        # Now, updating those new codes to old codes.
        sqlcmd = """
                update segment_attr_4326 set linkattr_c = 2 where linkattr_c in (100, 101);
                update segment_attr_4326 set linkattr_c = 4 where linkattr_c in (102);
                """        
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
    def __SplitLinkByAttr(self):
        #wb_log.log(self.ItemName, 'Start Split Link.', 'info')
        if self.pg.callproc('rdb_split_link_by_attr') != -1:
            self.pg.commit2()
        else:
            return -1
    
        
        if self.pg.callproc('rdb_make_new_link_by_attr') != -1:
            self.pg.commit2()
        else:
            return -1
        
        #wb_log.log(self.ItemName, 'End Split Link.', 'info')
        return 0
    
    def __CopyOldNode(self):
        #wb_log.log(self.ItemName, 'Start Copy Old Node.', 'info')
        sqlcmd = """
                insert into temp_road_node(objectid, signal_f, name_kanji, name_yomi, the_geom)
                (
                SELECT distinct(objectid), signal_f, name_kanji, name_yomi, the_geom
                FROM road_node_4326
                );
                """
        self.pg.connect2()
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()
            
        #wb_log.log(self.ItemName, 'End Copy Old Node.', 'info')
        return 0
    
    def __CreateTempRoadLink(self):
        #wb_log.log(self.ItemName, 'Start create Temp Road Linke.', 'info')
        self.pg.connect2()      
        if self.CreateTable2('temp_road_link') == -1:
            self.pg.close2()
            return -1
        
        self.pg.close2()
        #wb_log.log(self.ItemName, 'End create Temp Road Linke.', 'info')
        return 0
    
    def __CopyOldLink(self):
        ''' copy the old links, except the split links. '''
        
        #wb_log.log(self.ItemName, 'Start Copy Old Link.', 'info')
        
        ##################################################### 
        # 拷贝没有属性的link
        ##################################################### 
        sqlcmd = """
                INSERT INTO temp_road_link(
                            objectid, from_node_id, to_node_id, roadclass_c, naviclass_c, 
                            linkclass_c, roadmanager_c, roadwidth_c, widearea_f, smartic_c, 
                            bypass_f, caronly_f, island_f, road_no, road_code, nopassage_c, 
                            oneway_c, lane_count, the_geom)
                (
                SELECT  objectid, from_node_id, to_node_id, roadclass_c, naviclass_c, 
                        linkclass_c, roadmanager_c, roadwidth_c, widearea_f, smartic_c, 
                        bypass_f, caronly_f, island_f, road_no, road_code, nopassage_c, 
                        oneway_c, lane_count, ST_linemerge(the_geom)
                  FROM road_link_4326
                  where objectid not in (
                                         -- the link, have attribute. 
                                         select linkid
                                         from temp_new_link
                                        )
                );     
                """
        self.pg.connect2() 
        
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()
            
        #####################################################    
        # 拷贝link,有属性，但不用分割
        #####################################################
        sqlcmd = """
                INSERT INTO temp_road_link(
                            objectid, from_node_id, to_node_id, roadclass_c, naviclass_c, 
                            linkclass_c, roadmanager_c, roadwidth_c, widearea_f, smartic_c, 
                            bypass_f, caronly_f, island_f, road_no, road_code, nopassage_c, 
                            oneway_c, lane_count, linkattr_c, the_geom)
                (
                SELECT  objectid, from_node_id, to_node_id, roadclass_c, naviclass_c, 
                        linkclass_c, roadmanager_c, roadwidth_c, widearea_f, smartic_c, 
                        bypass_f, caronly_f, island_f, road_no, road_code, nopassage_c, 
                        oneway_c, lane_count, linkattr_c,
                        ST_linemerge(C.the_geom)
                  FROM (
                    SELECT linkid, linkattr_c
                      FROM temp_new_link
                      where linkid in (
                                 -- the link, have attribute and are not split. 
                                 select linkid
                                 from (
                                       select linkid, count(linkid) as cnt
                                    from temp_new_link
                                    group by linkid
                                 ) as A
                                 where cnt = 1
                                   ) 
                  ) AS B
                  LEFT JOIN road_link_4326 as C
                  ON B.linkid = C.objectid
                );
                """
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()
            
        #wb_log.log(self.ItemName, 'End Copy Old Link.', 'info')
        return 0
    
    def __AddNewNodeAndLink(self):
        #wb_log.log(self.ItemName, 'Start Add New Node And Link.', 'info')
        
        if self.pg.callproc('rdb_make_new_node_link') == -1:
            return -1
        else:
            self.pg.commit2()
        
        return 0
    
    def _DoCreateIndex(self):
        # create index
        # node
        self.CreateIndex2('temp_road_node_objectid_idx')
        # link
        self.CreateIndex2('temp_road_link_objectid_idx')
        self.CreateIndex2('temp_road_link_objectid_split_seq_num_idx')
        self.CreateIndex2('temp_road_link_from_node_id_idx')
        self.CreateIndex2('temp_road_link_to_node_id_idx')
        
    def _DoCheckLogic(self):
        self.log.info('Check split_Link_by_attr...')
        
        self.CreateFunction2('mid_check_split_Link_by_attr')
        self.pg.callproc('mid_check_split_Link_by_attr')
        
        self.log.info('Check split_Link_by_attr end.')
        return 0
    