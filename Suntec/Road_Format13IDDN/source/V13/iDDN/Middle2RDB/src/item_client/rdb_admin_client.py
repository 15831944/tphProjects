'''
Created on 2011-12-21

@author: liuxinxing
'''
from base.rdb_ItemBase import ItemBase
from common import rdb_log

class rdb_admin_client(ItemBase):
    '''
    classdocs
    '''


    def __init__(self):
        '''
        Constructor
        '''
        ItemBase.__init__(self, 'admin_client'
                          , 'rdb_tile_admin_zone'
                          , '*'
                          , 'rdb_tile_admin_zone_client'
                          , '*'
                          , False)

    def Do(self):   
        self.CreateTable2('rdb_tile_admin_zone_client')
        
        self.CreateFunction2('rdb_integer_2_octal_bytea')
        self.CreateFunction2('rdb_smallint_2_octal_bytea')
        self.CreateFunction2('rdb_makepolygon2bytea')

        if self.CreateFunction2('lonlat2pixel_tile') == -1:
            return -1
        if self.CreateFunction2('lonlat2pixel') == -1:
            return -1
        if self.CreateFunction2('tile_bbox') == -1:
            return -1
        if self.CreateFunction2('world2lonlat') == -1:
            return -1
        if self.CreateFunction2('lonlat2world') == -1:
            return -1
        if self.CreateFunction2('pixel2world') == -1:
            return -1
        if self.CreateFunction2('rdb_smallint_2_octal_bytea') == -1:
            return -1
        self.CreateFunction2('rdb_makepolygon2pixelbytea')
        sqlcmd = """
                    insert into rdb_tile_admin_zone_client(tile_id, ad_code, geom_blob)
                    (
                        select  tile_id, ad_code, 
                                (
                                case when the_geom is null then null
                                     else rdb_makepolygon2pixelbytea(14::smallint, ((tile_id >> 14) & 16383), (tile_id & 16383), the_geom) 
                                end
                                ) as geom_blob
                        from rdb_tile_admin_zone
                        order by gid
                    );
                 """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
         
        return 0
