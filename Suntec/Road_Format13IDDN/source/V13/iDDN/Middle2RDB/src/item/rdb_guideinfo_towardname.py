# -*- coding: cp936 -*-
'''
Created on 2012-3-6

@author: sunyifeng
'''

from base.rdb_ItemBase import ItemBase, ITEM_EXTEND_FLAG_IDX
from common import rdb_log

class rdb_guideinfo_towardname(ItemBase):
    '''
    classdocs
    '''

    def __init__(self):
        '''
        Constructor
        '''
        ItemBase.__init__(self, 'Toward_name'
                          , 'towardname_tbl'
                          , 'gid'
                          , 'rdb_guideinfo_towardname'
                          , 'gid'
                          , True
                          , ITEM_EXTEND_FLAG_IDX.get('TOWARDNAME'))


    def Do_CreateTable(self):

        if self.CreateTable2('temp_guideinfo_towardname_dic_name') == -1:
            return -1
        if self.CreateTable2('rdb_guideinfo_towardname') == -1:
            return -1

        return 0

    def Do_CreatIndex(self):

        if self.CreateIndex2('rdb_guideinfo_towardname_node_id_idx') == -1:
            return -1
        if self.CreateIndex2('rdb_guideinfo_towardname_in_link_id_node_id_idx') == -1:
            return -1
        if self.CreateIndex2('rdb_guideinfo_towardname_node_id_t_idx') == -1:
            return -1

        return 0


    def Do(self):

#        if self.__MakeTempNameDic() == -1:
#            return -1
        if self.__MakeTowardName() == -1:
            return -1
#        if self.__MakeNameDic() == -1:
#            return -1

        return 0


    def __MakeTempNameDic(self):

        sqlcmd = """
                select case when max(id) is null then 0 else max(id) end from
                (
                select max(name_id) as id from rdb_name_dictionary_jp
                union
                select max(name_id) as id from rdb_name_dictionary_yomi
                ) as T;
                """

        if self.pg.execute2(sqlcmd) == -1:
            rdb_log.log(self.ItemName, 'failed to fetch max nameid', 'error')
            return -1

        rowid = self.pg.fetchone2()
        cur_id = rowid[0]

        sqlcmd = """
                 select a.gid, b.language, b.name, b.py, b.seq_nm
                 from towardname_tbl as a left outer join name_dictionary_tbl as b on a.name_id = b.name_id
                 where not (b.name is null and b.py is null) and (b.language = 3 or b.language = 5)
                 order by a.gid, b.seq_nm, b.language;
        """

        if self.pg.execute2(sqlcmd) == -1:
            rdb_log.log(self.ItemName, 'failed to fetch max nameid', 'error')
            return -1

        old_gid = None
        old_seq = None
        old_name = None
        old_py = None
        old_en = None
        rs = None
        row = self.pg.fetchone2()
        flag = False
        if row:
            old_gid = row[0]
            old_seq = row[4]
            old_name = row[2]
            old_py = row[3]
            flag = True

        while row:

            if row[0] != old_gid and row[4] != old_seq:

                cur_id += 1
                rs = self.pg.insert('temp_guideinfo_towardname_dic_name',
                        ('gid', 'name_id', 'nm', 'py', 'en', 'seq'),
                        (old_gid, cur_id, old_name, old_py, old_en, old_seq)
                        )
                if rs == -1:
                    rdb_log.log(self.ItemName, 'error: inserting name dic data...', 'error')
                    return -1

                old_gid = row[0]
                old_seq = row[4]

            if row[1] == 3:
                old_en = row[2]
            if row[1] == 5:
                old_name = row[2]
                old_py = row[3]

            row = self.pg.fetchone2()

        if flag:
            rs = self.pg.insert('temp_guideinfo_towardname_dic_name',
                    ('gid', 'name_id', 'nm', 'py', 'en', 'seq'),
                    (old_gid, cur_id, old_name, old_py, old_en, old_seq))
            if rs == -1:
                rdb_log.log(self.ItemName, 'error: inserting name dic data...', 'error')
                return -1

        self.pg.commit2()

        return 0


    def __MakeNameDic(self):

        sqlcmd = """
                insert into rdb_name_dictionary_jp(name_id, "name")
                select distinct name_id, nm
                from temp_guideinfo_towardname_dic_name
                where nm is not null order by name_id;
            """
        rdb_log.log(self.ItemName, 'generating data for rdb_name_dictionary_jp.', 'info')
        if self.pg.execute2(sqlcmd) == -1:
            rdb_log.log(self.ItemName, 'InsertNameID():rdb_name_dictionary_jp failed.', 'error')
            return -1
        rdb_log.log(self.ItemName, 'OK.', 'info')

        sqlcmd = """
                insert into rdb_name_dictionary_yomi(name_id, "name")
                select distinct name_id, py
                from temp_guideinfo_towardname_dic_name
                where py is not null order by name_id;
                """
        rdb_log.log(self.ItemName, 'generating data for rdb_name_dictionary_yomi.', 'info')
        if self.pg.execute2(sqlcmd) == -1:
            rdb_log.log(self.ItemName, 'InsertNameID():rdb_name_dictionary_yomi failed.', 'info')
            return -1
        rdb_log.log(self.ItemName, 'OK.', 'info')

        self.pg.commit2()
        return 0

    def __MakeTowardName(self):
        sqlcmd = """
                insert into rdb_guideinfo_towardname
                            (
                            in_link_id,
                            in_link_id_t,
                            node_id,
                            node_id_t,
                            out_link_id,
                            out_link_id_t,
                            toward_name,
                            name_attr,
                            name_kind,
                            passlink_count
                            )
                  SELECT  a.tile_link_id as in_link_id
                        , a.tile_id as in_tile_id
                        , b.tile_node_id as node_id
                        , b.tile_id as node_tile_id
                        , c.tile_link_id as out_link_id
                        , c.tile_id as out_tile_id
                        , toward_name
                        , t.guideattr as name_attr
                        , t.namekind
                        , rdb_getpasslinkcnt(passlid, null) as passlink_count
                    FROM (
                        SELECT
                            gid,
                            id,
                            nodeid,
                            inlinkid,
                            outlinkid,
                            passlid,
                            passlink_cnt,
                            direction,
                            guideattr,
                            namekind,
                            toward_name,
                            "type"
                          FROM towardname_tbl
                    ) as t
                    LEFT OUTER JOIN rdb_tile_link as a
                    ON cast(t.inlinkid as bigint) = a.old_link_id
                    LEFT OUTER JOIN rdb_tile_node as b
                    ON cast(t.nodeid as bigint) = b.old_node_id
                    LEFT OUTER JOIN rdb_tile_link as c
                    ON cast(t.outlinkid as bigint) = c.old_link_id
                    order by t.gid--, d.seq_nm
                  """

        rdb_log.log(self.ItemName, 'Now it is inserting data to rdb_guideinfo_towardname ...', 'info')
        if self.pg.execute2(sqlcmd) == -1:
            rdb_log.log(self.ItemName, 'making rdb_guideinfo_towardname data failed.', 'error')
            return -1

        self.pg.commit2()

        rdb_log.log(self.ItemName, 'End make rdb_guideinfo_towardname.', 'info')

        return 0

    def _DoCheckValues(self):
        'check字段值'
        sqlcmd = """
                ALTER TABLE rdb_guideinfo_towardname DROP CONSTRAINT if exists check_name_attr;
                ALTER TABLE rdb_guideinfo_towardname
                  ADD CONSTRAINT check_name_attr CHECK (name_attr = ANY (ARRAY[0, 1, 2, 3, 4, 5, 6, 7]));

                ALTER TABLE rdb_guideinfo_towardname DROP CONSTRAINT if exists check_name_kind;
                ALTER TABLE rdb_guideinfo_towardname
                  ADD CONSTRAINT check_name_kind CHECK (name_kind = ANY (ARRAY[0, 1, 2, 3]));
                """
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()
            return 0

    def _DoContraints(self):
        '添加外键'
        sqlcmd = """
                ALTER TABLE rdb_guideinfo_towardname DROP CONSTRAINT if exists rdb_guideinfo_towardname_in_link_id_fkey;
                ALTER TABLE rdb_guideinfo_towardname
                  ADD CONSTRAINT rdb_guideinfo_towardname_in_link_id_fkey FOREIGN KEY (in_link_id)
                      REFERENCES rdb_link (link_id) MATCH FULL
                      ON UPDATE NO ACTION ON DELETE NO ACTION;

                ALTER TABLE rdb_guideinfo_towardname DROP CONSTRAINT if exists rdb_guideinfo_towardname_node_id_fkey;
                ALTER TABLE rdb_guideinfo_towardname
                  ADD CONSTRAINT rdb_guideinfo_towardname_node_id_fkey FOREIGN KEY (node_id)
                      REFERENCES rdb_node (node_id) MATCH FULL
                      ON UPDATE NO ACTION ON DELETE NO ACTION;

                ALTER TABLE rdb_guideinfo_towardname DROP CONSTRAINT if exists rdb_guideinfo_towardname_out_link_id_fkey;
                ALTER TABLE rdb_guideinfo_towardname
                  ADD CONSTRAINT rdb_guideinfo_towardname_out_link_id_fkey FOREIGN KEY (out_link_id)
                      REFERENCES rdb_link (link_id) MATCH FULL
                      ON UPDATE NO ACTION ON DELETE NO ACTION;

                --ALTER TABLE rdb_guideinfo_towardname DROP CONSTRAINT if exists rdb_guideinfo_towardname_name_id_fkey;
                --ALTER TABLE rdb_guideinfo_towardname
                --  ADD CONSTRAINT rdb_guideinfo_towardname_name_id_fkey FOREIGN KEY (name_id)
                --      REFERENCES rdb_name_dictionary (name_id) MATCH FULL
                --      ON UPDATE NO ACTION ON DELETE NO ACTION;
                """
        #sqlcmd = self.pg.ReplaceDictionary(sqlcmd)

        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()
            return 0

