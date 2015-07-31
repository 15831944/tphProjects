'''
Created on 2012-2-23
@author: sunyifeng
'''
import common
import base
import io

class comp_guideinfo_caution_jpn(base.component_base.comp_base):
    
    def __init__(self):
        '''
        Constructor
        '''
        base.component_base.comp_base.__init__(self, 'Guideinfo Caution') 
        
    def _DoCreateTable(self):
        
        self.CreateTable2('caution_tbl')
        
        self.CreateTable2('temp_admin_province_boundary')
        sqlcmd = 'Alter table temp_admin_province_boundary drop CONSTRAINT enforce_geotype_the_geom;'
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        self.CreateTable2('temp_highway_link')
        
        self.CreateTable2('temp_link_intersection_points')
        sqlcmd = 'Alter table temp_link_intersection_points drop CONSTRAINT enforce_geotype_the_geom;'
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        self.CreateTable2('temp_bnode')
        self.CreateTable2('temp_guideinfo_boundary')
        self.CreateTable2('temp_admin_wavid')
        
        return 0
    
    def _DoCreateFunction(self):
        
        self.CreateFunction2('mid_get_inout_link')
        
        return 0
    
    def _Do(self):
        
        self.log.info('making guideinfo caution info ...')

        self.log.info('making guideinfo caution boundary info ...')
        self._DoBoundary()
        self.log.info('making guideinfo caution info OK.')

        self.log.info('making guideinfo caution boundary info OK.')

    def _DoBoundary(self):
        
        pg = self.pg
        pgcur = self.pg.pgcur2
        admin_wav_path = common.GetPath('admin_wav')
        f = io.open(admin_wav_path, 'r', 8192, 'utf8')
        pgcur.copy_from(f, 'temp_admin_wavid', ',', "", 8192, None)
        pg.commit2()
        f.close()
        
        sqlcmd = """
            insert into temp_admin_province_boundary(ad_cd, the_geom)
            select ad_cd, ST_Boundary(the_geom) as the_geom
            from rdb_admin_province;
        """
        
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        sqlcmd = """
            insert into temp_highway_link(link_id, the_geom)
            select link_id, the_geom
            from link_tbl
            where road_type in (0,1) and link_type = 2;
        """   
        
        self.pg.execute2(sqlcmd)
        self.pg.commit2() 
        
        self.CreateIndex2('temp_admin_province_boundary_the_geom_idx')
        self.CreateIndex2('temp_highway_link_the_geom_idx')


        sqlcmd = """
            insert into temp_link_intersection_points(link_id, the_intersec_points_geom)
            select distinct a.link_id, a.s_node, a.e_node, a.one_way_code,
                   ST_Intersection(a.the_geom, b.the_geom) as the_intersec_points_geom
            from temp_highway_link as a
            inner join temp_admin_province_boundary as b
            on ST_Intersects(a.the_geom, b.the_geom) = TRUE
        """ 
        
        self.pg.execute2(sqlcmd)
        self.pg.commit2()  

        sqlcmd = """
            insert into temp_bnode(link_id, bnode, link_id2, the_geom)
            select t1.link_id, t1.bnode, t2.link_id, t1.the_geom
            from 
            (
                select m.link_id, m.the_geom as m_the_geom, n.the_geom as n_the_geom
                from 
                temp_link_intersection_points as m
                inner join link_tbl as n
                on m.link_id = n.link_id
            ) as t1
            inner join link_tbl as t2
            on t1.bnode = t2.s_node or t1.bnode = t2.e_node
            where t1.link_id != t2.link_id;
        """  
        
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        sqlcmd = """
            insert into temp_guideinfo_boundary(inlinkid, nodeid, outlinkid, innode, outnode, out_adcd)
            select distinct inlinkid, bnode, outlinkid, innode, outnode, b.ad_cd
            from
            (
                select links[1] as inlinkid, bnode, links[2] as outlinkid, 
                       links[3] as innode, links[4] as outnode
                from
                ( 
                    select bnode, mid_get_inout_link(link_id, s_node, e_node, one_way_code, 
                           link_id2, s_node2, e_node2, one_way_code2) as links
                    from temp_bnode
                ) as t
                where links is not null
            ) as a
            inner join node_tbl as c
            on a.outnode = c.node_id
            inner join rdb_admin_province as b
            on ST_Intersects(c.the_geom, b.the_geom) = TRUE;
        """  
        
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        sqlcmd = """
            INSERT INTO caution_tbl(inlinkid, nodeid, outlinkid, passlid, passlink_cnt, data_kind, voice_id)
                select inlinkid, nodeid, outlinkid, null, 0, 4, b.wav_id
                    from temp_guideinfo_boundary as a
                    inner join temp_admin_wavid as b
                    on a.out_adcd = b.ad_cd
                    order by inlinkid, nodeid, outlinkid;
        """  
        
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        
        return 0
        
