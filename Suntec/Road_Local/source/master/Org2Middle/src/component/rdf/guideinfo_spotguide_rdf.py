# -*- coding: cp936 -*-
'''
Created on 2012-4-27

@author: zhangliang
'''
import os
import common
import psycopg2
from common.dirwalker import DirWalker
import struct
import logging
import shutil
from component.default.guideinfo_spotguide import comp_guideinfo_spotguide
from common.common_func import GetPath


class comp_guideinfo_spotguide_rdf(comp_guideinfo_spotguide):
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
        return 0

    def _DoCreatIndex(self):

        return 0

    def _DoCreateFunction(self):
        'Create Function'
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

        return 0

    def _makeJV_CHN(self):

            # create temp junction view table.
            self.CreateTable2('temp_guideinfo_spotguide_junction_view_chn')

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
                        pattern as patternno,
                        arrow as arrowno,
                        1 as type 
                from
                (
                  select A.*, array_upper(all_links,1) as all_link_cnt
                  from temp_guideinfo_spotguide_junction_view_chn as A
                ) as B
            );
            """
            self.pg.execute2(sqlcmd)
            self.pg.commit2()
            
    def _makeJV_Common(self):
        
# 根据condition_type = 20来制作junctionview数据
#             if self.__CheckImagePath() == False:
#                 self.log.warning("Doesn't indicate the Image Path.")
#                 return 0
            # create temp junction view spotguide info
            self.CreateTable2('temp_guideinfo_spotguide_junction_view_full')

            # insert junction view image
            # imageid, image_data_blob
            # self.InsertJunctionViewImage()
#         -- this operation will be done in mid2rdb

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
                        1 as type from
                (
                  select A.*, array_upper(all_links,1) as all_link_cnt
                  from temp_guideinfo_spotguide_junction_view_full as A
                ) as B
            )
            """
            self.pg.execute2(sqlcmd)
            self.pg.commit2()

    def InsertJunctionViewImage(self):
        path = GetPath('illust')
        if self.MakePictoBinary(path) == -1:
            exit(1)
        pass

    def MakePictoBinary(self, fpath):
        FilePath = fpath
        self.pg.connect1()
        try:
            if self.pg.CreateTable1_ByName('rdb_guideinfo_pic_blob_bytea') == -1:
                self.pg.close1()
                return -1

            for files in DirWalker(FilePath):
                FileExt = os.path.splitext(files)
                FileName = os.path.split(FileExt[0])[1].encode("utf-8")
                jpgfile = open(files, 'rb')
                alldata = jpgfile.read()
                if self.pg.insert('rdb_guideinfo_pic_blob_bytea',
                                  ('image_id', 'data'),
                                  (FileName, psycopg2.Binary(alldata))
                                  ) == -1:
                    jpgfile.close()
                    self.pg.close2()
                    return -1
                # pgcur_Create.execute(sqlcmd1,(FileName,psycopg2.Binary(alldata),))
                jpgfile.close()

            self.pg.commit()
        except Exception, ex:
            print '%s:%s' % (Exception, ex)
            raise Exception, 'database operate wrong'
        finally:
            self.pg.close1()
        return 0

    def __CheckImagePath(self):
        '''检查图片路径是否被指定。有，返回True；无，返回False。'''
        path = GetPath('illust')
        if not path:
            return False
        else:
            return True

    def _get_gjv_junctions_info_by_gjv_table(self):
        '''
        '''
        sqlcmd = '''
            SELECT dp_node_id, originating_link_id, dest_link_id,
                    CASE WHEN road_type in (0,1) THEN 1 ELSE 4 END AS jv_type,
                   filename, side,
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
                    filename,side
            from rdfpl_gjv_lat_display_org
            where filename is not null
            )as a
            left join link_tbl as b
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
            SELECT dp_node_id, originating_link_id, out_link_id,
                    CASE WHEN road_type in (0,1) THEN 1 ELSE 4 END AS jv_type,
                   filename, side,
                   mid_findpasslinkbybothlinks(
                            originating_link_id,
                            out_link_id,
                            0,
                            dp_node_id,
                            0,
                            10
                            ) as pathlink
            FROM(
                select a.dp_node_id,a.originating_link_id,a.out_link_id,a.filename,a.side
                from(
                    select distinct dp_node_id,originating_link_id,
                        case when jv_dest_link is null or jv_dest_link = dest_link_id then dest_link_id
                            when jv_dest_link is not null and jv_dest_link <> dest_link_id then jv_dest_link
                        else 0
                        end as out_link_id,
                            filename,side
                    from rdfpl_all_jv_lat_display_org
                    where filename is not null
                    ) as a
                left join temp_gjv_junctionview_info as b
                on a.dp_node_id = b.nodeid and a.originating_link_id = b.inlink and a.out_link_id = b.outlink
                where b.nodeid is null
            )as a
            left join link_tbl as b
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
                CASE WHEN road_type in (0,1) THEN 1 ELSE 4 END as jv_type,
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
                    a.filename,
                    a.side,ejv_filename,dp1_dest_link
                from(
                    SELECT distinct dp_node_id, originating_link_id,
                        case when jv_dest_link is null or jv_dest_link = dest_link_id then dest_link_id
                            when jv_dest_link is not null and jv_dest_link <> dest_link_id then jv_dest_link
                        else 0
                        end as out_link_id,
                        filename, side, ejv_filename,dp1_dest_link
                    FROM rdfpl_all_jv_lat_display_org
                    where ejv_filename is not null
                )as a
            ) as c
            left join link_tbl as d
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
            SELECT distinct nodeid, inlink, outlink,
               jv_type, filename, side,
               pathlink
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
                self.log.warning("Cant't find the Path. InLink=%d, OutLink=%d"
                               % (inlink, outlink))
                continue
            # 求引导属性
#             attr_dir = self._get_direction_attr(ramp, bif, ca)
            # 保存到数据库
            insert_sqlcmd = '''
                    insert into spotguide_tbl(nodeid, inlinkid, outlinkid,
                                              passlid, passlink_cnt, direction,
                                              guideattr, namekind, guideclass,
                                              patternno, arrowno,
                                              type)
                      values(%s, %s, %s,
                             %s, %s, %s,
                             %s, %s, %s,
                             %s, %s,
                             %s)
            '''
            self.pg.execute2(insert_sqlcmd, (nodeid, inlink, outlink,
                                             passlink, pathcnt, 0,
                                             0, 0, 0,
                                             illust_backg_name, arrow_name,
                                             jv_type
                                             )
                             )
        self.pg.commit2()

    def make_ejv_spotguide_data(self):
        sqlcmd = '''
            SELECT distinct nodeid, inlink, outlink,
                  jv_type, filename, side,
                pathlink
            FROM temp_ejv_junctionview_info;
        '''
        self.pg.execute2(sqlcmd)
        rows = self.pg.fetchall2()
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
                self.log.warning("Cant't find the Path. InLink=%d, OutLink=%d"
                               % (inlink, outlink))
                continue
            # 求引导属性
#             attr_dir = self._get_direction_attr(ramp, bif, ca)
            # 保存到数据库
            insert_sqlcmd = '''
                    insert into spotguide_tbl(nodeid, inlinkid, outlinkid,
                                              passlid, passlink_cnt, direction,
                                              guideattr, namekind, guideclass,
                                              patternno, arrowno,
                                              type)
                      values(%s, %s, %s,
                             %s, %s, %s,
                             %s, %s, %s,
                             %s, %s,
                             %s)
            '''
            self.pg.execute2(insert_sqlcmd, (nodeid, inlink, outlink,
                                             passlink, pathcnt, 0,
                                             0, 0, 0,
                                             illust_backg_name, arrow_name,
                                             jv_type
                                             )
                             )
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


#==============================================================================
# comp_picture
#==============================================================================
class comp_picture(object):
    ''' picture entity'''
    def __init__(self):
        self.gid = 0
        self.dayName = None
        self.nightName = None
        self.arrow = None
        self.arrows = []

    def getGid(self):
        return self.gid

    def setGid(self, gid):
        self.gid = gid

    def getDayName(self):
        return self.dayName

    def setDayName(self, dayName):
        self.dayName = dayName

    def getNightName(self):
        return self.nightName

    def setNightName(self, nightName):
        self.nightName = nightName

    def getArrow(self):
        return self.arrow

    def setArrow(self, arrow):
        self.arrow = arrow

    def getArrows(self):
        return self.arrows

    def setArrows(self, arrow):
        self.arrows.append(arrow)


#==============================================================================
# comp_picture
#==============================================================================
class GeneratorPicBinary(object):

    def __init__(self):
        self.conn = psycopg2.connect('''host='172.26.179.184'
                        dbname='tmap_MEA8_RDF_CI'
                        user='postgres' password='pset123456' ''')
        self.pgcur2 = self.conn.cursor()
        curDir = "C:\\My\\20150409_mea_pic\\"
        self.gjvLogger = open(curDir + "gjv.csv", "w")
        self.ejvLogger = open(curDir + "ejv.csv", "w")
        self.birdLogger = open(curDir + "bird.csv", "w")
        
    def __del__(self):
        self.gjvLogger.close()
        self.ejvLogger.close()
        self.birdLogger.close()
        
    def select_gjv_Data(self):
        self.pgcur2.execute('''SELECT distinct dp_node_id, filename, side
                               FROM rdfpl_gjv_lat_display_org;''')
        rows = self.pgcur2.fetchall()
        pics = []
        i = 1
        for row in rows:
            pic = comp_picture()
            pic.setGid(i)
            pic.setDayName(row[1])
            pic.setNightName(row[1])
            pic.setArrow(row[2])
            pics.append(pic)
            i = i + 1

        self.pgcur2.execute('''SELECT distinct dp_node_id, filename, side
                                FROM rdfpl_all_jv_lat_display_org
                                where filename is not null and ejv_filename is null;''')
        allrows = self.pgcur2.fetchall()

        for row in allrows:
            pic = comp_picture()
            pic.setGid(i)
            pic.setDayName(row[1])
            pic.setNightName(row[1])
            pic.setArrow(row[2])
            pics.append(pic)
            i = i + 1
        return pics

    def select_ejv_Data(self):
        pics = []
        i = 1
        self.pgcur2.execute('''
            select ejv_filename,array_agg(outlinkid)
            from(
                SELECT distinct dp_node_id, ejv_filename,
                    case when jv_dest_link is null or jv_dest_link = dest_link_id then dest_link_id
                        when jv_dest_link is not null and jv_dest_link <> dest_link_id then jv_dest_link
                    else 0
                    end as outlinkid
                FROM rdfpl_all_jv_lat_display_org
                where ejv_filename is not null
            ) as a
            group by ejv_filename;''')
        '''and originating_link_id in (816431040,116164590)'''
        rows = self.pgcur2.fetchall()
        rowCount = len(rows)
        rowCount += 1
        rowCount -= 1
        for row in rows:
            pic = comp_picture()
            pic.setGid(i)
            pic.setDayName(row[0])
            pic.setNightName(row[0])
            for arrow in row[1]:
                pic.setArrows(arrow)
            pics.append(pic)
            i = i + 1
        return pics

    def select_ejv_condition_Data(self):
        self.pgcur2.execute('''
                select a.file_name,array_agg(outlink)
                from
                (
                    select distinct E.file_name, F.all_links[array_upper(F.all_links,1)] as outlink
                    from
                    (
                        select A.condition_id, A.condition_type, A.nav_strand_id, A.access_id,
                        B.file_id, C.file_type,C.file_name from
                        (
                            select condition_id, condition_type, nav_strand_id, access_id
                            from rdf_condition
                            where condition_type = 20
                        ) as A left join RDF_FILE_FEATURE as B
                        on A.condition_id = B.feature_id left join rdf_file as C
                        on B.file_id = C.file_id
                        where file_type = 34
                    ) as E left join
                    (
                        select nav_strand_id, array_agg(link_id) as all_links, array_agg(node_id) as all_nodes
                        from
                        (
                            select nav_strand_id, seq_num, link_id, node_id
                            from rdf_nav_strand
                            where nav_strand_id in
                            (
                                SELECT nav_strand_id
                                FROM rdf_condition
                                where condition_type = 20
                            ) order by nav_strand_id, seq_num
                        ) as D group by nav_strand_id
                    ) as F on E.nav_strand_id = F.nav_strand_id
                ) as a
                group by a.file_name;''')
        '''and originating_link_id in (816431040,116164590)'''
        rows = self.pgcur2.fetchall()
        pics = []
        i = 1
        for row in rows:
            pic = comp_picture()
            pic.setGid(i)
            pic.setDayName(row[0])
            pic.setNightName(row[0])
            for arrow in row[1]:
                pic.setArrows(arrow)
            pics.append(pic)
            i = i + 1
        return pics

    def makeGJunctionResultTable(self, srcDir, destDir):
        if(os.path.exists(destDir) == True):
            shutil.rmtree(destDir)
        os.mkdir(destDir)
        pictures = self.select_gjv_Data()
        for pic in pictures:
            pic_name_strs = (pic.getDayName()).split('/')
            pic_type = pic_name_strs[0]

            picname_bin = os.path.splitext(pic_name_strs[2])[0]
            picDirPath = ''
            arrow_file_name = ''
            side = pic.getArrow()
            if pic_type == 'SDPS':
                picDirPath = srcDir + "\\" + "SDPS"
                if side == 'R':
                    arrow_file_name = picname_bin + '_lane1'
                elif side == 'L':
                    arrow_file_name = picname_bin + '_lane2'
                else:
                    logging.error('SDPS side wrong!')
            elif pic_type == 'MDPS':
                picDirPath = srcDir + "\\" + "MDPS"
                if side == 'M':
                    arrow_file_name = picname_bin + '_lane1'
                elif side == 'R':
                    arrow_file_name = picname_bin + '_lane2'
                elif side == 'L':
                    arrow_file_name = picname_bin + '_lane3'
                else:
                    logging.error('MDPS side wrong!!')
            else:
                logging.error('pic_type wrong!')

            if (picname_bin.lower() == "gjv_l12"):
                inti = 1
                inti += 1
            destFile = destDir + "\\" + picname_bin.lower() + ".dat"
            arrowFile = destDir + "\\" + arrow_file_name.lower() + ".dat"
            if os.path.isdir(picDirPath):
                # day and nigth illust
                if  os.path.isfile(destFile) == False:
                    dayPicPath = os.path.join(picDirPath, "DAY", picname_bin + ".jpg")
                    if(os.path.isfile(dayPicPath) == False):
                        self.gjvLogger.write("error,gjv,day picture not exist," + picname_bin + ".jpg\n") 
                        continue
                    nightPicPath = os.path.join(picDirPath, "NIGHT", picname_bin + ".jpg")
                    if(os.path.isfile(nightPicPath) == False):
                        self.gjvLogger.write("error,gjv,day picture not exist," + picname_bin + ".jpg\n") 
                        continue
                    dayFis = open(dayPicPath, 'rb')
                    nightFis = open(nightPicPath, 'rb')
                    fos = open(destFile, 'wb')
                    dayPicLen = os.path.getsize(dayPicPath)
                    nightPicLen = os.path.getsize(nightPicPath)
                    headerBuffer = struct.pack("<HHbiibii", 0xFEFE, 2, 1, 22, \
                                               dayPicLen, 2, 22 + dayPicLen, \
                                               nightPicLen)
                    resultBuffer = headerBuffer + dayFis.read() \
                                            + nightFis.read()
                    dayFis.close()
                    nightFis.close()
                    fos.write(resultBuffer)
                    fos.close()
                    self.gjvLogger.write("success,gjv," + picname_bin + ".jpg\n") 
                    # ARROW PIC BUILD
                if os.path.isfile(arrowFile) == False:
                    picPathFile = picDirPath + "\\DAY\\" + arrow_file_name + ".png"
                    if(os.path.isfile(dayPicPath) == False):
                        self.gjvLogger.write("error,gjv,arrow picture not exist," + arrow_file_name + ".png\n") 
                        continue
                    arrowFis = open(picPathFile, 'rb')
                    fos = open(arrowFile, 'wb')
                    arrowPicLen = os.path.getsize(picPathFile)
                    headerBuffer = struct.pack("<HHbii", 0xFEFE, 1, 0, 13, arrowPicLen)
                    resultBuffer = headerBuffer + arrowFis.read()
                    arrowFis.close()
                    fos.write(resultBuffer)
                    fos.close()
                    self.gjvLogger.write("success,gjv arrow," + arrow_file_name + ".png\n")
                                          
    # 首先处理driver pic
    # 如果driver pic中背景图或者箭头图原图不存在，那么用bird pic对应
    def makeEJunctionResultTable(self, srcDir, destDir, flag):
        if(os.path.exists(destDir) == True):
            shutil.rmtree(destDir)
        os.mkdir(destDir)
        
        pictures = None
        if flag == 'condition':
            pictures = self.select_ejv_condition_Data()
        else:
            pictures = self.select_ejv_Data()
            picCount = len(pictures)
            picCount += 1
        arrowdir = os.path.join(srcDir, "DAY")
        pics_unfinished = []
        for pic in pictures:
            exist_flag = 1
            pic_name_strs = pic.getDayName()
            back_picname = os.path.splitext(pic_name_strs)[0]
            destFile = destDir + "\\" + back_picname.lower() + ".dat"

            if os.path.isdir(srcDir):
                dayPicPath = ''
                nightPicPath = ''
                # day and nigth illust
                if  os.path.isfile(destFile) == False:
                    dayPicPath = os.path.join(srcDir, "DAY", back_picname + ".jpg")
                    nightPicPath = os.path.join(srcDir, "NIGHT", back_picname + ".jpg")
                    if os.path.exists(dayPicPath) == False:
                        self.ejvLogger.write("warn,ejv,day picture not exist," + back_picname + ".jpg\n") 
                        pics_unfinished.append(pic)
                        exist_flag = 0
                        continue
                    if os.path.exists(nightPicPath) == False:
                        self.ejvLogger.write("warn,ejv,night picture not exist," + back_picname + ".jpg\n") 
                        pics_unfinished.append(pic)
                        exist_flag = 0


                    # ARROW PIC BUILD
                dest_arrow_names = []
                source_arrow_names = []
                for arrow in pic.getArrows():
                    arrow_file_name = back_picname + '_' + str(arrow)
                    arrowFile = destDir + "\\" + arrow_file_name.lower() + ".dat"
                    if os.path.isfile(arrowFile) == False:
                        dest_arrow_names.append(arrowFile)
                        picPathFile = ''
                        for source_arrow in os.listdir(arrowdir):
                            if source_arrow.find(arrow_file_name) >= 0:
                                picPathFile = srcDir + "\\DAY\\" + source_arrow
                                source_arrow_names.append(picPathFile)

                        if not os.path.exists(picPathFile):
                            self.ejvLogger.write("warn,ejv arrow,arrow picture not exist," + source_arrow + "\n")
                            pics_unfinished.append(pic)
                            exist_flag = 0
                            break
                if exist_flag > 0:
                    dayFis = open(dayPicPath, 'rb')
                    nightFis = open(nightPicPath, 'rb')
                    b_fos = open(destFile, 'wb')
                    dayPicLen = os.path.getsize(dayPicPath)
                    nightPicLen = os.path.getsize(nightPicPath)
                    b_headerBuffer = struct.pack("<HHbiibii", 0xFEFE, 2, 1, 22, \
                                               dayPicLen, 2, 22 + dayPicLen, \
                                               nightPicLen)
                    b_resultBuffer = b_headerBuffer + dayFis.read() \
                                            + nightFis.read()
                    dayFis.close()
                    nightFis.close()
                    b_fos.write(b_resultBuffer)
                    b_fos.close()
                    self.ejvLogger.write("success,ejv," + back_picname + ".jpg\n") 

                    for dest_arrow_name, source_arrow_name in zip(dest_arrow_names,source_arrow_names):
                        arrowFis = open(source_arrow_name, 'rb')
                        a_fos = open(dest_arrow_name, 'wb')
                        arrowPicLen = os.path.getsize(source_arrow_name)
                        a_headerBuffer = struct.pack("<HHbii", 0xFEFE, 1, 0, 13, arrowPicLen)
                        a_resultBuffer = a_headerBuffer + arrowFis.read()
                        arrowFis.close()
                        a_fos.write(a_resultBuffer)
                        a_fos.close()
                        self.ejvLogger.write("success,ejv arrow," + source_arrow_name + "\n") 
        return pics_unfinished

    def makeBIRDJunctionResultTable(self, srcDir, destDir, pics):
        self.birdLogger.write("info,total count should be %d\n" % len(pics))
        if(os.path.exists(destDir) == True):
            shutil.rmtree(destDir)
        os.mkdir(destDir)
            
        arrowdir = os.path.join(srcDir, "DAY")
        for pic in pics:
            pic_name_strs = pic.getDayName()
            back_picname = os.path.splitext(pic_name_strs)[0]
            destFile = destDir + "\\" + back_picname.lower() + ".dat"
            exist_flag = 1
            if os.path.isdir(srcDir):
                dayPicPath = ''
                nightPicPath = ''
                # day and nigth illust
                if  os.path.isfile(destFile) == False:
                    dayPicPath = os.path.join(srcDir, "DAY", back_picname + ".jpg")
                    nightPicPath = os.path.join(srcDir, "NIGHT", back_picname + ".jpg")
                    if os.path.exists(dayPicPath) == False:
                        self.birdLogger.write("error,bird day picture not exist," + back_picname + ".jpg\n")
                        exist_flag = 0
                        continue
                    if os.path.exists(nightPicPath) == False:
                        self.birdLogger.write("error,bird night picture not exist," + back_picname + ".jpg\n")
                        exist_flag = 0
                        continue
                    # ARROW PIC BUILD
                dest_arrow_names = []
                source_arrow_names = []
                for arrow in pic.getArrows():
                    arrow_file_name = back_picname + '_' + str(arrow)
                    arrowFile = destDir + "\\" + arrow_file_name.lower() + ".dat"
                    if os.path.isfile(arrowFile) == False:
                        dest_arrow_names.append(arrowFile)
                        picPathFile = ''
                        for source_arrow in os.listdir(arrowdir):
                            if source_arrow.find(arrow_file_name) >= 0:
                                picPathFile = srcDir + "\\DAY\\" + source_arrow
                                source_arrow_names.append(picPathFile)

                        if not os.path.exists(picPathFile):
                            self.birdLogger.write("error,bird arrow not exist," + source_arrow + "\n") 
                            exist_flag = 0
                            break
                if exist_flag > 0:
                    dayFis = open(dayPicPath, 'rb')
                    nightFis = open(nightPicPath, 'rb')
                    b_fos = open(destFile, 'wb')
                    dayPicLen = os.path.getsize(dayPicPath)
                    nightPicLen = os.path.getsize(nightPicPath)
                    b_headerBuffer = struct.pack("<HHbiibii", 0xFEFE, 2, 1, 22, \
                                               dayPicLen, 2, 22 + dayPicLen, \
                                               nightPicLen)
                    b_resultBuffer = b_headerBuffer + dayFis.read() \
                                            + nightFis.read()
                    dayFis.close()
                    nightFis.close()
                    b_fos.write(b_resultBuffer)
                    b_fos.close()
                    self.birdLogger.write("success,ejv bird," + back_picname + ".jpg\n") 

                    for dest_arrow_name, source_arrow_name in zip(dest_arrow_names, source_arrow_names):
                        arrowFis = open(source_arrow_name, 'rb')
                        a_fos = open(dest_arrow_name, 'wb')
                        arrowPicLen = os.path.getsize(source_arrow_name)
                        a_headerBuffer = struct.pack("<HHbii", 0xFEFE, 1, 0, 13, arrowPicLen)
                        a_resultBuffer = a_headerBuffer + arrowFis.read()
                        arrowFis.close()
                        a_fos.write(a_resultBuffer)
                        a_fos.close()
                        self.birdLogger.write("success,ejv bird arrow," + source_arrow + "\n") 
        return 0

if __name__ == '__main__':
    test = GeneratorPicBinary()
    test.makeGJunctionResultTable("C:\\My\\20150409_mea_pic\\2DGJ_2015Q1_MEA_OUTPUT_resized\\2DGJ_2015Q1_MEA\\MEA", "C:\\My\\20150409_mea_pic\\2DGJ_2015Q1_MEA_OUTPUT_DAT")
    pics = test.makeEJunctionResultTable("C:\\My\\20150409_mea_pic\\2DJ_2015Q1_MEA_OUTPUT_resized\\2DJ_2015Q1_ASIA\\DRIVER_VIEW\\LANDSCAPE\\ASPECT_RATIO_4x3", "C:\\My\\20150409_mea_pic\\2DJ_2015Q1_MEA_OUTPUT_DAT", '')
    test.makeBIRDJunctionResultTable("C:\\My\\20150409_mea_pic\\2DJ_2015Q1_MEA_OUTPUT_resized\\2DJ_2015Q1_ASIA\\BIRD_VIEW\\LANDSCAPE\\ASPECT_RATIO_4x3", "C:\\My\\20150409_mea_pic\\2DJ_2015Q1_MEA_OUTPUT_DAT_bird", pics)
    pass
