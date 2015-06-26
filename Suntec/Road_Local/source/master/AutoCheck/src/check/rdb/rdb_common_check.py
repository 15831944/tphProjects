# -*- coding: UTF8 -*-
'''
Created on 2012-12-7

@author: hongchenzai
rdb通用check类
'''
import platform.TestCase
import time

class CCheckCommonBase(platform.TestCase.CTestCase):
    def _isClient(self, table_name):
        if table_name == None: return False
        if table_name[len(table_name)-len('client'):] == 'client':
            return True
        return False
    
class CCheckLinkIDFKey(CCheckCommonBase):
    '''in_link_id/out_link_id的外键。'''
    def _do(self):
        sqlcmd = """
                ALTER TABLE [replace_table] DROP CONSTRAINT 
                  if exists [replace_table]_[replace_link_id]_fkey; 
                ALTER TABLE [replace_table]
                  ADD CONSTRAINT [replace_table]_[replace_link_id]_fkey FOREIGN KEY ([replace_link_id])
                      REFERENCES rdb_link (link_id) MATCH FULL
                      ON UPDATE NO ACTION ON DELETE NO ACTION;
                """
        table_name = self.caseinfo.getTableName()
        # 替换表名
        sqlcmd = sqlcmd.replace('[replace_table]', table_name)
        # 替换link id字段名
        sqlcmd = sqlcmd.replace('[replace_link_id]', self.caseinfo.getMajorItem())
        if self._isClient(table_name):
            sqlcmd = sqlcmd.replace('rdb_link', 'rdb_link_client')
        
        if self.pg.execute(sqlcmd) == -1:
            return False
        else:
            self.pg.commit()
            return True
        
class CCheckNodeIDFKey(CCheckCommonBase):
    '''node_id的外键。'''
    def _do(self):
        sqlcmd = """
                ALTER TABLE [replace_table] DROP CONSTRAINT 
                  if exists [replace_table]_[replace_node_id]_fkey;
                ALTER TABLE [replace_table]
                  ADD CONSTRAINT [replace_table]_[replace_node_id]_fkey FOREIGN KEY ([replace_node_id])
                      REFERENCES rdb_node (node_id) MATCH FULL
                      ON UPDATE NO ACTION ON DELETE NO ACTION;
                """
        table_name = self.caseinfo.getTableName()
        # 替换表名
        sqlcmd = sqlcmd.replace('[replace_table]', table_name)
        # 替换node id字段名
        sqlcmd = sqlcmd.replace('[replace_node_id]', self.caseinfo.getMajorItem())
        if self._isClient(table_name):
            sqlcmd = sqlcmd.replace('rdb_node', 'rdb_node_client')
            
        if self.pg.execute(sqlcmd) == -1:
            return False
        else:
            self.pg.commit()
            return True
    
class CCheckLinkTileId(CCheckCommonBase):
    '''check Link的Tile Id'''
    def _do(self):
        sqlcmd = """
                SELECT count(*)
                  FROM [replace_table]
                  left join rdb_tile_link
                  on [replace_link_id] = tile_link_id and [replace_tile_id] <> tile_id
                  where tile_link_id is not null;
              """
        # 替换表名
        sqlcmd = sqlcmd.replace('[replace_table]', self.caseinfo.getTableName())
        # 替换link id字段名
        sqlcmd = sqlcmd.replace('[replace_link_id]', self.caseinfo.getMajorItem())
        # 替换tile id字段名
        sqlcmd = sqlcmd.replace('[replace_tile_id]', self.caseinfo.getCheckContent())
        
        self.pg.execute(sqlcmd)
        row = self.pg.fetchone()
        if row:
            if row[0] != 0:
                return False
            else:
                return True
        else:
            return False

class CCheckNodeTileId(CCheckCommonBase):
    '''check点的Tile Id'''
    def _do(self):
        sqlcmd = """
                SELECT count(*)
                  FROM [replace_table]
                  left join rdb_tile_node
                  on [replace_node_id] = tile_node_id and [replace_tile_id] <> tile_id
                  where tile_node_id is not null;
              """
        # 替换表名
        sqlcmd = sqlcmd.replace('[replace_table]', self.caseinfo.getTableName())
        # 替换link id字段名
        sqlcmd = sqlcmd.replace('[replace_node_id]', self.caseinfo.getMajorItem())
        # 替换tile id字段名
        sqlcmd = sqlcmd.replace('[replace_tile_id]', self.caseinfo.getCheckContent())
        
        self.pg.execute(sqlcmd)
        row = self.pg.fetchone()
        if row:
            if row[0] != 0:
                return False
            else:
                return True
        else:
            return False

class CCheckPassLinkCount(CCheckCommonBase):
    '''check PassLink的条目。'''
    def __init__(self, suite, caseinfo, middle_table, passlink_cnt_col):
        platform.TestCase.CTestCase.__init__(self, suite, caseinfo)
        self._middle_table = middle_table
        self._passlink_cnt_col = passlink_cnt_col
        
    def _do(self):
        sqlcmd = """
                select count(*)
                  from [middle_tbl]
                  left join [rdb_tbl]
                  ON [middle_tbl].gid = [rdb_tbl].gid
                  where [middle_col] <> [rdb_col];
                """
                
        sqlcmd = sqlcmd.replace('[middle_tbl]', self._middle_table)
        sqlcmd = sqlcmd.replace('[middle_col]', self._passlink_cnt_col)
        sqlcmd = sqlcmd.replace('[rdb_tbl]', self.caseinfo.getTableName())
        sqlcmd = sqlcmd.replace('[rdb_col]', self.caseinfo.getMajorItem())
        
        self.pg.execute(sqlcmd)
        row = self.pg.fetchone()
        if row:
            if row[0] != 0:
                return False
            else:
                return True
        else:
            return False

class CCheckPassLinkCountWithGuideidParam(platform.TestCase.CTestCase):
    '''check PassLink的条目。'''
    def __init__(self, suite, caseinfo, middle_table, passlink_cnt_col):
        platform.TestCase.CTestCase.__init__(self, suite, caseinfo)
        self._middle_table = middle_table
        self._passlink_cnt_col = passlink_cnt_col
        
    def _do(self):
        sqlcmd = """
                select count(*)
                  from [middle_tbl]
                  left join [rdb_tbl]
                  ON [middle_tbl].gid = [rdb_tbl].guideinfo_id
                  where (case when [middle_col] is Null then 0::smallint else [middle_col] end) <> [rdb_col];
                """
                
        sqlcmd = sqlcmd.replace('[middle_tbl]', self._middle_table)
        sqlcmd = sqlcmd.replace('[middle_col]', self._passlink_cnt_col)
        sqlcmd = sqlcmd.replace('[rdb_tbl]', self.caseinfo.getTableName())
        sqlcmd = sqlcmd.replace('[rdb_col]', self.caseinfo.getMajorItem())
        
        self.pg.execute(sqlcmd)
        row = self.pg.fetchone()
        if row:
            if row[0] != 0:
                return False
            else:
                return True
        else:
            return False
    
class CCheckInOutLinkId(CCheckCommonBase):
    '''InLinkID和OutLInkID是否相等。'''
    def _do(self):
        sqlcmd = """
        ALTER TABLE [replace_table] DROP CONSTRAINT
          IF EXISTS check_in_out_linkid;
        ALTER TABLE [replace_table]
          ADD CONSTRAINT check_in_out_linkid CHECK (in_link_id <> out_link_id);
        """
        
        sqlcmd = sqlcmd.replace('[replace_table]', self.caseinfo.getTableName())
        if self.pg.execute(sqlcmd) == -1:
            return False
        else: 
            self.pg.commit()
            return True

class CCheckNumber(CCheckCommonBase):
    '''检查记录的条目数。[check内容]要填上：中间表名。'''        
    def _do(self):
        sqlcmd = """
                SELECT 
                (SELECT COUNT([src_key_col])
                  from [src_tbl] ) as src_num,
                (SELECT COUNT([dest_key_col])
                  from [dest_tbl] ) as dest_num;
                """
        src_tbl_name  = self.caseinfo.getCheckContent()
        dest_tbl_name = self.caseinfo.getTableName()
        # 取得源表的主键对应字段
        key_col_list  = self.pg.GetPKeyColumns(src_tbl_name)
        if key_col_list == None:
            self.logger.warning("Dosen't exist PKey in table :", src_tbl_name)
            src_key_col = '*'
        else:
            src_key_col = ','.join(key_col_list)
            
        # 取得目标表主键对应字段
        key_col_list  = self.pg.GetPKeyColumns(dest_tbl_name)
        if key_col_list == None:
            self.logger.warning("Dosen't exist PKey in table :", dest_tbl_name)
            dest_key_col = '*'
        else:
            dest_key_col = ','.join(key_col_list)
            
        sqlcmd = sqlcmd.replace('[src_tbl]', src_tbl_name)
        sqlcmd = sqlcmd.replace('[dest_tbl]', dest_tbl_name)
        sqlcmd = sqlcmd.replace('[src_key_col]', src_key_col)
        sqlcmd = sqlcmd.replace('[dest_key_col]', dest_key_col)
        
        self.pg.execute(sqlcmd)
        row = self.pg.fetchone()
        if row:
            if row[0] != row[1]:
                return False
            else:
                if row[1] == 0:
                    self.logger.warning('The Number of %s is: 0', self.caseinfo.getTableName())
                return True
        else:
            return False
        
class CCheckValue(CCheckCommonBase):
    '''检查字段的有效值。[check内容]要填上：有效值 。''' 
    def _do(self):
        sqlcmd = """
            ALTER TABLE [rdb_tbl] DROP CONSTRAINT 
              IF EXISTS check_[col_name];
            ALTER TABLE [rdb_tbl]
              ADD CONSTRAINT check_[col_name] CHECK ([col_name] = ANY ([ARRAY_VALUES]));
            """
        # 取得有效值
        content = self.caseinfo.getCheckMethod()
        values = content[content.find('(')+1:content.rfind(')')]
        if values == '':
            values = content[content.find('{')+1:content.rfind('}')]
            values = values.replace('{')
        if values == '':
            values = content[content.find('[')+1:content.rfind(']')]
        if values != '':
            values = 'ARRAY['+ values +']'
        else:
            return False
        
        sqlcmd = sqlcmd.replace('[rdb_tbl]', self.caseinfo.getTableName())
        sqlcmd = sqlcmd.replace('[col_name]', self.caseinfo.getMajorItem())
        sqlcmd = sqlcmd.replace('[ARRAY_VALUES]', values)
        if self.pg.execute(sqlcmd) == -1:
            return False
        else:
            self.pg.commit()
            return True
        
class CCheckClientColumn(CCheckCommonBase):
    '''检查客户端某列的值与服务器是否一致。[大项] 必须写上字段名'''
    def _getCondition(self):
        col = self.caseinfo.getMajorItem()
        # 一般server和 client字段同名
        condition = 'a.[col] <> b.[col]'
        return condition.replace('[col]', col)
    
    def _do(self):
        sqlcmd = """
                SELECT count(a.[src_key_col])
                  FROM [src_tbl] as a
                  left join [dest_tbl] as b
                  on a.[src_key_col] = b.[dest_key_col]
                  where [condition];
                """
        dest_tbl_name = self.caseinfo.getTableName()
        src_tbl_name  = dest_tbl_name[:dest_tbl_name.rfind('_client')]
        
        # 取提源主键列
        key_col_list  = self.pg.GetPKeyColumns(src_tbl_name)
        if key_col_list == None or len(key_col_list) > 1:
            return False
        else:
            src_key_col = key_col_list[0]
        # 取提目标主键列 
        key_col_list  = self.pg.GetPKeyColumns(dest_tbl_name)
        if key_col_list == None or len(key_col_list) > 1:
            return False
        else:
            dest_key_col = key_col_list[0]
        
        sqlcmd = sqlcmd.replace('[src_tbl]', src_tbl_name)
        sqlcmd = sqlcmd.replace('[dest_tbl]', dest_tbl_name)
        sqlcmd = sqlcmd.replace('[src_key_col]', src_key_col)
        sqlcmd = sqlcmd.replace('[dest_key_col]', dest_key_col)
        sqlcmd = sqlcmd.replace('[condition]', self._getCondition())
        #print sqlcmd
        self.pg.query(sqlcmd)
        row = self.pg.fetchone()
        if row:
            return (row[0] == 0)
        return False
    
class CCheckID(CCheckClientColumn):
    '''检查客户端Link_ID/Node_ID的值与服务器是否一致。'''
    def _getCondition(self):
        col = self.caseinfo.getMajorItem() # [大项]是的字段名
        # 一般server和 client字段同名
        condition = '(a.[col] & ((1::bigint << 32) - 1)) <> b.[col]'
        return condition.replace('[col]', col)
    
class CCheckItemTileIDSame():
    def __init__(self, pg_object, table_name, link_id_column, tile_id_column): 
        """
        param1: pg pointer
        param2: prepared for checked table name
        param3: checked column
        param4: compare column
        """
        self.__pg = pg_object
        self.__table_name = table_name
        self.__link_id_column = link_id_column
        self.__tile_id_column = tile_id_column
        
    def do(self):
        sqlcmd = """
        select count(*) from
            (
                select * from %s where (%s >> 32) <> %s
            ) as a
        """  %(self.__table_name, self.__link_id_column, self.__tile_id_column)
        
        dest_cnt = self.__pg.getOnlyQueryResult(sqlcmd)
        
        return dest_cnt == 0

class CCheckNodeExtendFlag():
    def __init__(self, pg_object, check_org_tbl_name, bit_pos):
        self.__pg = pg_object
        self.__org_tbl_name = check_org_tbl_name
        self.__check_bit_mask = bit_pos
    
    def do(self):
        # cnt equal
        sqlcmd = """
            select count(*) from rdb_node where (extend_flag & (1 << %d)) != 0
        """ %(self.__check_bit_mask)
        
        cnt_in_node_tbl = self.__pg.getOnlyQueryResult(sqlcmd)
        
        sqlcmd = """
            select count(distinct(node_id)) from %s
        """ %(self.__org_tbl_name)
        
        cnt_in_org_tbl = self.__pg.getOnlyQueryResult(sqlcmd)
        
        if (cnt_in_node_tbl != cnt_in_org_tbl):
            return False
        
        sqlcmd = """
            select count(*) from
            (
                select A.node_id, B.extend_flag from %s as A left join rdb_node as B
                on A.node_id = B.node_id
            ) as C 
            where (C.extend_flag & (1 << %d)) = 0
        """ %(self.__org_tbl_name, self.__check_bit_mask)
        
        return 0 == self.__pg.getOnlyQueryResult(sqlcmd)
    
class CCheckGuideInlinkOutlinkConnectivity(CCheckCommonBase):
    '''check guide 表单的node_id的有效性，inlink与outlink连通性'''
    
    def _do(self): 
        try:
            if self.pg.CreateFunction_ByName('rdb_check_guide_links') == -1:
                time.sleep(60)
        except:
            self.pg.rollback()
            pass
                    
        sqlcmd = """
        select count(*) from (
            select in_link_id,node_id,out_link_id,passlink_count,dir1,dir2,
                rdb_check_guide_links(in_link_id,node_id,passlink_count,out_link_id,dir1::smallint) as pass_flag1,
                case when dir2 is not null then rdb_check_guide_links(in_link_id,node_id,passlink_count,out_link_id,dir2::smallint) 
                end as pass_flag2
            from (
                select  a.*
                    ,case when b.one_way = 1 and a.node_id = b.start_node_id then 0
                         when b.one_way = 1 and a.node_id = b.end_node_id then 1
                         when b.one_way  = 2 and a.node_id = b.start_node_id then 0
                         when b.one_way  = 2 and a.node_id = b.end_node_id then 1
                         when b.one_way  = 3 and a.node_id = b.start_node_id then 1
                         when b.one_way  = 3 and a.node_id = b.end_node_id then 0
                         else 0
                    end as dir1
                    ,case when b.one_way = 1 and a.node_id = b.start_node_id then 1
                         when b.one_way = 1 and a.node_id = b.end_node_id then 0
                         else null
                    end as dir2
                from [replace_table] a
                left join rdb_link b
                on a.in_link_id = b.link_id
                where a.in_link_id is not null
            ) c
        ) d where pass_flag1 is false and pass_flag2 is false;
        """
        # 替换表名
        sqlcmd = sqlcmd.replace('[replace_table]', self.caseinfo.getTableName())
        
        self.pg.execute(sqlcmd)
        row = self.pg.fetchone()
        if row:
            if row[0] != 0:
                return False
            else:
                return True
        else:
            return False     
        
    