# -*- coding: UTF-8 -*-
'''
Created on 2015-6-26

@author: liushengqiang
'''



import component.default.guideinfo_spotguide

class comp_guideinfo_spotguide_zenrin(component.default.guideinfo_spotguide.comp_guideinfo_spotguide):
    def __init__(self):
        '''
        Constructor
        '''
        component.default.guideinfo_spotguide.comp_guideinfo_spotguide.__init__(self)
    
    def _DoCreateTable(self):
        
        self.CreateTable2('spotguide_tbl')
        
        return 0
    
    def _DoCreateIndex(self):
        
        self.CreateIndex2('spotguide_tbl_node_id_idx')
        
        return 0
    
    def _Do(self):
        
        self.__make_temp_org_spjcty()
        self.__make_temp_org_eci_jctv()
        self.__make_temp_spotguide_tbl1()
        self.__make_temp_spotguide_tbl2()
        self.__make_spotguide_tbl()
        
        component.default.guideinfo_spotguide.\
        comp_guideinfo_spotguide._GenerateSpotguideTblForTollStation(self)
        
        return 0
    
    def __make_temp_org_spjcty(self):
        self.log.info('make temp_org_spjcty...')
        
        # 创建表单temp_org_spjcty 作成：
        # 原始表单org_spjcty中id唯一化
        # 步骤1：根据jct_number获取meshcode，将字符串化的id整形化
        # 步骤2：根据mapping表获取唯一化id
        
        self.CreateFunction2('zenrin_get_meshcode')
        
        sqlcmd = """
                    drop table if exists temp_org_spjcty;
                    CREATE TABLE temp_org_spjcty
                    as (
                        select 
                            b.node_id as atten_intersetion_no, 
                            c.node_id as from_intersetion_no, 
                            mng_no_bkg, 
                            d.node_id as to_int_no11, 
                            e.node_id as to_int_no12, 
                            mng_no_arrow1, 
                            f.node_id as to_int_no21, 
                            g.node_id as to_int_no22, 
                            mng_no_arrow2
                        from (
                            select 
                                zenrin_get_meshcode(jct_number) as meshcode, 
                                atten_intersetion_no, from_intersetion_no, mng_no_bkg,
                                (case when to_int_no11 = '      ' then null else to_int_no11 end)::integer as to_int_no11,
                                (case when to_int_no12 = '      ' then null else to_int_no12 end)::integer as to_int_no12,
                                mng_no_arrow1,
                                (case when to_int_no21 = '      ' then null else to_int_no21 end)::integer as to_int_no21,
                                (case when to_int_no22 = '      ' then null else to_int_no22 end)::integer as to_int_no22,
                                mng_no_arrow2
                            from org_spjcty
                        ) a
                        left join temp_node_mapping b
                            on a.meshcode = b.meshcode and atten_intersetion_no = b.nodeno
                        left join temp_node_mapping c
                            on a.meshcode = c.meshcode and from_intersetion_no = c.nodeno
                        left join temp_node_mapping d
                            on a.meshcode = d.meshcode and to_int_no11 = d.nodeno
                        left join temp_node_mapping e
                            on a.meshcode = e.meshcode and to_int_no12 = e.nodeno
                        left join temp_node_mapping f
                            on a.meshcode = f.meshcode and to_int_no21 = f.nodeno
                        left join temp_node_mapping g
                            on a.meshcode = g.meshcode and to_int_no22 = g.nodeno
                    );
                    
                    analyze temp_org_spjcty;
                """
                
        self.pg.do_big_insert2(sqlcmd)
        
        self.log.info('make temp_org_spjcty end.')
    
    def __make_temp_org_eci_jctv(self):
        self.log.info('make temp_org_eci_jctv...')
        
        # 创建表单temp_org_eci_jctv 作成：
        # 原始表单org_eci_jctv中link id唯一化
        # 步骤1：根据mapping表获取唯一化id
            
        sqlcmd = """
                    drop table if exists temp_org_eci_jctv;
                    CREATE TABLE temp_org_eci_jctv 
                    as (
                        select a.eciid, a.seq, b.link_id as link1, c.link_id as link2,
                            d.link_id as link3, e.link_id as link4, f.link_id as link5,
                            g.link_id as link6, a.imagename
                        from org_eci_jctv a
                        left join temp_link_mapping b
                            on a.mesh1 = b.meshcode::integer and a.link1 = b.linkno
                        left join temp_link_mapping c
                            on a.mesh2 = c.meshcode::integer and a.link2 = c.linkno
                        left join temp_link_mapping d
                            on a.mesh3 = d.meshcode::integer and a.link3 = d.linkno
                        left join temp_link_mapping e
                            on a.mesh4 = e.meshcode::integer and a.link4 = e.linkno
                        left join temp_link_mapping f
                            on a.mesh5 = f.meshcode::integer and a.link5 = f.linkno
                        left join temp_link_mapping g
                            on a.mesh6 = g.meshcode::integer and a.link6 = g.linkno
                    );
                    
                    analyze temp_org_eci_jctv;
                """
                
        self.pg.do_big_insert2(sqlcmd)
        
        self.log.info('make temp_org_eci_jctv end.')
    
    def __make_temp_spotguide_tbl1(self):
        self.log.info('make temp_spotguide_tbl1...')
        
        # 使用JCT 创建表单temp_spotguide_tbl1 作成：
        # 引导路径link化
        # 原始数据提供的引导路径由node（最多4个点）组成，需要转换成对应的link列
        # 根据原始数据check，原始数据提供node列中相邻两点同属一条link
        # from_intersetion_no与atten_intersetion_no确定firstLink（进入link）
        # atten_intersetion_no作成引导点
        # atten_intersetion_no与to_int_no11（to_int_no21）确定secondLink（经过link或者脱出link）
        # to_int_no11（to_int_no12）与to_int_no12（to_int_no22）确定thirdLink（脱出link）
        # 根据原始数据check，to_int_no12/to_int_no22为空，所得thirdLink为空，故脱出link为secondLink
            
        sqlcmd = """
                    drop table if exists temp_spotguide_tbl1;
                    CREATE TABLE temp_spotguide_tbl1
                    as (
                        select 
                            b1.link_id as firstLink, 
                            atten_intersetion_no as node, 
                            c1.link_id as secondLink, 
                            d1.link_id as thirdLink, 
                            mng_no_bkg as patternno, mng_no_arrow1 as arrowno
                        from temp_org_spjcty a1
                        left join link_tbl b1
                            on from_intersetion_no = b1.s_node and atten_intersetion_no = b1.e_node
                        left join link_tbl c1
                            on atten_intersetion_no = c1.s_node and to_int_no11 = c1.e_node
                        left join link_tbl d1
                            on to_int_no11 = d1.s_node and to_int_no12 = d1.e_node
                        
                        union
                        
                        select 
                            b2.link_id as firstLink, 
                            atten_intersetion_no as node, 
                            c2.link_id as secondLink, 
                            d2.link_id as thirdLink, 
                            mng_no_bkg as patternno, mng_no_arrow2 as arrowno
                        from temp_org_spjcty a2
                        left join link_tbl b2
                            on from_intersetion_no = b2.s_node and atten_intersetion_no = b2.e_node
                        left join link_tbl c2
                            on atten_intersetion_no = c2.s_node and to_int_no21 = c2.e_node
                        left join link_tbl d2
                            on to_int_no21 = d2.s_node and to_int_no22 = d2.e_node
                    );
                    
                    analyze temp_spotguide_tbl1;
                """
                
        self.pg.do_big_insert2(sqlcmd)
        
        self.log.info('make temp_spotguide_tbl1 end.')
    
    def __make_temp_spotguide_tbl2(self):
        self.log.info('make temp_spotguide_tbl2...')
        
        # 使用Real JCT创建表单temp_spotguide_tbl2 作成：
        # 默认数据状况：link1/link2不为空，其他link可能为空；seq取值范围(2, 3, 4, 5, 6);link1~6依次相连   原始数据check保证
        # link1作为进入link
        # link1与link2共通node作为分歧点
        # link2~6最后一个有效link(不为空，记为linkX)作为脱出link
        # seq-2作为经过link个数
        # link1到linkX之间的link序列作为经过link
            
        sqlcmd = """
                    drop table if exists temp_spotguide_tbl2;
                    CREATE TABLE temp_spotguide_tbl2 
                    as (
                        select inlink, 
                            (case when b.s_node in (c.s_node, c.e_node) then b.s_node else b.e_node end) as nodeid,
                            passlid, passlink_cnt, outlink, imagename
                        from (
                            select link1 as inlink, link2 as seclink, 
                                (case seq when 2 then null
                                    when 3 then array_to_string(array[link2], '|')
                                    when 4 then array_to_string(array[link2, link3], '|')
                                    when 5 then array_to_string(array[link2, link3, link4], '|')
                                    else array_to_string(array[link2, link3, link4, link5], '|')
                                end)::varchar as passlid,
                                (seq-2)::smallint as passlink_cnt,
                                (case when link6 is not null then link6
                                    when link5 is not null then link5
                                    when link4 is not null then link4
                                    when link3 is not null then link3
                                    else link2
                                end) as outlink,
                                imagename
                            from temp_org_eci_jctv
                        ) a
                        left join link_tbl b
                            on a.inlink = b.link_id
                        left join link_tbl c
                            on a.seclink = c.link_id
                    );
                    
                    analyze temp_spotguide_tbl2;
                """
                
        self.pg.do_big_insert2(sqlcmd)
        
        self.log.info('make temp_spotguide_tbl2 end.')
    
    def __make_spotguide_tbl(self):
        self.log.info('make spotguide_tbl...')
        
        # 更新中间表spotguide_tbl
        
        # thirdLink为空，secondLink为最终脱出link，无经过link
        # 根据原始数据check，原始数据提供的spotguide不全是JCT分歧点，占比小于2%，故暂定所有spotguide全是JCT分歧点，type设定8
        sqlcmd = """
                    insert into spotguide_tbl (
                        nodeid, inlinkid, outlinkid, passlid, 
                        passlink_cnt, direction, guideattr, namekind, guideclass,
                        patternno, arrowno, type, is_exist_sar
                        ) 
                    select 
                        node as nodeid, 
                        firstLink as inlinkid, 
                        (case when thirdLink is null then secondLink else thirdLink end) as outlinkid,
                        (case when thirdLink is null then null else secondLink end)::varchar as passlid,
                        (case when thirdLink is null then 0 else 1 end)::smallint as passlink_cnt,
                        0::smallint as direction, 
                        0::smallint as guideattr, 
                        0::smallint as namekind,
                        0::smallint as guideclass, 
                        patternno, arrowno, 
                        8::smallint as type,
                        True as is_exist_sar
                    from temp_spotguide_tbl1
                """
                
        self.pg.do_big_insert2(sqlcmd)
        
        # Real JCT不需要arrowno
        # 根据APL反馈，其使用时不区分实景图类别，统一设定type=2(高速出口实景图)
        sqlcmd = """
                    insert into spotguide_tbl (
                        nodeid, inlinkid, outlinkid, passlid, 
                        passlink_cnt, direction, guideattr, namekind, guideclass,
                        patternno, type, is_exist_sar
                        ) 
                    select nodeid as nodeid, inlink as inlinkid, outlink as outlinkid,
                        passlid, passlink_cnt, 0::smallint as direction, 
                        0::smallint as guideattr, 0::smallint as namekind,
                        0::smallint as guideclass, imagename as patternno, 
                        2::smallint as type, True as is_exist_sar
                    from temp_spotguide_tbl2
                """
                
        self.pg.do_big_insert2(sqlcmd)  
        
        self.log.info('make spotguide_tbl end.')