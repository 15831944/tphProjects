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
        self.__make_temp_spotguide_tbl()
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
        self.CreateTable2('temp_org_spjcty')
        sqlcmd = """
                insert into temp_org_spjcty (
                    atten_intersetion_no, from_intersetion_no, mng_no_bkg, to_int_no11, 
                    to_int_no12, mng_no_arrow1, to_int_no21, to_int_no22, mng_no_arrow2
                    ) 
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
            """
        self.pg.do_big_insert2(sqlcmd)
        
        self.log.info('make temp_org_spjcty end.')
    
    def __make_temp_spotguide_tbl(self):
        self.log.info('make temp_spotguide_tbl...')
        
        # 创建表单temp_spotguide_tbl 作成：
        # 引导路径link化
        # 原始数据提供的引导路径由node（最多4个点）组成，需要转换成对应的link列
        # 根据原始数据check，原始数据提供node列中相邻两点同属一条link
        # from_intersetion_no与atten_intersetion_no确定firstLink（进入link）
        # atten_intersetion_no作成引导点
        # atten_intersetion_no与to_int_no11（to_int_no21）确定secondLink（经过link或者脱出link）
        # to_int_no11（to_int_no12）与to_int_no12（to_int_no22）确定thirdLink（脱出link）
        # 根据原始数据check，to_int_no12/to_int_no22为空，所得thirdLink为空，故脱出link为secondLink
        self.CreateTable2('temp_spotguide_tbl')
        sqlcmd = """
                insert into temp_spotguide_tbl (
                    firstLink, node, secondLink, 
                    thirdLink, patternno, arrowno
                    ) 
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
            """
        self.pg.do_big_insert2(sqlcmd)
        
        self.log.info('make temp_spotguide_tbl end.')
    
    def __make_spotguide_tbl(self):
        self.log.info('make spotguide_tbl...')
        
        # 获取中间表spotguide_tbl
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
                from temp_spotguide_tbl
            """
        self.pg.do_big_insert2(sqlcmd)
        
        self.log.info('make spotguide_tbl end.')