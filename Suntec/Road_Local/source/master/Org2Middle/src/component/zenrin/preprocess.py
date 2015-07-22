'''
Created on 2015-7-9

@author: liuxinxing
'''

import component.component_base
import common.ntsamp_to_lhplus
from common import cache_file

class comp_proprocess_zenrin(component.component_base.comp_base):
    
    def __init__(self):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self, 'PreProcess') 
            
    def _Do(self):
        self.__prepare_for_convert_coord()
        self.__fix_phoneme()
        
        return 0
    
    def __prepare_for_convert_coord(self):
        self.CreateFunction2('mid_transtwd67totwd97_bentu')
        self.CreateFunction2('mid_transtwd67totwd97_jinmen')
        self.CreateFunction2('mid_transtwd67totwd97_lianjiang')
        sqlcmd='''
                delete from spatial_ref_sys where srid = 999902;
                insert into spatial_ref_sys
                values (999902, 'EPSG', 999902,
                        'GEOGCS["TWD67",DATUM["Taiwan_Datum_1967",SPHEROID["GRS 1967 Modified",6378160,298.25,AUTHORITY["EPSG","7050"]],AUTHORITY["EPSG","1025"]],PRIMEM["Greenwich",0,AUTHORITY["EPSG","8901"]],UNIT["degree",0.01745329251994328,AUTHORITY["EPSG","9122"]],AUTHORITY["EPSG","3821"]]',
                        '+proj=longlat +ellps=aust_SA +towgs84=-294.142671,-78.211380,-244.115627,16.919001,-25.619608,5.519133,3.903426 +no_defs');
             
            
                delete from spatial_ref_sys where srid = 999901;
                insert into spatial_ref_sys
                values (999901, 'EPSG', 999901,
                        'GEOGCS["TWD67",DATUM["Taiwan_Datum_1967",SPHEROID["GRS 1967 Modified",6378160,298.25,AUTHORITY["EPSG","7050"]],AUTHORITY["EPSG","1025"]],PRIMEM["Greenwich",0,AUTHORITY["EPSG","8901"]],UNIT["degree",0.01745329251994328,AUTHORITY["EPSG","9122"]],AUTHORITY["EPSG","3821"]]',
                        '+proj=longlat +ellps=aust_SA +towgs84=-195.579304,-79.470107,-99.160512,13.400337,-29.403993,7.830643,1.407280 +no_defs');
             
            
                delete from spatial_ref_sys where srid = 999900;
                insert into spatial_ref_sys
                values (999900, 'EPSG', 999900,
                        'GEOGCS["TWD67",DATUM["Taiwan_Datum_1967",SPHEROID["GRS 1967 Modified",6378160,298.25,AUTHORITY["EPSG","7050"]],AUTHORITY["EPSG","1025"]],PRIMEM["Greenwich",0,AUTHORITY["EPSG","8901"]],UNIT["degree",0.01745329251994328,AUTHORITY["EPSG","9122"]],AUTHORITY["EPSG","3821"]]',
                        '+proj=longlat +ellps=aust_SA +towgs84=-145.216113,-183.012890,-161.517550,2.114225,-8.671589,17.827958,21.788069 +no_defs');            
                '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
    def __fix_phoneme(self):
        self.log.info('fix phoneme...')
        
        #
        #self.__union_seprate_bpmf()
        phoneme_column_list = [
                               ('org_rname_bpmf', 'snamey'),
                               ('org_rname_bpmf', 'tnamey'),
                               ('org_tunnelname', 'tnamey'),
                               ('org_underpass', 'bpmf'),
                               ('org_poi', 'offi_name_py'),
                               ('org_poi_ext', 'offi_name_py'),
                               ('org_cross_name', 'reading'),
                               ('org_dest_guide', 'reading'),
                               ]
        
        for (table, column) in phoneme_column_list:
            self.__fix_phoneme_table_column(table,column)
    
    def __union_seprate_bpmf(self):
        #
        sqlcmd = """
        drop table if exists temp_org_cross_name;
        create table temp_org_cross_name
        as
        select  min(gid) as gid,
                meshcode, crossnmno, tonodeno, fromnodeno,
                array_to_string(array_agg(name), '') as name,
                array_to_string(array_agg(reading), '') as reading
        from
        (
            select meshcode, crossnmno, tonodeno, fromnodeno, nextflg, gid, "name", reading
            from org_cross_name
            order by meshcode, crossnmno, tonodeno, fromnodeno, nextflg
        )as t
        group by meshcode, crossnmno, tonodeno, fromnodeno;
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        #
        sqlcmd = """
        drop table if exists temp_org_dest_guide;
        create table temp_org_dest_guide
        as
        select  min(gid) as gid,
                meshcode, destnmno, stotlinkno, ttoelinkno, snodeno, tnodeno, enodeno,
                array_to_string(array_agg(name), '') as name,
                array_to_string(array_agg(reading), '') as reading
        from
        (
            select meshcode, destnmno, stotlinkno, ttoelinkno, snodeno, tnodeno, enodeno, nextflg, gid, "name", reading
            from org_dest_guide
            order by meshcode, destnmno, stotlinkno, ttoelinkno, snodeno, tnodeno, enodeno, nextflg
        )as t
        group by meshcode, destnmno, stotlinkno, ttoelinkno, snodeno, tnodeno, enodeno;
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
    
    def __fix_phoneme_table_column(self, table, column):
        self.log.info('fix phoneme for %s.%s...' % (table,column))
        self.__get_temp_phoneme_table(table, column)
        self.__update_phoneme_table(table, column)
    
    def __get_temp_phoneme_table(self, table, column):
        # gid
        if not self.pg.IsExistColumn(table, 'gid'):
            sqlcmd = """
                    alter table %s add column gid serial primary key;
                    """ % table
            self.pg.execute2(sqlcmd)
            self.pg.commit2()
        
        #
        temp_file_obj = cache_file.open(table)
        sqlcmd = """
                drop table if exists temp_phoneme_%s;
                create table temp_phoneme_%s
                (
                    gid bigint,
                    pym varchar
                );
                """ % (table, table)
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        # 
        sqlcmd = """
                select gid, 'BPMF', %s
                from %s
                where %s is not null;
                """ % (column, table, column)
        phlist = self.get_batch_data(sqlcmd)
        for ph in phlist:
            gid = ph[0]
            language = ph[1]
            phoneme = ph[2]
            phoneme_pym = common.ntsamp_to_lhplus.ntsamapa2lhplus(language, phoneme)
            phoneme_pym = phoneme_pym.replace('\\', '\\\\')
            temp_file_obj.write('%s\t%s\n' % (str(gid), phoneme_pym))
        
        #
        temp_file_obj.seek(0)
        self.pg.copy_from2(temp_file_obj, 'temp_phoneme_%s' % table)
        self.pg.commit2()
        sqlcmd = """
                create index temp_phoneme_%s_gid_idx
                    on temp_phoneme_%s
                    using btree
                    (gid);
                """ % (table, table)
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        cache_file.close(temp_file_obj,True)
    
    def __update_phoneme_table(self, table, column):
        # update
        sqlcmd = """
                alter table %s drop column if exists %s_pym;
                alter table %s add column %s_pym varchar;
                """ % (table, column, table, column)
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        sqlcmd = """
                update %s as a set %s_pym = b.pym
                from temp_phoneme_%s as b
                where a.gid = b.gid
                """ % (table, column, table)
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        