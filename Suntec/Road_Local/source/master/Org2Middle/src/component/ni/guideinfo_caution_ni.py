# -*- coding: cp936 -*-
'''
Created on 2015-5-4

@author:
'''



import io
import common.common_func
import component.default.guideinfo_caution

class comp_guideinfo_caution_ni(component.default.guideinfo_caution.comp_guideinfo_caution):
    def __init__(self):
        '''
        Constructor
        '''
        component.default.guideinfo_caution.comp_guideinfo_caution.__init__(self)
    
    def _DoCreateTable(self):
        
        if self.CreateTable2('caution_tbl') == -1:
            return -1

        return 0
    
    def _DoCreateFunction(self):
        
        return 0
    
    def _DoCreateIndex(self):
        
        return 0
    
    def _Do(self):
        
        if self._Deal_TrfcSign() == -1:
            return -1
        
        if self._Deal_admin() == -1:
            return -1
        
        if self._Deal_update_caution_tbl() == -1:
            return -1
        
        return 0
    
    def _Deal_TrfcSign(self):
        
        if self._create_temp_trfcsign_type_wavid() == -1:
            return -1
        
        if self._create_temp_trfcsign_type_picid() == -1:
            return -1

        if self._create_temp_trfcsign_caution_tbl() == -1:
            return -1
        
        return 0
    
    def _Deal_admin(self):
        
        if self._create_temp_admin_wavid() == -1:
            return -1
        
        if self._create_temp_admin_picid() == -1:
            return -1
        
        if self._create_temp_order8_boundary() == -1:
            return -1
        
        if self._create_temp_inode() == -1:
            return -1
        
        if self._create_temp_guideinfo_boundary() == -1:
            return -1
        
        if self._create_temp_admin_caution_tbl() == -1:
            return -1
        
        return 0
    
    def _Deal_update_caution_tbl(self):
        
        self.log.info('Now it is updating caution_tbl...')
        
        sqlcmd = """
                insert into caution_tbl (
                    inlinkid, nodeid, outlinkid, passlid, passlink_cnt, data_kind,
                    voice_id, strtts, image_id
                )
                select
                    inlinkid, nodeid, outlinkid, passlid, passlink_cnt, data_kind,
                    voice_id, strtts, image_id
                from temp_trfcsign_caution_tbl
                
                union
                
                select
                    inlinkid, nodeid, outlinkid, passlid, passlink_cnt, data_kind,
                    voice_id, strtts, image_id
                from temp_admin_caution_tbl
            """
        
        if self.pg.do_big_insert2(sqlcmd) == -1:
            return -1
        
        self.log.info('updating caution_tbl succeeded')
        return 0
    
    def _create_temp_trfcsign_type_wavid(self):
        
        self.log.info('Now it is making temp_trfcsign_type_wavid...')
        # create a relationship between traffic sign type and voice id 
        if self.CreateTable2('temp_trfcsign_type_wavid') == -1:
            return -1
        
        pg = self.pg
        pgcur = self.pg.pgcur2
        trfcsign_type_wav_path = common.common_func.GetPath('trfcsign_type_wav')
        f = io.open(trfcsign_type_wav_path, 'r', 8192, 'utf8')
        pgcur.copy_from(f, 'temp_trfcsign_type_wavid', ',', "", 8192, None)        
        pg.commit2()
        f.close()
        
        self.log.info('making temp_trfcsign_type_wavid succeeded')
        return 0
    
    def _create_temp_trfcsign_type_picid(self):
        
        self.log.info('Now it is making temp_trfcsign_type_picid...')
        # create a relationship between traffic sign type and pic id 
        if self.CreateTable2('temp_trfcsign_type_picid') == -1:
            return -1
        
        pg = self.pg
        pgcur = self.pg.pgcur2
        trfcsign_type_pic_path = common.common_func.GetPath('trfcsign_type_pic')
        f = io.open(trfcsign_type_pic_path, 'r', 8192, 'utf8')
        pgcur.copy_from(f, 'temp_trfcsign_type_picid', ',', "", 8192, None)        
        pg.commit2()
        f.close()
        
        self.log.info('making temp_trfcsign_type_picid succeeded')
        return 0
    
    def _create_temp_trfcsign_caution_tbl(self):
        
        self.log.info('Now it is making temp_trfcsign_caution_tbl...')
        if self.CreateTable2('temp_trfcsign_caution_tbl') == -1:
            return -1
        
        if self.CreateIndex2('org_trfcsign_type_idx') == -1:
            return -1
        
        sqlcmd = """
                insert into temp_trfcsign_caution_tbl (
                    inlinkid, 
                    nodeid, 
                    type,
                    data_kind,
                    voice_id, 
                    image_id,
                    inlink_geom
                )
                select 
                    inlinkid, nodeid, type, 
                    5 as data_kind,
                    voice_id, image_id, 
                    e.the_geom as inlink_geom
                from (
                    select 
                    inlinkid::bigint, nodeid::bigint, type::integer,
                    b.wav_id as voice_id, 
                    c.pic_id as image_id
                    from org_trfcsign a
                    left join temp_trfcsign_type_wavid b
                        on type::integer = b.trfcsign_type
                    left join temp_trfcsign_type_picid c
                        on type::integer = c.trfcsign_type
                    order by type, inlinkid, nodeid
                ) as d
                left join link_tbl e
                    on d.inlinkid = e.link_id
            """
        
        if self.pg.do_big_insert2(sqlcmd) == -1:
            return -1
        
        self.log.info('making temp_trfcsign_caution_tbl succeeded')
        return 0
    
    def _create_temp_admin_wavid(self):
        
        self.log.info('Now it is making temp_admin_wavid...')
        # create a relationship between admin code and voice id 
        if self.CreateTable2('temp_admin_wavid') == -1:
            return -1
        
        pg = self.pg
        pgcur = self.pg.pgcur2
        admin_wav_path = common.common_func.GetPath('admin_wav')
        f = io.open(admin_wav_path, 'r', 8192, 'utf8')
        pgcur.copy_from(f, 'temp_admin_wavid', ',', "", 8192, None)        
        pg.commit2()
        f.close()
        
        self.log.info('making temp_admin_wavid succeeded')
        return 0
    
    def _create_temp_admin_picid(self):
        
        self.log.info('Now it is making temp_admin_picid...')
        # create a relationship between admin code and pic id
        if self.CreateTable2('temp_admin_picid') == -1:
            return -1
        
        pg = self.pg
        pgcur = self.pg.pgcur2
        admin_pic_path = common.common_func.GetPath('admin_pic')
        f = io.open(admin_pic_path, 'r', 8192, 'utf8')
        pgcur.copy_from(f, 'temp_admin_picid', ',', "", 8192, None)        
        pg.commit2()
        f.close()
        
        self.log.info('making temp_admin_picid succeeded')
        return 0
    
    def _create_temp_order8_boundary(self):
       
        self.log.info('Now it is making temp_order8_boundary...')                 
        if self.CreateIndex2('mid_admin_zone_ad_order_idx') == -1:
            return -1
        
        if self.CreateIndex2('mid_admin_zone_order0_id_idx') == -1:
            return -1
        
        if self.CreateIndex2('mid_admin_zone_order1_id_idx') == -1:
            return -1
        
        if self.CreateIndex2('mid_admin_zone_order2_id_idx') == -1:
            return -1
        
        if self.CreateTable2('temp_order8_boundary') == -1:
            return -1
        
        sqlcmd = """
                insert into temp_order8_boundary (
                    ad_code, 
                    order8_id, 
                    order8_name,
                    order2_id, 
                    order2_name,
                    order1_id, 
                    order1_name,
                    order0_id, 
                    order0_name,
                    order8_geom
                )
                select 
                    a.ad_code, a.order8_id, 
                    a.ad_name as order8_name, 
                    a.order2_id, 
                    d.ad_name as order2_name, 
                    a.order1_id, 
                    c.ad_name as order1_name, 
                    a.order0_id, 
                    b.ad_name as order0_name, 
                    ST_Boundary(a.the_geom) as order8_geom
                from mid_admin_zone a
                left join mid_admin_zone b
                    on a.order0_id = b.ad_code
                left join mid_admin_zone c
                    on a.order1_id = c.ad_code
                left join mid_admin_zone d
                    on a.order2_id = d.ad_code
                where a.ad_order = 8
            """
        
        if self.pg.do_big_insert2(sqlcmd) == -1:
            return -1
        
        if self.CreateIndex2('temp_order8_boundary_order8_geom_idx') == -1:
            return -1
        
        if self.CreateIndex2('temp_order8_boundary_ad_code_idx') == -1:
            return -1
        
        self.log.info('making temp_order8_boundary succeeded')
        return 0
    
    def _create_temp_inode(self):
        
        self.log.info('Now it is making temp_inode...')
        if self.CreateTable2('temp_inode') == -1:
            return -1
        
        sqlcmd = """
                insert into temp_inode (
                    link_id, s_node, e_node, one_way_code, link_type, 
                    b_node, slocate, elocate, the_geom
                )
                select
                    link_id, s_node, e_node, one_way_code, link_type,
                    case when slocate < 0.5 and slocate < 1 - elocate then s_node else e_node end as b_node,
                    slocate, elocate,
                    case when slocate < 0.5 and slocate < 1 - elocate then sp else ep end as the_geom
                from (
                    select 
                        link_id, s_node, e_node, one_way_code, link_type,
                        ST_Line_Locate_Point(the_geom, sbp) as slocate, 
                        ST_Line_Locate_Point(the_geom, ebp) as elocate, 
                        sp, ep
                    from (
                        select
                            c.link_id, c.s_node, c.e_node, c.one_way_code, c.link_type, d.the_geom,
                            ST_ClosestPoint(c.the_geom, e.the_geom) as sbp,
                            ST_ClosestPoint(c.the_geom, f.the_geom) as ebp,
                            e.the_geom as sp,
                            f.the_geom as ep
                        from (
                            select 
                                distinct b.link_id, b.s_node, b.e_node, b.one_way_code, b.link_type,
                                ST_Intersection(a.order8_geom, b.the_geom) as the_geom
                            from temp_order8_boundary a
                            inner join link_tbl b
                                on ST_Intersects(a.order8_geom, b.the_geom) = TRUE
                        ) as c
                        inner join link_tbl d
                            on c.link_id = d.link_id
                        inner join node_tbl e
                            on d.s_node = e.node_id
                        inner join node_tbl f
                            on d.e_node = f.node_id
                    ) as g
                ) as h
            """
        
        if self.pg.do_big_insert2(sqlcmd) == -1:
            return -1
        
        if self.CreateIndex2('temp_inode_link_id_idx') == -1:
            return -1
        
        if self.CreateIndex2('temp_inode_b_node_idx') == -1:
            return -1
        
        self.log.info('making temp_inode succeeded')
        return 0
    
    def _create_temp_guideinfo_boundary(self):
        
        self.log.info('Now it is making temp_guideinfo_boundary...')
        if self.CreateTable2('temp_guideinfo_boundary') == -1:
            return -1
        
        sqlcmd = """
                insert into temp_guideinfo_boundary (
                    inlinkid, nodeid, outlinkid, innode, outnode, out_adcd
                )
                select 
                    distinct inlinkid, nodeid, outlinkid, innode, outnode, 
                    h.ad_code as out_adcd
                from (
                    select 
                        in_link_id as inlinkid, nodeid, out_link_id as outlinkid,
                        case when in_s_node = nodeid then in_e_node else in_s_node end as innode,
                        case when out_s_node  = nodeid then out_e_node else out_s_node end as outnode
                    from (
                        select 
                            b.link_id as in_link_id, 
                            a.b_node as nodeid, 
                            c.link_id as out_link_id, 
                            b.s_node as in_s_node, 
                            b.e_node as in_e_node, 
                            c.s_node as out_s_node, 
                            c.e_node as out_e_node
                        from temp_inode a
                        left join link_tbl b
                            on 
                                (a.b_node = b.e_node and b.one_way_code in (1, 2)) or 
                                (a.b_node = b.s_node and b.one_way_code in (1, 3))
                        left join link_tbl c
                            on 
                                (a.b_node = c.s_node and c.one_way_code in (1, 2)) or 
                                (a.b_node = c.e_node and c.one_way_code in (1, 3))
                        where 
                            b.s_node != b.e_node and
                            c.s_node != c.e_node
                    ) as d
                    inner join temp_inode e
                        on e.link_id in (in_link_id, out_link_id)
                ) as f
                inner join node_tbl g
                    on outnode = g.node_id
                inner join (
                    select ad_code, the_geom
                    from mid_admin_zone
                    where ad_order = 8
                ) as h
                    on ST_Intersects(g.the_geom, h.the_geom) = TRUE
            """
        
        if self.pg.do_big_insert2(sqlcmd) == -1:
            return -1
        
        if self.CreateIndex2('temp_guideinfo_boundary_out_adcd_idx') == -1:
            return -1
        
        if self.CreateIndex2('temp_guideinfo_boundary_inlinkid_idx') == -1:
            return -1
        
        if self.CreateIndex2('temp_guideinfo_boundary_nodeid_idx') == -1:
            return -1
        
        if self.CreateIndex2('temp_guideinfo_boundary_outlinkid_idx') == -1:
            return -1
        
        self.log.info('making temp_guideinfo_boundary succeeded')
        return 0
    
    def _create_temp_admin_caution_tbl(self):
        
        self.log.info('Now it is making temp_admin_caution_tbl...')
        if self.CreateTable2('temp_admin_caution_tbl') == -1:
            return -1
        
        sqlcmd = """
                insert into temp_admin_caution_tbl (
                    inlinkid, nodeid, outlinkid, data_kind, voice_id, strTTS, image_id,
                    node_geom, inlink_geom, outlink_geom
                )
                select 
                    inlinkid, nodeid, outlinkid, 
                    4 as data_kind,
                    b.wav_id as voice_id,
                    h.order8_name as strTTS,
                    c.pic_id as image_id,
                    f.the_geom as node_geom,
                    e.the_geom as inlink_geom,
                    g.the_geom as outlink_geom
                from temp_guideinfo_boundary a
                left join temp_admin_wavid b
                    on a.out_adcd = b.ad_cd
                left join temp_admin_picid c
                    on a.out_adcd = c.ad_cd
                left join link_tbl e
                    on a.inlinkid = e.link_id
                left join node_tbl f
                    on a.nodeid = f.node_id
                left join link_tbl g
                    on a.outlinkid = g.link_id
                left join temp_order8_boundary h
                    on a.out_adcd = h.ad_code
            """
        
        if self.pg.do_big_insert2(sqlcmd) == -1:
            return -1
        
        self.log.info('making temp_admin_caution_tbl succeeded')
        return 0