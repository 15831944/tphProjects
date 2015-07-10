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

PIC_TYPE = {'Day':'1',
            'Night':'2'
            }


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
        self._make_junction_link()
        self._make_spotguide_tbl()
        return 0

    def _make_junction_link(self):
        '''get oneinlink to oneoutlink,nodeid record'''
        insert_sqlcmd = '''
            INSERT INTO temp_junction_tbl(
                inlinkid, outlinkid, nodeid, road_lyr, arrow, "time"
                )
            VALUES (%s, %s, %s, %s, %s, %s);


        '''
        sqlcmd = '''
            SELECT  fm_edge,
                    array[to_edge1, to_edge2, to_edge3, to_edge4] as outlinks,
                    case when sign_lyr is not null then substring(road_lyr,0,length(road_lyr)-5)
                         || '_' || substring(sign_lyr,0,length(sign_lyr)-5)
                    else substring(road_lyr,0,length(road_lyr)-5)
                    end, 
                    array[arrow1, arrow2, arrow3, arrow4] as arrows, 
                    "time"
            FROM org_jv_location
            order by fm_edge;
        '''
        rows = self.get_batch_data(sqlcmd)

        inti = 1
        for row in rows:
            inlink = row[0]
            outlinks = row[1]
            road_lyr = row[2]
            arrows = row[3]
            time = row[4]
            for outlink, arrow in zip(outlinks, arrows):
                if outlink and arrow:
                    '''get nodeid'''
                    nodeid = self._getnode_between_links(inlink, outlink)
                    self.pg.execute2(insert_sqlcmd, (inlink, outlink, nodeid, road_lyr, arrow, time))
                    inti += 1
                elif not outlink and not arrow:
                    pass
                else:
                    self.log.error("outlink can't match with arrow!!!")
            
            if inti > 100:
                break;
        self.pg.commit2()
        return 0

    def _make_spotguide_tbl(self):
        insert_sqlcmd = '''
            INSERT INTO spotguide_tbl(
                id, nodeid, inlinkid, outlinkid, passlid, passlink_cnt,
                direction, guideattr, namekind, guideclass, patternno, arrowno,
                "type"
                )
            VALUES (%s, %s, %s, %s, %s, %s,
                    %s, %s, %s, %s, %s, %s,
                    %s);
        '''
        sqlcmd = '''
            SELECT inlinkid, outlinkid,nodeid,
                array_agg(sky_lyr), array_agg(arrow),
                array_agg("time")
            FROM (
                   SELECT distinct inlinkid, outlinkid, nodeid, road_lyr, arrow, "time"
                   FROM temp_junction_tbl
                 ) as a
            group by inlinkid, outlinkid,nodeid;
        '''
        rows = self.get_batch_data(sqlcmd)
        g_id = 1
        for row in rows:
            inlink = row[0]
            outlink = row[1]
            nodeid = row[2]
            road_lyrs = row[3]
            arrows = row[4]
            times = row[5]
            passlink = ''
            passlinkcnt = 0
            if not nodeid:
                '''get passlink passlinkcnt node'''
                passlink, passlinkcnt, nodeid = \
                    self._get_passlink_passlinkcnt(inlink, outlink)
            '''check time match with pic'''
            check_f = self._check_time_pic(road_lyrs, arrows, times)
            if check_f:
                road_name = road_lyrs[0]
                arrow_name = (arrows[0])[:-6:1]
                direction = 0
                if 'L' in arrow_name:
                    direction = 5
                elif 'R' in arrow_name:
                    direction = 2
                self.pg.execute2(insert_sqlcmd, (g_id, nodeid, inlink, outlink,
                                                 passlink, passlinkcnt,
                                                 direction, 0, 0, 0,
                                                 road_name, arrow_name, 1))
                g_id = g_id + 1
            else:
                self.log.error("time is not match with pic!!!")
        return 0

    def _check_time_pic(self, sky_lyrs, arrows, times):
        skys_len = len(sky_lyrs)
        arrows_len = len(arrows)
        times_len = len(times)
        if skys_len != arrows_len or arrows_len != times_len:
            return False
        for i in range(skys_len):
            t_sky_f = ((sky_lyrs[i]).split("."))[3]
            t_arrow_f = ((arrows[i]).split("."))[3]
            if t_sky_f != t_arrow_f or t_arrow_f != PIC_TYPE.get(times[i]):
                return False
        return True

    def _getnode_between_links(self, link1, link2):
        ''' get intersect node between link1 and link2'''
        node_sqlcmd = '''
            SELECT id, f_jnctid, t_jnctid
            FROM org_city_nw_gc_polyline
            where id = %s;
        '''
        sql1 = (node_sqlcmd % (link1,))
        self.pg.execute2(sql1)
        inres_row = self.pg.fetchone2()
        self.pg.execute2(node_sqlcmd, (link2,))
        outres_row = self.pg.fetchone2()
        nodeid = 0
        if inres_row[1] in (outres_row[1], outres_row[2]):
            nodeid = inres_row[1]
        elif inres_row[2] in (outres_row[1], outres_row[2]):
            nodeid = inres_row[2]
        else:
            return None
        return nodeid

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
        self.gid = 0
        self.dayName = None
        self.nightName = None
        self.dayArrow = None
        self.nightArrow = None
        self.commonPatter = None
        self.commonArrow = None

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

    def getNightArrow(self):
        return self.nightArrow

    def setNightArrow(self, nightArrow):
        self.nightArrow = nightArrow

    def getDayArrow(self):
        return self.dayArrow

    def setDayArrow(self, dayArrow):
        self.dayArrow = dayArrow

    def getCommonPatter(self):
        return self.commonPatter

    def setCommonPatter(self, commonPatter):
        self.commonPatter = commonPatter

    def getCommonArrow(self):
        return self.commonArrow

    def setCommonArrow(self, commonArrow):
        self.commonArrow = commonArrow


#==============================================================================
# comp_picture
#==============================================================================
class GeneratorPicBinary(object):

    def __init__(self):
        self.conn = psycopg2.connect(''' host='172.26.179.184'
                        dbname='17cy_IND_MMI_CI'
                        user='postgres' password='pset123456' ''')
        self.pgcur2 = self.conn.cursor()

    def selectData(self):
        self.pgcur2.execute('''SELECT distinct patternno, arrowno FROM spotguide_tbl;''')
        rows = self.pgcur2.fetchall()
        pics = []
        i = 1
        for row in rows:
            patter = row[0]
            arrow = row[1]
            dayCode = PIC_TYPE.get('Day')
            nightCode = PIC_TYPE.get('Night')
            pic = comp_picture()
            pic.setGid(i)
            pic.setDayName(patter + '.' + dayCode)
            pic.setNightName(patter + '.' + nightCode)
            pic.setDayArrow(arrow + '.' + dayCode)
            pic.setNightArrow(arrow + '.' + nightCode)
            pic.setCommonPatter(patter)
            pic.setCommonArrow(arrow)
            pics.append(pic)
            i = i + 1
        return pics

    def makeJunctionResultTable(self, srcDir, destDir):
        if os.path.isdir(srcDir) == False:
            return
        if(os.path.exists(destDir) == True):
            shutil.rmtree(destDir)
        os.mkdir(destDir)
        pictures = self.selectData()
        for pic in pictures:
            # day and night illust
            destFile = os.path.join(destDir, pic.getCommonPatter() + '.dat')
            if  os.path.isfile(destFile) == False:
                dayPicPath = os.path.join(srcDir, pic.getDayName() + ".jpg")
                nightPicPath = os.path.join(srcDir, pic.getNightName() + ".jpg")
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
            arrowFile = os.path.join(destDir, pic.getCommonArrow() + '.dat')
            if os.path.isfile(arrowFile) == False:
                d_arrow_path = os.path.join(srcDir, pic.getDayArrow() + '.png')
                n_arrow_path = os.path.join(srcDir, pic.getNightArrow() + '.png')
                dayArrowFis = open(d_arrow_path, 'rb')
                nightArrowFis = open(n_arrow_path, 'rb')
                dayArrowLen = os.path.getsize(d_arrow_path)
                nightArrowLen = os.path.getsize(n_arrow_path)
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
    test.makeJunctionResultTable("C:\\psdlib\\Pattern", "C:\\illustsource\\mmipic")
    pass
