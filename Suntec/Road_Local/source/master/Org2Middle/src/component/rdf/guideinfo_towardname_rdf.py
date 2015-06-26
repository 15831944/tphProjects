# -*- coding: cp936 -*-
'''
Created on 2012-4-27

@author: zhangliang
'''
import component

class new_link_struct():
    __node_id = int()
    __connect_lid = int()
    __s_node = int()
    __e_node = int()
    __lane_dir = int()
    
class new_search_node_struct():
    __parent_nodeid = int()
    __current_nodeid = int()
    __current_linkid = int()
    __current_link_dir = int()
    __level = int()
    
class comp_guideinfo_towardname_rdf(component.guideinfo_towardname.comp_guideinfo_towardname):
    '''
    This class is used for uc towardname
    '''

    def __init__(self):
        '''
        Constructor
        '''
        component.guideinfo_towardname.comp_guideinfo_towardname.__init__(self)
    
    def _DoCreateTable(self):
        
        component.guideinfo_towardname.comp_guideinfo_towardname._DoCreateTable(self)
        self.CreateTable2('temp_node_tbl_expand')
        self.CreateIndex2('temp_node_tbl_expand_nodeid_idx')
        # 把计算得来的node_id和PassLink, 暂存在该表
        self.CreateTable2('temp_towardname_guide_middle_layer')
        return 0
    
    def _DoCreatIndex(self):
        
        return 0
        
    def _DoCreateFunction(self):
        'Create Function'
        self.CreateFunction2('mid_get_connected_nodeid_uc')
        
        return 0
    
    def _Do(self):
        
        # 处理InLink和OutLink不直接相连的方面名称
        self.__DealWithUnconnectTowardName()
        
        # 处理InLink和OutLink直接相连的方面名称
        self.__DealWithConnectTowardName()
        
        # create middle data table with toward name
        self.MakeTowardName()
    
        return 0  
    
    def MakeTowardName(self):
        '''把名称和link_id关联起来。'''
        # 注：1. 默认情况下，都把种别做“0”。
        # 2. 那些只有番号，没有名称的记录做被收录。
        # 3. 那些PassLink路径没有找到的，也不收录。
        sqlcmd = """
            INSERT INTO towardname_tbl(
                        id, nodeid, inlinkid, outlinkid, passlid, passlink_cnt, 
                        direction, guideattr, namekind, namekind2, name_id, "type")
            (
            SELECT A.sign_id, nodeid, inlinkid, outlinkid, array_to_string(passlink, '|'), 
                   passlink_cnt, 0 as direction, 0 as guideattr, 0 as namekind, 0 as namekind2,
                   name_id, 0 as type
              FROM temp_toward_name as A
              LEFT JOIN temp_towardname_guide_middle_layer as B
              on A.sign_id = B.sign_id and A.destination_number = B.destination_number
              where B.sign_id is not null
            );
            """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        return 0
         
    def __DealWithUnconnectTowardName(self):
        '''处理InLink和OutLink不直接相连的方面名称。'''
        
        # 取出InLink和OutLink不直接相连的方面名称
        self.__MakeUnconnectTowardGuide()
        
        # 对InLink和OutLink不直接相连的方面名称，进行探路
        self.__SearchLinkSeries3()
        
        # 把探索出来的node_id和pass_link，存到表temp_unconnect_guide_middle_layer
        self.__InsertUnconnectTowardMiddleLayer()
        
        return 0
        
    def __MakeUnconnectTowardGuide(self):
        '''取出InLink和OutLink不直接相连的方面名称。'''
        self.CreateTable2('temp_unconnect_toward_guide')
        # 只要两条link的node点，两两不相等，就是不直接相连
        sqlcmd = """
            INSERT INTO temp_unconnect_toward_guide
            (
            select sign_id, in_link_id, in_direct, in_start_node_id, in_end_node_id, 
                out_link_id, out_direct, out_start_node_id, out_end_node_id, 
                destination_number
              from (
                select a.sign_id, originating_link_id as in_link_id, c.travel_direction as in_direct,
                    c.ref_node_id as in_start_node_id, c.nonref_node_id as in_end_node_id,
                    dest_link_id as out_link_id, d.travel_direction as out_direct,
                    d.ref_node_id as out_start_node_id, d.nonref_node_id as out_end_node_id,
                    destination_number
                  from rdf_sign_origin as a
                  left join rdf_sign_destination as b
                  on a.sign_id = b.sign_id
                  left join temp_rdf_nav_link as c
                  on a.originating_link_id = c.link_id
                  left join temp_rdf_nav_link as d
                  on b.dest_link_id = d.link_id
              ) as e
              where (in_start_node_id <> out_start_node_id and in_start_node_id <> out_end_node_id) and 
                    (in_end_node_id <> out_start_node_id and in_end_node_id <> out_end_node_id)
            );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        return 0
    
    def __DealWithConnectTowardName(self):
        '''处理InLink和OutLink直接相连的方面名称。'''
        # 对于InLink和OutLink直接相连的，找出两条link相交的点。
        # passlink_cnt设为0， passlink设为空
        sqlcmd = """
            INSERT INTO  temp_towardname_guide_middle_layer(sign_id, inlinkid, nodeid,  
                   outlinkid, passlink_cnt, passlink, destination_number)  
            (
            select sign_id, in_link_id,
                mid_get_connected_nodeid_uc(in_start_node_id, in_end_node_id, out_start_node_id, out_end_node_id) as node_id,
                out_link_id,
                0 as passlink_cnt,
                null as passlink,
                destination_number
              from (
                select a.sign_id, originating_link_id as in_link_id, 
                    c.ref_node_id as in_start_node_id, c.nonref_node_id as in_end_node_id,
                    dest_link_id as out_link_id, 
                    d.ref_node_id as out_start_node_id, d.nonref_node_id as out_end_node_id,
                    destination_number
                  from rdf_sign_origin as a
                  left join rdf_sign_destination as b
                  on a.sign_id = b.sign_id
                  left join temp_rdf_nav_link as c
                  on a.originating_link_id = c.link_id
                  left join temp_rdf_nav_link as d
                  on b.dest_link_id = d.link_id
              ) as e
              where (in_start_node_id = out_start_node_id or in_start_node_id = out_end_node_id) or 
                    (in_end_node_id = out_start_node_id or in_end_node_id = out_end_node_id)
            );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        return 0
    
    def __SearchLinkSeries3(self):
        f = open('middel_toward_file.txt','w',True)
        
        sqlcmd = """
                SELECT sign_id, in_link_id, in_direct, in_start_node_id, in_end_node_id, 
                       out_link_id, out_direct, out_start_node_id, out_end_node_id, 
                       destination_number
                  FROM temp_unconnect_toward_guide;
                """
        self.pg.execute2(sqlcmd) 
        row = self.pg.fetchone2()
        sqlcmd = ''
                
        while row:
            global_level       = 0
            discover_node      = list()
            sign_id            = row[0]
            inlink_id          = row[1]
            inlink_dir         = row[2]
            inlink_startnode   = row[3]
            inlink_endnode     = row[4]
            outlink_id         = row[5]
            destination_number = row[9]
            if inlink_dir == 'F':
                discover_node.append(str(inlink_endnode))
            elif inlink_dir == 'T':
                discover_node.append(str(inlink_startnode))
            else :
                discover_node.append(str(inlink_endnode))
                discover_node.append(str(inlink_startnode))
                
            str_discover_node  = ','.join(discover_node)
            all_node_container = list()
            normal_passlink    = list()
            
            retvalue, level_cnt, last_expand_nodeid = self.__findpasslink2(str_discover_node, all_node_container, global_level, outlink_id)
            if retvalue == False :
                self.log.warning("Can't find the PassLink for this Sign(sign_id = %s)." % (sign_id))
                row = self.pg.fetchone2()
                continue
            else:
                pass
            iter_cnt = len(all_node_container) - 1
            while iter_cnt >= 0:
                item = all_node_container[iter_cnt]
                
                if (item.__current_nodeid == last_expand_nodeid) and (item.__level == level_cnt -1) :
                    last_expand_nodeid = item.__parent_nodeid
                    level_cnt = level_cnt - 1
                    normal_passlink.append(str(item.__current_linkid))
                
                iter_cnt = iter_cnt - 1
                        
            passlinkcnt = len(normal_passlink)
            
            normal_passlink.reverse()
            str_normal_passlink = ','.join(normal_passlink)
            self.WriteSql2File(f, sign_id, inlink_id, last_expand_nodeid, outlink_id, 
                               passlinkcnt, str_normal_passlink, destination_number)
                
            row = self.pg.fetchone2()
                        
        f.close()
        
        self.pg.close1()
          
        return 0
    
    def WriteSql2File(self, f_point, sign_id, inlink_id, search_node, outlink_id, passlinkcnt, passlinklids, destination_number):
        # make log and insert into record db
        sqlcmd = "insert into temp_towardname_guide_middle_layer values(%s,%s,%s,%s,%s,'{%s}',%s);\n" % \
                (sign_id, inlink_id, search_node, outlink_id, passlinkcnt, passlinklids, destination_number)                                                                                             
        f_point.write(sqlcmd)
        return 0
    
    def __InsertUnconnectTowardMiddleLayer(self):
        '''把探索出来的node_id和pass_link，存到表temp_towardname_guide_middle_layer'''

        fp = open('middel_toward_file.txt','r')
        for line_cmd in fp.readlines():
            self.pg.execute2(line_cmd)
        self.pg.commit2()
        fp.close()
        return 0
        
    def __findpasslink2(self, str_discover_node, prepare_node_list, global_level, out_link_id):              
        if global_level == 15 :
            return False, 0, 0
            
        global_level = global_level + 1
        str_discover_node_new = list()
        
        self.pg.connect1()
        sqlcmd = """select node_id, connect_lid, s_node,e_node,lane_dir from temp_node_tbl_expand where node_id in (%s)""" %(str_discover_node)
        self.pg.execute1(sqlcmd) 
        row2 = self.pg.fetchone()
        while row2:
            #judgment whether the node could be input to prepare_node_list
            db_node_id     = row2[0]
            db_connect_lid = row2[1]
            db_s_node      = row2[2]
            db_e_node      = row2[3]
            db_dir         = row2[4]
            
            if db_connect_lid == out_link_id :
                return True, global_level, db_node_id
            
            if db_s_node == db_node_id and (db_dir == 1 or db_dir == 3) :
                str_discover_node_new.append(str(db_e_node))
                self.input_new_node(prepare_node_list,db_node_id, db_e_node,db_connect_lid,db_dir,global_level)
            elif db_e_node == db_node_id and (db_dir == 2 or db_dir == 3):
                str_discover_node_new.append(str(db_s_node))
                self.input_new_node(prepare_node_list,db_node_id, db_s_node,db_connect_lid,db_dir,global_level)
            else :
                row2 = self.pg.fetchone()
                continue
            
            row2 = self.pg.fetchone()
            
        str_discover_node = ','.join(str_discover_node_new)
        
        return self.__findpasslink2(str_discover_node, prepare_node_list, global_level, out_link_id)

    
    def input_new_node(self,prepare_node_list, pre_search_node, db_node, db_link, db_dir, db_level):
        new_node = new_search_node_struct()
        new_node.__parent_nodeid = pre_search_node
        new_node.__current_nodeid = db_node
        new_node.__current_linkid = db_link
        new_node.__current_link_dir = db_dir
        new_node.__level = db_level
        prepare_node_list.append(new_node)
                    
                 
        
        
    
