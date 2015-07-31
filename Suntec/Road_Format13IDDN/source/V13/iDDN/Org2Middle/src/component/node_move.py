# -*- coding: cp936 -*-
'''
Created on 2012-8-17

@author: liuxinxing
'''
import base

class comp_node_move(base.component_base.comp_base):
    '''
    Node Move
    '''

    def __init__(self):
        '''
        Constructor
        '''
        base.component_base.comp_base.__init__(self, 'Node_Move')        
        
    def _Do(self):
        
        self.CreateFunction2('lonlat2pixel_tile')
        self.CreateFunction2('lonlat2pixel')
        self.CreateFunction2('tile_bbox')
        self.CreateFunction2('world2lonlat')
        self.CreateFunction2('lonlat2world')
        self.CreateFunction2('pixel2world')
        
        self.CreateTable2('link_tbl_bak_moving')
        self.CreateTable2('node_tbl_bak_moving')
        self.CreateIndex2('link_tbl_bak_moving_link_id_idx')
        self.CreateIndex2('node_tbl_bak_moving_node_id_idx')
        
        for cur_try_time in [1,2]:
            self.log.info("Processing short link, Round %d..." % cur_try_time)
            if self.__findShortLink():
                self.__calcNewPosForVertexOfShortLink()
                self.__moveNode()
            else:
                break
        else:
            nCount = self.__findShortLink()
            if nCount:
                self.log.error("%d short links still exist." % nCount)
                raise Exception, "%d short links still exist." % nCount
        return 0
    
    def __findShortLink(self):
        self.log.info('find short link...')
            
        self.CreateTable2('temp_move_shortlink')
        self.CreateIndex2('temp_move_shortlink_link_id_idx')
        
        self.pg.execute2('select count(*) from temp_move_shortlink')
        row = self.pg.fetchone2()
        return row[0]
        
    def __calcNewPosForVertexOfShortLink(self):
        self.log.info('calculate new position for vertex of short link...')
        
        self.CreateTable2('temp_move_shortlink_newshape')
        self.CreateTable2('temp_move_shortlink_vertex_newpos')
        
        self.CreateFunction2('mid_calc_new_pos_for_vertex')
        self.pg.callproc('mid_calc_new_pos_for_vertex')
        
        self.CreateIndex2('temp_move_shortlink_newshape_link_id_idx')
        self.CreateIndex2('temp_move_shortlink_vertex_newpos_node_id_idx')
        
        self.pg.commit2()
    
    def __moveNode(self):
        self.log.info('move node...')
        
        sqlcmd = """
                    update node_tbl as a set the_geom = b.the_geom
                    from 
                    (
                        select node_id, (array_agg(the_geom))[1] as the_geom
                        from temp_move_shortlink_vertex_newpos
                        group by node_id
                    ) as b
                    where a.node_id = b.node_id;
                    
                    update link_tbl as a set the_geom = b.the_geom
                    from temp_move_shortlink_newshape as b
                    where a.link_id = b.link_id;
                    
                    update link_tbl as a 
                    set the_geom = b.the_geom, length = b.length
                    from 
                    (
                        select  link_id, the_geom, 
                                ST_Length_Spheroid(the_geom,'SPHEROID("WGS_84", 6378137, 298.257223563)') as length
                        from
                        (
                            select  a.link_id, 
                                    (
                                    case 
                                    when c.node_id is null then st_setpoint(a.the_geom, 0, b.the_geom)
                                    when b.node_id is null then st_setpoint(a.the_geom, st_numpoints(a.the_geom)-1, c.the_geom)
                                    else st_setpoint(st_setpoint(a.the_geom, 0, b.the_geom), st_numpoints(a.the_geom)-1, c.the_geom)
                                    end
                                    )as the_geom
                            from link_tbl as a
                            left join temp_move_shortlink_vertex_newpos as b
                            on a.s_node = b.node_id
                            left join temp_move_shortlink_vertex_newpos as c
                            on a.e_node = c.node_id
                            where b.node_id is not null or c.node_id is not null
                        )as t
                    ) as b
                    where a.link_id = b.link_id;
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
    
    