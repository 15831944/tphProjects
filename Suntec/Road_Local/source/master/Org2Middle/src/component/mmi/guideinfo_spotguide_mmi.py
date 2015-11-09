# -*- coding: cp936 -*-
'''
Created on 2012-4-27

@author: zhangliang
'''

import common
from component.default import link_graph
from component.default.guideinfo_spotguide import comp_guideinfo_spotguide

roadType2SpotguideTypeMap = {0:1, 1:1, 2:4,
                             3:4, 4:4, 5:4,
                             6:4, 7:4, 8:4,
                             9:4, 10:4, 11:4,
                             12:4, 13:4, 14:4,
                             15:4}
class comp_guideinfo_spotguide_mmi(comp_guideinfo_spotguide):
    '''
    This class is used for uc spotguide
    '''

    def __init__(self):
        '''
        Constructor
        '''
        comp_guideinfo_spotguide.__init__(self)

    def _DoCreateTable(self):

        comp_guideinfo_spotguide._DoCreateTable(self)
        # 这个表的记录样式：oneinlink--->oneoutlink
        if self.CreateTable2('temp_junction_tbl') == -1:
            return -1
        
        return 0

    def _DoCreatIndex(self):

        return 0

    def _DoCreateFunction(self):
        'Create Function'
        return 0

    def _Do(self):
        self._generate_temp_info_of_junction_links()
        self._generate_spotguide_tbl(common.common_func.GetPath('care_about_sar'))
        tollIllustName = common.common_func.GetPath('toll_station_illust')
        comp_guideinfo_spotguide._GenerateSpotguideTblForTollStation(self, tollIllustName)
        

    def _generate_temp_info_of_junction_links(self):
        sqlcmd = '''
            SELECT  fm_edge,
                    array[to_edge1, to_edge2, to_edge3, to_edge4] as outlinks,
                    road_lyr,
                    sign_lyr,
                    array[arrow1, arrow2, arrow3, arrow4] as arrows, 
                    "time"
            FROM org_jv_location
            order by fm_edge;
        '''
        rows = self.get_batch_data(sqlcmd)
        for row in rows:
            inlink = row[0]
            outlinks = row[1]
            road_lyr = row[2]
            sign_lyr = row[3]
            arrows = row[4]
            time = row[5]
            for outlink, arrow in zip(outlinks, arrows):
                if outlink and arrow:
                    self.pg.execute2('''
                                    INSERT INTO temp_junction_tbl(
                                        inlinkid, outlinkid, road_lyr, sign_lyr, arrow, "time"
                                        )
                                    VALUES (%s, %s, %s, %s, %s, %s);
                                ''', 
                                (inlink, outlink, road_lyr, sign_lyr, arrow, time))
                elif not outlink and not arrow:
                    pass
                else:
                    self.log.error("outlink can't match with arrow!!!")
        self.pg.commit2()
        
        # 优化：建立临时表，查询link两端节点时使用，提高查询速度
        sqlcmd = '''
            drop table if exists temp_junction_links;
			
            select *
            into temp_junction_links
            from org_city_nw_gc_polyline
            where id in 
            (
                select distinct (unnest(all_junction_links)) as all_link
                from
                (
                    select array[fm_edge, to_edge1, to_edge2, to_edge3, to_edge4] as all_junction_links
                    from org_jv_location
                ) as m
            );
			
			drop index if exists org_jv_location_id_idx;
			create index org_jv_location_id_idx
			on org_jv_location
			using btree
			(id);
        ''' 
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        return 0

    # bCareAboutSignpost: 有的时候式样要求不考虑是否存在sar的情况，通过此参数设置。
    # 'false'：此时即使可以找到sar图片，也不进行合成，不处理sar。
    # 'true'：  此时才考虑是否存在sar图片并合并。
    def _generate_spotguide_tbl(self, careAboutSignpost='true'):
        # 此时每条记录对应的road_lyr数应为2，一张白天图一张黑夜图。
        sqlcmd = '''
            SELECT a.inlinkid, b.road_type as inlink_roadtype, a.outlinkid, array_agg(road_lyr), 
                   array_agg(sign_lyr), array_agg(arrow), array_agg("time")
            FROM (
                   SELECT distinct inlinkid, outlinkid, road_lyr, sign_lyr, arrow, "time"
                   FROM temp_junction_tbl
                 ) as a
            left join link_tbl as b
            on a.inlinkid=b.link_id
            group by a.inlinkid, b.road_type, a.outlinkid;
        '''
        rows = self.get_batch_data(sqlcmd)
        g_id = 1
        for row in rows:
            inlink = row[0]
            inlinkRoadType = row[1]
            outlink = row[2]
            road_lyrs = row[3]
            sign_lyrs = row[4]
            arrows = row[5]
            times = row[6]
            
            passlink = ''
            passlinkcnt = 0
            
            isExistSar = False
            pattern_name = ''
            if (careAboutSignpost.lower()=='true') and (None not in sign_lyrs):
                
                if(self._check_time_stamp_with_signpost(road_lyrs, sign_lyrs, arrows, times) == False):
                    continue
                pattern_name = sign_lyrs[0][:-6:1] # 存在sar，pattern图名在制作资源时实际使用的是signpost名。
                isExistSar = True # 存在sar。
            else:
                if(self._check_time_stamp_no_signpost(road_lyrs, arrows, times) == False):
                    continue
                pattern_name = road_lyrs[0][:-6:1] # 不存在sar，pattern图名在制作资源时实际使用的是road图片名。
                isExistSar = False # 不存在sar。
                
            nodeid = self._getnode_between_links(inlink, outlink)
            if not nodeid:
                # inlink与outlink不相连，查找中间link列表
                passlink, passlinkcnt, nodeid = self._get_passlink_passlinkcnt(inlink, outlink)
            
            arrow_name = arrows[0][:-6:1]
            direction = 0
            if 'L' in arrow_name:
                direction = 5
            elif 'R' in arrow_name:
                direction = 2
            
            spotguideType = roadType2SpotguideTypeMap[inlinkRoadType]
            
            self.pg.execute2('''
                            INSERT INTO spotguide_tbl(
                                id, nodeid, inlinkid, outlinkid, passlid, passlink_cnt,
                                direction, guideattr, namekind, guideclass, patternno, arrowno,
                                "type", is_exist_sar
                                )
                            VALUES (%s, %s, %s, %s, %s, %s,
                                    %s, %s, %s, %s, %s, %s,
                                    %s, %s);
                            ''', (g_id, nodeid, inlink, outlink, passlink, passlinkcnt,
                                  direction, 0, 0, 0, pattern_name, arrow_name, 
                                  spotguideType, isExistSar))
            g_id = g_id + 1
            
        self.pg.commit2()
        return 0
    
    def _check_time_stamp_with_signpost(self, road_lyrs, sign_lyrs, arrows, times):
        tempTuple = zip(road_lyrs, sign_lyrs, arrows, times)
        for item in tempTuple:
            roadTime = item[0].split('.')[3]
            signTime = item[1].split('.')[3]
            arrowTime = item[2].split('.')[3]
            timeStamp = item[3]
            if(roadTime == '1' and signTime == '1' and arrowTime == '1' and timeStamp.lower() == 'day'):
                return True
            if(roadTime == '2' and signTime == '2' and arrowTime == '2' and timeStamp.lower() == 'night'):
                return True
        return False
    
    def _check_time_stamp_no_signpost(self, road_lyrs, arrows, times):
        tempTuple = zip(road_lyrs, arrows, times)
        for item in tempTuple:
            roadTime = item[0].split('.')[3]
            arrowTime = item[1].split('.')[3]
            timeStamp = item[2]
            if(roadTime == '1' and arrowTime == '1' and timeStamp.lower() == 'day'):
                return True
            if(roadTime == '2' and arrowTime == '2' and timeStamp.lower() == 'night'):
                return True
        return False
    
    # 通过查询link与node的信息确定两条link是否相交
    # 如果相交，返回连接点，否则返回空
    # "org_city_nw_gc_polyline"是一个巨大的表，查询速度很慢，此处查询优化表temp_junction_links。
    # 通过查询优化过后的小表可以提高速度。
    def _getnode_between_links(self, link1, link2):
        ''' get intersect node between link1 and link2'''
        node_sqlcmd = '''
            SELECT id, f_jnctid, t_jnctid
            FROM temp_junction_links
            where id = %s;
        '''
        self.pg.execute2(node_sqlcmd, (link1,))
        inres_row = self.pg.fetchone2()
        self.pg.execute2(node_sqlcmd, (link2,))
        outres_row = self.pg.fetchone2()

        if inres_row[1] in (outres_row[1], outres_row[2]):
            return inres_row[1]
        elif inres_row[2] in (outres_row[1], outres_row[2]):
            return inres_row[2]
        else:
            return None

    def _get_passlink_passlinkcnt(self, inlink, outlink):
        # 求path 路径 和 pathlink count
        sqlcmd = '''
                SELECT s_node, e_node, one_way_code
                FROM link_tbl
                where link_id = %s;
        '''
        self.pg.execute2(sqlcmd, (inlink,))
        in_row = self.pg.fetchone2()
        in_snode = in_row[0]
        in_enode = in_row[1]
        in_oneway = in_row[2]
        self.pg.execute2(sqlcmd, (outlink,))
        out_row = self.pg.fetchone2()
        out_snode = out_row[0]
        out_enode = out_row[1]
        out_oneway = out_row[2]
        if in_oneway == link_graph.ONE_WAY_PROHIBITION \
                    or out_oneway == link_graph.ONE_WAY_PROHIBITION:
            return None

        init_box = '''
                SELECT ST_ASEWKT(mid_get_expand_box(%s::bigint,
                                            %s::bigint, %s)
                        );
        '''
        self.pg.execute2(init_box, (inlink, outlink, link_graph.UNITS_TO_EXPAND))
        expand_box = (self.pg.fetchone2())[0]
        graph_obj = link_graph.LinkGraph()
        paths = link_graph.all_shortest_paths_in_expand_box(graph_obj,
                                                    expand_box,
                                                    (in_snode, in_enode),
                                                    (out_snode, out_enode),
                                                    in_oneway_code = in_oneway,
                                                    out_oneway_code = out_oneway
                                                    )
        if not paths:
            self.log.warning("Can't find the path. inlink=%d, outlink=%d" %
                                     (inlink, outlink))
            return None

        if len(paths) > 1:  # 存在多条最短路径
            self.log.warning('Shortest Paths Number>1.Inlink=%d,outlink=%d'
                            % (inlink, outlink))
        node_id = paths[0][0]
        pass_link = graph_obj.get_linkid_of_path(paths[0])
        pass_link_cnt = 0
        if pass_link:
            if None in pass_link:
                self.log.error('Error PassLink.')
            pass_link_cnt = len(pass_link)
            pass_link = '|'.join(pass_link)
        else:
            self.log.error('not normal stuation!!!.')
        return pass_link, pass_link_cnt, node_id
