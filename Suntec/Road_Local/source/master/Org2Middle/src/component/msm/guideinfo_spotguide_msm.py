# -*- coding: UTF8 -*-
'''
Created on 2013-12-5

@author: zym
'''
import os
import struct
import psycopg2
import string
from component.default.guideinfo_spotguide import comp_guideinfo_spotguide


class comp_guideinfo_spotguide_msm(comp_guideinfo_spotguide):

    def __init__(self):
        '''
        Constructor
        '''
        comp_guideinfo_spotguide.__init__(self)

    def _DoCreateTable(self):
        self.CreateTable2('spotguide_tbl')
        self.CreateIndex2('spotguide_tbl_node_id_idx')

    def _Do(self):
        self._create_temp_spotguide_info()
        self._orglink_to_midlink_spotguide()
        self._get_passlink_dir_spotguide()
        self._make_spotguide_tbl()
        comp_guideinfo_spotguide._GenerateSpotguideTblForTollStation(self)
        return 0
    
    def _create_temp_spotguide_info(self):
        self.CreateTable2('temp_spotguide_info')
        self.CreateFunction2('make_temp_spotguide_info')
        self.pg.execute2('select make_temp_spotguide_info();')
        return 0

    def _orglink_to_midlink_spotguide(self):
        self.CreateTable2('temp_spotguide_in_topolink')
        sql_cmd = '''
            insert into temp_spotguide_in_topolink(
                link_id,outlinkid,illustname,arrowid,signid,folder
            )
            (
                select c.new_link_id,b.new_link_id,
                        SUBSTRING(a.illustname from '[A-z0-9]+'),a.arrowid,
                        a.signid,a.folder
                from temp_spotguide_info as a
                left join temp_topo_link as b
                on a.outlinkid = b.link_id and a.folder = b.folder
                left join temp_topo_link as c
                on a.link_id = c.link_id and a.folder = c.folder
            );
        '''
        self.pg.execute2(sql_cmd)
        self.pg.commit2()
        return 0

    def _get_passlink_dir_spotguide(self):
        self.CreateTable2('temp_spotguide_info_passlink_dir')
        insert_sql_cmd = '''
            INSERT INTO temp_spotguide_info_passlink_dir(
            link_id, outlinkid, illustname, arrowid, signid, passlink, 
            passlink_dir, nodeid)
            VALUES (%s, %s, %s, %s, %s, %s, 
                    %s, %s);
        '''
        sql_cmd = '''
            select a.link_id,a.outlinkid,a.illustname,a.arrowid,a.signid,
                passlinkstr,dir::smallint,make_nodeid_by_passlink(passlinkstr)
            from(
                select a.*,SUBSTRING(passlink,1,pos_cnt-1) as passlinkstr,
                    SUBSTRING(passlink,pos_cnt+1,1) as dir
                from(
                    select a.*,position('(' in passlink) as pos_cnt
                    from(
                        select a.link_id,a.outlinkid,a.illustname,a.arrowid,a.signid,
                            case when b.oneway = 1 then mid_findpasslinkbybothlinks(a.link_id,a.outlinkid,0,b.end_node_id,0)
                                 when b.oneway = 0 then mid_findpasslinkbybothlinks(a.link_id,a.outlinkid,b.start_node_id,b.end_node_id,1)
                                else ''
                                end as passlink
                        from (
                            SELECT distinct link_id, outlinkid, illustname, arrowid, signid, folder
                            FROM temp_spotguide_in_topolink
                        ) as a
                        left join temp_topo_link_org_oneway as b
                        on a.link_id = b.link_id
                    ) as a
                ) as a
            ) as a;
        '''
        self.pg.execute2(sql_cmd)
        rows = self.pg.fetchall2()
        for row in rows:
            inlinkid = row[0]
            outlinkid = row[1]
            illustname = row[2]
            arrowid = row[3]
            signid = row[4]
            passlinkstr = row[5]
            link_dir = row[6]
            nodeid = row[7]
            if passlinkstr:
                self.pg.execute2(insert_sql_cmd, (inlinkid, outlinkid, illustname,
                                    arrowid,signid,passlinkstr,link_dir,nodeid))
            else:
                self.log.warning("Can't find the path. inlink=%d, outlink=%d" %
                                 (inlinkid, outlinkid))
        self.pg.commit2()
        return 0

    def _make_spotguide_tbl(self):
        sql_cmd = '''
            INSERT INTO spotguide_tbl(
            id, nodeid, inlinkid, outlinkid, passlid, passlink_cnt,
            direction, guideattr, namekind, guideclass, patternno, arrowno,
            "type")
            VALUES (%s, %s, %s, %s, %s, %s,
                    %s, %s, %s, %s, %s, %s,
                    %s);
        '''
        i = 1
        get_data_sql = '''
            SELECT link_id, outlinkid, illustname,
                    case when arrowid::integer = 0 then illustname||'-'||'l'
                    else
                        illustname||'-'||'r'
                    end as arrowname, passlink, nodeid
             FROM temp_spotguide_info_passlink_dir;
        '''
        self.pg.execute2(get_data_sql)
        rows = self.pg.fetchall2()
        for row in rows:
            inlinkid = row[0]
            outlinkid = row[1]
            illustname = row[2]
            arrowname = row[3]
            pathlinks = (row[4]).split("|")
            pathlinkslen = len(pathlinks)
            nodeid = row[5]
            passlinklib = "|".join(pathlinks[1:pathlinkslen - 1])
            self.pg.execute2(sql_cmd, (i, nodeid, inlinkid, outlinkid, passlinklib,
                                       pathlinkslen - 2, 0, 0, 0, 0, illustname,
                                       arrowname, 1
                                       ))
            i = i + 1
        self.pg.commit2()
        return 0

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
                        dbname='AP_Malsing_2014Q4_0065_0114'
                        user='postgres' password='pset123456' ''')
        self.pgcur2 = self.conn.cursor()

    def selectData(self):
        self.pgcur2.execute('''SELECT distinct patternno, arrowno
                                FROM spotguide_tbl;''')
        rows = self.pgcur2.fetchall()
        pics = []
        i = 1
        for row in rows:
            patter = row[0]
            arrow = row[1]
            pic = comp_picture()
            pic.setGid(i)
            pic.setDayName(patter)
            pic.setNightName(patter)
            pic.setDayArrow(arrow)
            pic.setNightArrow(arrow)
            pic.setCommonPatter(patter)
            pic.setCommonArrow(arrow)
            pics.append(pic)
            i = i + 1
        return pics

    def makeJunctionResultTable(self, dirFileDir, destFileDir):
        pictures = self.selectData()
        no_arrow_backgroud_pic = []
        for pic in pictures:
            destFile = os.path.join(destFileDir, pic.getCommonPatter() + '.dat')
            arrowFile = os.path.join(destFileDir, pic.getCommonArrow() + '.dat')
            if os.path.isdir(dirFileDir):
                if  os.path.isfile(destFile) == False:
                    
                    dayPicPath = os.path.join(dirFileDir, pic.getCommonPatter() + ".jpg")
                    nightPicPath = os.path.join(dirFileDir, pic.getCommonPatter() + ".jpg")
                    if not os.path.exists(dayPicPath):
                        continue
                    dayfis = open(dayPicPath, 'rb')
                    nightfis = open(nightPicPath, 'rb')
                    fos = open(destFile, 'wb')
                    dayPicLen = os.path.getsize(dayPicPath)
                    nightPicLen = os.path.getsize(nightPicPath)
#                    headerBuffer = struct.pack("<hhbii", 0xFEFE, 1, 0, 13, dayPicLen)
#                    resultBuffer = headerBuffer + fis.read()
                    b_headerBuffer = struct.pack("<HHbiibii", 0xFEFE, 2, 1, 22, \
                                               dayPicLen, 2, 22 + dayPicLen, \
                                               nightPicLen)
                    b_resultBuffer = b_headerBuffer + dayfis.read() \
                                            + nightfis.read()               
                    dayfis.close()
                    nightfis.close()
                    fos.write(b_resultBuffer)
                    fos.close()
                    # ARROW PIC BUILD
                if os.path.isfile(arrowFile) == False:
                    arrow_path = os.path.join(dirFileDir, pic.getCommonArrow() + '.png')
                    arrow_path2 = os.path.join(dirFileDir, (string.capwords((pic.getCommonArrow())[::-1]))[::-1] + '.png')
                    arrowFis = None
                    if os.path.exists(arrow_path):
                        arrowFis = open(arrow_path, 'rb')
                    elif os.path.exists(arrow_path2):
                        arrowFis = open(arrow_path2, 'rb')
                    else:
                        no_arrow_backgroud_pic.append(destFile)
                        print pic.getCommonArrow()
                        continue
                    a_fos = open(arrowFile, 'wb')
                    arrowLen = os.path.getsize(arrow_path)
                    a_headerBuffer = struct.pack("<hhbii", 0xFEFE, 1, 0, 13, arrowLen)
                    a_resultBuffer = a_headerBuffer + arrowFis.read()
                    arrowFis.close()
                    a_fos.write(a_resultBuffer)
                    a_fos.close()
        return no_arrow_backgroud_pic
if __name__ == '__main__':
    test = GeneratorPicBinary()
    delete_backpic = test.makeJunctionResultTable("D:\\Data\\MSM2014Q4_PIC\\ORG", "D:\Data\MSM2014Q4_PIC\OUTPUT2")
    for picname in delete_backpic:
        print picname
        if os.path.exists(picname):
            os.remove(picname)
    pass
