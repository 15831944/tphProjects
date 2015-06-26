'''
Created on 2012-1-9

@author: liuxinxing
'''
from base.rdb_ItemBase import ItemBase
from common import rdb_log

class rdb_poi_name(ItemBase):
    '''
    classdocs
    '''
    def __init__(self):
        '''
        Constructor
        '''
        ItemBase.__init__(self, 'Poiname')
        
    def Do(self):
        self.CreateTable2('rdb_poi_name')
        return 0
    
        rdb_log.log('POI Name', 'Start make POI names.', 'info')
        
        sqlcmd = """
                    INSERT INTO rdb_poi_name(
                            kind_code, name_type, name_prior
                           , low_level, high_level, name_id, the_geom)
                    (
                      SELECT case when kind_code is null then 1 else kind_code end, 
                             name_type, priority, 
                             low_level, high_level, new_name_id, a.the_geom
                       FROM poi_tbl as a
                       LEFT JOIN temp_kind_code as k
                       ON lower(a.poi_type) = lower(k.ni_code)
                       LEFT JOIN temp_name_dictionary as d
                       ON  a.name_id = d.old_name_id
                    );
                  """
                        
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        # create index
        self.CreateIndex2('rdb_poi_name_display_level_idx')
        self.CreateIndex2('rdb_poi_name_level_prior_kind_idx')
        self.CreateIndex2('rdb_poi_name_the_geom_idx')
        
        rdb_log.log('POI Name', 'End make POI names.', 'info')
        
        return 0
        
    def Do_JPN(self):
        rdb_log.log('POI Name', 'Start make POI names.', 'info')
        
        sqlcmd = """
                    INSERT INTO rdb_poi_name(
                            kind_code, name_type, name_prior
                           , low_level, high_level, name_id, the_geom)
                    (
                      SELECT (case when kind_code is null then ant.annocls_c else kind_code end)
                            , name_type, priority
                            ,low_level, high_level, name_id, ant.the_geom
                       FROM
                       (select * from %s_xda
                           where lineanno_f = 0
                       )AS xda
                       LEFT JOIN %s_ant as ant
                       ON xda.annoid = ant.objectid
                       LEFT JOIN temp_kind_code as k
                       ON  cast(ant.annocls_c as character varying) = k.ni_code
                       LEFT JOIN temp_poi_name as n
                       ON n.objectid = ant.objectid and n.layer_name = '%s' and n.seq_nm = xda.nameid
                    )
                  """
                  
        if self.CreateTable2('rdb_poi_name') == -1:
            rdb_log.log('POI Name', 'Create table rdb_poi_name failed.', 'error') 
            self.pg.close2()
            exit(1)
        
        rdb_log.log('POI Name', 'Now it is inserting POI name...', 'info')
        maptypes = ['topmap', 'middlemap', 'basemap', 'citymap']      
        for maptype in maptypes:
            if self.pg.execute2(sqlcmd % (maptype, maptype, maptype)) == -1:
                rdb_log.log('POI Name', 'Insert POI name failed.', 'error') 
                self.pg.close2()
                exit(1)
        
        self.pg.commit2()
        
        # create index
        self.CreateIndex2('rdb_poi_name_display_level_idx')
        self.CreateIndex2('rdb_poi_name_level_prior_kind_idx')
        self.CreateIndex2('rdb_poi_name_the_geom_idx')
        
        rdb_log.log('POI Name', 'End make POI names.', 'info')
        
        return 0
        