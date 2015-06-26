# -*- coding: UTF8 -*-
'''
Created on 2012-9-17

@author: zhangliang
'''

import os
import psycopg2
import struct

import component.component_base
import common.common_func


class comp_guideinfo_spotguide_ta(component.component_base.comp_base):
    '''
    deal with junction view
    '''

    def __init__(self):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self, 'GuideInfo_SpotGuide')

    def _DoCreateTable(self):
        self.CreateTable2('spotguide_tbl')
        self.CreateTable2('temp_jv_link_walked')
        self.CreateTable2('mid_all_sar_files')

    def _DoCreateFunction(self):
        if self.CreateFunction2('mid_get_all_links_array') == -1:
            return -1
        if self.CreateFunction2('mid_get_jv_passlink_array') == -1:
            return -1

        return 0

    def _Do(self):
        sqlcmd = '''
        select count(*)
        from pg_tables
        where 'org_nw' = tablename
            or 'org_jv' = tablename
            or 'a0' = tablename;
        '''
        self.pg.execute2(sqlcmd)
        row = self.pg.fetchone2()
        if row[0] != 3:
            return
        if self.CreateIndex2('nw_f_jnctid_idx') == -1:
            return -1
        if self.CreateIndex2('nw_t_jnctid_idx') == -1:
            return -1
        if self.CreateIndex2('jv_f_trpelid_mnl_idx') == -1:
            return -1
        if self.CreateIndex2('jv_id_idx') == -1:
            return -1
        if self.CreateIndex2('jv_jnctid_mnl_idx') == -1:
            return -1
        if self.CreateIndex2('jv_t_trpelid_mnl_idx') == -1:
            return -1

        sqlcmd = """
            select gid from a0 where name = 'United States';
             """

        if self.pg.execute(sqlcmd) == -1:
            return 0
        else:
            if self.pg.getcnt2() > 0:
                self.__MakeSpotGuideFromJV()
            else:
                self.__Importjv2db()
                self.__MakeMiddleSpotGuide()

        return 0

    def __MakeMiddleSpotGuide(self):
        self.log.info('start to make middle spotguide data.')
        sqlcmd = """
          insert into spotguide_tbl
            (
                nodeid, inlinkid, outlinkid, passlid, passlink_cnt,
                direction, guideattr, namekind, guideclass,
                patternno, arrowno, type
            )
            (
            select nodeid, inlinkid, outlinkid, passlid, passlink_cnt,
                0 as direction, 0 as guideattr, 0 as namekind, 0 as guideclass,
                patternno, null arrowno, type
            from (
                  select nodeid, inlinkid, passlid, passlink_cnt,
                        outlinkid,patternno,
                        case
                            when b.road_type in(0,1)
                                and (c.link_type = 5 and c.road_type in (0,1))
                            then 2
                            when b.road_type not in (0,1)
                                and (c.link_type = 5 and c.road_type in (0,1))
                            then 3
                            when b.road_type in(0,1)
                                and (c.link_type <> 5 and c.road_type in (0,1))
                            then 1
                            else 4
                        end as type
                    from (
                          select jdn_id::bigint as nodeid,
                                passlids[1]::bigint as inlinkid,
                                passlids[array_length(passlids, 1)]::bigint
                                as outlinkid,
                                case
                                    when array_length(passlids, 1) >2
                                    then array_to_string(passlids[2:array_length(passlids, 1)-1], '|')
                                    else null
                                end as passlid,
                                (array_length(passlids, 1) - 2) as passlink_cnt,
                                image_name as patternno
                          from (
                            select jdn_id,
                                mid_get_all_links_array(pass_node_link_list)
                                as passlids,
                                image_name
                            from temp_ta_jv_tbl
                            ) a
                    ) a
                    left join link_tbl b
                    on a.inlinkid = b.link_id
                    left join link_tbl c
                    on a.outlinkid = c.link_id
            ) a
            order by nodeid,inlinkid,outlinkid,passlid,passlink_cnt,direction,guideattr,namekind,guideclass,patternno,arrowno,type
        )
    """

        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()

        return 0

    def __MakeSpotGuideFromJV(self):
        sqlcmd = """
        insert into spotguide_tbl
            (
            nodeid, inlinkid, outlinkid, passlid, passlink_cnt,
            direction, guideattr, namekind, guideclass,
            patternno, arrowno, type
            )
            (
            select  nodeid as nodeid, inlinkid as inlinkid,
                outlinkid as outlinkid,
                case when passlid = '' then  null
                else passlid end as passlid,
                case
                 when array_length(string_to_array(passlid,'|'), 1) is not null
                 then array_length(string_to_array(passlid,'|'), 1)
                 else 0
                end
                as passlink_cnt,
                0 as direction, 0 as guideattr, 0 as namekind, 0 as guideclass,
                road as patternno, arrow as arrowno, 1 as type
            from(
                select jnctid_mnl as nodeid,f_trpelid_mnl as inlinkid,
                    t_trpelid_mnl as outlinkid,
                    case
                        when b.id is not null then b.passlid
                        else array_to_string(mid_get_jv_passlink_array(a.id::bigint, f_trpelid_mnl::bigint,jnctid_mnl::bigint,t_trpelid_mnl::bigint),'|')
                    end as passlid,
                    case
                        when a.mirroring = 1 then lower(substr(a.road, 1, (strpos(a.road,'.') - 1)) || '_M')
                        else lower(substr(a.road, 1, (strpos(a.road,'.') - 1)))
                    end as road,
                    case
                        when a.mirroring = 1 then lower(substr(a.arrow, 1, (strpos(a.arrow,'.') - 1)) || '_M')
                        else lower(substr(a.arrow, 1, (strpos(a.arrow,'.') - 1)))
                    end as arrow
                from (
                    select a.*
                    from jv a
                    left join (
                               select id
                               from (
                                    select a.*,b.id as bid
                                    from jv a
                                    left join nw b
                                    on a.f_trpelid_mnl = b.id
                                ) a
                                where bid is null
                            union
                                select id
                                from (
                                      select a.*,b.id as bid
                                    from jv a
                                    left join nw b
                                    on a.t_trpelid_mnl = b.id
                                ) a
                                where bid is null
                            ) b
                            on a.id = b.id
                            where b.id is null
                        ) a
                        left join temp_signpost_path_links b
                        on a.f_trpelid_mnl = b.in_link_id
                            and a.t_trpelid_mnl = b.out_link_id
                    ) as a
            order by nodeid, inlinkid, outlinkid, passlid,patternno, arrowno
            )
             """

        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()

        self.log.info('end to make middle spotguide data.')
        return 0

    def __Importjv2db(self):
        self.log.info('Now it is import jv original data into db, normally this operation do once only.')

        self.CreateTable2('temp_ta_jv_tbl')

        ta_jv_sidefile = common.common_func.GetPath('ta_jv_sidefile')
        print ta_jv_sidefile
        file_list = os.listdir(ta_jv_sidefile)
        if ta_jv_sidefile and file_list:
            for current_file in file_list:
                objFileAnalyst = CFileAnalyst(ta_jv_sidefile + '\\' + current_file)
                listrecord = objFileAnalyst.analyse()

                for record in listrecord:
                    self.pg.insert('temp_ta_jv_tbl',
                                  (
                                   'image_name',
                                   'jdn_id',
                                   'pass_node_link_list'
                                  ),
                                  (
                                   record[1],
                                   int(record[3]),
                                   record[4:]
                                  )
                                  )

                self.pg.commit2()


class CFileAnalyst:
    def __init__(self, strDataPath):
        self.strDataPath = strDataPath

    def analyse(self):
        objFile = open(self.strDataPath)
        listline = objFile.readlines()
        objFile.close()

        listrecord = []
        for line in listline[1:]:
            line = line.strip()
            if line:
                record = self._analyseLine(line)
                if record:
                    listrecord.append(record)

        return listrecord

    def _analyseLine(self, line):

        listrow = line.split("|")

        return listrow


class comp_picture(object):
    ''' picture entity'''
    def __init__(self):
        self.gid = 0

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

    def setSharePicName(self, sharePicName):
        self.sharePicName = sharePicName

    def getSharePicName(self):
        return self.sharePicName


class GeneratorPicBinary(object):

    def __init__(self):
        self.conn = psycopg2.connect(''' host='172.26.179.195'
                        dbname='AP_NOSTRA_201307_0054_0002'
                        user='postgres' password='pset123456' ''')
        self.pgcur2 = self.conn.cursor()

    def selectData(self):
        self.pgcur2.execute('''SELECT image_name
                             FROM temp_ta_jv_tbl;''')
        rows = self.pgcur2.fetchall()
        pics = []
        gid = 1
        for row in rows:
            pic = comp_picture()
            pic.setGid(gid)
            pic.setSharePicName(row[1])
            pic.setDayName(row[1] + 'd02')
            pic.setNightName(row[1] + 'n02')
            pics.append(pic)
            gid = gid + 1
        return pics

    def makeJunctionResultTable(self, dirFileDir, destFileDir):
        pictures = self.selectData()
        for pic in pictures:
            destFile = os.path.join(destFileDir,
                                    pic.getSharePicName() + ".dat")
            if os.path.isdir(dirFileDir):
                # day and nigth illust
                if  os.path.isfile(destFile) == False:
                    dayPicPath = os.path.join(dirFileDir, pic.getDayName())
                    nightPicPath = os.path.join(dirFileDir, pic.getNightName())
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
                    print len(resultBuffer)
                    dayFis.close()
                    nightFis.close()
                    fos.write(resultBuffer)
                    fos.close()

if __name__ == '__main__':
    test = GeneratorPicBinary()
    test.makeJunctionResultTable("C:\\dir", "C:\\test")
    pass
