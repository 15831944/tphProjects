# -*- coding: UTF8 -*-
'''
Created on 2013-12-5

@author: zym
'''
import os
import struct
import psycopg2
import logging
from component.default.guideinfo_spotguide import comp_guideinfo_spotguide


class comp_guideinfo_spotguide_nostra(comp_guideinfo_spotguide):

    def __init__(self):
        '''
        Constructor
        '''
        comp_guideinfo_spotguide.__init__(self)

    def _DoCreateTable(self):
        self.CreateTable2('spotguide_tbl')
        self.CreateIndex2('spotguide_tbl_node_id_idx')
        self.CreateTable2('temp_org_junctionview')
        self.CreateTable2('temp_mid_junctionview_tbl')

    def _Do(self):
        self.update_junctionview_handler()
        self.add_junctionnode()
        self.add_junctionPicName()
        self.makeJunctionViewInfo()
        self.setDefualtValue()
        return 0

    def update_junctionview_handler(self):
        self.CreateTable2('temp_update_org_junctionview_tbl')
        sql_cmd_org = '''
            INSERT INTO temp_update_org_junctionview_tbl(
            arc1, arc2, day_pic, night_pic, arrowimg, lon, lat)
            (
                select a.arc1,a.arc2,a.day_pic,
                a.night_pic,a.arrowimg,a.lon,a.lat
                from org_junctionview as a
                left join update_junctionview_tbl as b
                on a.arc1 = b.old_inlinkid and a.arc2 = b.old_outlinkid
                where b.gid is null
            )
            ;
        '''
        self.pg.execute(sql_cmd_org)
        self.pg.commit2()
        sql_cmd_org_update = '''
            INSERT INTO temp_update_org_junctionview_tbl(
            arc1, arc2, day_pic, night_pic, arrowimg, lon, lat)
            (
                select b.new_inlinkid,b.new_outlinkid,a.day_pic,
                a.night_pic,a.arrowimg,a.lon,a.lat
                from org_junctionview as a
                inner join update_junctionview_tbl as b
                on a.arc1 = b.old_inlinkid and a.arc2 = b.old_outlinkid
            )
            ;
        '''
        self.pg.execute(sql_cmd_org_update)
        self.pg.commit2()

        sql_cmd_org_update = '''
            UPDATE temp_update_org_junctionview_tbl
            SET arc1 =
            (
                select case when count_flag > 0 then links[1]
                    else arc1
                    end
                from
                (
                    select array_agg(new_inlinkid) as links,
                        count(*) as count_flag
                    from
                    (
                        SELECT old_inlinkid,new_inlinkid
                        FROM update_junctionview_tbl
                        where old_inlinkid <> new_inlinkid
                    ) as a
                    where old_inlinkid = arc1
                ) as a
            );
        '''
        self.pg.execute(sql_cmd_org_update)
        self.pg.commit2()
        return 0

    def add_junctionnode(self):
        self.CreateFunction2("make_temp_org_junctionview_node")
        make_jtv_sql = '''
            INSERT INTO temp_org_junctionview(
            arc1, arc2, nodeid, day_pic, night_pic, arrowimg, passlib)
            VALUES (%s, %s, %s, %s, %s, %s, %s);
        '''
        sql_cmd = '''
                SELECT arc1, arc2, day_pic, night_pic, arrowimg,
                    make_temp_org_junctionview_node(arc1::bigint,arc2::bigint),
                    b.s_node,b.e_node,c.s_node,c.e_node
                FROM temp_update_org_junctionview_tbl as a
                left join link_tbl as b
                on a.arc1 = b.link_id
                left join link_tbl as c
                on a.arc2 = c.link_id;
        '''
        rows = self.get_batch_data(sql_cmd)
        for row in rows:
            arc1 = row[0]
            arc2 = row[1]
            day_pic = row[2]
            night_pic = row[3]
            arrowimg = row[4]
            nodeid = row[5]
            if nodeid:
                self.pg.execute2(make_jtv_sql, (arc1, arc2, nodeid,
                                                day_pic, night_pic,
                                                arrowimg, ''
                                                ))
            else:
                from component.default import link_graph as lg
                expand_box = lg.get_daufalt_expand_box(int(arc1),
                                                       int(arc2))
                graph_obj = lg.LinkGraph()
                paths = lg.all_shortest_paths_in_expand_box(graph_obj,
                                                   expand_box,
                                                   (row[6], row[7]),
                                                   (row[8], row[9])
                                                   )
                if not paths:
                    self.log.error("Node is none: in_link=%s, out_link=%s"
                                   % (arc1, arc2))
                    continue
                node_id = paths[0][0]
                if not node_id:
                    self.log.error('No NodeId.')
                pass_link = graph_obj.get_linkid_of_path(paths[0])
                if pass_link:
                    if None in pass_link:
                        self.log.error('Error PassLink.')
                    pass_link_cnt = len(pass_link)
                    pass_link = '|'.join(pass_link)
                self.pg.execute2(make_jtv_sql, (arc1, arc2, node_id,
                                            day_pic, night_pic,
                                            arrowimg, pass_link
                                            ))
        self.pg.commit2()

    def add_junctionPicName(self):
        sqlcmd = '''
            INSERT INTO temp_mid_junctionview_tbl(gid, arc1, arc2, nodeid,
                                        patternno, arrowno,passlib)
            SELECT gid, arc1, arc2, nodeid, day_pic, arrowimg,passlib
            FROM temp_org_junctionview;
        '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

    def makeJunctionViewInfo(self):
        '''make spotguide_tbl'''
        insertSql = '''
            INSERT INTO spotguide_tbl(
                id, nodeid, inlinkid, outlinkid, passlid,
                passlink_cnt, patternno, arrowno, "type")
            VALUES (%s, %s, %s, %s, %s,
                     %s, %s, %s, %s);
        '''
        selectSql = '''
            SELECT gid, arc1, arc2, nodeid, patternno, arrowno,passlib
            FROM temp_mid_junctionview_tbl;
        '''
        self.pg.execute2(selectSql)
        resultList = self.pg.fetchall2()
        self.pg.commit2()
        for junct in resultList:
            passlib = junct[6]
            lenth = 0
            if passlib:
                lenth = len(passlib.split('|'))
            self.pg.execute2(insertSql, (junct[0], junct[3], junct[1],
                            junct[2], passlib, lenth,
                            junct[4], junct[5], 1))
        self.pg.commit2()

    def setDefualtValue(self):
        updateSql = '''
        UPDATE spotguide_tbl
        SET direction=%s, guideattr=%s, namekind=%s, guideclass=%s;
        '''
        self.pg.execute2(updateSql, (0, 0, 0, 0))
        self.pg.commit2()


class comp_picture(object):
    ''' picture entity'''
    def __init__(self):
        self.gid = 0

    def __eq__(self, other):
        if self.dayName == other.dayName:
            return True
        else:
            return False

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


class GeneratorPicBinary(object):

    def __init__(self):
        self.conn = psycopg2.connect(''' host='172.26.179.195'
                        dbname='AP_NOSTRA_201401_0061_0102'
                        user='postgres' password='pset123456' ''')
        self.pgcur2 = self.conn.cursor()

    def selectData(self):
        self.pgcur2.execute('''SELECT gid,day_pic,night_pic,
                        arrowimg FROM org_junctionview''')
        rows = self.pgcur2.fetchall()
        pics = []
        for row in rows:
            pic = comp_picture()
            pic.setGid(row[0])
            pic.setDayName(row[1])
            pic.setNightName(row[2])
            pic.setArrow(row[3])
            pics.append(pic)
        return pics

    def makeJunctionResultTable(self, dirFileDir, destFileDir):
        pictures = self.selectData()
        # 鍒犻櫎娌℃湁鍥剧墖淇℃伅鐨勬暟鎹褰�
        noexsit_pics = []
        for pic in pictures:
            picDirPath = os.path.join(dirFileDir, pic.getDayName() + ".psd")
            if os.path.isdir(picDirPath):
                dayPicPath = os.path.join(picDirPath, "Main_Day.jpg")
                nightPicPath = os.path.join(picDirPath, "Main_Night.jpg")
                arrowPathFile = os.path.join(picDirPath, pic.getArrow() + ".png")
                if os.path.isfile(dayPicPath) == False or\
                    os.path.isfile(nightPicPath) == False or\
                    os.path.isfile(arrowPathFile) == False:
                    noexsit_pics.append(pic)
                    logging.error('%s not day or night or arrow pic source', pic.getDayName())
        for pic in pictures:
            if pic in noexsit_pics:
                continue
            picDirPath = os.path.join(dirFileDir, pic.getDayName() + ".psd")
            destFile = os.path.join(destFileDir, pic.getDayName() + ".dat")
            arrowFile = os.path.join(destFileDir, pic.getArrow() + ".dat")
            if os.path.isdir(picDirPath):
                dayPicPath = os.path.join(picDirPath, "Main_Day.jpg")
                nightPicPath = os.path.join(picDirPath, "Main_Night.jpg")
                arrowPathFile = os.path.join(picDirPath, pic.getArrow() + ".png")
                # day and nigth illust
                if  os.path.isfile(destFile) == False:
                    dayFis = open(dayPicPath, 'rb')
                    nightFis = open(nightPicPath, 'rb')
                    fos = open(destFile, 'wb')
                    dayPicLen = os.path.getsize(dayPicPath)
                    nightPicLen = os.path.getsize(nightPicPath)
                    headerBuffer = struct.pack("<hhbiibii", 0xFEFE, 2, 1, 22, \
                                               dayPicLen, 2, 22 + dayPicLen, \
                                               nightPicLen)
                    resultBuffer = headerBuffer + dayFis.read() \
                                            + nightFis.read()
#                     print len(resultBuffer)
                    dayFis.close()
                    nightFis.close()
                    fos.write(resultBuffer)
                    fos.close()
                if os.path.isfile(arrowFile) == False:
                    arrowFis = open(arrowPathFile, 'rb')
                    fos = open(arrowFile, 'wb')
                    arrowPicLen = os.path.getsize(arrowPathFile)
                    headerBuffer = struct.pack("<hhbii", 0xFEFE, 1, 0, 13, arrowPicLen)
                    resultBuffer = headerBuffer + arrowFis.read()
                    arrowFis.close()
                    fos.write(resultBuffer)
                    fos.close()

if __name__ == '__main__':
    test = GeneratorPicBinary()
    test.makeJunctionResultTable("C:\\dir", "C:\\g_pic\\v_3")
    pass
