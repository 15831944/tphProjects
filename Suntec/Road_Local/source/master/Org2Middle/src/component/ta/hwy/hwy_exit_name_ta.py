# -*- coding: UTF-8 -*-
'''
Created on 20150317

@author: PC_ZH
'''
from component.component_base import comp_base
from component.rdf.hwy.hwy_exit_name_rdf import HwyExitNameRDF
from component.ta.multi_lang_name_ta import MultiLangNameTa
from common import cache_file
from component.default.multi_lang_name import NAME_TYPE_OFFICIAL
IS_Motorway_Exit = 1
IS_Motorway_Interchange = 2


class HwyExitNameTa(HwyExitNameRDF):
    '''exit name tomtom'''
    def __init__(self):
        '''
        Constructor
        '''
        HwyExitNameRDF.__init__(self)

    def _DoCreateTable(self):
        self.CreateTable2('mid_temp_hwy_exit_name_ta')
        return 0

    def _DoCreateFunction(self):
        return 0

    def _DoCreateIndex(self):
        return 0

    def _make_hwy_exit_name(self):
        self.log.info('Make exit Name.')
        self.CreateIndex2('org_ig_elemid_idx')
        self.CreateIndex2('org_ig_id_idx')
        self.CreateIndex2('org_is_id_idx')
        sqlcmd = """
        SELECT distinct node_tbl.node_id, org_is.fwinttyp,
               org_is.name, org_is.namelc
        FROM org_is
        LEFT JOIN org_ig
          ON org_is.id = org_ig.id
        INNER JOIN node_tbl
        on org_ig.elemid = node_tbl.node_id
        WHERE org_is.inttyp = 1 -- FREE WAY
          AND org_is.fwinttyp IN (1, 2)  -- MOTORWAY
          AND org_ig.elemtyp = 4120 -- Road
          AND org_is.name is not null;
        """
        temp_file_obj = cache_file.open('hwy_exit_name')

        names = self.get_batch_data(sqlcmd)
        for name_info in names:
            node_id = name_info[0]
            fwin_type = name_info[1]
            name = name_info[2]
            lang_code = name_info[3]
            name_type = NAME_TYPE_OFFICIAL
            is_exit_name, is_junction_name = self._get_name_type(fwin_type)
            if not name:
                self.log.warning('No name and number. id=%d' % node_id)
                continue
            else:
                multi_name = MultiLangNameTa(node_id,
                                             lang_code,
                                             name,
                                             name_type)

                json_name = multi_name.json_format_dump()
                if not json_name:
                    self.log.error('Json Name is none. node_id=%d' % node_id)
            self._store_name_to_temp_file(temp_file_obj, node_id,
                                          is_exit_name, is_junction_name,
                                          json_name)
        temp_file_obj.seek(0)
        self.pg.copy_from2(temp_file_obj, 'mid_temp_hwy_exit_name_ta')
        self.pg.commit2()
        cache_file.close(temp_file_obj, True)
        self.log.info('End Make exit Name.')
        return 0

    def _store_name_to_temp_file(self, file_obj, link_id,
                                 is_exit_name, is_junction_name,
                                 json_name):
        if file_obj:
            file_obj.write('%d\t%s\t%s\t%s\n' %
                           (link_id, is_exit_name,
                            is_junction_name, json_name)
                           )
        return 0

    def _get_name_type(self, fwin_type):
        exit_type, junction_type = 'N', 'N'
        if fwin_type == IS_Motorway_Exit:
            exit_type = 'Y'
        elif fwin_type == IS_Motorway_Interchange:
            junction_type = 'Y'
        else:
            return None, None
        return exit_type, junction_type

    def _make_link_facil(self):
        self.log.info('make mid_node_facil')
        self.CreateTable2('mid_node_facil')
        sqlcmd = '''
        INSERT INTO mid_node_facil(node_id, facil_id)
        (
        SELECT distinct node_id, org_is.id AS is_id
          FROM org_is
          LEFT JOIN org_ig
          ON org_is.id = org_ig.id
          INNER JOIN node_tbl
          ON org_ig.elemid = node_id
          WHERE org_is.inttyp = 1 -- FREE WAY
             AND org_is.fwinttyp IN (1, 2)  -- MOTORWAY
             AND org_ig.elemtyp = 4120 -- Junction
        );
        '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        self.log.info('End make mid_node_facil.')
        return 0
