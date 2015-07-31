# -*- coding: UTF8 -*-
'''
Created on 2012-3-29

@author: hongchenzai
'''
import component
from common import cache_file
from component.nostra.mult_lang_name_nostra import MultLangName


class comp_dictionary_nostra(component.dictionary.comp_dictionary):
    '''泰国名称
    '''

    def __init__(self):
        '''
        Constructor
        '''
        component.dictionary.comp_dictionary.__init__(self)
        self.mult_obj = MultLangName()  # 多语言

    def _DoCreateTable(self):
        # 名称字典表
        self.CreateTable2('language_tbl')
        # link和名称的临时关联表
        self.CreateTable2('temp_link_name')
        self.CreateTable2('temp_link_shield')
        self.CreateTable2('temp_link_name_delete')
        return 0

    def _Do(self):
        # 创建语言种别的中间表
        self._InsertLanguages()
        # 道路名称字典
        self._make_link_name()
        # 道路shield和番号
        self._make_link_shield()
        return 0

    def _make_link_shield(self):
        self.log.info('Start Make Link Shield Info')
        sqlcmd = """
        SELECT routeid, nav_rdnum, c_access, intrdlnnum
          FROM org_l_tran
          where nav_rdnum is not null or intrdlnnum is not null;
        """
        temp_number_file_obj = cache_file.open('link_shield')  # 创建临时文件
        numbers = self.get_batch_data(sqlcmd)
        for number_info in numbers:
            link_id = number_info[0]
            link_num = number_info[1]
            c_access = number_info[2]
            intrdlnnum = number_info[3]
            num_list = []
            nums = self._merge_route_num(link_num, intrdlnnum)
            for routenum in nums:
                shield_id = self._convert_shield_id(c_access, routenum)
                if not shield_id:
                    self.log.error('No Shield Id. linkId=%s' % link_id)
                    continue
                str_number = shield_id + '\t' + routenum
                number = self.mult_obj.convert_names_to_list(str_number,
                                                             str_number,
                                                             '',
                                                             'shield'
                                                             )
                if number:
                    num_list += number
            if num_list:
                # shield的泰文和英文一样
                json_shield = self.mult_obj.json_format_dump2(num_list)
                # 存到本地的临时文件
                self._store_name_to_temp_file(temp_number_file_obj,
                                              link_id,
                                              json_shield
                                              )
            else:
                self.log.error('No Shield Info. linkID=%s' % link_id)
        # ## 导入数据库
        temp_number_file_obj.seek(0)
        self.pg.copy_from2(temp_number_file_obj, 'temp_link_shield')
        self.pg.commit2()
        # close file
        #temp_number_file_obj.close()
        cache_file.close(temp_number_file_obj,True)
        self.log.info('End Make Link Shield Info')
        return 0

    def _merge_route_num(self, linknum, intrdlnnum):
        '合并一般番号和亚洲番号, 并且一亚洲番号放在前面。'
        nums = []
        if intrdlnnum:
            nums = intrdlnnum.split(',')  # 多个番号','分开
        if linknum and linknum != '|':
            nums.append(linknum)
        return nums

    def _make_link_name(self):
        '''道路名称'''
        self.log.info('Start Make Link Name.')
        sqlcmd = """
        SELECT routeid::bigint, brdnamt, brdname, nav_namt, nav_name,
               tts_name, alt_namt, alt_name, nav_rdnum, intrdlnnum
          FROM org_l_tran
          where (nav_namt is not null
            or nav_name is not null
            or tts_name is not null
            or alt_namt is not null
            or alt_name is not null
            or brdnamt is not null
            or brdname is not null
            or nav_rdnum is not null
            or intrdlnnum is not null)
            ;
        """
        temp_file_obj = cache_file.open('link_name')  # 创建临时文件
        temp_file_name_del_obj = cache_file.open('link_name_delete')  # 创建临时文件
        names = self.get_batch_data(sqlcmd)

        for nameinfo in names:
            link_id = nameinfo[0]
            brd_name_tha = nameinfo[1]  # 桥名（泰文）
            brd_name_the = nameinfo[2]  # 桥名（英文）
            nav_name_tha = nameinfo[3]  # 官方名（泰文）
            nav_name_the = nameinfo[4]  # 官方名（英文）
            tts_name_tha = nameinfo[5]  # 官方名的TTS文本（泰文）
            alt_name_tha = nameinfo[6]  # 别名（泰文）
            alt_name_the = nameinfo[7]  # 别名（英文）
            nav_rdnum = nameinfo[8]  # 道路番号
            intrdlnnum = nameinfo[9]  # 亚洲道-番号
            if brd_name_tha == '' or brd_name_tha == '|':
                brd_name_tha = None
            if brd_name_the == '' or brd_name_the == '|':
                brd_name_the = None
            if nav_name_tha == '' or nav_name_tha == '|':
                nav_name_tha = None
            if nav_name_the == '' or nav_name_the == '|':
                nav_name_the = None
            if tts_name_tha == '' or tts_name_tha == '|':
                tts_name_tha = None
            if alt_name_tha == '' or alt_name_tha == '|':
                alt_name_tha = None
            if alt_name_the == '' or alt_name_the == '|':
                alt_name_the = None
            if nav_rdnum == '' or nav_rdnum == '|':
                nav_rdnum = None
            office_name = []
            link_number = []
            alter_name = []
            birdge_name = []
            all_name = []
            office_tts = ''
            brd_tts = ''
            # 桥名存在时，TTS是桥名的TTS
            if brd_name_tha:
                brd_tts = tts_name_tha
            else:
                office_tts = tts_name_tha
            # print link_id
            # ## 官方名
            if (nav_name_tha or nav_name_the):
                office_name = self._convert_office_name(nav_name_tha,
                                                        nav_name_the,
                                                        office_tts)
            # ## 别名
            # 英文和泰文至少有一个不为空
            if (alt_name_tha or alt_name_the):
                # 别名和官方名不重复
                # 别名不带字符串"Bridge" (带"Bridge")
                if not (self._is_include_alt_name(nav_name_tha, nav_name_the,
                                                  alt_name_tha, alt_name_the)
                        or
                        (alt_name_the and alt_name_the.find('Bridge') >= 0 and
                         brd_name_the)):
                    alter_name = self._convert_other_name(alt_name_tha,
                                                          alt_name_the,
                                                          '',  # TTS
                                                          'alter_name')
            # ## 桥名
            if brd_name_tha or brd_name_the:
                birdge_name = self._convert_other_name(brd_name_tha,
                                                       brd_name_the,
                                                       brd_tts,
                                                       'bridge_name')
            # 道路番号（道路名称显示用）
            nums = self._merge_route_num(nav_rdnum, intrdlnnum)
            for routenum in nums:
                if not self._is_name_include_number(nav_name_the,
                                                    nav_name_tha,
                                                    routenum
                                                    ):
                    link_number += self._convert_link_number(routenum)
            # ## 名称存入顺序：官方名、别名、桥名、番号
            if office_name:
                all_name += office_name
            if alter_name:
                all_name += alter_name
            if birdge_name:
                all_name += birdge_name
            if link_number:
                all_name += link_number
            # 名称存在
            if all_name:
                json_name = self.mult_obj.json_format_dump2(all_name)
                # 存到本地的临时文件
                self._store_name_to_temp_file(temp_file_obj,
                                              link_id,
                                              json_name)
            else:
                if nav_name_tha:
                    # 名称被删除, 如："(Overpass)".
                    temp_file_name_del_obj.write('%d\n' % (link_id,))

        # does not necessarily write the file’s data to disk
        # temp_file_obj.flush()
        # ## 把名称导入数据库
        temp_file_obj.seek(0)
        self.pg.copy_from2(temp_file_obj, 'temp_link_name')
        self.pg.commit2()
        # close file
        #temp_file_obj.close()
        cache_file.close(temp_file_obj,True)
        

        # ## 名称被删除的link记录，导入数据库
        temp_file_name_del_obj.seek(0)
        self.pg.copy_from2(temp_file_name_del_obj, 'temp_link_name_delete')
        self.pg.commit2()
        #temp_file_name_del_obj.close()
        cache_file.close(temp_file_name_del_obj,True)

        self.log.info('End Make Link Name.')
        return 0

    def _store_name_to_temp_file(self, file_obj, nid, json_name):
        if file_obj:
            file_obj.write('%d\t%s\n' % (nid, json_name))
        return 0

    def _delete_structinfo(self, name_tha, name_the, name_tts):
        """delete road structure info from name."""
        # 道路属性文本， 英文: 泰文
        struct_the = ["(overpass)",
                      "(bypass)",
                      "(entrance)",
                      "(exit)",
                      "(underpass)",
                      "(u-turn overpass)",
                      "(flyover)",
                      "(tunnel)",
                      "(u-turn tunnel)"
                  ]
        struct_tha = ["(สะพานยกระดับ)",
                      "(เลี่ยงเมือง)",
                      "(เข้าทางหลัก)",
                      "(ออกทางคู่ขนาน)",
                      "(อุโมงค์)",
                      "(สะพานกลับรถ)",
                      "(ทางยกระดับ)",
                      "(อุโมงค์)",
                      "(อุโมงค์กลับรถ)"
                      # 下面这些下现在桥名，后面一般指河流名，地名等
                      # "(บางปะกง)",  # (Bang Pakong)
                      # "(คลองฉะไกรใหญ่)",  # (Khlong Cha Kriyai)
                      # "(ราษฎร์บำรุง)",  # (Rat Bamrung)
                      # "(บ้านสันขี้เหล็ก)"  # (Ban San Khi Lek)
                      ]
        rst_tha, rst_the, rst_tts = name_tha, name_the, name_tts
        import re
        p = re.compile(r'\(.+?\)')  # 取得两个括号以及之间的字符串, ‘?’：表示非贪婪
        if name_the and len(name_the) > 0:
            struct_the_list = p.findall(name_the)
        else:
            struct_the_list = []
        if name_tha and len(name_tha) > 0:
            struct_tha_list = p.findall(name_tha)
        else:
            struct_tha_list = []
        if name_tts and len(name_tts) > 0:
            struct_tts_list = p.findall(name_tts)
        else:
            struct_tts_list = []
        match_the_num = len(struct_the_list)
        match_tha_num = len(struct_tha_list)
        match_tts_num = len(struct_tts_list)
        if match_the_num == 0 and match_tha_num == 0 and match_tts_num == 0:
            return name_tha, name_the, name_tts

        if (match_the_num != match_tha_num or
            match_the_num != match_tts_num):
            # 很多TTS文本不带包括，如 route_id = 3200002699343
            if match_the_num > match_tts_num:
                pass
            elif match_the_num < match_tha_num:
                # ## 有一条路,英文是:Highway 37"， 泰文："ทางหลวง-สามเจ็ด (เลี่ยงเมือง)"
                # self.log.debug('Match Info 0: %s, %s, %s'
                #                    % (name_the, name_tha, name_tts))
                pass
            else:
                self.log.debug('Match Info 1: %s, %s, %s'
                                 % (rst_tha, rst_the, rst_tts))
        elif (match_the_num > 1 or
              match_tha_num > 1 or
              match_tts_num > 1):  # 多个
            # 有多个的情况，如："(Overpass) Highway 101 (Bypass)"
            if (match_the_num > 1 and
                (struct_the_list[0] in ["(Entrance)", "(Overpass)", "(Exit)"]
                 or struct_the_list[1] == "(Bypass)")
                ):
                pass
            else:
                self.log.debug('Match Info 2: %s, %s, %s'
                               % (name_the, name_tha, name_tts))
            pass
        else:  # 一个
            pass
        # ## 英文
        for struct_str in struct_the_list:
            if struct_str.lower() in struct_the:
                rst_the = self._delete_str(rst_the, struct_str)
            else:
                self.log.error('Error StructInfo(THE): struct_info=%s'
                                 % (struct_str))
        # ## 泰文
        for struct_str in struct_tha_list:
            if struct_str in struct_tha:
                rst_tha = self._delete_str(rst_tha, struct_str)
            else:
                self.log.error('Error StructInfo(THA): struct_info=%s'
                                 % (struct_str))
        # ## TTS
        for struct_str in struct_tts_list:
            if struct_str in struct_tha:
                rst_tts = self._delete_str(rst_tts, struct_str)
            else:
                self.log.warning('Error StructInfo (TTS): struct_info=%s'
                                 % (struct_str))
        return rst_tha, rst_the, rst_tts

    def _delete_str(self, s, del_str):
        pos = s.find(del_str)
        while pos >= 0:
            if pos == 0:  # 最左边
                s = s.replace(del_str, '', 1).lstrip()
                pos = s.find(del_str)
                continue
            elif pos + len(del_str) == len(s):  # 最右边
                s = s.replace(del_str, '', 1).rstrip()
                pos = s.find(del_str)
                continue
            else:  # 中间
                start_char = s[pos - 1]
                end_char = s[pos + len(del_str)]
                if start_char == ' ' and end_char == ' ':
                    s = s.replace(del_str, '', 1)
                else:
                    s = s.replace(' ' + del_str, '', 1)
                pos = s.find(del_str)
                continue
        return s

    def _convert_link_number(self, link_num):
        number = []
        if not link_num:
            return number
        # if not link_num.isdigit():
        #    self.log.warning('link number is not digit:%s' % link_num)
            # return number
        # 显示用道路番号
        number = self.mult_obj.convert_names_to_list(link_num,
                                                     link_num,
                                                     '',
                                                     'route_num')
        return number

    def _is_include_alt_name(self, name_tha, name_the,
                             alt_name_tha, alt_name_the):
        # 别名和官方名重复
        # 1) 英文泰文都相等
        # 2) 英文相等，泰文别名为空
        # 3) 泰文相等，英文别名为空
        if alt_name_the and alt_name_the == name_the:  # 英文相同
            if alt_name_tha == name_tha:  # 泰文也相同
                return True
            elif not alt_name_tha:  # 泰文为空
                return True
            else:  # 泰文不相同
                self.log.info('Thai Name Difference:alt_name_tha = %s, alt_name_the = %s'
                              % (alt_name_tha, alt_name_the))
                return False
        if alt_name_tha and alt_name_tha == name_tha:  # 泰文相同
            if not alt_name_the:  # 英文为空
                return True
            else:  # 英文不同
                # 如：别名--'1st Expressway';  官方名--'1st Exp.'
                if alt_name_the.replace('Expressway', 'Exp.') == name_the:
                    return True
                if alt_name_the != 'Wireless Road':  # 这个名称太多了，所以不输出log
                    self.log.info('Eng Name Difference: alt_name_tha = %s, alt_name_the = %s'
                                  % (alt_name_tha, alt_name_the))
                return False
        return False

    def _is_name_include_number(self, office_name, office_namt, link_num):
        """判断官方名称是否包了番号. 如：
        Yes: road name = "Highway 2229", number = "2229"
        NO: road name = "Highway 2229", number = "2"
        """
        if not link_num:  # 番号为空
            return False

        if office_name:
            temp_office_name = office_name
        elif office_namt:
            self.log.info('English office name is None.')
            temp_office_name = office_namt
        else:  # 官方名为空
            return False

        pos = temp_office_name.find(link_num)
        if pos >= 0:
            num_len = len(link_num)
            # ## 判断番号前的字符
            if pos > 0:
                start_char = temp_office_name[pos - 1]  # 番号的后一个字符
                if start_char == ' ' or start_char == ')':  # 番号后跟空格或右括号
                    pass
                else:  # 番号前不是空格和右括号
                    self.log.info('Not Include number: road_name=%s,road_num=%s.'
                                   % (temp_office_name, link_num))
                    return False
            else:  # 番号处在名称的最开头
                pass
            # ## 判断番号后的字符
            if pos + num_len == len(temp_office_name):  # 番号刚好在名称的末尾
                return True
            else:
                end_char = temp_office_name[pos + num_len]  # 番号的后一个字符
                if (end_char == ' ' or
                    end_char == '('
                    or end_char == '-'):  # 番号后跟空格、左括号、杠
                    return True
                else:  # 其他
                    self.log.info('Not Include number: road_name=%s; road_num=%s.'
                                   % (temp_office_name, link_num))
                    return False
        else:
            return False

    def _convert_office_name(self, name_tha, name_the, name_tts):
        # ## 官方名
        if not name_tha:
            self.log.info("Lack Thai for Office_name:name_tha = %s, name_the=%s, name_tts=%s"
                          % (name_tha, name_the, name_tts))
            # return None
        if not name_the:
            self.log.info("Lack Eng for Office_name:name_tha = %s, name_the=%s, name_tts=%s"
                          % (name_tha, name_the, name_tts))
        # 删除道路结构信息: "(Overpass) Highway 37" ==> "Highway 37"
        temp_name_tha, temp_name_the, temp_name_tts = \
            self._delete_structinfo(name_tha, name_the, name_tts)
        # 判断官方英文是否还含有括号
        if (temp_name_the and
            (temp_name_the.find('(') >= 0 or
             temp_name_the.find(')') >= 0)):
            self.log.warning('Office THE Name include Brackets. name=%s'
                             % temp_name_the)
        # 判断官方泰文是否还含有括号
        if (temp_name_tha and
            (temp_name_tha.find('(') >= 0 or
             temp_name_tha.find(')') >= 0)):
            self.log.warning('Office THE Name include Brackets. name=%s'
                             % temp_name_tha)
        # 判断官方TTS是否还含有括号
        if (temp_name_tts and
            (temp_name_tts.find('(') >= 0 or
             temp_name_tts.find(')') >= 0)):
            self.log.warning('Office TTS include Brackets. TTS=%s'
                             % temp_name_tts)
        if ((not temp_name_tha) or (not temp_name_the)):
            # self.log.debug("Name was deleted: name_tha = %s, name_the=%s, name_tts=%s"
            #              % (name_tha, name_the, name_tts))
            return None
        name = self.mult_obj.convert_names_to_list(temp_name_tha,
                                                  temp_name_the,
                                                  temp_name_tts,
                                                  'office_name')
        return name

    def _convert_other_name(self, name_tha, name_the, name_tts, name_type):
        if (not name_tha) and (not name_the):
            return None
        if not name_tha:  # 无泰文
            self.log.info("Lack Thai_Name for %s: name_tha = %s, name_the=%s."
                          % (name_type, name_tha, name_the))
        if not name_the:  # 无英文
            self.log.info("Lack Eng_Name for %s: name_tha = %s, name_the=%s."
                          % (name_type, name_tha, name_the))
        alter_name = self.mult_obj.convert_names_to_list(name_tha, name_the,
                                                         name_tts, name_type)
        return alter_name

    def _convert_shield_id(self, c_access, routenum):
        routenum = routenum.strip()
        if routenum[0:2].upper() == 'AH':  # Asian Highway
            return '4003'
        if c_access and c_access.upper() == 'Y':
            return '4001'  # Motorway/Expressway
        else:
            return '4002'  # National Road
        # Rural Road  -- 找不到case
