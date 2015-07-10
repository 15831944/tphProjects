# -*- coding: UTF-8 -*-
'''
Created on 2012-3-6

@author: sunyifeng
'''
from base.rdb_ItemBase import ItemBase, ITEM_EXTEND_FLAG_IDX
from common import rdb_log
from common import rdb_common


class rdb_guideinfo_signpost(ItemBase):
    '''方面看板类
    '''

    def __init__(self):
        '''
        Constructor
        '''

        ItemBase.__init__(self, 'Signpost'
                          , 'signpost_tbl'
                          , 'gid'
                          , 'rdb_guideinfo_signpost'
                          , 'gid'
                          , True
                          , ITEM_EXTEND_FLAG_IDX.get('SIGNPOST'))

    def Do_CreateTable(self):
        if self.CreateTable2('temp_guideinfo_signpost_dic_name') == -1:
            return -1
        if self.CreateTable2('rdb_guideinfo_signpost') == -1:
            return -1

        return 0

    def Do_CreatIndex(self):

        if self.CreateIndex2('rdb_guideinfo_signpost_in_link_id_node_id_idx') == -1:
            return -1
        if self.CreateIndex2('rdb_guideinfo_signpost_node_id_idx') == -1:
            return -1
        if self.CreateIndex2('rdb_guideinfo_signpost_node_id_t_idx') == -1:
            return -1

        return 0

    def Do(self):

        # if self.__MakeTempNameDic() == -1:
        #    return -1
        if self.__MakeSignpost() == -1:
            return -1
        # if self.__MakeNameDic() == -1:
        #    return -1

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
        global_max_dic_id = rowid[0]
        cur_id = global_max_dic_id

        sqlcmd = """
                 select a.gid, b.language, b.name, b.py, b.seq_nm
                 from signpost_tbl as a left outer join name_dictionary_tbl as b on a.name_id = b.name_id
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
                rs = self.pg.insert('temp_guideinfo_signpost_dic_name',
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
            rs = self.pg.insert('temp_guideinfo_signpost_dic_name',
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
                from temp_guideinfo_signpost_dic_name
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
                from temp_guideinfo_signpost_dic_name
                where py is not null order by name_id;
                """
        rdb_log.log(self.ItemName, 'generating data for rdb_name_dictionary_yomi.', 'info')
        if self.pg.execute2(sqlcmd) == -1:
            rdb_log.log(self.ItemName, 'InsertNameID():rdb_name_dictionary_yomi failed.', 'info')
            return -1
        rdb_log.log(self.ItemName, 'OK.', 'info')

        self.pg.commit2()
        return 0

    def __MakeSignpost(self):

        sqlcmd = """
                  INSERT into rdb_guideinfo_signpost
                  (
                         in_link_id
                       , node_id
                       , out_link_id
                       , in_link_id_t
                       , node_id_t
                       , out_link_id_t
                       , pattern_id
                       , arrow_id
                       , sp_name
                       , passlink_count
                       , is_pattern
                  )
                  SELECT i.tile_link_id as inlinkid
                      , n.tile_node_id as nodeid
                      , o.tile_link_id --as outlinkid
                      , i.tile_id as in_tile_id
                      , n.tile_id as node_tile_id
                      , o.tile_id as out_tile_id
                      , (case
                         when d.gid is not null then d.gid
                         else cast(sp.patternno as smallint) end) as pattern_no
                      , (case
                         when e.gid is not null then e.gid
                         else cast(-1 as smallint) end) as arrow_no
                      , sp_name
                      , (case
                         when sp.passlink_cnt is not null then sp.passlink_cnt
                         else 0 end) as passlink_count
                      , sp.is_pattern
                  FROM  signpost_tbl as sp
                  LEFT OUTER JOIN rdb_tile_link as i
                  ON cast(sp.inlinkid as bigint) = i.old_link_id
                  LEFT OUTER JOIN rdb_tile_node as n
                  ON cast(sp.nodeid as bigint) = n.old_node_id
                  LEFT OUTER JOIN rdb_tile_link as o
                  ON cast(sp.outlinkid as bigint) = o.old_link_id
                  LEFT JOIN temp_guideinfo_pic_blob_id_mapping as d
                  on lower(sp.patternno) = lower(d.image_id)
                  LEFT JOIN temp_guideinfo_pic_blob_id_mapping as e
                  on lower(sp.arrowno) = lower(e.image_id)
                  order by sp.gid--, f.seq_nm;
                  """
        rdb_log.log(self.ItemName, 'Now it is inserting data to rdb_guideinfo_signpost ...', 'info')
        if self.pg.execute2(sqlcmd) == -1:
            rdb_log.log(self.ItemName, 'making the rdb_guideinfo_signpost data failed.', 'error')
            return -1
        else:
            self.pg.commit2()

        rdb_log.log(self.ItemName, 'End make rdb_guideinfo_signpost.', 'info')

        return 0

    def _DoContraints(self):
        '添加外键'
        sqlcmd = """
                --ALTER TABLE rdb_guideinfo_signpost DROP CONSTRAINT if exists rdb_guideinfo_signpost_arrow_id_fkey;
                --ALTER TABLE rdb_guideinfo_signpost
                --  ADD CONSTRAINT rdb_guideinfo_signpost_arrow_id_fkey FOREIGN KEY (arrow_id)
                --      REFERENCES rdb_guideinfo_pic_blob_bytea (gid) MATCH FULL
                --      ON UPDATE NO ACTION ON DELETE NO ACTION;

                ALTER TABLE rdb_guideinfo_signpost DROP CONSTRAINT if exists rdb_guideinfo_signpost_in_link_id_fkey;
                ALTER TABLE rdb_guideinfo_signpost
                  ADD CONSTRAINT rdb_guideinfo_signpost_in_link_id_fkey FOREIGN KEY (in_link_id)
                      REFERENCES rdb_link (link_id) MATCH FULL
                      ON UPDATE NO ACTION ON DELETE NO ACTION;

                ALTER TABLE rdb_guideinfo_signpost DROP CONSTRAINT if exists rdb_guideinfo_signpost_node_id_fkey;
                ALTER TABLE rdb_guideinfo_signpost
                  ADD CONSTRAINT rdb_guideinfo_signpost_node_id_fkey FOREIGN KEY (node_id)
                      REFERENCES rdb_node (node_id) MATCH FULL
                      ON UPDATE NO ACTION ON DELETE NO ACTION;

                --ALTER TABLE rdb_guideinfo_signpost DROP CONSTRAINT if exists rdb_guideinfo_signpost_name_id_fkey;
                --ALTER TABLE rdb_guideinfo_signpost
                --  ADD CONSTRAINT rdb_guideinfo_signpost_name_id_fkey FOREIGN KEY (name_id)
                --      REFERENCES rdb_name_dictionary (name_id) MATCH FULL
                --      ON UPDATE NO ACTION ON DELETE NO ACTION;
                """
        #sqlcmd = self.pg.ReplaceDictionary(sqlcmd)

        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()
            return 0
