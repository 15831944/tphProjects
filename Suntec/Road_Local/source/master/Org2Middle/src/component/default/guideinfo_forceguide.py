# -*- coding: UTF8 -*-
'''
Created on 2015-11-2

@author: lsq
'''



import os
import common.cache_file
from common.common_func import GetPath
import component.component_base


class com_guideinfo_forceguide(component.component_base.comp_base):
    '''强制诱导的Base Class
    '''

    def __init__(self):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self, 'Guideinfo_ForceGuide')
        
    def _DoCreateTable(self):
        
        self.CreateTable2('force_guide_tbl')
        
        return 0
    
    def _DoCreateFunction(self):
        
        return 0
        
        
    def _Do(self):
        
        self._make_forceguide_from_patch()
        self._make_forceguide_from_origin()
        
        return 0
    
    def _make_forceguide_from_origin(self):
        
        # 因数据协议不同，其提供的force guide元数据也不同
        # 针对上述不同，各数据协议单独处理，在此不作统一处理
        
        return 0
    
    def _make_forceguide_from_patch(self):
        
        self.log.info('Begin making force guide from patch file...')
        
        # force guide补丁采用统一的格式存储，其对应的处理逻辑如下所示
        
        # 根据配置文件信息制作force guide数据
        # force guide配置信息以node形式给出，配置文件数据组织形式：
        # 1、各数据以\t分割
        # 2、第一个字段force guide种别
        # 3、后面每2个字段一组 第一个记录node经纬度（geometry），第二个对应node对应的z level
        if self.import_patch() == -1:
            self.log.warning('End with can not find force guide patch file!!!')
            return -1
        
        self._update_temp_force_guide_patch_node_tbl()
        self._update_temp_force_guide_patch_link_tbl()
        self._update_force_guide_tbl_by_patch_data()
        
        self.log.info('End making force guide from patch file.')
        return 0
        
    def import_patch(self): 
        
        # 作成表单temp_force_guide_patch_tbl记录force guide配置信息，作成：
        # 1、抽取force guide type
        # 2、将node经纬度、node z level分别收录到一起，以‘|’分割
        forceguide_patch_full_path = GetPath('forceguide_patch_full_path')
        recIdx = 0
        if forceguide_patch_full_path:
            if os.path.exists(forceguide_patch_full_path):
                self.log.info('Begin importing force_guide_patch...')
                self.CreateTable2('temp_force_guide_patch_tbl')
                temp_file_obj = common.cache_file.open('temp_force_guide_patch_tbl')
                f_force_guide_patch = open(forceguide_patch_full_path, 'r') 
                for line in f_force_guide_patch.readlines():
                    linelist = []
                    linelist = line.split('\t')
                    linelength = len(linelist)
                    guidetype = int(linelist[0])
                    recIdx = recIdx + 1
                    temp_geom = ''
                    temp_z = ''
                    loop = 1
                    while loop <= (linelength-2):
                        if linelist[loop] != '':
                            temp_geom += linelist[loop]
                            temp_z += linelist[loop+1]
                            temp_geom += '|'
                            temp_z += '|'
                        loop += 2
                        
                    if temp_geom[-1] == '|':
                        temp_geom = temp_geom[0:-1]
                        
                    if temp_z[-1] == '|':
                        temp_z = temp_z[0:-1]
                    
                    rec_string = '%d\t%d\t%s\t%s\n' % (recIdx, guidetype, temp_geom, temp_z)
                    temp_file_obj.write(rec_string)
                    temp_file_obj.flush()
                temp_file_obj.seek(0)
                self.pg.copy_from2(temp_file_obj, 'temp_force_guide_patch_tbl')
                self.pg.commit2()
                common.cache_file.close(temp_file_obj,True)
                f_force_guide_patch.close()
                
                self.log.info('End importing force_guide_patch.')
                return 0

        return -1
    
    def _update_temp_force_guide_patch_node_tbl(self):
        
        self.log.info('Begin converting force guide patch geometry to node...')
        
        # 作成表单temp_force_guide_patch_node_tbl记录force guide关联的node信息。作成：
        # 将表单temp_force_guide_patch_tbl中提供的node经纬度信息转换为对应的node id
        # 1、从node_tbl中收录指定经纬度方圆10米内所有node
        # 2、选取距离最近且在指定z level的node
        sqlcmd = """
                DROP TABLE IF EXISTS temp_patch_node_tbl;
                CREATE TABLE temp_patch_node_tbl
                AS (
                    SELECT gid, 
                        string_to_array(str_geom, '|')::geometry[] as the_geom_list, 
                        string_to_array(str_z, '|')::smallint[] as z_level_list
                    FROM temp_force_guide_patch_tbl
                );
            """
        
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        self.CreateTable2('mid_temp_patch_node_tbl')
        self.CreateFunction2('mid_update_temp_patch_node_tbl')
        self.pg.callproc('mid_update_temp_patch_node_tbl')
        self.pg.commit2()
        
        self.log.info('End converting force guide patch geometry to node.')
        return 0
    
    def _update_temp_force_guide_patch_link_tbl(self):
        
        self.log.info('Begin converting force guide patch node to link...')
        
        # 作成表单temp_force_guide_patch_link_tbl记录force guide关联的link信息，作成：
        # 1、从temp_force_guide_patch_node_tbl中取一条记录中任意相邻两点算路，得到一段路径
        # 2、将一条记录的所有node算路所获路径连在一起，即为当前force guide对应的link
        self.CreateTable2('mid_temp_patch_link_tbl')
        self.CreateFunction2('mid_findpasslinkbybothnodes')
        self.CreateFunction2('mid_update_temp_patch_link_tbl')
        self.pg.callproc('mid_update_temp_patch_link_tbl')
        self.pg.commit2()
        
        self.log.info('End converting force guide patch node to link.')
        return 0
    
    def _update_force_guide_tbl_by_patch_data(self):
        
        self.log.info('Begin updating force guide table by patch data...')
        
        # 更新表单force_guide_tbl
        # 根据force guide关联的link信息（表单temp_force_guide_patch_link_tbl）计算引导点、位置信息等
        sqlcmd = """
                DROP TABLE IF EXISTS mid_temp_patch_link_tbl_bak;
                CREATE TABLE mid_temp_patch_link_tbl_bak
                AS (
                    SELECT *
                    FROM mid_temp_patch_link_tbl
                );
            """
        
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        self.CreateTable2('mid_temp_patch_link_tbl')
        self.CreateFunction2('mid_update_force_guide_temp_patch_link_tbl')
        self.pg.callproc('mid_update_force_guide_temp_patch_link_tbl')
        self.pg.commit2()
        
        sqlcmd = """
                DROP SEQUENCE IF EXISTS temp_link_forceguide_seq;
                CREATE SEQUENCE temp_link_forceguide_seq;
                SELECT setval('temp_link_forceguide_seq', cast(max_id as bigint))
                FROM
                (
                    SELECT max(force_guide_id) as max_id
                    FROM force_guide_tbl
                ) a;
            """
        
        self.pg.execute2(sqlcmd)
        self.pg.commit2() 
        
        self.CreateFunction2('mid_get_position_type_by_links')
        
        sqlcmd = """
                INSERT INTO force_guide_tbl(
                    force_guide_id, nodeid, inlinkid, outlinkid,
                    passlid, passlink_cnt, guide_type, position_type
                )
                SELECT nextval('temp_link_forceguide_seq') as force_guide_id,
                    nodeid, inlinkid, outlinkid, passlid, passlink_cnt, guide_type, 
                    mid_get_position_type_by_links(inlinkid, outlinkid) as position_type
                FROM (
                    SELECT c.node_id_list[2] as nodeid, b.link_id_list[1] as inlinkid,
                        b.link_id_list[array_upper(b.link_id_list, 1)] as outlinkid,
                        (case when array_upper(b.link_id_list, 1) = 2 then null else array_to_string(b.link_id_list[2:array_upper(b.link_id_list, 1)-1], '|') end)::varchar as passlid,
                        (array_upper(b.link_id_list, 1) - 2)::smallint as passlink_cnt,
                        d.guide_type
                    FROM mid_temp_patch_link_tbl b
                    LEFT JOIN mid_temp_patch_node_tbl c
                        ON b.gid = c.gid
                    LEFT JOIN temp_force_guide_patch_tbl d
                        ON b.gid = d.gid
                ) a
            """
        
        self.pg.execute2(sqlcmd)
        self.pg.commit2() 
         
        self.log.info('End updating force guide table by patch data.')
        return 0
    
    def _update_force_guide_tbl_by_origin_data(self):
        
        self.log.info('Begin updating force guide table by origin data...')
        
        sqlcmd = """
                DROP SEQUENCE IF EXISTS temp_link_forceguide_seq;
                CREATE SEQUENCE temp_link_forceguide_seq;
                SELECT setval('temp_link_forceguide_seq', cast(max_id as bigint))
                FROM
                (
                    select max(force_guide_id) as max_id
                    from force_guide_tbl
                ) a;
            """
        
        self.pg.execute2(sqlcmd)
        self.pg.commit2()  
        
        sqlcmd = """
                INSERT INTO force_guide_tbl (
                    force_guide_id, nodeid, inlinkid, outlinkid, passlid,
                    passlink_cnt, guide_type, position_type
                )
                SELECT nextval('temp_link_forceguide_seq') as force_guide_id,
                    nodeid, inlinkid, outlinkid, passlid, passlink_cnt,
                    guide_type, 0 as position_type
                FROM (
                    SELECT distinct a.nodeid, a.inlinkid, a.outlinkid, a.passlid, a.passlink_cnt, a.guide_type
                    FROM mid_temp_force_guide_tbl a 
                    LEFT JOIN force_guide_tbl b
                        ON 
                            b.nodeid = a.nodeid and
                            b.inlinkid = a.inlinkid and
                            b.outlinkid = a.outlinkid and
                            not(b.passlid is distinct from a.passlid) and
                            b.passlink_cnt = a.passlink_cnt
                    WHERE b.gid IS NULL
                ) c;
            """
        
        self.pg.execute2(sqlcmd)
        self.pg.commit2()  
        
        self.log.info('End updating force guide table by origin data.') 
        return 0