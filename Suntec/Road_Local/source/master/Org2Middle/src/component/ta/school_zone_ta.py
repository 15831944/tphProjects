# -*- coding: UTF8 -*-
'''
Created on 2015-3-13
@author: zhaojie
'''

import component.component_base

class comp_school_zone_ta(component.component_base.comp_base):
    '''
    classdocs
    '''


    def __init__(self):
        '''
        Constructor 
        '''
        component.component_base.comp_base.__init__(self, 'school_zone')
    
    #Creat table
    def _DoCreateTable(self):
        
        if not self.pg.IsExistTable('safety_zone_tbl'):
            self.CreateTable2('safety_zone_tbl')
        
        return 0
    #alter table    
    def _Do(self):
        if not self.pg.IsExistTable("org_school_zones") :
            self.log.warning('table is not exist!!!')
            return 0
        self.__get_speedlimit()
        self.__get_speedlimit_type()
        self.__get_guide_info_id()
        self.__insert_Safety_Zone()
        
        return 0
        
    def __get_speedlimit(self):            
        self.log.info('Begin get speedlimit.')
        
        self.CreateTable2('temp_school_zone_speed')
        sqlcmd = """
                insert into temp_school_zone_speed(id, dataset, speed, valdir)
                (
                    select shp_id::bigint, dataset, speed_array[1], valdir_array[1]
                    from
                    (
                        select dataset, shp_id, array_agg(b.speed) as speed_array, 
                                array_agg(valdir) as valdir_array
                        from org_school_zones as a
                        left join 
                        (
                            select id, seqnr, speed, valdir
                            from org_sr
                            where vt in (0, 11)
                            order by id, speed
                        )b
                        on a.shp_id::bigint = b.id
                        left join org_st as c
                        on b.id = c.id and b.seqnr = c.seqnr
                        where c.id is not null
                        group by dataset, shp_id
                    )temp
                );
                """
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        self.pg.commit2()
        
        return 0
    
    def __get_speedlimit_type(self):            
        self.log.info('Begin get speedlimit_type.')
        
        self.CreateIndex2('org_ta_id_idx')
        self.CreateTable2('temp_school_zone_a0')
        sqlcmd = """
                insert into temp_school_zone_a0(id, munit)
                (
                    SELECT distinct shp_id::bigint, (case when c.munit = 0 then 0
                                                when c.munit = 1 then 2
                                                else 1 end) as munit_temp
                    FROM org_school_zones as a
                    left join org_ta as b
                    on a.shp_id::bigint = b.id and b.aretyp = 1111
                    left join org_a0 as c
                    on b.areid = c.id
                    where c.munit is not null
                );
                """
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        self.pg.commit2()
        
        self.CreateIndex2('temp_school_zone_a0_id_idx')
        return 0
    
    def __get_guide_info_id(self):            
        self.log.info('Begin get guide_info_id.')

        self.CreateTable2('temp_school_zone_guide_id')
        self.CreateTable2('temp_school_zone_s_e_node')
        self.CreateIndex2('temp_school_zone_s_e_node_id_idx')
        self.CreateIndex2('temp_school_zone_s_e_node_speed_idx')
        self.CreateIndex2('temp_school_zone_s_e_node_s_node_idx')
        self.CreateIndex2('temp_school_zone_s_e_node_e_node_idx')
        
        self.CreateFunction2('mid_get_school_zone_connect_link')
        self.CreateFunction2('mid_get_school_zone_guide_id')
        
        self.pg.callproc('mid_get_school_zone_guide_id')
        self.pg.commit2()
        
        self.CreateIndex2('temp_school_zone_guide_id_id_idx')
        

        return 0
    
    def __insert_Safety_Zone(self):            
        self.log.info('Begin get insert into Safety_Zone.')
        
        sqlcmd = """
                delete from safety_zone_tbl
                where safety_type = 3;
                """
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        self.pg.commit2()
        
        sqlcmd = """
                insert into safety_zone_tbl(safetyzone_id,linkid, speedlimit, 
                                        speedunit_code, direction, safety_type)
                (
                    select c.guide_id as safetyzone_id,
                           a.id as linkid, 
                           speed as speedlimit, 
                           munit as speedunit_code, 
                           valdir as direction, 
                           3 as safety_type
                    from temp_school_zone_speed as a
                    left join temp_school_zone_a0 as b
                    on a.id = b.id
                    left join temp_school_zone_guide_id as c
                    on a.id = c.id
                );
                """
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        self.pg.commit2()
        

        return 0
    
    def __GetRows(self,sqlcmd):
        if sqlcmd:
            self.pg.execute2(sqlcmd)
            return self.pg.fetchall2()
        else:
            self.log.error('sqlcmd is null;')
    
    def _DoCheckValues(self):
        self.log.info('Begin CheckValues.')
        
        sqlcmd = """
                select count(*)
                from safety_zone_tbl
                where safety_type =3 and (speedlimit is null 
                                       or speedunit_code not in (1,2)
                                       or direction not in (1,2,3));
                """
        row_num = (self.__GetRows(sqlcmd))[0][0]
        
        if row_num != 0:
            self.log.error('school_zone type is error! %d',row_num)
        
        return 0
    def _DoCheckNumber(self):
        self.log.info('Begin CheckNumber.')
        
        sqlcmd = """
                select count(*)
                from safety_zone_tbl
                where safety_type =3;
                """
        row_num_tbl = (self.__GetRows(sqlcmd))[0][0]
        
        if not self.pg.IsExistTable("org_school_zones") :
            row_num_org = 0
        else:
            sqlcmd = """             
                    select count(*)
                    from org_school_zones;
                    """
            row_num_org = (self.__GetRows(sqlcmd))[0][0]
        
        if row_num_tbl < row_num_org:
            self.log.warning('org_data num is %d, table_data num is %d',row_num_org, row_num_tbl)
            
        return 0

    def _DoCheckLogic(self):
        self.log.info('Begin CheckLogic.')
        
                
        sqlcmd = """
                select count(*)
                from safety_zone_tbl as a
                left join link_tbl as b
                on a.linkid = b.link_id and b.one_way_code = 4              
                where safety_type =3 and b.link_id is not null
                """
        row_num = (self.__GetRows(sqlcmd))[0][0]
        
        if row_num <> 0:
            self.log.warning('school_zone link oneway is error! %d',row_num)
        
        return 0    

        