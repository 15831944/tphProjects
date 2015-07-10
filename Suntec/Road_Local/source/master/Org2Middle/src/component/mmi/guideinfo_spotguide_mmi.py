# -*- coding: cp936 -*-
'''
Created on 2012-4-27

@author: zhangliang
'''
import os
import psycopg2
import struct
import shutil

from component.default import link_graph
from component.default.guideinfo_spotguide import comp_guideinfo_spotguide

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
        '''这个表的记录样式：oneinlink--->oneoutlink'''
        if self.CreateTable2('temp_junction_tbl') == -1:
            return -1
        
        if self.CreateTable2('mid_all_sar_files') == -1:
            return -1
        
        return 0

    def _DoCreatIndex(self):

        return 0

    def _DoCreateFunction(self):
        'Create Function'
        return 0

    def _Do(self):
        self._generate_temp_info_of_junction_links()
        self._generate_spotguide_tbl()
        return 0

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
        self.pg.execute(sqlcmd)
        rows = self.pg.fetchall()
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
                break;
        self.pg.commit2()
        
        # 优化：建立临时表，查询link两端节点时使用，提高查询速度
        # todo: 加索引到新表里去
        sqlcmd = '''
            drop table if exists temp_nodes_of_junction_links;
            select *
            into temp_nodes_of_junction_links
            from org_city_nw_gc_polyline
            where id in 
            (
                select distinct (unnest(a)) as all_link
                from
                (
                    select array[fm_edge, to_edge1, to_edge2, to_edge3, to_edge4] as a
                    from org_jv_location
                ) as m
            )
        ''' 
        self.pg.execute(sqlcmd)
        return 0

    def _generate_spotguide_tbl(self, bCareAboutSignpost):
        sqlcmd = '''
            SELECT inlinkid, outlinkid, array_agg(road_lyr), array_agg(sign_lyr), array_agg(arrow), array_agg("time")
            FROM (
                   SELECT distinct inlinkid, outlinkid, road_lyr, sign_lyr, arrow, "time"
                   FROM temp_junction_tbl
                 ) as a
            group by inlinkid, outlinkid;
        '''
        self.pg.execute(sqlcmd)
        rows = self.pg.fetchall()
        g_id = 1
        for row in rows:
            inlink = row[0]
            outlink = row[1]
            road_lyrs = row[2]
            sign_lyrs = row[3]
            arrows = row[4]
            times = row[5]
            
            passlink = ''
            passlinkcnt = 0
            
            road_name = ''
            if bCareAboutSignpost and (None in sign_lyrs):
                
                if(self._check_3(road_lyrs, sign_lyrs, arrows, times) == False):
                    continue
                road_name = road_lyrs[0][:-6:1] + '_' + sign_lyrs[0][:-6:1]
            else:
                if(self._check_2(road_lyrs, arrows, times) == False):
                    continue
                # 删去".1.png"字段, 下同
                road_name = road_lyrs[0][:-6:1] 
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

            self.pg.execute2('''
                            INSERT INTO spotguide_tbl(
                                id, nodeid, inlinkid, outlinkid, passlid, passlink_cnt,
                                direction, guideattr, namekind, guideclass, patternno, arrowno,
                                "type"
                                )
                            VALUES (%s, %s, %s, %s, %s, %s,
                                    %s, %s, %s, %s, %s, %s,
                                    %s);
                            ''', (g_id, nodeid, inlink, outlink,
                                         passlink, passlinkcnt,
                                         direction, 0, 0, 0,
                                         road_name, arrow_name, 1))
            g_id = g_id + 1
        return 0
    
    def _check_3(self, road_lyrs, sign_lyrs, arrows, times):
        return True
    
    def _check_2(self, road_lyrs, arrows, times):
        return True
    
    # 通过查询link与node的信息确定两条link是否相交
    # 如果相交，返回连接点，否则返回空
    # "org_city_nw_gc_polyline"是一个巨大的表，查询速度很慢，此处设为默认查询表。
    # 通过查询优化过后的小表可以提高速度。
    def _getnode_between_links(self, linkId1, linkId2):
        ''' get intersect node between link1 and link2'''
        sqlCmd = '''
            SELECT id, f_jnctid, t_jnctid
            FROM temp_nodes_of_junction_links
            where id = %s;
        '''
        
        self.pg.execute2(sqlCmd % linkId1)
        inres_row = self.pg.fetchone2()
        self.pg.execute2(sqlCmd % linkId1)
        outres_row = self.pg.fetchone2()

        if inres_row[1] in (outres_row[1], outres_row[2]):
            return inres_row[1]
        elif inres_row[2] in (outres_row[1], outres_row[2]):
            return inres_row[2]
        else:
            return None

    def _get_passlink_passlinkcnt(self, inlink, outlink):
        '''求path 路径 和 pathlink count'''
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

#==============================================================================
# comp_picture
#==============================================================================
class comp_picture(object):
    ''' picture entity'''
    def __init__(self):
        self.patternno = None
        self.arrowno = None

    def getPatternno(self):
        return self.patternno

    def setPatternno(self, patternno):
        self.patternno = patternno

    def getArrowno(self):
        return self.arrowno

    def setArrowno(self, arrowno):
        self.arrowno = arrowno
#==============================================================================
# comp_picture
#==============================================================================
class GeneratorPicBinary(object):

    def __init__(self):
        self.conn = psycopg2.connect(''' host='172.26.179.184'
                        dbname='IND_MMI_2014Q4_0081_0001'
                        user='postgres' password='pset123456' ''')
        self.pgcur2 = self.conn.cursor()

    def selectData(self):
        self._make_junction_link()
        self._make_spotguide_tbl()
        self.pgcur2.execute('''SELECT distinct patternno, arrowno FROM spotguide_tbl;''')
        rows = self.pgcur2.fetchall()
        pics = []
        for row in rows:
            pic = comp_picture()
            pic.setPatternno(row[0])
            pic.setArrowno(row[1])
            pics.append(pic)
        return pics

    # 合并背景图
    # 名字未发生任何改变
    def compositeBackground(self, srcDir, destDir):
        if(os.path.isdir(srcDir) == False):
            print "source directory not exist: " + srcDir
            return
        if(os.path.exists(destDir) == True):
            shutil.rmtree(destDir)
        shutil.copytree(srcDir, destDir)
        
        self.pgcur2.execute("SELECT distinct road_lyr, sky_lyr FROM org_jv_location")
        rows = self.pgcur2.fetchall()
        for row in rows:
            road_ = row[0] # road图字段
            sky_ = row[1] # 背景图字段            
            roadPicPath = os.path.join(srcDir, road_)
            if(os.path.isfile(roadPicPath) == False):
                print "can't find road picture: " + road_
                continue
            
            skyPicPath = os.path.join(srcDir, sky_)
            if(os.path.isfile(skyPicPath) == False):
                print "can't find background picture: " + sky_
                continue
            
            # 合并背景图，以road图名字命名
            outputPic = os.path.join(destDir, road_)
            cmd = "composite.exe -gravity north %s %s %s" % (roadPicPath, skyPicPath, outputPic)
            os.system(cmd)
              
    # 合并signpost图
    # 有signpost的会生成一个新文件，命名如下：
    # JV_11.02.001.1.png， 与 JV_11.03.003.1.png 合并为 JV_11.02.001_JV_11.03.003.png
    # 箭头图和其他保持不变 
    def compositeSignpost(self, srcDir, destDir):
        if(os.path.isdir(srcDir) == False):
            print "source directory not exist: " + srcDir
            return
        if(os.path.exists(destDir) == True):
            shutil.rmtree(destDir)
        shutil.copytree(srcDir, destDir)

        self.pgcur2.execute("SELECT distinct sign_lyr, road_lyr FROM org_jv_location WHERE sign_lyr is not null")
        rows = self.pgcur2.fetchall()
        for row in rows:
            road_ = row[1] # road图字段
            sign_ = row[0] # signpost图字段            
            roadPicPath = os.path.join(srcDir, road_)
            if(os.path.isfile(roadPicPath) == False):
                print "can't find road picture: " + road_
                continue
                
            signPicPath = os.path.join(srcDir, sign_)
            if(os.path.isfile(signPicPath) == False):
                print "can't find road picture: " + sign_
                continue
            
            # 合并signpost图,以 road名+sign名 命名并输出到destDir
            # 删掉road的".1.png"
            outputPic = road_[:-4:1] + '_'+ sign_
            outputPic = os.path.join(destDir, outputPic)
            cmd = "composite.exe -gravity north %s %s %s" % (signPicPath, roadPicPath, outputPic)
            os.system(cmd)

    def makeJunctionResultTable(self, srcDir, destDir):
        if os.path.isdir(srcDir) == False:
            return
        if(os.path.exists(destDir) == True):
            shutil.rmtree(destDir)
        os.mkdir(destDir)
        pictures = self.selectData()
        for pic in pictures:
            # day and night illust
            destFile = os.path.join(destDir, pic.getPatternno() + '.dat')
            if  os.path.isfile(destFile) == False:
                dayPicPath = ''
                nightPicPath = ''
                parttenno = pic.getPatternno()
                if(len(parttenno) == 13): # 没有signpost
                    dayPicPath = os.path.join(srcDir, parttenno + ".1.png")
                    nightPicPath = os.path.join(srcDir, parttenno + ".2.png")
                elif(len(parttenno) == 25):
                    roadPart = parttenno[0:12]
                    signPart = parttenno[13:]
                    dayPicPath = os.path.join(srcDir, "%s.1_%s.1.png" % (roadPart, signPart))
                    nightPicPath = os.path.join(srcDir, "%s.2_%s.2.png" % (roadPart, signPart))
                else:
                    print "error road name from spotguide_tbl, its length must be 11(without signpost) or 23(with signpost) " + parttenno
                    continue
                
                if(os.path.isfile(dayPicPath) == False):
                    print "cannot find day picture: " + dayPicPath
                    continue
                if(os.path.isfile(nightPicPath) == False):
                    print "cannot find night picture: " + nightPicPath
                    continue
                
                dayFis = open(dayPicPath, 'rb')
                nightFis = open(nightPicPath, 'rb')
                dayPicLen = os.path.getsize(dayPicPath)
                nightPicLen = os.path.getsize(nightPicPath)
                headerBuffer = struct.pack("<HHbiibii", 0xFEFE, 2, 1, 22, \
                                           dayPicLen, 2, 22 + dayPicLen, \
                                           nightPicLen)
                resultBuffer = headerBuffer + dayFis.read() \
                                        + nightFis.read()
                dayFis.close()
                nightFis.close()
                
                fos = open(destFile, 'wb')
                fos.write(resultBuffer)
                fos.close()
                
            # ARROW PIC BUILD
            arrowno = pic.getArrowno()
            arrowFile = os.path.join(destDir, arrowno + '.dat')
            if os.path.isfile(arrowFile) == False:
                dayArrowPath = os.path.join(srcDir, arrowno + '1.png')
                nightArrowPath = os.path.join(srcDir, arrowno + '2.png')
                if(os.path.isfile(dayArrowPath) == False):
                    print "cannot find day arrow picture: " + dayArrowPath
                    continue
                if(os.path.isfile(dayPicPath) == False):
                    print "cannot find night arrow picture: " + nightArrowPath
                    continue
                
                dayArrowFis = open(dayArrowPath, 'rb')
                nightArrowFis = open(nightArrowPath, 'rb')
                dayArrowLen = os.path.getsize(dayArrowPath)
                nightArrowLen = os.path.getsize(nightArrowPath)
                a_headerBuffer = struct.pack("<HHbiibii", 0xFEFE, 2, 1, 22, \
                                           dayArrowLen, 2, 22 + dayArrowLen, \
                                           nightArrowLen)
                a_resultBuffer = a_headerBuffer + dayArrowFis.read() \
                                        + nightArrowFis.read()
                dayArrowFis.close()
                nightArrowFis.close()
                
                a_fos = open(arrowFile, 'wb')
                a_fos.write(a_resultBuffer)
                a_fos.close()

if __name__ == '__main__':
    test = GeneratorPicBinary()
    test.compositeBackground("C:\\My\\20150410_mmi_pic\\Pattern", "C:\\My\\20150410_mmi_pic\\Pattern_background")
    test.compositeSignpost("C:\\My\\20150410_mmi_pic\\Pattern_background", "C:\\My\\20150410_mmi_pic\\Pattern_signpost")
    test.makeJunctionResultTable("C:\\My\\20150410_mmi_pic\\Pattern_signpost", "C:\\My\\20150410_mmi_pic\\illust_pic")
    pass
