'''
Created on 2014-07-31

@author: yuanrui
'''

import platform.TestCase
import json

class CCheckTrf14(platform.TestCase.CTestCase):

    def _do(self):
        
        sqlcmd = """
        select count(*) from rdb_trf_org2rdb;
            """
        
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt != 0)
      
class CCheckTrf4(platform.TestCase.CTestCase):

    def _do(self):
        
        sqlcmd = """
        select count(*) from rdb_region_trf_org2rdb_layer4;
            """
        
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt != 0)
    
class CCheckTrf6(platform.TestCase.CTestCase):

    def _do(self):
        
        sqlcmd = """
        select count(*) from rdb_region_trf_org2rdb_layer6;
            """
        
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt != 0)    
    
class CCheckAreaNo_NotNull(platform.TestCase.CTestCase):

    def _do(self):
        
        sqlcmd = """
        select count(*) from rdb_trf_tile_area_no;
            """
        
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt != 0)      

class CCheckAreaNo_Null(platform.TestCase.CTestCase):

    def _do(self):
        
        sqlcmd = """
        select count(*) from rdb_trf_tile_area_no;
            """
        
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0)    
    
class CCheckEvent(platform.TestCase.CTestCase):

    def _do(self):
        
        sqlcmd = """
        select count(*) from rdb_trf_event;
            """
            
        if self.pg.execute(sqlcmd) == -1:
            return False
        else:
            rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
            return (rec_cnt != 0)        
    
class CCheckSupplementary(platform.TestCase.CTestCase):

    def _do(self):
        
        sqlcmd = """
        select count(*) from rdb_trf_supplementary;
            """
            
        if self.pg.execute(sqlcmd) == -1:
            return False
        else:
            rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
            return (rec_cnt != 0)
        
class CCheckLocationTable(platform.TestCase.CTestCase):

    def _do(self):
        
        sqlcmd = """
        select count(*) from rdb_trf_locationtable;
            """
            
        if self.pg.execute(sqlcmd) == -1:
            return False
        else:
            rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
            return (rec_cnt != 0)

class CCheckLocationTable_name(platform.TestCase.CTestCase):
    def _do(self):
        
        if self.pg.IsExistTable('rdb_trf_locationtable'):
            sqlcmd = """
                select road_name,first_name
                from rdb_trf_locationtable
                where location_type = 1;                 
                """
            datas = self.pg.get_batch_data(sqlcmd) 
    
            count_all = 0
            count_road = 0
            count_first = 0
            
            for data in datas:
                road_name = data[0]
                first_name = data[1]
    
                multi_name_info_road = json.loads(road_name)
                for one_name_info_road in multi_name_info_road:
    
                    val_road = one_name_info_road.get('val')
                    
                    count_all += 1
                    if val_road is None or val_road == '':
                            count_road = count_road + 1
    #                        self.logger.error("roadname: null")
    #                        return 0
    
                multi_name_info_first = json.loads(first_name)
                for one_name_info_first in multi_name_info_first:
    
                    val_first = one_name_info_first.get('val')
                    
                    if val_first is None or val_first == '':
                            count_first = count_first + 1
    #                        self.logger.error("firstname: null")
    #                        return 0

            if (count_road * 1.0)/count_all > 0.05 or (count_first* 1.0/count_all) > 0.05:
                self.logger.error("too much: roadname is null or firstname is null. roadname is null:%d, firstname is null:%d" % (count_road,count_first))
                return 0
            
            return 1
        
        return 1  
    
class CCheckTrfLocationCode(platform.TestCase.CTestCase):

    def _do(self):
        
        sqlcmd = """
        select count(*) from rdb_trf_org2rdb a
        left join rdb_trf_locationtable b
        on a.area_code = b.country_code and a.extra_flag = b.table_no and a.infra_id = b.location_code
        where b.location_code is null;
            """
            
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0)

class CCheckRTICLinkGeom(platform.TestCase.CTestCase):

    def _do(self):
        
        sqlcmd = """
            select count(*) from temp_rtic_link_order2 
            where (s_fraction < 0 or e_fraction < 0) or (s_fraction > 1 or e_fraction > 1) 
            """
            
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0) 

class CCheckRTICRecNum(platform.TestCase.CTestCase):

    def _do(self):
        
        sqlcmd_org = """
            select count(*) from (
                select distinct meshcode,kind,rticid from temp_rtic_link_temp
            ) a;
            """           
        rec_cnt_org = self.pg.getOnlyQueryResult(sqlcmd_org)
        
        sqlcmd_rdb = """
            select count(*) from (
                select distinct area_code,extra_flag,infra_id from rdb_trf_org2rdb
            ) a;
            """            
        rec_cnt_rdb = self.pg.getOnlyQueryResult(sqlcmd_rdb)        
        
        return (rec_cnt_org - rec_cnt_rdb) * 1.0 /  rec_cnt_org < 0.005
    
class CCheckRTICLength_NI(platform.TestCase.CTestCase):

    def _do(self):
        
        sqlcmd_org = """
            with temp_rtic_link_all_length as
            (
                SELECT ST_Length_Spheroid( b.the_geom,'SPHEROID("WGS_84", 6378137, 298.257223563)' 
                       ) as link_length
                FROM (
                    select distinct linkid from org_rtic_incar 
                ) a
                left join org_r b
                on a.linkid = b.id
            )
            select sum(link_length) from temp_rtic_link_all_length;
            """ 
        rec_cnt_org = self.pg.getOnlyQueryResult(sqlcmd_org)
        
        sqlcmd_rdb = """
            with temp_rtic_link_all_length as
            (
                SELECT b.link_length
                FROM (
                    select distinct rdb_link_id from rdb_trf_org2rdb 
                ) a
                left join rdb_link b
                on a.rdb_link_id = b.link_id
            )
            select sum(link_length) from temp_rtic_link_all_length; 
            """ 
        rec_cnt_rdb = self.pg.getOnlyQueryResult(sqlcmd_rdb)        
        
        return (rec_cnt_org - rec_cnt_rdb) * 1.0 / rec_cnt_org < 0.005   
                 
class CCheckSequenceAndCount(platform.TestCase.CTestCase):

    def _do(self):
        
        sqlcmd = """
            select count(*) from 
            (    select type_flag, area_code, extra_flag, infra_id, dir, group_id, max(seq) as sum
                from rdb_trf_org2rdb group by type_flag, area_code, extra_flag, infra_id, dir, group_id
            ) a
            left join 
            (
                select type_flag, area_code, extra_flag, infra_id, dir, group_id, count(seq) as sum
                from rdb_trf_org2rdb group by type_flag, area_code, extra_flag, infra_id, dir, group_id
            ) b
            on a.area_code = b.area_code and a.extra_flag = b.extra_flag and a.infra_id = b.infra_id and a.dir = b.dir and a.group_id = b.group_id
            where a.sum <> b.sum 
            """
            
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0)  

class CCheckFraction(platform.TestCase.CTestCase):

    def _do(self):
        
        sqlcmd = """
            select count(*) from rdb_trf_org2rdb a
            left join (
                select type_flag,area_code,extra_flag,infra_id,dir,group_id
                    ,min(seq) as min, max(seq) as max
                from rdb_trf_org2rdb  where pos_type = 0
                group by type_flag, area_code, extra_flag, infra_id, dir,group_id
            ) b
            on a.area_code = b.area_code and a.extra_flag = b.extra_flag 
            and a.infra_id = b.infra_id and a.dir = b.dir and a.group_id = b.group_id
            where a.pos_type = 0 and
            ((a.e_fraction <> 65535 and a.link_dir = 0 and not (a.seq = b.max))
            or (a.s_fraction <> 0 and a.link_dir = 0 and not (a.seq = min))
            or (a.e_fraction <> 65535 and a.link_dir = 1 and not (a.seq = min))
            or (a.s_fraction <> 0 and a.link_dir = 1 and not (a.seq = b.max)))
            union
            select count(*) from rdb_trf_org2rdb a
            left join (
                select type_flag,area_code,extra_flag,infra_id,dir,group_id
                    ,min(seq) as min, max(seq) as max
                from rdb_trf_org2rdb  where pos_type = 1
                group by type_flag, area_code, extra_flag, infra_id, dir,group_id
            ) b
            on a.area_code = b.area_code and a.extra_flag = b.extra_flag 
            and a.infra_id = b.infra_id and a.dir = b.dir and a.group_id = b.group_id
            where a.pos_type = 1 and
            ((a.e_fraction <> 65535 and a.link_dir = 0 and not (a.seq = b.max))
            or (a.s_fraction <> 0 and a.link_dir = 0 and not (a.seq = min))
            or (a.e_fraction <> 65535 and a.link_dir = 1 and not (a.seq = min))
            or (a.s_fraction <> 0 and a.link_dir = 1 and not (a.seq = b.max)))            
            
            """
            
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0) 
    
class CCheckCtnFlag(platform.TestCase.CTestCase):

    def _do(self):
        
        sqlcmd = """
            select count(*) from rdb_trf_org2rdb a
            left join (
                select * from rdb_trf_org2rdb where ctn_flag = 0
            ) b
            on a.area_code = b.area_code and a.extra_flag = b.extra_flag 
            and a.infra_id = b.infra_id and a.dir = b.dir and a.group_id = b.group_id 
            and a.seq = b.seq + 1
            where b.rdb_link_id is not null 
            and (a.rdb_link_id <> b.rdb_link_id or 
                not ((a.pos_type = 1 and b.pos_type = 0) 
                or (a.pos_type = 0 and b.pos_type = 1))
            )
            """
            
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0)     
    
class CCheckPreNextFrcation(platform.TestCase.CTestCase):

    def _do(self):
        
        sqlcmd = """
            select count(*) from rdb_trf_org2rdb a
            left join rdb_trf_org2rdb b
            on a.area_code = b.area_code and a.extra_flag = b.extra_flag 
            and a.infra_id = b.infra_id and a.dir = b.dir and a.group_id = b.group_id 
            and a.seq = b.seq - 1 and a.rdb_link_id = b.rdb_link_id
            where b.rdb_link_id is not null and b.s_fraction <> a.e_fraction
            and a.ctn_flag != 0;
            """
            
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0)      
    
class CCheckDupli(platform.TestCase.CTestCase):

    def _do(self):
        
        sqlcmd = """
            select count(*) from (
                select area_code,extra_flag,infra_id,dir,group_id,rdb_link_id,link_dir,s_fraction 
                from rdb_trf_org2rdb 
                group by area_code,extra_flag,infra_id,dir,group_id,rdb_link_id,link_dir,s_fraction 
                having count(*) >  1
            ) a 
            """
            
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0)    
    
class CCheckFractionValue(platform.TestCase.CTestCase):

    def _do(self):
        
        sqlcmd = """
            select count(*) from rdb_trf_org2rdb 
            where (s_fraction = 0 and e_fraction = 0) or (s_fraction = 65535 and e_fraction = 65535)            
            """
            
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0)    
     
class CCheckFractionMerge(platform.TestCase.CTestCase):

    def _do(self):
        
        sqlcmd = """
            select count(*) from rdb_trf_org2rdb a
            left join (
                select area_code,extra_flag,infra_id,dir,group_id,rdb_link_id,
                    round((s_fraction * 65535)::numeric,0) as s_fraction,
                    round((e_fraction * 65535)::numeric,0) as e_fraction,
                    count
                    from (
                    select area_code,extra_flag,infra_id,dir,group_id,rdb_link_id,unnest(s_fraction_array) as s_fraction
                        ,unnest(e_fraction_array) as e_fraction,count 
                    from (
                        select area_code,extra_flag,infra_id,dir,group_id,rdb_link_id
                            ,array_agg(s_fraction) as s_fraction_array
                            ,array_agg(e_fraction) as e_fraction_array,count(*) as count
                         from (
                            select area_code,extra_flag,infra_id,dir,group_id,rdb_link_id, link_dir,
                                case when s_fraction > e_fraction then e_fraction else s_fraction 
                                end as s_fraction,
                                case when s_fraction > e_fraction then s_fraction else e_fraction 
                                end as e_fraction
                            from temp_trf_org2rdb_prepare 
                        ) a group by area_code,extra_flag,infra_id,dir,group_id,rdb_link_id,link_dir
                    ) c
                ) pre
            ) b
            on a.area_code = b.area_code and a.extra_flag = b.extra_flag and a.infra_id = b.infra_id 
            and a.group_id = b.group_id and a.dir = b.dir and a.rdb_link_id = b.rdb_link_id
            where (a.s_fraction <> b.s_fraction or a.e_fraction <> b.e_fraction) and b.count = 1
           
            union
           
            select count(*) from rdb_trf_org2rdb a
            left join (
                select area_code,extra_flag,infra_id,dir,group_id,rdb_link_id
                    ,unnest(s_fraction_array) as s_fraction
                    ,unnest(e_fraction_array) as e_fraction,count 
                from (
                    select area_code,extra_flag,infra_id,dir,group_id,rdb_link_id
                        ,array_agg(s_fraction) as s_fraction_array,
                        array_agg(e_fraction) as e_fraction_array,count(*) as count 
                    from temp_trf_org2rdb_prepare
                    group by area_code,extra_flag,infra_id,dir,group_id,rdb_link_id
                ) c
            ) b
            on a.area_code = b.area_code and a.extra_flag = b.extra_flag 
            and a.infra_id = b.infra_id and a.dir = b.dir and a.group_id = b.group_id 
            and a.rdb_link_id = b.rdb_link_id
            where (a.s_fraction = b.s_fraction and a.e_fraction = b.e_fraction) and b.count <> 1                                                                               
            """
            
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0)
    
class CCheckLinkDir(platform.TestCase.CTestCase):

    def _do(self):
        
        sqlcmd = """
            select count(*) from rdb_trf_org2rdb a
            left join temp_trf_org2rdb_prepare b
            on a.area_code = b.area_code and a.extra_flag = b.extra_flag 
            and a.infra_id = b.infra_id and a.dir = b.dir and a.group_id = b.group_id 
            and a.rdb_link_id = b.rdb_link_id
            where a.link_dir <> b.link_dir ; 
            """
            
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0)    
    
class CCheckDupli_4(platform.TestCase.CTestCase):

    def _do(self):
        
        sqlcmd = """
        select count(*) from (
            select area_code,extra_flag,infra_id,dir,group_id,rdb_link_id,link_dir,s_fraction 
            from rdb_region_trf_org2rdb_layer4 
            group by area_code,extra_flag,infra_id,dir,group_id,rdb_link_id,link_dir,s_fraction
            having count(*) >  1
        ) a
            """
            
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0)    

class CCheckDupli_6(platform.TestCase.CTestCase):

    def _do(self):
        
        sqlcmd = """
        select count(*) from (
            select area_code,extra_flag,infra_id,dir,group_id,rdb_link_id,link_dir,s_fraction 
            from rdb_region_trf_org2rdb_layer6 
            group by area_code,extra_flag,infra_id,dir,group_id,rdb_link_id,link_dir,s_fraction
            having count(*) >  1
        ) a
            """
            
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0)
        
class CCheckFractionValue_4(platform.TestCase.CTestCase):

    def _do(self):
        
        sqlcmd = """
        select count(*) from rdb_region_trf_org2rdb_layer4 
        where (s_fraction < 0 or e_fraction < 0) or (s_fraction > 65535 or e_fraction > 65535)
            """
            
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0)    
    
class CCheckFractionValid_4(platform.TestCase.CTestCase):

    def _do(self):
        
        sqlcmd = """
        select count(*) from rdb_region_trf_org2rdb_layer4 
        where (s_fraction = 0 and e_fraction = 0) or (s_fraction = 65535 and e_fraction = 65535)
            """
            
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0)    
    
class CCheckFractionValue_6(platform.TestCase.CTestCase):

    def _do(self):
        
        sqlcmd = """
        select count(*) from rdb_region_trf_org2rdb_layer6 
        where (s_fraction < 0 or e_fraction < 0) or (s_fraction > 65535 or e_fraction > 65535)
            """
            
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0)    
    
class CCheckFractionValid_6(platform.TestCase.CTestCase):

    def _do(self):
        
        sqlcmd = """
        select count(*) from rdb_region_trf_org2rdb_layer6 
        where (s_fraction = 0 and e_fraction = 0) or (s_fraction = 65535 and e_fraction = 65535)
            """
            
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0)      
    
class CCheckFractionMerge_4(platform.TestCase.CTestCase):

    def _do(self):
        
        sqlcmd = """
        select count(*) from rdb_region_trf_org2rdb_layer4 a
        left join (
            select area_code,extra_flag,infra_id,dir,group_id,rdb_link_id,
                s_fraction,
                e_fraction,
                count
            from (
                select area_code,extra_flag,infra_id,dir,group_id
                    ,rdb_link_id,unnest(s_fraction_array) as s_fraction
                    ,unnest(e_fraction_array) as e_fraction,count 
                from (
                    select area_code,extra_flag,infra_id,dir,group_id
                        ,rdb_link_id,array_agg(s_fraction) as s_fraction_array
                        ,array_agg(e_fraction) as e_fraction_array
                        ,count(*) as count
                     from temp_region_trf_org2rdb_layer4_prepare6
                     group by area_code,extra_flag,infra_id,dir,group_id,rdb_link_id
                ) c
            ) pre
        ) b
        on a.area_code = b.area_code and a.extra_flag = b.extra_flag 
        and a.infra_id = b.infra_id and a.dir = b.dir and a.group_id = b.group_id 
        and a.rdb_link_id = b.rdb_link_id
        where (a.s_fraction <> b.s_fraction or a.e_fraction <> b.e_fraction) and b.count = 1
        """
            
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0)     
    
class CCheckFractionMerge_6(platform.TestCase.CTestCase):

    def _do(self):
        
        sqlcmd = """
        select count(*) from rdb_region_trf_org2rdb_layer6 a
        left join (
            select area_code,extra_flag,infra_id,dir,group_id,rdb_link_id,
                s_fraction,
                e_fraction,
                count
            from (
                select area_code,extra_flag,infra_id,dir,group_id
                    ,rdb_link_id,unnest(s_fraction_array) as s_fraction
                    ,unnest(e_fraction_array) as e_fraction,count 
                from (
                    select area_code,extra_flag,infra_id,dir,group_id
                        ,rdb_link_id,array_agg(s_fraction) as s_fraction_array
                        ,array_agg(e_fraction) as e_fraction_array
                        ,count(*) as count
                     from temp_region_trf_org2rdb_layer6_prepare6
                     group by area_code,extra_flag,infra_id,dir,group_id,rdb_link_id
                ) c
            ) pre
        ) b
        on a.area_code = b.area_code and a.extra_flag = b.extra_flag 
        and a.infra_id = b.infra_id and a.dir = b.dir and a.group_id = b.group_id 
        and a.rdb_link_id = b.rdb_link_id
        where (a.s_fraction <> b.s_fraction or a.e_fraction <> b.e_fraction) and b.count = 1
        """
            
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0)       
    
class CCheckSequenceAndCount_4(platform.TestCase.CTestCase):

    def _do(self):
        
        sqlcmd = """
        select count(*) from 
        (    select type_flag, area_code, extra_flag, infra_id, dir,group_id,max(seq) as sum
            from rdb_region_trf_org2rdb_layer4 
            group by type_flag, area_code, extra_flag, infra_id, dir,group_id
        ) a
        left join 
        (
            select type_flag, area_code, extra_flag, infra_id, dir,group_id,count(seq) as sum
            from rdb_region_trf_org2rdb_layer4 
            group by type_flag, area_code, extra_flag, infra_id, dir,group_id
        ) b
        on a.area_code = b.area_code and a.extra_flag = b.extra_flag 
        and a.infra_id = b.infra_id and a.dir = b.dir and a.group_id = b.group_id
        where a.sum <> b.sum         
         """
            
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0)     

class CCheckSequenceAndCount_6(platform.TestCase.CTestCase):

    def _do(self):
        
        sqlcmd = """
        select count(*) from 
        (    select type_flag, area_code, extra_flag, infra_id, dir,group_id,max(seq) as sum
            from rdb_region_trf_org2rdb_layer6 
            group by type_flag, area_code, extra_flag, infra_id, dir,group_id
        ) a
        left join 
        (
            select type_flag, area_code, extra_flag, infra_id, dir,group_id,count(seq) as sum
            from rdb_region_trf_org2rdb_layer6 
            group by type_flag, area_code, extra_flag, infra_id, dir,group_id
        ) b
        on a.area_code = b.area_code and a.extra_flag = b.extra_flag 
        and a.infra_id = b.infra_id and a.dir = b.dir and a.group_id = b.group_id
        where a.sum <> b.sum         
         """
            
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0) 
    
class CCheckFraction_4(platform.TestCase.CTestCase):

    def _do(self):
        
        sqlcmd = """
            select count(*) from rdb_region_trf_org2rdb_layer4 a
            left join (
                select type_flag,area_code,extra_flag,infra_id,dir,group_id
                    ,min(seq) as min, max(seq) as max
                from rdb_region_trf_org2rdb_layer4  where pos_type = 0
                group by type_flag, area_code, extra_flag, infra_id, dir,group_id
            ) b
            on a.area_code = b.area_code and a.extra_flag = b.extra_flag 
            and a.infra_id = b.infra_id and a.dir = b.dir and a.group_id = b.group_id
            where a.pos_type = 0 and
            ((a.e_fraction <> 65535 and a.link_dir = 0 and not (a.seq = b.max))
            or (a.s_fraction <> 0 and a.link_dir = 0 and not (a.seq = min))
            or (a.e_fraction <> 65535 and a.link_dir = 1 and not (a.seq = min))
            or (a.s_fraction <> 0 and a.link_dir = 1 and not (a.seq = b.max)))
            union
            select count(*) from rdb_region_trf_org2rdb_layer4 a
            left join (
                select type_flag,area_code,extra_flag,infra_id,dir,group_id
                    ,min(seq) as min, max(seq) as max
                from rdb_region_trf_org2rdb_layer4  where pos_type = 1
                group by type_flag, area_code, extra_flag, infra_id, dir,group_id
            ) b
            on a.area_code = b.area_code and a.extra_flag = b.extra_flag 
            and a.infra_id = b.infra_id and a.dir = b.dir and a.group_id = b.group_id
            where a.pos_type = 1 and
            ((a.e_fraction <> 65535 and a.link_dir = 0 and not (a.seq = b.max))
            or (a.s_fraction <> 0 and a.link_dir = 0 and not (a.seq = min))
            or (a.e_fraction <> 65535 and a.link_dir = 1 and not (a.seq = min))
            or (a.s_fraction <> 0 and a.link_dir = 1 and not (a.seq = b.max)))        
         """
            
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0)     
    
class CCheckFraction_6(platform.TestCase.CTestCase):

    def _do(self):
        
        sqlcmd = """
            select count(*) from rdb_region_trf_org2rdb_layer6 a
            left join (
                select type_flag,area_code,extra_flag,infra_id,dir,group_id
                    ,min(seq) as min, max(seq) as max
                from rdb_region_trf_org2rdb_layer6  where pos_type = 0
                group by type_flag, area_code, extra_flag, infra_id, dir,group_id
            ) b
            on a.area_code = b.area_code and a.extra_flag = b.extra_flag 
            and a.infra_id = b.infra_id and a.dir = b.dir and a.group_id = b.group_id
            where a.pos_type = 0 and
            ((a.e_fraction <> 65535 and a.link_dir = 0 and not (a.seq = b.max))
            or (a.s_fraction <> 0 and a.link_dir = 0 and not (a.seq = min))
            or (a.e_fraction <> 65535 and a.link_dir = 1 and not (a.seq = min))
            or (a.s_fraction <> 0 and a.link_dir = 1 and not (a.seq = b.max)))
            union
            select count(*) from rdb_region_trf_org2rdb_layer6 a
            left join (
                select type_flag,area_code,extra_flag,infra_id,dir,group_id
                    ,min(seq) as min, max(seq) as max
                from rdb_region_trf_org2rdb_layer6  where pos_type = 1
                group by type_flag, area_code, extra_flag, infra_id, dir,group_id
            ) b
            on a.area_code = b.area_code and a.extra_flag = b.extra_flag 
            and a.infra_id = b.infra_id and a.dir = b.dir and a.group_id = b.group_id
            where a.pos_type = 1 and
            ((a.e_fraction <> 65535 and a.link_dir = 0 and not (a.seq = b.max))
            or (a.s_fraction <> 0 and a.link_dir = 0 and not (a.seq = min))
            or (a.e_fraction <> 65535 and a.link_dir = 1 and not (a.seq = min))
            or (a.s_fraction <> 0 and a.link_dir = 1 and not (a.seq = b.max)))        
         """
            
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0) 
    
class CCheckCtnFlag_4(platform.TestCase.CTestCase):

    def _do(self):
        
        sqlcmd = """
        select count(*) from rdb_region_trf_org2rdb_layer4 a
        left join (
            select * from rdb_region_trf_org2rdb_layer4 where ctn_flag = 0
        ) b
        on a.area_code = b.area_code and a.extra_flag = b.extra_flag 
        and a.infra_id = b.infra_id and a.dir = b.dir and a.group_id = b.group_id 
        and a.seq = b.seq + 1
        where b.rdb_link_id is not null 
        and (a.rdb_link_id <> b.rdb_link_id or 
            not ((a.pos_type = 1 and b.pos_type = 0) 
                or (a.pos_type = 0 and b.pos_type = 1))
        );       
         """
            
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0)
    
class CCheckCtnFlag_6(platform.TestCase.CTestCase):

    def _do(self):
        
        sqlcmd = """
        select count(*) from rdb_region_trf_org2rdb_layer6 a
        left join (
            select * from rdb_region_trf_org2rdb_layer6 where ctn_flag = 0
        ) b
        on a.area_code = b.area_code and a.extra_flag = b.extra_flag 
        and a.infra_id = b.infra_id and a.dir = b.dir and a.group_id = b.group_id 
        and a.seq = b.seq + 1
        where b.rdb_link_id is not null 
        and (a.rdb_link_id <> b.rdb_link_id or 
            not ((a.pos_type = 1 and b.pos_type = 0) 
                or (a.pos_type = 0 and b.pos_type = 1))
        );       
         """
            
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0)                                     

class CCheckTrafficInfoMustExists(platform.TestCase.CTestCase):

    def _do(self):
        
        sqlcmd = """
select * 
from 
rdb_rtic_id_matching as a
left join rdb_trf_org2rdb as b
on a.link_id_64=b.rdb_link_id
where a.link_attribute=1 and b.rdb_link_id is null;       
         """
            
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0)    

# ensure link_id_32 is unique.
class CCheckInt32LinkIdUnique(platform.TestCase.CTestCase):

    def _do(self):
        
        sqlcmd = """
ALTER TABLE rdb_rtic_id_matching ADD CONSTRAINT link_id_32_unique UNIQUE(link_id_32);  
         """
        try:
            self.pg.execute(sqlcmd)
        except:
            return False
        return True










