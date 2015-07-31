# -*- coding: cp936 -*-
'''
Created on 2011-10-12
转换四维数据用到的一些公用函数
@author: hongchenzai
'''
import log
from xlrd import open_workbook
import types
import mail
import struct

class Stratify(object):
    ''' 处理文字分导层的类
    '''

    def Open(self, path='.\docs\背景名称注记Code分层表_iPC日本.xls'):
        self.book = open_workbook(path)

    def GetStratify(self, kind, arrt):
        """取得分层信息
           Kind: 要转换的种别  1: link,          2: link名字，            3: 背景，          4： 背景字，             5：  POI文字,  6:背景线        7:背景面      8: 水系
           arrt: 相关属性            1: link等级和属性    2: link等级和属性  3: 背景属性, 4:  背景字代码      5:  POI代码       6:背景种别   7:背景种别 8: 水系种别
           return(返回值):
           code:  种别
           nametype: 类别  0:RESERVED；1: 重心文字；2: 线状名称；3：记号+文字列
           prior: 优先级
           low_level: 数值越大,level等级越低; 19---0
           high_level:同上
        """
        module_name = ''
        if kind == 2:  # 求link名字分层
            module_name = 'Link Name'
        elif kind == 5:  # POI文字
            module_name = 'POI Name'
        elif kind == 6:  # 背景线
            module_name = 'Line'
        elif kind == 7:  # 背景面
            module_name = 'Polygon'
        elif kind == 8:  # 水系
            module_name = 'water'
        else:
            log('Stratify', "error kind" + repr(kind), 'warning')
            return None, None, None, None, None

        i = 0
        sheet = self.book.sheet_by_index(0)
        ni_code_col = ord('O') - ord('A')
        for ni_code in sheet.col(ni_code_col):
            if ni_code.value.lower().find(arrt.lower()) != -1 :
                rdb_code_col = ord('D') - ord('A')  # 种别Code列号
                rdb_prio_col = ord('E') - ord('A')  # 名称表示優先度列号
                rdb_type_col = ord('F') - ord('A')  # 文字列Type列号
                rdb_low_col = ord('V') - ord('A')  # 最低层列号
                rdb_high_col = ord('W') - ord('A')  # 最高层列号
                rdb_code, name_prio, name_type, low_col, high_col = None, None, None, None, None
                if sheet.cell_value(i, rdb_code_col) != '':
                    rdb_code = int(sheet.cell_value(i, rdb_code_col)[0:6], 16)  # 把十六进制的字串转为十进制数字
                else:
                    log(module_name, "can't find rdb code for NI_Code " + arrt, 'warning')
                if sheet.cell_value(i, rdb_prio_col) != '':
                    name_prio = sheet.cell_value(i, rdb_prio_col)
                else :
                    if module_name != 'Line' and module_name != 'Polygon' and module_name != 'water':
                        log(module_name, "can't find Priority code for NI_Code: " + arrt, 'warning')
                if sheet.cell_value(i, rdb_type_col) != '':
                    name_type = sheet.cell_value(i, rdb_type_col)
                else :
                    if module_name != 'Line' and module_name != 'Polygon' and module_name != 'water':
                        log(module_name, " can't find type code for NI_Code: " + arrt, 'warning')
                if sheet.cell_value(i, rdb_low_col) != '':
                    low_col = sheet.cell_value(i, rdb_low_col)
                else:
                    if module_name != 'Line' and module_name != 'Polygon' and module_name != 'water':
                        log(module_name, "can't find low level code for NI_Code: " + arrt, 'warning')
                if sheet.cell_value(i, rdb_high_col) != '':
                    high_col = sheet.cell_value(i, rdb_high_col)
                else:
                    if module_name != 'Line' and module_name != 'Polygon' and module_name != 'water':
                        log(module_name, "can't find high level code for NI_Code: " + arrt, 'warning')

                return rdb_code, name_type, name_prio, low_col, high_col
            i += 1

        log(module_name, "can't find the information for NI_Code: " + arrt, 'warning')

        return None, None, None, None, None

    def GetRoadDisplayClass(self, road_kind):
        module_name = 'Road'
        i = 0
        sheet = self.book.sheet_by_index(0)
        ni_code_col = ord('O') - ord('A')
        for ni_code in sheet.col(ni_code_col):
            if ni_code.value.lower().find(road_kind.lower()) != -1 :
                rdb_road_dc_col = ord('X') - ord('A')  # Road Display Class
                rdb_dc = None
                if sheet.cell_value(i, rdb_road_dc_col) != '':
                    rdb_dc = int(sheet.cell_value(i, rdb_road_dc_col))  # 把十六进制的字串转为十进制数字
                else:
                    log(module_name, "can't find rdb code for NI_Code: " + road_kind, 'warning')

                return rdb_dc
            i += 1

        return None

    def GetAllStratify(self):

        '''将分层信息导入数据库表'''
        kind_code_list = []
        sheet = self.book.sheet_by_index(0)

        kind_code_col = ord('D') - ord('A')
        kiwi_name_col = ord('G') - ord('A')  # Kiwi种别名称
        emg_code_col = ord('H') - ord('A')  # EMG-Code
        ni_code_col = ord('O') - ord('A')  # NI-Code

        rdb_code_col = ord('D') - ord('A')  # 种别Code列号
        rdb_prio_col = ord('E') - ord('A')  # 名称表示優先度列号
        rdb_type_col = ord('F') - ord('A')  # 文字列Type列号
        rdb_low_col = ord('V') - ord('A')  # 最低层列号
        rdb_high_col = ord('W') - ord('A')  # 最高层列号
        disp_class_col = ord('X') - ord('A')  # Display Class
        emg_codes_num = 0
        ni_codes_num = 0
        emg_code_list = []
        ni_code_list = []

        i = 6
        while i < sheet.nrows:
            kind_code = sheet.cell_value(i, kind_code_col)
            if kind_code == None or kind_code == '':
                i += 1
                continue
            else:
                kind_code = int(kind_code, 16)

            if sheet.cell_value(i, emg_code_col) != '':
                emg_code_list = str(sheet.cell_value(i, emg_code_col)).split('\n')
                emg_codes_num = len(emg_code_list)
            else:
                emg_code_list = []
                emg_codes_num = 0

            if sheet.cell_value(i, ni_code_col) != '':
                cell_value = sheet.cell_value(i, ni_code_col)
                if type(cell_value) in [types.StringType, types.UnicodeType]:
                    ni_code_list = cell_value.split('\n')
                else:
                    ni_code_list = [str(int(cell_value))]
                ni_codes_num = len(ni_code_list)
            else:
                ni_code_list = []
                ni_codes_num = 0

            if sheet.cell_value(i, rdb_prio_col) != '':
                priority = int(sheet.cell_value(i, rdb_prio_col))
            else:
                priority = None
            if sheet.cell_value(i, rdb_type_col) != '':
                name_type = int(sheet.cell_value(i, rdb_type_col))
            else:
                name_type = None

            kiwi_name = sheet.cell_value(i, kiwi_name_col)
            emg_name = sheet.cell_value(i, emg_code_col + 1)
            ni_name = sheet.cell_value(i, ni_code_col + 1)

            if sheet.cell_value(i, rdb_low_col) != '':
                low_level = int(sheet.cell_value(i, rdb_low_col))
            else :
                low_level = None

            if sheet.cell_value(i, rdb_high_col) != '':
                high_level = int(sheet.cell_value(i, rdb_high_col))
            else:
                high_level = None

            if sheet.cell_value(i, disp_class_col) != '':
                disp_class = int(sheet.cell_value(i, disp_class_col))
            else:
                disp_class = None

            if emg_codes_num == 0 and  ni_codes_num == 0:
                kind_code_list.append((kind_code, priority, name_type, kiwi_name, None, emg_name, None, ni_name, low_level, high_level, disp_class))
                # print kind_code, priority, name_type, kiwi_name, None, emg_name, None, ni_name, low_level, high_level
                i += 1
                continue

            if emg_codes_num >= ni_codes_num:
                j = 0
                while j < ni_codes_num:
                    dot_idx = emg_code_list[j].find('.')
                    kind_code_list.append((kind_code, priority, name_type, kiwi_name, emg_code_list[j][0:dot_idx], emg_name,
                                           ni_code_list[j], ni_name, low_level, high_level, disp_class))
                    # print kind_code, priority, name_type, kiwi_name, emg_code_list[j][0:dot_idx], emg_name, ni_code_list[j][2:], ni_name, low_level, high_level, disp_class
                    j += 1
                while j < emg_codes_num:
                    dot_idx = emg_code_list[j].find('.')
                    kind_code_list.append((kind_code, priority, name_type, kiwi_name, emg_code_list[j][0:dot_idx], emg_name,
                                           None, ni_name, low_level, high_level, disp_class))
                    # print kind_code, priority, name_type, kiwi_name, emg_code_list[j][0:dot_idx], emg_name, None, ni_name, low_level, high_level, disp_class
                    j += 1
            else:
                j = 0
                while j < emg_codes_num:
                    dot_idx = emg_code_list[j].find('.')
                    kind_code_list.append((kind_code, priority, name_type, kiwi_name, emg_code_list[j][0:dot_idx], emg_name,
                                           ni_code_list[j], ni_name, low_level, high_level, disp_class))
                    # print kind_code, priority, name_type, kiwi_name, emg_code_list[j][0:dot_idx], emg_name, ni_code_list[j][2:], ni_name, low_level, high_level, disp_class
                    j += 1

                while j < ni_codes_num:
                    kind_code_list.append((kind_code, priority, name_type, kiwi_name, None, emg_name,
                                           ni_code_list[j], ni_name, low_level, high_level, disp_class))
                    # print kind_code, priority, name_type, kiwi_name, None, emg_name, ni_code_list[j][2:], ni_name, low_level, high_level, disp_class
                    j += 1
            i += 1
        return kind_code_list

def GetPath(name):
    path = ''
    file_object = open('DataBasePath.txt', 'r')

    try:
        lines = file_object.readlines()
        for line in lines:
            if line.find(name + '=') == 0:
                path = line[len(name + '='):].strip()
                break
    except:
        file_object.close()
        log('common_func', "Doesn't exist file DataBasePath.txt")
        exit(1)
    finally:
        file_object.close()

    return path

def GetProjName():
    return GetPath('proj_name')

def readlines(path):
    lines = []
    file_object = open(path, 'r')

    try:
        lines = file_object.readlines()
    except:
        file_object.close()
        log('common_func', "Doesn't exist file DataBasePath.txt")
        exit(1)
    finally:
        file_object.close()

    return lines

def CountPassLink(PassLid, PassLid2=None):
    count = 0;
    if PassLid != None and PassLid != '':
        count = PassLid.count('|') + 1
    if PassLid2 != None and PassLid2 != '':
        count += PassLid2.count('|') + 1
    return count

def proj_exit(param):
    # rdb_mail.rdb_mail().send_mail()
    exit(param)

def GetAllLanguages(path):
    book = open_workbook(path)
    Lang_list = []
    sheet = book.sheet_by_index(0)
    language_id_col = ord('A') - ord('A')  #
    l_full_name_col = ord('B') - ord('A')  # 语言名称
    l_talbe_col = ord('C') - ord('A')  # 语言表名
    pronunciation_col = ord('D') - ord('A')  # 读音的全称
    p_talbe_col = ord('E') - ord('A')  # 读音表名
    code_col = ord('F') - ord('A')  # 缩写
    lang_code_client = ord('G') - ord('A')
    lang_id_client = ord('H') - ord('A')
    i = 3
    while i < sheet.nrows:
        language_id = sheet.cell_value(i, language_id_col)
        if language_id == None:
            break

        Lang_list.append((language_id
                          , sheet.cell_value(i, l_full_name_col)
                          , sheet.cell_value(i, l_talbe_col)
                          , sheet.cell_value(i, pronunciation_col)
                          , sheet.cell_value(i, p_talbe_col)
                          , sheet.cell_value(i, code_col)
                          , sheet.cell_value(i, lang_code_client)
                          , sheet.cell_value(i, lang_id_client)
                         ))
        i += 1

    return Lang_list

def get_int_value(file_obj, read_bytes_cnt):
    content_string = file_obj.read(read_bytes_cnt)
    conver_content_string = content_string[::-1]
    rtn_value, = struct.unpack("i", conver_content_string)

    return rtn_value


def GetSheetRegion(path, from_col='A', to_col='A', from_row=0, to_row=0):
    import copy
    book = open_workbook(path)
    rstlist = []
    rowlist = []
    sheet = book.sheet_by_index(0)
    i = from_row - 1
    while i < to_row:
        rowlist = []
        j = ord(from_col) - ord('A')
        nID = sheet.cell_value(i, j)
        if nID == None:
            break
        # Get Values For Row
        while j <= (ord(to_col) - ord('A')):
            rowlist.append(sheet.cell_value(i, j))
            j += 1
        if len(rowlist) > 0:
            rstlist.append(copy.deepcopy(rowlist))
        i += 1
    return rstlist
