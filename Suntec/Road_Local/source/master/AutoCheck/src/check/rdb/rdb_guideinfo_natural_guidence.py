# -*- coding: UTF8 -*-

import platform.TestCase
import json
from check.rdb import rdb_common_check

class CCheckInlinkidValidate(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from rdb_guideinfo_natural_guidence as a
                left join rdb_link as b
                on a.in_link_id = b.link_id
                where b.link_id is null
                """
        nRecCount = self.pg.getOnlyQueryResult(sqlcmd)
        return (nRecCount == 0)
    
class CCheckNodeidValidate(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from rdb_guideinfo_natural_guidence as a
                left join rdb_node as b
                on a.node_id = b.node_id
                where b.node_id is null
                """
        nRecCount = self.pg.getOnlyQueryResult(sqlcmd)
        return (nRecCount == 0)

class CCheckOutlinkidValidate(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from rdb_guideinfo_natural_guidence as a
                left join rdb_link as b
                on a.out_link_id = b.link_id
                where b.link_id is null
                """
        nRecCount = self.pg.getOnlyQueryResult(sqlcmd)
        return (nRecCount == 0)
        
class CCheckFeatPositionValidate(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from rdb_guideinfo_natural_guidence
                where feat_position not in (0,1,2,3)
                """
        nRecCount = self.pg.getOnlyQueryResult(sqlcmd)
        return (nRecCount == 0)
        
class CCheckFeatImportanceValidate(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from rdb_guideinfo_natural_guidence
                where not (feat_importance >= 0 and feat_importance <= 10)
                """
        nRecCount = self.pg.getOnlyQueryResult(sqlcmd)
        return (nRecCount == 0)

class CCheckInlinkTileSame(platform.TestCase.CTestCase):
    def _do(self):
        checkobject = rdb_common_check.CCheckItemTileIDSame(self.pg, 'rdb_guideinfo_natural_guidence', 'in_link_id', 'in_link_id_t')
        return checkobject.do()
        
class CCheckNodeTileSame(platform.TestCase.CTestCase):
    
    def _do(self):
        checkobject = rdb_common_check.CCheckItemTileIDSame(self.pg, 'rdb_guideinfo_natural_guidence', 'node_id', 'node_id_t')
        return checkobject.do()
    
class CCheckOutlinkTileSame(platform.TestCase.CTestCase):
    def _do(self):
        checkobject = rdb_common_check.CCheckItemTileIDSame(self.pg, 'rdb_guideinfo_natural_guidence', 'out_link_id', 'out_link_id_t')
        return checkobject.do()

class CCheckInLinkOutLinkEqual(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*) from rdb_guideinfo_natural_guidence where in_link_id = out_link_id
        """
        return 0 == self.pg.getOnlyQueryResult(sqlcmd)

class CCheckExtendFlag(platform.TestCase.CTestCase):
    def _do(self):
        checkobject = rdb_common_check.CCheckNodeExtendFlag(self.pg, 'rdb_guideinfo_natural_guidence', 17)
        return checkobject.do()

class CCheckFeatName(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from rdb_guideinfo_natural_guidence
                where feat_name is null or feat_name not like '%"tts_type": "phoneme"%'
                """
        nRecCount = self.pg.getOnlyQueryResult(sqlcmd)
        return (nRecCount == 0)

class CCheckFeatName2(platform.TestCase.CTestCase):
    'use mmi data to get rdb data'
    def _do(self):
        sqlcmd = """
                select count(*)
                from rdb_guideinfo_natural_guidence
                where feat_name is null
                """
        nRecCount = self.pg.getOnlyQueryResult(sqlcmd)
        return (nRecCount == 0)

class CCheckPreposition(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from rdb_guideinfo_natural_guidence
                where not (preposition_code >= 0 and preposition_code <= 255)
                """
        nRecCount = self.pg.getOnlyQueryResult(sqlcmd)
        return (nRecCount == 0)

class CCheckGuideinfoCondition(platform.TestCase.CTestCase):
    def _do(self):
        try:
            self.pg.CreateFunction_ByName('rdb_check_guideinfo_condition')
            self.pg.callproc('rdb_check_guideinfo_condition')
            return True
        except:
            self.logger.exception('fail to check...')
            return False

class CCheckFeatName_type(platform.TestCase.CTestCase):
    def _do(self):
        #type取值范围{'office_name','route_num','alter_name','bridge_name'}。
        sqlcmd = """
            select gid, feat_name
            from rdb_guideinfo_natural_guidence
            where feat_name is not null;                 
            """
        datas = self.pg.get_batch_data(sqlcmd) 

        for data in datas:
            gid = data[0]
            feat_name = data[1]
            
            find_flag = 0
            multi_name_info = json.loads(feat_name)
            for one_name_info in multi_name_info:
                for name_dict in one_name_info:
                    name_type = name_dict.get('type')
                    tts_type = name_dict.get('tts_type')                    
                    left_right = name_dict.get('left_right')

                    if name_type == 'office_name' or name_type == 'route_num'\
                        or name_type == 'alter_name' or name_type == 'bridge_name':
                        find_flag = 1
                    if tts_type == 'not_tts' or tts_type == 'text'\
                        or tts_type == 'phoneme' or tts_type == 'phoneme_text':
                        find_flag = 1
                    if left_right == 'left_right' or left_right == 'left_name'\
                        or left_right == 'right_name':
                        find_flag = 1
                                                
            if find_flag == 0:
                self.logger.info("RoadName: Wrong type/tts_type/left_right (id=%d)" % (gid))
                return 0
        
        return 1 
           
class CCheckFeatName_TTS(platform.TestCase.CTestCase):
    def _do(self):
        #tts必须与非tts同时存在.
        sqlcmd = """
            select gid, feat_name
            from rdb_guideinfo_natural_guidence
            where feat_name is not null;                 
            """
        datas = self.pg.get_batch_data(sqlcmd) 

        for data in datas:
            gid = data[0]
            feat_name = data[1]
            tts_type_array = []
            multi_name_info = json.loads(feat_name)
            for one_name_info in multi_name_info:
                for name_dict in one_name_info:
                    tts_type = name_dict.get('tts_type')
                    tts_type_array.append(tts_type)
                    
                try:
                    if tts_type_array.index(u'phoneme') >= 0 or tts_type_array.index(u'phoneme_text') >= 0 \
                        or tts_type_array.index(u'text') >= 0:
                        # 'phoneme' or 'phoneme_text' was found.
                        try :
                            if tts_type_array.index(u'not_tts') >= 0:
                                pass
                        except:
                            # 'not_tts' not found.
                            self.logger.error("RoadName: Wrong TTS (only TTS, no name, id=%d)" % (gid))
                            return 0
                except:
                    return 1
            
        return 1 
    






