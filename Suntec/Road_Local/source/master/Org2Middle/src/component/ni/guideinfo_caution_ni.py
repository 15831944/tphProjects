# -*- coding: UTF8 -*-
'''
Created on 2015-5-4

@author:
'''



import io
import os
import common.common_func
import component.default.guideinfo_caution



class comp_guideinfo_caution_ni(component.default.guideinfo_caution.comp_guideinfo_caution):
    def __init__(self):
        '''
        Constructor
        '''
        component.default.guideinfo_caution.comp_guideinfo_caution.__init__(self)
    
    def _make_caution_from_origin(self):
        
        # 从NI协议中抽取caution信息 
        
        self.CreateTable2('temp_trfcsign_caution_tbl')
        self.CreateTable2('temp_admin_caution_tbl')
        
        self._make_caution_from_trfcsign()
        
        # 与机能组确认，县境案内不提供专有数据，机能组根据行政界自己判定县境案内触发情况，以下代码屏蔽
        #self._make_caution_from_admin()
        
        self._update_caution_tbl()
        
        return 0
    
    def _make_caution_from_trfcsign(self):
        
        # 作成表单temp_trfcsign_caution_tbl记录中国专有注意点案内，作成：
        # 1、从配置文件中获取中国专有注意点类型与音声、图片的对照关系
        # 2、从配置文件中获取中国专有注意点类型与caution种别的对照关系
        # 3、通过1/2所获数据、原始数据作成中国专有注意点案内
        
        self._create_temp_trfcsign_type_wavid()
        self._create_temp_trfcsign_type_picid()
        self._create_temp_trfcsign_type_data_kind()
        self._create_temp_trfcsign_caution_tbl()
        
        return 0
    
    def _make_caution_from_admin(self):
        
        # 县境案内（取消）
        # 与机能组确认，该机能保留，但不需要dataformat提供数据，机能组应用时根据行政界范围制作对应的县境案内
        
        self._create_temp_admin_wavid()
        self._create_temp_admin_picid()
        self._create_temp_order8_boundary()
        self._create_temp_inode()
        self._create_temp_guideinfo_boundary()
        self._create_temp_admin_caution_tbl()
        
        return 0
    
    def _update_caution_tbl(self):
        
        self.log.info('Now it is updating caution_tbl...')
        
        # 更新表单 caution_tbl
        # 插入中国专有注意点案内、县境案内
        
        sqlcmd = """
                INSERT INTO caution_tbl (
                    inlinkid, nodeid, outlinkid, passlid, passlink_cnt, data_kind,
                    voice_id, strtts, image_id
                )
                SELECT inlinkid, nodeid, outlinkid, passlid, passlink_cnt, 
                    data_kind, voice_id, strtts, image_id
                FROM (
                    SELECT inlinkid, nodeid, outlinkid, passlid, passlink_cnt, data_kind, 
                        voice_id, strtts, image_id
                    FROM temp_trfcsign_caution_tbl
                    
                    UNION
                    
                    SELECT inlinkid, nodeid, outlinkid, passlid, passlink_cnt, data_kind, 
                        voice_id, strtts, image_id
                    FROM temp_admin_caution_tbl
                ) a
                
                ORDER BY inlinkid, nodeid, data_kind
            """
        
        self.pg.do_big_insert2(sqlcmd)
        
        self.log.info('updating caution_tbl succeeded')
        
        return 0
    
    def _create_temp_trfcsign_type_wavid(self): 
        
        self.log.info('Now it is making temp_trfcsign_type_wavid...')
        
        # 作成表单temp_trfcsign_type_wavid记录中国专有注意点类型与音声的对照关系
        # 每一种中国专有注意点案内对应一种提示语音，需要制作对照关系。该关系通过配置文件提供
        # create a relationship between traffic sign type and voice id 
         
        self.CreateTable2('temp_trfcsign_type_wavid')
        
        trfcsign_type_wav_path = common.common_func.GetPath('trfcsign_type_wav')
        if trfcsign_type_wav_path:
            if os.path.exists(trfcsign_type_wav_path):
                f = io.open(trfcsign_type_wav_path, 'r', 8192, 'utf8')
                self.pg.copy_from2(f, 'temp_trfcsign_type_wavid', ',')
                self.pg.commit2()
                f.close()
        
        self.log.info('making temp_trfcsign_type_wavid succeeded')
        
        return 0
    
    def _create_temp_trfcsign_type_picid(self): 
        
        self.log.info('Now it is making temp_trfcsign_type_picid...')
        
        # 作成表单temp_trfcsign_type_picid记录中国专有注意点类型与图片的对照关系
        # 每一种中国专有注意点案内对应一种显示牌，需要制作对照关系。该关系通过配置文件提供
        # create a relationship between traffic sign type and pic id  
        
        self.CreateTable2('temp_trfcsign_type_picid')

        trfcsign_type_pic_path = common.common_func.GetPath('trfcsign_type_pic')
        if trfcsign_type_pic_path:
            if os.path.exists(trfcsign_type_pic_path):
                f = io.open(trfcsign_type_pic_path, 'r', 8192, 'utf8')
                self.pg.copy_from2(f, 'temp_trfcsign_type_picid', ',')       
                self.pg.commit2()
                f.close()
        
        self.log.info('making temp_trfcsign_type_picid succeeded')
        
        return 0
    
    def _create_temp_trfcsign_type_data_kind(self): 
        
        self.log.info('Now it is making temp_trfcsign_type_data_kind...')
        
        # 作成表单temp_trfcsign_type_data_kind记录中国专有注意点种别与caution种别的对照关系
        # create a relationship between traffic sign type and data kind
        
        self.CreateTable2('temp_trfcsign_type_data_kind')

        trfcsign_type2data_kind_path = common.common_func.GetPath('trfcsign_type2data_kind')
        if trfcsign_type2data_kind_path:
            if os.path.exists(trfcsign_type2data_kind_path):
                f = io.open(trfcsign_type2data_kind_path, 'r', 8192, 'utf8')
                self.pg.copy_from2(f, 'temp_trfcsign_type_data_kind', ',')       
                self.pg.commit2()
                f.close()
        
        self.log.info('making temp_trfcsign_type_data_kind succeeded')
        
        return 0
    
    def _create_temp_trfcsign_caution_tbl(self):  
         
        self.log.info('Now it is making temp_trfcsign_caution_tbl...')
        
        # 更新表单temp_trfcsign_caution_tbl记录专有注意点案内，作成
        # 1、从原始数据表单org_trfcsign获取注意点案内关联的link信息、node信息
        # 2、根据注意点种别与音声、图片、caution种别的对照关系更新相关字段
        # 注意点：
        # 以下data_kind机能需求外，不作成
        # 1(カーブ), 3(合流分岐), 4(県境), 8(反向弯路（右）), 15(窄桥), 27(村庄), 29(路面不平), 31(有人看守铁路道口), 
        # 36(注意危险), 40(连续下坡), 41(文字性警示标牌（现场为文字提示，且无法归类到国标危险信息标牌中）), 
        # 46(减速让行), 47(隧道开灯), 48(潮汐车道), 49(路面高凸), 50(路面低洼), 51(交通意外黑点)
        
        self.CreateIndex2('org_trfcsign_type_idx')
        
        sqlcmd = """
                INSERT INTO temp_trfcsign_caution_tbl (
                    inlinkid, nodeid, type, data_kind,
                    voice_id, image_id, inlink_geom
                )
                SELECT inlinkid, nodeid, type, data_kind,
                    voice_id, image_id, e.the_geom as inlink_geom
                FROM (
                    SELECT inlinkid::bigint, nodeid::bigint, type::integer, f.data_kind,
                        b.wav_id as voice_id, c.pic_id as image_id
                    FROM org_trfcsign a
                    LEFT JOIN temp_trfcsign_type_wavid b
                        ON type::integer = b.trfcsign_type
                    LEFT JOIN temp_trfcsign_type_picid c
                        ON type::integer = c.trfcsign_type
                    LEFT JOIN temp_trfcsign_type_data_kind f
                        ON type::integer = f.trfcsign_type
                    WHERE f.data_kind NOT IN (1, 3, 4, 8, 15, 27, 29, 31, 36, 40, 41, 46, 47, 48, 49, 50, 51)
                    ORDER BY type, inlinkid, nodeid
                ) d
                LEFT JOIN link_tbl e
                    ON d.inlinkid = e.link_id
            """
        
        self.pg.do_big_insert2(sqlcmd)
        
        self.log.info('making temp_trfcsign_caution_tbl succeeded')
        
        return 0
    
    def _create_temp_admin_wavid(self):   
        
        self.log.info('Now it is making temp_admin_wavid...')
        
        # create a relationship between admin code and voice id 
        self.CreateTable2('temp_admin_wavid')

        admin_wav_path = common.common_func.GetPath('admin_wav')
        f = io.open(admin_wav_path, 'r', 8192, 'utf8') 
        self.pg.copy_from2(f, 'temp_admin_wavid', ',')       
        self.pg.commit2()
        f.close()
        
        self.log.info('making temp_admin_wavid succeeded')
        
        return 0
    
    def _create_temp_admin_picid(self): 
        
        self.log.info('Now it is making temp_admin_picid...')
        
        # create a relationship between admin code and pic id
        self.CreateTable2('temp_admin_picid')

        admin_pic_path = common.common_func.GetPath('admin_pic')
        f = io.open(admin_pic_path, 'r', 8192, 'utf8')
        self.pg.copy_from2(f, 'temp_admin_picid', ',')      
        self.pg.commit2()
        f.close()
        
        self.log.info('making temp_admin_picid succeeded')
        
        return 0
    
    def _create_temp_order8_boundary(self):
        
        self.log.info('Now it is making temp_order8_boundary...')
                         
        self.CreateIndex2('mid_admin_zone_ad_order_idx')
        self.CreateIndex2('mid_admin_zone_order0_id_idx')
        self.CreateIndex2('mid_admin_zone_order1_id_idx')
        self.CreateIndex2('mid_admin_zone_order2_id_idx')
        
        sqlcmd = """
                drop table if exists temp_order8_boundary;
                CREATE TABLE temp_order8_boundary 
                as (
                    select a.ad_code, a.order8_id, a.ad_name as order8_name, 
                        a.order2_id, d.ad_name as order2_name, 
                        a.order1_id, c.ad_name as order1_name, 
                        a.order0_id, b.ad_name as order0_name, 
                        ST_Boundary(a.the_geom) as order8_geom
                    from mid_admin_zone a
                    left join mid_admin_zone b
                        on a.order0_id = b.ad_code
                    left join mid_admin_zone c
                        on a.order1_id = c.ad_code
                    left join mid_admin_zone d
                        on a.order2_id = d.ad_code
                    where a.ad_order = 8
                ); 
                
                DROP INDEX IF EXISTS temp_order8_boundary_order8_geom_idx;
                CREATE INDEX temp_order8_boundary_order8_geom_idx
                  ON temp_order8_boundary
                  USING gist
                  (order8_geom);
                
                DROP INDEX IF EXISTS temp_order8_boundary_ad_code_idx;
                CREATE INDEX temp_order8_boundary_ad_code_idx
                  ON temp_order8_boundary
                  USING btree
                  (ad_code);
                  
                analyze temp_order8_boundary;
            """
        
        self.pg.do_big_insert2(sqlcmd)
        
        self.log.info('making temp_order8_boundary succeeded')
        
        return 0
    
    def _create_temp_inode(self):
        
        self.log.info('Now it is making temp_inode...')
        
        sqlcmd = """
                drop table if exists temp_inode;
                CREATE TABLE temp_inode 
                as (
                    select link_id, s_node, e_node, one_way_code, link_type,
                        case when slocate < 0.5 and slocate < 1 - elocate then s_node else e_node end as b_node,
                        slocate, elocate,
                        case when slocate < 0.5 and slocate < 1 - elocate then sp else ep end as the_geom
                    from (
                        select link_id, s_node, e_node, one_way_code, link_type,
                            ST_Line_Locate_Point(the_geom, sbp) as slocate, 
                            ST_Line_Locate_Point(the_geom, ebp) as elocate, 
                            sp, ep
                        from (
                            select c.link_id, c.s_node, c.e_node, c.one_way_code, c.link_type, d.the_geom,
                                ST_ClosestPoint(c.the_geom, e.the_geom) as sbp,
                                ST_ClosestPoint(c.the_geom, f.the_geom) as ebp,
                                e.the_geom as sp, f.the_geom as ep
                            from (
                                select distinct b.link_id, b.s_node, b.e_node, b.one_way_code, b.link_type,
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
                );
                
                DROP INDEX IF EXISTS temp_inode_b_node_idx;
                CREATE INDEX temp_inode_b_node_idx
                  ON temp_inode
                  USING btree
                  (b_node);
                
                DROP INDEX IF EXISTS temp_inode_link_id_idx;
                CREATE INDEX temp_inode_link_id_idx
                  ON temp_inode
                  USING btree
                  (link_id);
                
                analyze temp_inode;
            """
        
        self.pg.do_big_insert2(sqlcmd)
        
        self.log.info('making temp_inode succeeded')
        
        return 0
    
    def _create_temp_guideinfo_boundary(self):
        
        self.log.info('Now it is making temp_guideinfo_boundary...')
        
        sqlcmd = """
                drop table if exists temp_guideinfo_boundary;
                CREATE TABLE temp_guideinfo_boundary 
                as (
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
                );
                
                DROP INDEX IF EXISTS temp_guideinfo_boundary_outlinkid_idx;
                CREATE INDEX temp_guideinfo_boundary_outlinkid_idx
                  ON temp_guideinfo_boundary
                  USING btree
                  (outlinkid);
                
                DROP INDEX IF EXISTS temp_guideinfo_boundary_nodeid_idx;
                CREATE INDEX temp_guideinfo_boundary_nodeid_idx
                  ON temp_guideinfo_boundary
                  USING btree
                  (nodeid);
                
                DROP INDEX IF EXISTS temp_guideinfo_boundary_inlinkid_idx;
                CREATE INDEX temp_guideinfo_boundary_inlinkid_idx
                  ON temp_guideinfo_boundary
                  USING btree
                  (inlinkid);
                
                DROP INDEX IF EXISTS temp_guideinfo_boundary_out_adcd_idx;
                CREATE INDEX temp_guideinfo_boundary_out_adcd_idx
                  ON temp_guideinfo_boundary
                  USING btree
                  (out_adcd);
  
                analyze temp_guideinfo_boundary;
            """
        
        self.pg.do_big_insert2(sqlcmd)
        
        self.log.info('making temp_guideinfo_boundary succeeded')
        
        return 0
    
    def _create_temp_admin_caution_tbl(self):
        
        self.log.info('Now it is making temp_admin_caution_tbl...')
        
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
        
        self.pg.do_big_insert2(sqlcmd)
        
        self.log.info('making temp_admin_caution_tbl succeeded')
        
        return 0