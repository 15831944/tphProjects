# -*- coding: cp936 -*-
'''
Created on 2012-3-14

@author: hongchenzai
'''
from base.rdb_ItemBase import ItemBase
from common import rdb_log
from common import rdb_common

class rdb_dictionary(ItemBase):
    '''
    dictionary�࣬ͳһ�ֵ䴦��
    '''

    def __init__(self):
        '''
        Constructor
        '''
        ItemBase.__init__(self, 'Dictionary')
        
    def Do_CreateTable(self):
        self.CreateTable2('temp_name_dictionary')
        self.CreateTable2('temp_link_name_and_number')
        
        # �����Ա�������ݿ�
        # ����org2middle����
        # self.__InsertLanguages()
        
        rows = self.pg.GetLanguageTableName()
        # �������иð汾���ݰ��������Ե��ֵ��
        for row in rows:
            self.pg.CreateDictionaryTable(row[1])
            self.pg.CreateDictionaryTable(row[2])
            
        return 0
    
    def Do_CreateFunction(self):
        # �ú���Ϊ��name_id, seq_nm�� һһ��Ӧһ���µ�name_id
        self.CreateFunction2('rdb_insert_temp_name_dictionary')
        return 0
    
    def Do(self):
        # Ϊ��name_id, seq_nm�� һһ��Ӧһ���µ�name_id
        self.pg.callproc('rdb_insert_temp_name_dictionary')
        self.pg.commit2()
        self.CreateIndex2('temp_name_dictionary_old_name_id_idx')
        
        rows = self.pg.GetLanguageTableName()
        if rows == None:
            return 0
        
        sqlcmd1 = """
                INSERT INTO rdb_name_dictionary (
                            name_id, "name")
                (
                SELECT new_name_id, "name"
                  FROM temp_name_dictionary AS A
                  LEFT JOIN name_dictionary_tbl AS B
                  ON  old_name_id = name_id
                  where "language" = %s
                  ORDER BY new_name_id
                );
        """
        sqlcmd2 = """
                INSERT INTO rdb_name_dictionary(
                            name_id, "name")
                (
                SELECT new_name_id, py
                  FROM temp_name_dictionary AS A
                  LEFT JOIN name_dictionary_tbl AS B
                  ON  old_name_id = name_id
                  where "language" = %s
                  ORDER BY new_name_id
                );
                """
        for row in rows:
            if row[1] != None:
                # ��������
                script = sqlcmd1.replace('rdb_name_dictionary', row[1])
                self.pg.execute2(script, [row[0],])
                # ����idһ����-1���Ŀ�����
                self.pg.execute2("INSERT INTO " + row[1]+ "(name_id, name) values(-1, '');")
                self.pg.commit2()
                # ����ƴ��
                if row[2] != None and row[2] != '':
                    script = sqlcmd2.replace('rdb_name_dictionary', row[2])
                    self.pg.execute2(script, [row[0],])
                    # ����idһ����-1���Ŀ�����
                    self.pg.execute2("INSERT INTO " + row[2]+ "(name_id, name) values(-1, '');")
                    self.pg.commit2()
        
        # "������link_id������ID, ��·����ID�� ��ϵ��(һ��һ) "     
        self.__MakeLinkNameRelationTbl()
        
        # "������link_id������ID, ��·����ID�� ��ϵ��(��Զ�) "     
        self._MakeLinkNameAndNumber()
            
        return 0
        
    def __MakeLinkNameRelationTbl(self):
        "link_id������ID, ��·����ID�� "
        "ע��1. link��·�����ƣ����з���ʱ������ID��ֵ�ǵ�·����ID��"
        "   2. ����ID, ��·����ID��ѡ��link_name_relation_tbl��seq_nmΪ'1'�ļ�¼��"
        
        rdb_log.log(self.ItemName, 'It is make link name relation table.', 'info')
        self.CreateTable2('temp_rdb_link_name')
        sqlcmd = """
                insert into temp_rdb_link_name
                (
                SELECT a.link_id, (case when b.new_name_id is not null then b.new_name_id else c.new_name_id end) as road_name_id, c.new_name_id as road_number_id
                FROM (
                    SELECT distinct(link_id) as link_id
                      FROM link_name_relation_tbl
                     ) AS a
                 LEFT JOIN (
                    SELECT link_id, name_id
                      FROM link_name_relation_tbl
                      WHERE seq_nm = 1 AND name_type <> 3
                 ) as t
                 ON a.link_id = t.link_id
                 LEFT JOIN  temp_name_dictionary as b
                 ON t.name_id = b.old_name_id
                 LEFT JOIN (
                    SELECT link_id, name_id
                      FROM link_name_relation_tbl
                      WHERE seq_nm = 1 AND name_type = 3
                 ) as r
                 ON a.link_id = r.link_id
                 LEFT JOIN  temp_name_dictionary as c
                 ON r.name_id = c.old_name_id
                );
        """
        
        self.pg.execute2(sqlcmd)      
        self.pg.commit2()
        
    def _MakeLinkNameAndNumber(self):
        "link�����ƺͷ��ŵĹ�ϵ������Ҫ��ZhouChao�ã�"
        
        sqlcmd = """
            INSERT INTO temp_link_name_and_number(link_id, name_id, scale_type, shield_id, seq_nm, name_type, "name", country_code)
            (
            SELECT tile_link_id, d.new_name_id, scale_type, shield_id, seq_nm, name_type, name, country_code
              FROM link_name_relation_tbl as a
              LEFT JOIN rdb_tile_link as b
              ON old_link_id = link_id
              LEFT JOIN name_dictionary_tbl as c
              ON a.name_id = c.name_id
              LEFT JOIN temp_name_dictionary as d
              ON  a.name_id = d.old_name_id
            );
        """
        
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()
            
        self.CreateIndex2('temp_link_name_and_number_link_id_idx')
        self.CreateIndex2('temp_link_name_and_number_name_id_idx')
        
    def _DoCheckNumber(self):
        'check��Ŀ'
        # ����в������Ŀ���ܿ�������Ϊrdb_name_dictionary_x�����һ��name=-1�ļ�¼��
        
        dest_name_num  = 0
        # ȡ�������ֵ��
        lang_tbls = self.pg.GetLanguageTableName()
        # ȡ�������ֵ���¼������
        for tbl in lang_tbls:
            lang_tbl_name = tbl[1]
            sqlcmd = """
                    select count(name_id)
                      from rdb_name_dictionary;
                    """
            sqlcmd = sqlcmd.replace('rdb_name_dictionary', lang_tbl_name)
            self.pg.execute2(sqlcmd)
            row = self.pg.fetchone2()
            if row:
                dest_name_num += row[0]
        
        # ȡ��ԭ�ֵ�������
        src_name_num = 0
        sqlcmd = """
                SELECT count(gid)
                  FROM name_dictionary_tbl;
                """
        self.pg.execute2(sqlcmd)
        row = self.pg.fetchone2()
        if row:
            src_name_num = row[0]
        
        if src_name_num != dest_name_num:
            rdb_log.log(self.ItemName, 'Number of name_dictionary_tbl: ' + str(src_name_num) + ','\
                                'But Number of rdb_name_dictionarys: ' + str(dest_name_num) + '.', 'warning')
        else:
            rdb_log.log(self.ItemName, 'Number of  name_dictionary_tbl and  rdb_name_dictionarys: ' + str(src_name_num) + '.', 'info')
        
        return 0  
        
        
        
        
        
    
    