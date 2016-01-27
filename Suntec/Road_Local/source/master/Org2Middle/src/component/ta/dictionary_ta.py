# -*- coding: UTF8 -*-
'''
Created on 2012-8-29

@author: hongchenzai
'''

import component.default.dictionary
from common import cache_file
from component.default.multi_lang_name import TTS_TYPE_NOT_TTS
from component.default.multi_lang_name import MultiLangShield
from component.ta.multi_lang_name_ta import MultiLangNameTa
import common


#==============================================================================
# comp_dictionary_ta
#==============================================================================
class comp_dictionary_ta(component.default.dictionary.comp_dictionary):
    '''字典(TomTom)
    '''

    def __init__(self):
        '''
        Constructor
        '''
        component.default.dictionary.comp_dictionary.__init__(self)

    def _DoCreateTable(self):
        # component.dictionary.comp_dictionary._DoCreateTable(self)
        # self.CreateTable2('')
        return 0

    def _DoCreateFunction(self):
        self.CreateFunction2('mid_add_one_name')
        self.CreateFunction2('mid_get_language_id')

        return 0

    def _DoCreateIndex(self):
        return 0

    def _Do(self):
        self._IsAUS = False
        self._JudeCountry()
        # 创建语言种别的中间表
        self._InsertLanguages()
        # 设置语言code
        self.set_language_code()

        # 按音素id，合并音素文本
        self._merge_phoneme_text()

        # 建立link_id和音素id对应表
        self._make_phoneme_relation()

        # 制作link的左右官方语种(只针对有名称的link)
        self._make_link_language_code()

        # 道路shield和番号
        self._make_link_shield()

        # 道路名称字典，名称必须在番号之后
        self._make_link_name()

        # 行政区名称
        self._make_admin_name()

        return 0
    
    def _JudeCountry(self):
        self.log.info('jude country.')
              
        str_country = common.common_func.getProjCountry()
        if (str_country.find('AUS') >= 0 and len(str_country) == 3) or \
           (str_country.find('NZL') >= 0 and len(str_country) == 3) or \
           (str_country.find('AUS') >= 0 and str_country.find('NZL') >= 0 and len(str_country) == 7):
            self._IsAUS = True
            
    def _Judecode(self, l_code, r_code):
        
        if (not l_code or l_code in ('UND', 'ENG')) and (not r_code or r_code in ('UND', 'ENG')):
            return True
        else:
            return False
    
    def _merge_phoneme_text(self):
        '''按音素id，合并音素文本'''
        self.CreateTable2('mid_vm_pt')
        # 同个语种下，取preference最小的
        sqlcmd = '''
        INSERT INTO mid_vm_pt(ptid, pt_labels, alphabets, transcriptions,
                              lanphonsets, preferences, lanprons)
        (
        SELECT ptid,
               array_agg(pt_label) as pt_labels,
               array_agg(alphabet) as alphabets,
               array_agg(transcription) as transcriptions,
               array_agg(lanphonset) as lanphonsets,
               array_agg(preference) as preferences,
               array_agg(lanpron) as lanprons
          FROM (
                SELECT ptid,
                       (array_agg(pt_label))[1] as pt_label,
                       (array_agg(alphabet))[1] as alphabet,
                       (array_agg(transcription))[1] as transcription,
                       lanphonset,
                       (array_agg(preference))[1] as preference,
                       (array_agg(lanpron))[1] as lanpron
                  from (
                        SELECT distinct pt.ptid, pt_label, alphabet,
                               transcription, lanphonset, preference,
                               lanpron
                          FROM org_vm_pt as pt
                          order by ptid, lanphonset, preference
                  ) as a
                  group by ptid, lanphonset
           ) AS A
           GROUP BY ptid
        );
        '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        return 0

    def _make_phoneme_relation(self):
        '''建立link_id和音素id对应表'''
        self.CreateTable2('mid_vm_phoneme_relation')
        sqlcmd = """
        INSERT INTO mid_vm_phoneme_relation(
                    id, featclass, sideofline, attitemid,
                    attseqnr, langcode, nametype, namesubtype,
                     "name", normname, ptid)
        (
        SELECT distinct shapeid as id, nefa.featclass, sideofline, attitemid,
               attseqnr, nefa.langcode, nametype, namesubtype,
                "name", normname, ptid
          FROM org_vm_nefa as nefa
          LEFT JOIN org_vm_foa AS foa
          ON (nefa.featdsetid = foa.featdsetid
              and nefa.featsectid = foa.featsectid
              and nefa.featlayerid = foa.featlayerid
              AND nefa.featitemid = foa.featitemid
              AND nefa.featcat = foa.featcat
              and nefa.featclass = foa.featclass)
          LEFT JOIN org_vm_ne as ne
          ON (nefa.featdsetid = ne.namedsetid
              and nefa.featsectid = ne.namesectid
              and nefa.featlayerid = ne.namelayerid
              AND nefa.nameitemid = ne.nameitemid)
          -- 4110: Road Elements, -- 2128: SignPost
          -- [1111, 1120]: [Country, Order 9 Area]
          where nefa.featclass in (4110, 2128) or
                nefa.featclass >= 1111 and nefa.featclass <= 1120
          order by id, attitemid, attseqnr
        );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        self.CreateIndex2('mid_vm_phoneme_relation_id_idx')
        self.CreateIndex2('mid_vm_phoneme_relation_ptid_idx')
        return 0

    def _make_link_language_code(self):
        '''制作link的左右官方语种(只针对有名称的link)。'''
        self.CreateTable2('mid_temp_link_lang_code')
        self.CreateIndex2('gc_id_idx')
        sqlcmd = """
        INSERT INTO mid_temp_link_lang_code(
                        id, l_laxonlc, r_laxonlc, l_order00, r_order00)
        (
        SELECT distinct id, (case when l_order00 = 'MOZ' then 'POR' else l_laxonlc end) as l_laxonlc, 
            (case when r_order00 = 'MOZ' then 'POR' else r_laxonlc end), l_order00, r_order00
          FROM org_gc         
          order by id
        );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        return 0

    def _make_link_name(self):
        "道路名称(包括显示用番号)"
        self.log.info('Make Link Name.')
        # 比较名称的种别和音素的种别
        self.CreateFunction2('mid_compare_nametype')
        # 取得音素信息，返回格式'[音素文本1|音素文本2|...][语种1|语种2|...]'
        self.CreateFunction2('mid_get_phoneme')

        # 名称排序规则：
        # 1. id, 2. sol(左右侧)
        # 3. official name, 4. Street Name,
        # 5. Brunnel Name(桥/隧道名), 6. Locality Name
        # 7. Postal Street Name,   8. gid
        # 9. 所有番号排在名称之后
        # 注：1. 这里使用Full Join是因为，有些道路只有番号，没有名称。
        # 2. 一个越南语文本，一般会有一个越南音素和一个英文音素
        # 3. 音素种别ON, AN, RN, RJ, BU，其中RJ不知道是什么名称
        sqlcmd = """
        SELECT code.id::bigint, fullnames, lang_codes,
               nametypes, sols, phoneme_infos,
               routenums, num_phoneme_infos, l_laxonlc,
               r_laxonlc
          FROM mid_temp_link_lang_code AS code
          LEFT JOIN
          -- link names
          (
                SELECT id, array_agg(fullname) as fullnames,
                       array_agg(namelc) as lang_codes,
                       array_agg(nametyp) as nametypes,
                       array_agg(sol) as sols,
                       array_agg(phoneme_info) as phoneme_infos
                FROM (
                        SELECT id, fullname, namelc, nametyp, sol,
                               mid_get_phoneme(id::bigint,
                                               4110,     -- 4110: Road
                                               sol,
                                               fullname,
                                               namelc,
                                               nametyp::character varying
                                               ) as phoneme_info
                          FROM org_gc
                          WHERE nametyp & 4 <> 4
                          ORDER BY id,
                                   (nametyp & 1) desc,   -- official name
                                   (nametyp & 16) desc,  -- Street Name
                                   (nametyp & 8) desc,   -- Brunnel Name
                                   (nametyp & 32) desc,  -- Locality Name
                                   (nametyp & 64) desc,  -- Postal Street Name
                                   (case when mid_get_phoneme(id::bigint, 4110, sol, fullname, 
                                                         namelc, nametyp::character varying) is null then 1
                                         else 0 end),                                
                                   sol,  -- left, right
                                   namelc,
                                   fullname
                   ) AS A
                   GROUP BY id
          ) AS link_name
          ON code.id = link_name.id
          -- Route Numbers
          LEFT JOIN (
                SELECT link_id, array_agg(routenum) as routenums,
                       array_agg(num_phoneme_info) as num_phoneme_infos
                  from (
                        SELECT link_id, routenum,
                               -- 4110: Road, 0: Both, 4: Route Number
                               mid_get_phoneme(link_id,
                                               4110, -- 4110: Road
                                               0,    -- 0: both side
                                               routenum,
                                               'UND',
                                               '4'   -- 4: Route Num
                                               ) as num_phoneme_info
                          FROM temp_link_route_num
                          order by gid
                  ) as rn
                  group by link_id
          ) as route_num
          on code.id = link_id;
        """
        temp_file_obj = cache_file.open('link_name')  # 创建临时文件
        self.CreateTable2('temp_link_name')
        names = self.get_batch_data(sqlcmd)
        for name_info in names:
            link_id = name_info[0]
            names = name_info[1]
            lang_codes = name_info[2]
            nametypes = name_info[3]
            sols = name_info[4]  # 左右侧名称
            phoneme_infos = name_info[5]  # 名称音素
            routenums = name_info[6]
            num_phoneme_infos = name_info[7]  # 番号的音素
            l_lang_code = name_info[8]  # 左边官方语种
            r_lang_code = name_info[9]  # 右边官方语种
                
            if self._IsAUS:
                if self._Judecode(l_lang_code, r_lang_code):
                    l_lang_code = 'ENG'  # 左边官方语种
                    r_lang_code = 'ENG'  # 右边官方语种
                else:
                    continue
                
            if not names and not routenums:
                self.log.warning('No name and number. id=%d' % link_id)
                continue
            multi_name = MultiLangNameTa(link_id)
            multi_name.set_lang_code(l_lang_code, r_lang_code)
            if multi_name.l_lang_code != multi_name.r_lang_code :
                len_lang = len(lang_codes)
                for i in range(len_lang):
                    if (not lang_codes[i] or lang_codes[i]  == 'UND') and \
                        (not sols[i] or sols[i] == 0) :
                        self.log.warning('id=%d, namelc is None or UND, l_lang and r_lang not same! ')
                        

            multi_name.set_multi_name(names,
                                      lang_codes,
                                      nametypes,
                                      sols,
                                      phoneme_infos
                                      )
            multi_name.set_multi_routenum(names, routenums, num_phoneme_infos)

            json_name = multi_name.json_format_dump()
            if not json_name:
                self.log.error('Json Name is none. linkid=%d' % link_id)
            # 存到本地的临时文件
            self._store_name_to_temp_file(temp_file_obj,
                                          link_id,
                                          json_name)
        # ## 把名称导入数据库
        temp_file_obj.seek(0)
        self.pg.copy_from2(temp_file_obj, 'temp_link_name')
        self.pg.commit2()
        # close file
        #temp_file_obj.close()
        cache_file.close(temp_file_obj,True)
        self.log.info('End Make Link Name.')
        return 0

    def _make_link_shield(self):
        # 盾牌番号字典
        self.log.info('Make Link Shield.')
        temp_shield_file = cache_file.open('link_shield')  # 创建临时文件
        temp_routenum_file = cache_file.open('route_num')  # 创建临时文件
        self.CreateTable2('temp_link_shield')
        self.CreateTable2('temp_link_route_num')

        sqlcmd = """
          SELECT num.id, shieldids, shieldnums, routenums,
               l_laxonlc, r_laxonlc
          FROM (
                SELECT id, array_agg(shiledid) as shieldids,
                       array_agg(shieldnum) as shieldnums,
                       array_agg(routenum) as routenums
                  FROM (
                        SELECT distinct id, a.rteprior, length(a.shieldnum),a.routenum,
                                   a.shieldnum, b.namelc,a.rtetyp as shiledid, a.gid
                          FROM org_rn a
                          join org_gc b
                          using(id)
                          where shieldnum is not null
                          ORDER BY id, a.rteprior, length(a.shieldnum),
                                   a.shieldnum, b.namelc, shiledid, a.gid
                         
                   ) AS A
                   GROUP BY id
           ) AS num
           LEFT JOIN mid_temp_link_lang_code as code
           on num.id = code.id;
        """
        numbers = self.get_batch_data(sqlcmd)
        for number_info in numbers:
            link_id = number_info[0]
            shieldids = number_info[1]
            shieldnums = number_info[2]  # 盾牌上用
            routenums = number_info[3]  # 道路名称上用
            l_lang_code = number_info[4]  # 左边官方语种
            r_lang_code = number_info[5]  # 右边官方语种
            
            if self._IsAUS:
                if self._Judecode(l_lang_code, r_lang_code):
                    l_lang_code = 'ENG'  # 左边官方语种
                    r_lang_code = 'ENG'  # 右边官方语种
                else:
                    continue

            if not link_id:
                self.log.error("No Link_id.")
                continue
            multi_shield_obj, route_num_list, phoneme_list = \
                    self._get_shield_info(link_id,
                                          shieldids,
                                          shieldnums,
                                          routenums,
                                          l_lang_code,
                                          r_lang_code,
                                          None  # phoneme info
                                          )
            if multi_shield_obj:
                # ## shield number
                json_name = multi_shield_obj.json_format_dump()
                if not json_name:
                    self.log.error('Json Shield is none. linkid=%d' % link_id)
                # 存到本地的临时文件
                self._store_name_to_temp_file(temp_shield_file,
                                              link_id,
                                              json_name)

                # ## route number
                if not route_num_list:
                    self.log.error('RouteNum is none. linkid=%d' % link_id)
                else:
                    # 存到本地的临时文件
                    for route_num in route_num_list:
                        self._store_name_to_temp_file(temp_routenum_file,
                                                      link_id,
                                                      route_num)
            else:  # shield 无
                self.log.error('multi_shield_obj is none. linkid=%d'
                               % link_id)

        # ## 把shield导入数据库
        temp_shield_file.seek(0)
        self.pg.copy_from2(temp_shield_file, 'temp_link_shield')
        self.pg.commit2()
        # close file
        #temp_shield_file.close()
        cache_file.close(temp_shield_file,True)

        # ## 把RouteNum导入数据库
        temp_routenum_file.seek(0)
        self.pg.copy_from2(temp_routenum_file,
                           'temp_link_route_num',
                           columns=('link_id', 'routenum')
                           )
        self.pg.commit2()
        # close file
        #temp_routenum_file.close()
        cache_file.close(temp_routenum_file,True)
        self.log.info('End Make Link Shield Info.')
        return 0

    def _get_shield_info(self,
                         link_id,
                         shieldids,
                         shieldnums,
                         routenums,
                         l_lang_code,
                         r_lang_code,
                         phoneme_infos=None
                         ):
        multi_shield_obj = None
        route_num_list = list()
        num_phoneme_list = list()
        shield_obj = ShieldTA()
        temp_m_name = MultiLangNameTa(n_id=link_id)
        temp_m_name.set_lang_code(l_lang_code, r_lang_code)
        lang_infos = temp_m_name.get_side_lang_infos()  # 取各左右两边的语种
        if not phoneme_infos:
            phoneme_infos = [None] * len(shieldids)
        for shieldid, shieldnum, routenum, phoneme_info in \
                zip(shieldids, shieldnums, routenums, phoneme_infos):
            if not shieldid:  # shield id 无
                self.log.error("No new shield id. link_id=%d", link_id)
                continue
            if not shieldnum:  # shield number 无
                self.log.error("No new shield num. link_id=%d", link_id)
                continue
            if not routenum:  # route number 无
                self.log.error("No new route num. link_id=%d", link_id)
                continue

            # 判断是否多个番号, 如: “17/92”，“60  70”，“14-16”
            if shield_obj.is_mulit_shield_num(shieldnum):
                if shield_obj.is_shield_num_repeat(shieldnum, shieldnums):
                    # 每个单独番号已存在, 就不在收录。
                    continue
            # 转成新的全球统一的shield id
            new_shieldid = shield_obj.convert_shield_id(shieldid)
            if not new_shieldid:  # 新shield id无
                self.log.error("No new shield id. link_id=%s, shieldid=%s", link_id, shieldid)
            for (left_right, lang_code) in lang_infos:
                if lang_code is None:
                    lang_code = 'ENG'
                    
                if multi_shield_obj:
                    # ## shield number
                    alter_shield = MultiLangShield(new_shieldid,
                                                   shieldnum,
                                                   lang_code,
                                                   TTS_TYPE_NOT_TTS,
                                                   left_right
                                                   )
                    multi_shield_obj.add_alter(alter_shield)
                else:
                    # ## shield number
                    multi_shield_obj = MultiLangShield(new_shieldid,
                                                       shieldnum,
                                                       lang_code,
                                                       TTS_TYPE_NOT_TTS,
                                                       left_right
                                                       )
            # route number
            route_num_list.append(routenum)
            num_phoneme_list.append(phoneme_info)

        return multi_shield_obj, route_num_list, num_phoneme_list

    def _store_name_to_temp_file(self, file_obj, nid, json_name):
        if file_obj:
            file_obj.write('%d\t%s\n' % (nid, json_name))
        return 0

    def _make_admin_name(self):
        '''行政区名称'''
        self.log.info('Make Feature Name.')
        self.CreateFunction2('mid_compare_nametype')
        # 取得音素信息，返回格式'[音素文本1|音素文本2|...][语种1|语种2|...]'
        self.CreateFunction2('mid_get_phoneme')
        # 排序名规则
        # 1. id, 2. nametyp降序, 3. gid
        # 注：行政名称里有很多重复的名称，不知为啥, 如：id = 17040000000001
        # 一个越南语文本，一般会有一个越南音素和一个英文音素
        sqlcmd = """
        SELECT id,
               array_agg(nametyp) as nametypes,
               array_agg("name") as names,
               array_agg(namelc) as lang_codes,
               array_agg(phoneme_info) as phoneme_infos
          FROM (
                SELECT min_gid, id, nametyp, "name", namelc, phoneme_info
                  FROM (
                        SELECT  min(gid) as min_gid, id, nametyp,
                                "name", namelc, feattyp,
                                mid_get_phoneme(id::bigint,
                                                feattyp,
                                                0,      -- 0: both side
                                                "name",
                                                namelc,
                                                nametyp
                                                ) as phoneme_info
                          FROM org_an
                          where "name" is not null
                          GROUP BY id, nametyp, namelc, "name", feattyp
                  ) as b
                  order by id, nametyp DESC, min_gid
          ) AS an
          GROUP BY id;
        """
        temp_file_obj = cache_file.open('admin_name')  # 创建临时文件
        self.CreateTable2('mid_temp_admin_name')
        features = self.get_batch_data(sqlcmd)
        from component.ta.multi_lang_name_ta import MultiLangAdminNameTa
        for feature in features:
            admin_id = feature[0]
            nametypes_all = feature[1]  # ON官方名，AN别名
            names_all = feature[2]
            lang_codes_all = feature[3]
            phoneme_infos_all = feature[4]  # 音素
            
            nametypes = list()  # ON官方名，AN别名
            names = list()
            lang_codes = list()
            phoneme_infos = list()  # 音素
            
            if self._IsAUS:
                num = len(feature[3])
                for num_idx in range(num):
                    if self._Judecode(lang_codes_all[num_idx], None):
                        nametypes.append(nametypes_all[num_idx])
                        names.append(names_all[num_idx])
                        lang_codes.append('ENG')
                        phoneme_infos.append(phoneme_infos_all[num_idx])
            else:
                nametypes = nametypes_all
                names = names_all
                lang_codes = lang_codes_all
                phoneme_infos = phoneme_infos_all

            multi_name = MultiLangAdminNameTa(admin_id)
            multi_name.set_multi_name(names,
                                      lang_codes,
                                      nametypes,
                                      None,  # side of line
                                      phoneme_infos
                                      )
            json_name = multi_name.json_format_dump()
            if not json_name:
                self.log.error('Json Name is none. id=%d' % admin_id)
            # 存到本地的临时文件
            self._store_name_to_temp_file(temp_file_obj,
                                          admin_id,
                                          json_name
                                          )
        # ## 把名称导入数据库
        temp_file_obj.seek(0)
        self.pg.copy_from2(temp_file_obj, 'mid_temp_admin_name')
        self.pg.commit2()
        # close file
        #temp_file_obj.close()
        cache_file.close(temp_file_obj,True)
        self.log.info('End Feature Name.')

    def merge_links_name(self, link_list):
        '''把所有给定link的名称、番号合并起来,返回JSON格式的名称和shield。'''
        self.pg.connect1()
        self.set_language_code()
        str_name = None
        str_shield = None
        if not link_list:
            return str_name, str_shield
        # ## 取得link的语种
        l_lang_code, r_lang_code = None, None
        l_langs, r_langs = self._merge_link_lang_code(link_list)
        if l_langs:
            l_lang_code = l_langs[0]
        if r_langs:
            r_lang_code = r_langs[0]
        if len(l_langs) > 1 or len(r_langs) > 1:
            self.log.warning('Number of language code > 1. links=%s'
                             % link_list)
        # 合并盾牌
        multi_shield, routenums, num_phoneme_infos = \
                self._merge_links_shield(link_list, l_lang_code, r_lang_code)
        if multi_shield:
            json_shield = multi_shield.json_format_dump()
            if not json_shield:
                self.log.error('Error JSON Name. linkid=%d' % link_list[-1])
        else:
            json_shield = None

        sqlcmd = """
        SELECT array_agg(id) as ids,
               array_agg(fullname) as fullnames,
               array_agg(namelc) as lang_codes,
               array_agg(nametyp) as nametypes,
               array_agg(sol) as sols,
               array_agg(phoneme_info) as phoneme_infos
          FROM (
                SELECT 1 as id_for_group, id, fullname, namelc,
                       nametyp, sol, phoneme_info
                  FROM (
                        SELECT min(id) as id, fullname, namelc, nametyp, sol,
                               -- 4110: Road
                               mid_get_phoneme(id::bigint,
                                               4110,  -- 4110: Road
                                               sol,
                                               fullname,
                                               namelc,
                                               nametyp::character varying
                                               ) as phoneme_info
                          FROM org_gc
                          WHERE nametyp & 4 <> 4  -- not route number
                                and id in (links)
                          group by fullname, namelc, nametyp, sol, phoneme_info
                  ) as A
                  ORDER BY sol,  -- 0:both, 1:left, 2:right
                           (nametyp & 1) desc,   -- official name
                           (nametyp & 16) desc,  -- Street Name
                           (nametyp & 8) desc,   -- Brunnel Name
                           (nametyp & 32) desc,  -- Locality Name
                           (nametyp & 64) desc,  -- Postal Street Name
                           id
          ) as B
          group by id_for_group;
        """
        str_link_ist = str(tuple(link_list)).replace('L', '')
        sqlcmd = sqlcmd.replace('(links)', str_link_ist)
        self.pg.execute1(sqlcmd)
        row = self.pg.fetchone()
        self.pg.close1()
        json_name = None
        multi_name = MultiLangNameTa(link_list[-1])
        # 设置语种
        multi_name.set_lang_code(l_lang_code, r_lang_code)
        if row:
            # ids = row[0]
            names = row[1]
            lang_codes = row[2]
            nametypes = row[3]
            sols = row[4]  # 左右侧名称
            phoneme_infos = row[5]
            # 去重处理，同一个番号，在不同的link间，等组不同，引起的
            # ...
            if names:
                multi_name.set_multi_name(names,
                                          lang_codes,
                                          nametypes,
                                          sols,
                                          phoneme_infos
                                          )
                multi_name.set_multi_routenum(names,
                                              routenums,
                                              num_phoneme_infos
                                              )

                json_name = multi_name.json_format_dump()
                if not json_name:
                    self.log.error('Error JSON Name.linkid=%d' % link_list[-1])
                else:
                    pass
            else:  # 没有名称list(这个不可能进来)
                pass
        else:  # 没有名称
            if routenums:
                multi_name.set_multi_routenum(None,
                                              routenums,
                                              num_phoneme_infos
                                              )
                json_name = multi_name.json_format_dump()
                if not json_name:
                    self.log.error('Error JSON Name.linkid=%d' % link_list[-1])
                else:
                    pass
            else:
                pass
        if json_shield and not json_name:  # 有番号，没有名称
            self.log.error('Exist JSON shield, but no JSON Name.linkid=%d'
                           % link_list[-1])
        return json_name, json_shield

    def _merge_links_shield(self, link_list, l_lang_code, r_lang_code):
        sqlcmd = """
        SELECT array_agg(id) as ids,
               array_agg(shiledid) as shieldids,
               array_agg(shieldnum) as shieldnums,
               array_agg(routenum) as routenums,
               array_agg(phoneme_info) as phoneme_infos
          FROM (
                SELECT 1 as id_for_group, id, shiledid,
                       shieldnum, routenum, rteprior,
                       phoneme_info
                  FROM (
                        SELECT min(id) as id, rtetyp as shiledid,
                               shieldnum, routenum, rteprior,
                               -- 4110: Road, 0: Both, 4: Route Number
                               mid_get_phoneme(id::bigint,
                                               4110,  -- 4110: Road
                                               0,     -- 0: Both side
                                               routenum,
                                               'UND',
                                               '4'    -- Route Num
                                               ) as phoneme_info
                          FROM org_rn
                          where id in (links)
                          group by shiledid, shieldnum, routenum,
                                   rteprior, phoneme_info
                  ) AS A
                  ORDER BY rteprior, length(shieldnum),
                           shieldnum, shiledid, id
          ) AS B
          GROUP BY id_for_group;
        """
        multi_shield = None
        route_num_list = list()
        num_phoneme_list = list()
        link_list.sort()
        str_link_ist = str(tuple(link_list)).replace('L', '')
        sqlcmd = sqlcmd.replace('(links)', str_link_ist)
        self.pg.execute1(sqlcmd)
        row = self.pg.fetchone()
        if row:
            # ids = row[0]
            shieldids = row[1]
            shieldnums = row[2]
            routenums = row[3]
            phoneme_infos = row[4]
            # 去重处理，同一个番号，在不同的link间，等级不同，引起的
            # ...
            multi_shield, route_num_list, num_phoneme_list = \
                            self._get_shield_info(link_list[0],  # 小最的
                                                  shieldids,
                                                  shieldnums,
                                                  routenums,
                                                  l_lang_code,
                                                  r_lang_code,
                                                  phoneme_infos
                                                  )
        return multi_shield, route_num_list, num_phoneme_list

    def _merge_link_lang_code(self, link_list):
        '''合并link的语言种别。'''
        sqlcmd = """
        SELECT array_agg(l_laxonlc) as left_code,
               array_agg(r_laxonlc) as right_code
          from (
                select 1 as id_for_group,
                       l_laxonlc, r_laxonlc
                  from mid_temp_link_lang_code AS code
                  where code.id in (links)
                  order by id
          ) as a
          group by id_for_group
        """
        l_set, r_set = set(), set()
        str_link_ist = str(tuple(link_list)).replace('L', '')
        sqlcmd = sqlcmd.replace('(links)', str_link_ist)
        self.pg.execute1(sqlcmd)
        row = self.pg.fetchone()
        if row:
            l_set = set(row[0])
            r_set = set(row[1])
        if l_set:
            l_set.remove(None)
        if r_set:
            r_set.remove(None)
        return list(l_set), list(r_set)

    def _delete_repeat(self, attr_list):
        rst_atti_list = attr_list[0:1]
        rst_num = 1
        attr_cnt = 1
        for attr in attr_list[1:-1]:
            if attr in rst_atti_list:  # 重复
                pass
            else:  # 不重复
                rst_atti_list.append(attr)
        return rst_atti_list

    def _DoCheckValues(self):
        d = [('temp_link_name', 'name'),
             ('mid_temp_admin_name', 'admin_name')
            ]
        for table, col in d:
            self._check_json_name(table, col)

    def _check_json_name(self, table, col):
        self.log.info('Checking %s' % table)
        sqlcmd = "SELECT " + col + " FROM " + table + ";"
        names = self.get_batch_data(sqlcmd)
        import json
        for name in names:
            json.loads(name[0])


#==============================================================================
# ShieldTA--处理盾牌和番号的通用类
#==============================================================================
class ShieldTA(object):
    def __init__(self):
        '''
        Constructor
        '''

    def convert_shield_id(self, shieldid):
        shieldid_dict = {8800: '4141',  # Asian Highway
                         8801: '4142',  # Motorway
                         8802: '4143',  # National Road / Province Road
                         #AUS,NEL
                         400: '2000',
                         405: '2003',
                         410: '2005',
                         425: '2001',
                         430: '2002',
                         420: '2004',
                         435: '2007',
                         450: '2006',
                         440: '2008',
                         445: '2009',
                         455: '2010',
                         460: '2011',
                         465: '2012',
                         470: '2013',
                         4130: '2014',
                         #ZAF
                         9500: '2100',
                         9505: '2101',
                         9506: '2102',
                         9510: '2103',
                         9515: '2104',
                         #BWA
                         8540: '2110',
                         8545: '2111',
                         8550: '2112',
                         #LSO
                         8610: '2120',
                         8612: '2121',
                         8614: '2122',
                         #NAM
                         8670: '2130',
                         8672: '2131',
                         8674: '2132',
                         8678: '2133',
                         8679: '2134',
                         #SWZ
                         8700: '2140',
                         8702: '2141',
                         #ZWE
                         8752: '2150',
                         8754: '2151',
                         #MOZ
                         8630: '2160',
                         8632: '2161',
                         8634: '2162',
                         8636: '2163',
                         #ZMB
                         8740: '2170',
                         8742: '2171',
                         8744: '2172',
                         8746: '2173',
                         8750: '2174',                         
                         }

        return shieldid_dict.get(shieldid)

    def is_mulit_shield_num(self, shieldnum):
        '''判断是否多个番号, 如: “17/92”，“60  70”，“14-16”'''
        return False

    def is_shield_num_repeat(self, shieldnum, shieldnums):
        '''判断多个番号里的每个单独番号已存在。如：“17/92”里的17和19, 已在其他记录里出现过。'''
        return False

    def is_name_include_number(self, names, routenum):
        """判断名称是否包了番号. 如：
        Yes: road name = "Highway 2229", number = "2229"
        NO: road name = "Highway 2229", number = "2"
        """
        if not names:  # 无名称
            return False
        if not routenum:  # 番号为空
            return False
        names = list(names)

        for name in names:
            if not name:  # 名为空
                continue
            pos = name.find(routenum)
            if pos >= 0:
                num_len = len(routenum)
                # ## 判断番号前的字符
                if pos > 0:
                    start_char = name[pos - 1]  # 番号的后一个字符
                    if start_char in (' ', ')', '-'):  # 番号前空格或右括号
                        pass
                    else:  # 番号前不是空格和右括号
#                        print ('Not Include number: road_name=%s,road_num=%s.'
#                                       % (name, routenum))
                        return False
                else:  # 番号处在名称的最开头
                    pass
                # ## 判断番号后的字符
                if pos + num_len == len(name):  # 番号刚好在名称的末尾
                    return True
                else:
                    end_char = name[pos + num_len]  # 番号的后一个字符
                    if (end_char == ' ' or
                        end_char == '('
                        or end_char == '-'):  # 番号后跟空格、左括号、杠
                        return True
                    else:  # 其他
#                        print ('Not Include number: road_name=%s; road_num=%s.'
#                                       % (name, routenum))
                        return False
        return False
