# -*- coding: UTF8 -*-
'''
Created on 2013-12-25

@author: minghongtu
'''
import platform.TestCase
import re

# key的值
class CCheckRoadNameKey(platform.TestCase.CTestCase):

    def _do(self):

        sqlcmd = """
                 select distinct road_name from link_tbl where road_name is not null

                 """
        self.pg.RecreateCursor('my_cursor')
        self.pg.execute(sqlcmd)
        key = ["lang", "left_right", "tts_type", "type", "val"]

        rows = self.pg.fetchmany(200000)
        while len(rows) != 0:
            for row in rows:
                road_name_list = eval(row[0])
                for road_name in road_name_list:
                    for road_name_dict in road_name:
                        k = road_name_dict.keys()
                        k.sort()
                        if k != key:
                            return False
            rows = self.pg.fetchmany(200000)

        return True


# #key的顺序
# class CCheckRoadNameKeyOrder(platform.TestCase.CTestCase):
#
#    def _do(self):
#
#        sqlcmd = """
#                 select distinct road_name from link_tbl where road_name is not null
#
#                 """
#
#        self.pg.RecreateCursor('my_cursor')
#        self.pg.execute(sqlcmd)
#        key_order=["lang", "left_right", "tts_type", "type", "val"]
#        rows = self.pg.fetchmany(200000)
#        while len(rows) != 0:
#            for row in rows:
#                row = row[0]
#                sign=['[',']','"',' ']
#                for s in sign:
#                    row = row.replace(s,'')
#                row = row.split('},')
#                for road_name_dict_list in row:
#                    road_name_dict_list=road_name_dict_list.replace('{','')
#                    road_name_dict_list=road_name_dict_list.replace('}','')
#                    road_name_dict_list=road_name_dict_list.split(',',4)
#                    lis=[]
#                    for road_name_dict in road_name_dict_list:
#                        road_name_dict=road_name_dict.split(':')
#                        lis.append(road_name_dict[0])
#                    if lis != key_order:
#                        return False
#            rows = self.pg.fetchmany(200000)
#
#        return True

# key的顺序
class CCheckRoadNameKeyOrder(platform.TestCase.CTestCase):

    def _do(self):

        sqlcmd = """
                 select distinct road_name from link_tbl where road_name is not null

                 """
        self.pg.RecreateCursor('my_cursor')
        self.pg.execute(sqlcmd)
        key_order = 'lang:.*left_right:.*tts_type:.*type:.*val:.*'
        rows = self.pg.fetchmany(200000)
        while len(rows) != 0:
            for row in rows:
                row = row[0]
                sign = ['[', ']', '"', ' ']
                for s in sign:
                    row = row.replace(s, '')
                row = row.split('},')
                for road_name_dict in row:
                    m = re.search(key_order, road_name_dict)
                    if m is None:
                        return False
            rows = self.pg.fetchmany(200000)

        return True

# type的有效值
class CCheckRoadNameType(platform.TestCase.CTestCase):

    def _do(self):

        sqlcmd = """
                 select distinct road_name from link_tbl where road_name is not null

                 """

        self.pg.RecreateCursor('my_cursor')
        self.pg.execute(sqlcmd)
        type = ["office_name", "route_num", "alter_name", "bridge_name"]
        rows = self.pg.fetchmany(200000)
        while len(rows) != 0:
            for row in rows:
                road_name_list = eval(row[0])
                for road_name in road_name_list:
                    for road_name_dict in road_name:
                        if road_name_dict['type'] not in type:
                            return False
            rows = self.pg.fetchmany(200000)

        return True

# tts_type有效值
class CCheckRoadNameTTSType(platform.TestCase.CTestCase):

    def _do(self):

        sqlcmd = """
                 select distinct road_name from link_tbl where road_name is not null

                 """

        self.pg.RecreateCursor('my_cursor')
        self.pg.execute(sqlcmd)
        tts_type = ['not_tts', 'text']
        rows = self.pg.fetchmany(200000)
        while len(rows) != 0:
            for row in rows:
                road_name_list = eval(row[0])
                for road_name in road_name_list:
                    for road_name_dict in road_name:
                        if road_name_dict['tts_type'] not in tts_type:
                            return False
            rows = self.pg.fetchmany(200000)

        return True

# left_right有效值
class CCheckRoadNameLeftRight(platform.TestCase.CTestCase):

    def _do(self):

        sqlcmd = """
                 select distinct road_name from link_tbl where road_name is not null

                 """

        self.pg.RecreateCursor('my_cursor')
        self.pg.execute(sqlcmd)
        left_right = ['left_right']
        rows = self.pg.fetchmany(200000)
        while len(rows) != 0:
            for row in rows:
                road_name_list = eval(row[0])
                for road_name in road_name_list:
                    for road_name_dict in road_name:
                        if road_name_dict['left_right'] not in left_right:
                            return False
            rows = self.pg.fetchmany(200000)

        return True

# lang有效值
class CCheckRoadNameLanguage(platform.TestCase.CTestCase):

    def _do(self):

        sqlcmd = """
                 select distinct road_name from link_tbl where road_name is not null

                 """

        self.pg.RecreateCursor('my_cursor')
        self.pg.execute(sqlcmd)
        language = ['THA', 'THE']
        rows = self.pg.fetchmany(200000)
        while len(rows) != 0:
            for row in rows:
                road_name_list = eval(row[0])
                for road_name in road_name_list:
                    for road_name_dict in road_name:
                        if road_name_dict['lang'] not in language:
                            return False
            rows = self.pg.fetchmany(200000)

        return True

# 一个名称的不同语言之间的顺序
class CCheckRoadNameLangOrder(platform.TestCase.CTestCase):

    def _do(self):

        sqlcmd = """
                 select distinct road_name from link_tbl where road_name is not null

                 """

        self.pg.execute(sqlcmd)
        order_dict = {'THAnot_tts':1, 'THAtext':2, 'THEnot_tts':3}
        rows = self.pg.fetchmany(200000)
        while len(rows) != 0:
            for row in rows:
                road_name_list = eval(row[0])
                for road_name in road_name_list:
                    order = []
                    for road_name_dict in road_name:
                        order.append(order_dict[road_name_dict['lang'] + road_name_dict['tts_type']])
                    if len(order) == 1 :
                        pass
                    else:
                        count = 0
                        while count < len(order) - 1:
                            if order[count] > order[count + 1] :
                                return False
                            count += 1
            rows = self.pg.fetchmany(200000)
        return True


# 多个名称之间的顺序
class CCheckRoadNameOrder(platform.TestCase.CTestCase):

    def _do(self):

        sqlcmd = """
                 select distinct road_name from link_tbl where road_name is not null

                 """
        self.pg.RecreateCursor('my_cursor')
        self.pg.execute(sqlcmd)
        rows = self.pg.fetchmany(200000)
        order_dict = {'office_name':1, 'route_num':2, 'alter_name':3, 'bridge_name':4}
        while len(rows) != 0:
            for row in rows:
                road_name_list = eval(row[0])
                order = []
                for road_name in road_name_list:
                    order.append(order_dict[road_name[0]['type']])

                if len(order) == 1 :
                    pass
                else:
                    count = 0
                    while count < len(order) - 1:
                        if order[count] > order[count + 1] :
                            return False
                        count += 1
            rows = self.pg.fetchmany(200000)
        return True

# val的值不能包含括号
class CCheckRoadNameValue(platform.TestCase.CTestCase):

    def _do(self):

        sqlcmd = """
                 select distinct road_name  from link_tbl where road_name is not null

                 """
        self.pg.RecreateCursor('my_cursor')
        self.pg.execute(sqlcmd)
        rows = self.pg.fetchmany(200000)
        while len(rows) != 0:
            for row in rows:
                road_name_list = eval(row[0])
                for road_name in road_name_list:
                    for road_name_dict in road_name:
                        m = re.search(r'\) | \( | \（  | \）', road_name_dict['val'])
                        if m is not None:
                            return False
            rows = self.pg.fetchmany(200000)

        return True

# 有桥名是别名不能含有‘bridge’
class CCheckRoadNameBridgeAlter(platform.TestCase.CTestCase):

    def _do(self):

        sqlcmd = """
                 select distinct road_name from link_tbl where road_name is not null

                 """
        self.pg.RecreateCursor('my_cursor')
        self.pg.execute(sqlcmd)
        rows = self.pg.fetchmany(200000)
        while len(rows) != 0:
            for row in rows:
                road_name_list = eval(row[0])
                flag = False
                for road_name in road_name_list:
                    if road_name[0]['type'] == 'bridge_name' and flag == False:
                        flag = True
                        for j in road_name_list:
                            if j[0]['type'] == 'alter_name':
                                for road_name_dict in j:
                                    if road_name_dict['lang'] == 'THE':
                                        m = re.search('bridge', road_name_dict['val'].lower())
                                        if m is not None:
                                            return False
            rows = self.pg.fetchmany(200000)

        return True

# 有泰文桥名是必须有tts
class CCheckRoadNameBridgeTTS(platform.TestCase.CTestCase):

    def _do(self):

        sqlcmd = """
                 select distinct road_name  from link_tbl where road_name is not null

                 """
        self.pg.RecreateCursor('my_cursor')
        self.pg.RecreateCursor('my_cursor')
        self.pg.execute(sqlcmd)
        rows = self.pg.fetchmany(200000)
        while len(rows) != 0:
            for row in rows:
                road_name_list = eval(row[0])
                for road_name in road_name_list:
                    if road_name[0]['type'] == 'bridge_name' and road_name[0]['lang'] == 'THA':
                        tts_type = []
                        for road_name_dict in road_name:
                            tts_type.append(road_name_dict['tts_type'])
                        if 'text' not in tts_type and 'phoneme_text' not in tts_type and 'phoneme' not in tts_type:
                            return False
            rows = self.pg.fetchmany(200000)

        return True


# 没有桥名是官方名必须有tts
class CCheckRoadNameBridgeOffice(platform.TestCase.CTestCase):
    def _do(self):

        sqlcmd = """
                 select distinct road_name  from link_tbl where road_name is not null

                 """
        self.pg.RecreateCursor('my_cursor')
        self.pg.execute(sqlcmd)
        rows = self.pg.fetchmany(200000)
        while len(rows) != 0:
            for row in rows:
                road_name_list = eval(row[0])
                flag = False
                for road_name in road_name_list:
                    if road_name[0]['type'] == 'bridge_name':
                        flag = True
                        row = self.pg.fetchone()
                if flag == True:
                    continue
                tts_type = []
                for road_name in road_name_list:
                    if road_name[0]['type'] == 'office_name':
                        for road_name_dict in road_name:
                            tts_type.append(road_name_dict['tts_type'])
                        if 'text' not in tts_type and 'phoneme_text' not in tts_type and 'phoneme' not in tts_type:
                            return False
            rows = self.pg.fetchmany(200000)

            return True


#===============================================================================
# CCheckNameText
#===============================================================================
class CCheckNameText(platform.TestCase.CTestCase):
    '''结构能被JSon.loads解析; 英文名称不能含有非常规字符; 泰文名称不能只含有常规字作字符.
    '''

    def _do(self):
        sqlcmd = """
        SELECT link_id, road_name
          FROM link_tbl
          where road_name is not null;
        """
        import json
        datas = self.pg.get_batch_data(sqlcmd)
        result = True
        for data in datas:
            link_id = data[0]
            road_name = data[1]
            multi_name_info = json.loads(road_name)
            for one_name_info in multi_name_info:
                for name_dict in one_name_info:
                    lang_code = name_dict.get('lang')
                    name_type = name_dict.get('type')
                    name = name_dict.get('val')
                    if name_type == 'route_num':
                        # 判断非数据
                        pattern = r'[^\d]+'
                        if not self.has_other_character(name, pattern):
                            self.logger.error('Exist THA character. linkid=%s,name=%s'
                                              % (link_id, name))
                    elif lang_code == 'THE':
                        # 英文名称不能含有非常规字符。如:泰文
                        pattern = r"[^\w\s\.,-/&();=\+']+?"
                        if not self.has_other_character(name, pattern):
                            # 名称里有泰文
                            self.logger.error('Exist THA character. linkid=%s,name=%s'
                                              % (link_id, name))
                            result = False
                    elif lang_code == 'THA':
                        # 泰文名称不能只含有常规字作字符。如: 全是数字和英文
                        pattern = r'[^ -~]+?'  # 空格到~
                        if self.has_other_character(name, pattern):
                            # 名称里没有泰文
                            self.logger.error('No THA character. linkid=%s,name=%s'
                                              % (link_id, name))
                            result = False
                    else:
                        self.logger.error('Error Language code. linkid=%s'
                                          % link_id)
                        result = False
        return result

    def has_other_character(self, s, pattern=r'[^\w\s\.,-/&]+?'):
        '''非(字符、数字、空白、句号、逗号)等'''
        p = re.compile(pattern)
        find_list = p.findall(s)
        if find_list:
            return False
        return True
