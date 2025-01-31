# -*- coding: cp936 -*-
'''
Created on 2012-4-27

@author: zhangliang
'''
import os
import common
from component.default.guideinfo_spotguide import comp_guideinfo_spotguide

spotguide_tbl_insert_str = '''
        insert into spotguide_tbl(nodeid, inlinkid, outlinkid,
                                  passlid, passlink_cnt, direction,
                                  guideattr, namekind, guideclass,
                                  patternno, arrowno, type,
                                  is_exist_sar)
          values(%s, %s, %s,
                 %s, %s, %s,
                 %s, %s, %s,
                 %s, %s, %s,
                 %s)
'''

class comp_guideinfo_spotguide_rdf(comp_guideinfo_spotguide):
    def __init__(self):
        comp_guideinfo_spotguide.__init__(self)

    def _DoCreateTable(self):
        comp_guideinfo_spotguide._DoCreateTable(self)
        return 0

    def _DoCreatIndex(self):
        return 0

    def _DoCreateFunction(self):
#        self.CreateFunction2('rdb_get_connected_nodeid_uc')
        self.CreateFunction2('mid_findpasslinkbybothlinks')
        return 0

    def _Do(self):
        jv_flag = common.common_func.GetPath('jv_flag')
        # 判断提供的junction view类型
        if jv_flag == 'by_table':
            
            if common.common_func.GetPath('proj_country').lower() in ('hkg','mac'):                 
                self._makeJV_CHN()
            else:
                self._makeJV_Common()
            
        elif jv_flag == 'by_file':
            self.CreateTable2('temp_gjv_junctionview_info')
            self.CreateTable2('temp_ejv_junctionview_info')
            # 根据GJV表来制作junctionview数据
            self._get_gjv_junctions_info_by_gjv_table()
            self._get_gjv_junctions_info_by_alljv_table()
            self._get_ejv_junctions_info_by_alljv_table()
            # 不能颠倒
            self.make_ejv_spotguide_data()
            self.make_gjv_spotguide_data()

        else:
            self.log.error('not so data source!!!')

        tollIllustName = common.common_func.GetPath('toll_station_illust')
        comp_guideinfo_spotguide._GenerateSpotguideTblForTollStation(self, tollIllustName)
        return 0

    def _makeJV_CHN(self):
            self.CreateTable2('temp_spotguide_mdps_pictures')
            mdps_pic_list = common.common_func.GetPath('mdps_pic_list')
            with open(mdps_pic_list, 'a+') as f:
                self.pg.copy_from2(f, 'temp_spotguide_mdps_pictures')
            
            # create temp junction view table.
            self.CreateTable2('temp_guideinfo_spotguide_junction_view_chn')
            sqlcmd = """
            insert into spotguide_tbl(nodeid, inlinkid, outlinkid,
                passlid,passlink_cnt,direction,
                guideattr,namekind,guideclass,patternno,arrowno,type)
            (
                select nodeid,inlinkid,outlinkid,passlinks,pass_cnt,
                direction,guideattr,namekind,guideclass,
                patternno,arrowno,
                case when D.pic_name is not null then 13
                    when pattern_file_type = 3 and road_type in (0,1) then 1 
                    when pattern_file_type = 3 and road_type not in (0,1) then 4
                    when pattern_file_type = 13 and road_type in (0,1) then 2
                    when pattern_file_type = 13 and road_type not in (0,1) then 5
                    else null end as type 
                from
                (
                    select all_nodes[1] as nodeid, all_links[1] as inlinkid,
                                all_links[all_link_cnt] as outlinkid,
                            case when all_link_cnt = 2 then null
                            else array_to_string(all_links[2:all_link_cnt-1], '|')
                            end as passlinks,
                            all_link_cnt - 2 as pass_cnt,
                            0 as direction,0 as guideattr,
                            0 as namekind,0 as guideclass,
                            pattern as patternno,
                            arrow as arrowno,
                            pattern_file_type,
                            arrow_file_type
                    from
                    (
                      select A.*, array_upper(all_links,1) as all_link_cnt
                      from temp_guideinfo_spotguide_junction_view_chn as A
                    ) as B
                ) as M
                left join link_tbl_bak_splitting as C
                on M.inlinkid = C.link_id
                left join temp_spotguide_mdps_pictures as D
                on M.patternno=D.pic_name
            );
            """
            self.pg.execute2(sqlcmd)
            self.pg.commit2()
            
    def _makeJV_Common(self):
            # create temp junction view spotguide info
            self.CreateTable2('temp_guideinfo_spotguide_junction_view_full')
            sqlcmd = """
            insert into spotguide_tbl(nodeid, inlinkid, outlinkid,
                passlid,passlink_cnt,direction,
                guideattr,namekind,guideclass,patternno,arrowno,type)
            (
                select all_nodes[1] as nodeid, all_links[1] as inlinkid,
                            all_links[all_link_cnt] as outlinkid,
                        case when all_link_cnt = 2 then null
                        else array_to_string(all_links[2:all_link_cnt-1], '|')
                        end as passlinks,
                        all_link_cnt - 2 as pass_cnt,
                        0 as direction,0 as guideattr,
                        0 as namekind,0 as guideclass,
                        trim(trailing '.svg' from file_name) as patternno,
                        trim(trailing '.svg' from file_name) || '_' || all_links[all_link_cnt]
                        as arrowno,
                        1 as type
                from
                (
                  select A.*, array_upper(all_links,1) as all_link_cnt
                  from temp_guideinfo_spotguide_junction_view_full as A
                ) as B
            )
            """
            self.pg.execute2(sqlcmd)
            self.pg.commit2()

    def _get_gjv_junctions_info_by_gjv_table(self):
        sqlcmd = '''
            SELECT a.dp_node_id, a.originating_link_id, a.dest_link_id,
                    CASE WHEN a.mdps='Y' THEN 13 
                    WHEN b.road_type in (0,1) THEN 1 
                    ELSE 4 END AS jv_type,
                   a.filename, a.side,
                   mid_findpasslinkbybothlinks(
                            originating_link_id,
                            dest_link_id,
                            0,
                            dp_node_id,
                            0,
                            10
                            ) as pathlink
            FROM(
            select distinct dp_node_id,originating_link_id,dest_link_id,
                    filename,side, mdps
            from rdfpl_gjv_lat_display_org
            where filename is not null
            )as a
            left join link_tbl_bak_splitting as b
            on a.originating_link_id = b.link_id;
        '''
        insert_sqlcmd = '''
                INSERT INTO temp_gjv_junctionview_info(
                    nodeid, inlink, outlink, jv_type,
                    filename, side, pathlink
                    )
                values(%s, %s, %s, %s,
                       %s, %s, %s);
                '''
        self.pg.execute2(sqlcmd)
        rows = self.pg.fetchall2()
        for row in rows:
            nodeid = row[0]
            inlink = row[1]
            outlink = row[2]
            jv_type = row[3]
            filename = row[4]
            side = row[5]
            pathlink = row[6]
            if pathlink:
                self.pg.execute2(insert_sqlcmd, (nodeid, inlink, outlink,
                                                 jv_type, filename, side,
                                                 pathlink
                                                 )
                                 )
            else:
                self.log.warning('inlink = %s,outlink = %s pathlink is null',
                                inlink, outlink)
        self.pg.commit2()

    def _get_gjv_junctions_info_by_alljv_table(self):
        '''
        '''
        sqlcmd = '''
            SELECT a.dp_node_id, a.originating_link_id, a.out_link_id,
                    CASE WHEN a.mdps='Y' THEN 13 
                    WHEN b.road_type in (0,1) THEN 1 
                    ELSE 4 END AS jv_type,
                   a.filename, a.side,
                   mid_findpasslinkbybothlinks(
                            originating_link_id,
                            out_link_id,
                            0,
                            dp_node_id,
                            0,
                            10
                            ) as pathlink
            FROM(
                select a.dp_node_id,a.originating_link_id,a.out_link_id,a.filename,a.side, a.mdps
                from(
                    select distinct dp_node_id,originating_link_id,
                        case when jv_dest_link is null or jv_dest_link = dest_link_id then dest_link_id
                            when jv_dest_link is not null and jv_dest_link <> dest_link_id then jv_dest_link
                        else 0
                        end as out_link_id,
                            filename,side, mdps
                    from rdfpl_all_jv_lat_display_org
                    where filename is not null
                    ) as a
                left join temp_gjv_junctionview_info as b
                on a.dp_node_id = b.nodeid and a.originating_link_id = b.inlink and a.out_link_id = b.outlink
                where b.nodeid is null
            )as a
            left join link_tbl_bak_splitting as b
            on a.originating_link_id = b.link_id;
        '''
        insert_sqlcmd = '''
                INSERT INTO temp_gjv_junctionview_info(
                    nodeid, inlink, outlink, jv_type,
                    filename, side, pathlink
                    )
                values(%s, %s, %s, %s,
                       %s, %s, %s);
                '''
        self.pg.execute2(sqlcmd)
        rows = self.pg.fetchall2()
        for row in rows:
            nodeid = row[0]
            inlink = row[1]
            outlink = row[2]
            jv_type = row[3]
            filename = row[4]
            side = row[5]
            pathlink = row[6]
            if pathlink:
                self.pg.execute2(insert_sqlcmd, (nodeid, inlink, outlink,
                                                 jv_type, filename, side,
                                                 pathlink
                                                 )
                                 )
            else:
                self.log.warning('inlink = %s,outlink = %s pathlink is null',
                                inlink, outlink)
        self.pg.commit2()

    def _get_ejv_junctions_info_by_alljv_table(self):
        self.log.info('make ejv spotguide illust info')
        sqlcmd = '''
            select distinct dp_node_id,originating_link_id,out_link_id,
                CASE WHEN c.mdps='Y' THEN 13 
                WHEN d.road_type in (0,1) THEN 1 
                ELSE 4 END AS jv_type,
                c.filename,
                c.side,ejv_filename,dp1_dest_link,
                mid_findpasslinkbybothlinks(
                    originating_link_id,
                    out_link_id,
                    0,
                    dp_node_id,
                    0,
                    10
                    ) as pathlink
            from(
                select distinct dp_node_id,originating_link_id,out_link_id,
                    a.filename, a.mdps, 
                    a.side,ejv_filename,dp1_dest_link
                from(
                    SELECT distinct dp_node_id, originating_link_id,
                        case when jv_dest_link is null or jv_dest_link = dest_link_id then dest_link_id
                            when jv_dest_link is not null and jv_dest_link <> dest_link_id then jv_dest_link
                        else 0
                        end as out_link_id,
                        filename, mdps, side, ejv_filename,dp1_dest_link
                    FROM rdfpl_all_jv_lat_display_org
                    where ejv_filename is not null
                )as a
            ) as c
            left join link_tbl_bak_splitting as d
            on c.originating_link_id = d.link_id;
        '''
        insert_jv_sqlcmd = '''
                INSERT INTO {0}(
                    nodeid, inlink, outlink, jv_type,
                    filename, side, pathlink
                    )
                values(%s, %s, %s, %s,
                       %s, %s, %s)
                '''
        self.pg.execute2(sqlcmd)
        rows = self.pg.fetchall2()
        for row in rows:
            nodeid = row[0]
            inlink = row[1]
            outlink = row[2]
            jv_type = row[3]
            filename = row[4]
            side = row[5]
            ejv_filename = row[6]
            dp1_dest_link = row[7]
            pathlink = row[8]
            if pathlink:
                self.pg.execute2(insert_jv_sqlcmd.format('temp_ejv_junctionview_info'),
                                                    (nodeid, inlink, outlink,
                                                     jv_type, ejv_filename, side,
                                                     pathlink
                                                     ))
            else:
                self.log.warning('inlink = %s,outlink = %s pathlink is null',
                                inlink, outlink)
        self.pg.commit2()

    def make_gjv_spotguide_data(self):
        sqlcmd = '''
        select a.nodeid,a.inlink,a.outlink,a.jv_type,a.filename,a.side,a.pathlink
        from(
            SELECT distinct nodeid, inlink, outlink, jv_type, filename, side, pathlink
            FROM temp_gjv_junctionview_info
            ) as a
        left join temp_ejv_junctionview_info  as b
        on a.nodeid = b.nodeid and a.inlink = b.inlink
        where b.nodeid is null
        '''
        self.pg.execute2(sqlcmd)
        rows = self.pg.fetchall2()
        for row in rows:
            nodeid = row[0]
            inlink = row[1]
            outlink = row[2]
            jv_type = row[3]
            org_filename = row[4]
            side = row[5]
            passlib = row[6]
            temp_strs = org_filename.split('/')
            file_type = temp_strs[0]
            illust_backg_name = (os.path.splitext(temp_strs[2])[0]).lower()
            arrow_name = None
            # Illust Arrow filename
            arrow_name = self._get_arrow_file_name(file_type,
                                                   illust_backg_name,
                                                   side)
            if not arrow_name:
                self.log.error('No Arrow File Name.')
                continue
            if passlib:
                passlink_list = passlib.split('|')  # 包含Inlink、OutLink
                passlink = '|'.join(passlink_list[1:-1])  # 去掉Inlink、OutLink
                pathcnt = len(passlink_list) - 2
            else:
                self.log.warning("Gjv: Cant't find the Path. InLink=%d, OutLink=%d"
                               % (inlink, outlink))
                continue
            
            # GJV没有SAR
            is_exist_sar = False
            
            # 求引导属性
            # attr_dir = self._get_direction_attr(ramp, bif, ca)

            # 保存到数据库
            self.pg.execute2(spotguide_tbl_insert_str, 
                             (nodeid, inlink, outlink,
                             passlink, pathcnt, 0,
                             0, 0, 0,
                             illust_backg_name, arrow_name, jv_type,
                             is_exist_sar))
        self.pg.commit2()

    def make_ejv_spotguide_data(self):
        sqlcmd = '''
            SELECT distinct nodeid, inlink, outlink,
                  jv_type, filename, side, pathlink
            FROM temp_ejv_junctionview_info;
        '''
        self.pg.execute2(sqlcmd)
        rows = self.pg.fetchall2()
        allSarArrowList = self._get_all_sar_arrow_name_list()
        for row in rows:
            nodeid = row[0]
            inlink = row[1]
            outlink = row[2]
            jv_type = row[3]
            org_filename = row[4]
            passlib = row[6]
            illust_backg_name = (os.path.splitext(org_filename)[0]).lower()
            # Illust Arrow filename
            arrow_name = illust_backg_name + '_' + str(outlink)
            passlink = ''
            pathcnt = 0
            if passlib:
                passlink_list = passlib.split('|')  # 包含Inlink、OutLink
                passlink = '|'.join(passlink_list[1:-1])  # 去掉Inlink、OutLink
                pathcnt = len(passlink_list) - 2
            else:
                self.log.warning("Ejv: Cant't find the Path. InLink=%d, OutLink=%d"
                               % (inlink, outlink))
                continue
            
            # 该箭头图包含sar信息。
            is_exist_sar = arrow_name.lower() in allSarArrowList
            
            # 求引导属性
            # attr_dir = self._get_direction_attr(ramp, bif, ca)
            
            # 保存到数据库
            self.pg.execute2(spotguide_tbl_insert_str,
                              (nodeid, inlink, outlink,
                               passlink, pathcnt, 0,
                               0, 0, 0,
                               illust_backg_name, arrow_name, jv_type,
                               is_exist_sar))
        self.pg.commit2()
        return 0

    def _get_arrow_file_name(self, file_type, pattern_file_name, side):
        if file_type == 'MDPS':  # 多分歧(三分歧及以上)
            suffix_dict = {'M': '_lane1',
                           'R': '_lane2',
                           'L': '_lane3'
                           }
        else:  # 二分歧(及以下)
            suffix_dict = {'R': '_lane1',
                           'L': '_lane2'
                           }
        suffix_name = suffix_dict.get(side)
        if suffix_name:
            return pattern_file_name + suffix_name
        else:
            self.log.error('Error Side. Side=%s' % side)
            return None
        
    # 从指定的文件中读入所有包含了sar信息的arrow图名字，用以查询是否存在sar，并确定is_exist_sar字段。
    # 该文件在制作图片时生成。
    # 使用了配置文件项：all_sign_as_real_file_name
    # 取名时调用了lower()，返回的png名字列表均为小写。
    def _get_all_sar_arrow_name_list(self):
        allSarCsv = common.common_func.GetPath('all_sign_as_real_file_name')
        if os.path.isfile(allSarCsv) == False:
            return []
    
        inFStream = open(allSarCsv)
        listline = inFStream.readlines()
        inFStream.close()
        
        allSarPicList = set()
        for line in listline:
            allSarPicList.add(line.lower().strip('\n') )
        return allSarPicList
        




