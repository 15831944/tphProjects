# -*- coding: UTF8 -*-
'''
Created on 2012-10-24

@author: hongchenzai
'''
import re
from common import cache_file
from component.default.multi_lang_name import MultiLangName
from component.default.multi_lang_name import MultiLangShield
from component.mmi.dictionary_mmi import ShieldMmi
from component.mmi.dictionary_mmi import comp_dictionary_mmi
from component.default.guideinfo_signpost_uc import comp_guideinfo_signpost_uc
from component.mmi.dictionary_mmi import MMI_OFFICIAL_LANG_CODE
SIGN_POST_TYPE_EXIT_NO = 1
SIGN_POST_TYPE_SHIELD_ID = 2
SIGN_POST_TYPE_NAME = 4

class comp_guideinfo_signpost_uc_mmi(comp_guideinfo_signpost_uc):
    '''TomTom方面
    '''

    def __init__(self):
        '''
        Constructor
        '''
        comp_guideinfo_signpost_uc.__init__(self)

    def _Do(self):
        dict_mmi = comp_dictionary_mmi()
        dict_mmi.set_language_code()
        # 名称、番号、出口番号
        self._make_signpost_element()
        # SignPost的link序
        self._make_path_link()
        # 合并以上所有的信息
        self._merge_all_info()
        return 0

    def _make_signpost_element(self):
        self.log.info('Start Make SignPost element.')
        sqlcmd = """
        SELECT folder,
               sign_id,
               array_agg("row") as "rows",
               array_agg("column") as "columns",
               array_agg(itemtype) as itemtypes,
               array_agg(iteminfo) as iteminfos
          FROM (
                SELECT gid, sign_id::BIGINT, "row"::INTEGER, "column"::INTEGER,
                       itemtype::INTEGER, iteminfo, folder
                  FROM org_signpost_table_2
                  order by folder, sign_id, "row", "column", gid
          ) AS A
          GROUP BY folder, sign_id
          ORDER BY folder, sign_id;
        """
        self.CreateTable2('mid_temp_signpost_element')
        temp_file_obj = cache_file.open('signpost_element')  # 创建临时文件
        signs = self.get_batch_data(sqlcmd)
        for sign_info in signs:
            folder = sign_info[0]  # folder名or区域名
            sign_id = sign_info[1]  # 看板id
            # rows = sign_info[2]  # 方向番号
            # columns = sign_info[3]  # 同个方向内顺序号
            itemtypes = sign_info[4]  # 种别
            iteminfos = sign_info[5]  # 种别
            sign_post = SignPostElementMmi(sign_id, folder)
            exit_no = None
            route_no = None
            signpost_name = None
            lang_code = MMI_OFFICIAL_LANG_CODE
            for itemtype, iteminfo in zip(itemtypes, iteminfos):
                # 去掉距离
                name = self._delete_distance_str(iteminfo)
                if not name:  # 去掉只有距离，没有名称的记录
                    # self.log.info(iteminfo)
                    continue
                sign_type = self._get_sign_type(itemtype)
                if not sign_type:  # 空不收录
                    continue
                name_type = self._cvt_name_type(sign_type)
                # ## 处理各种种别的数据
                if SIGN_POST_TYPE_SHIELD_ID == sign_type:  # shield id
                    route_no = self._get_route_no(name, lang_code)
                    if not route_no:
                        # 'National Highway 5', 'National Highway 5 10 Km'
                        c = re.compile(r'^National\s*Highway', re.I)
                        if not c.findall(name):
                            self.log.error('Error RouteNo. '
                                           'SignId=%s,Folder=%s,info=%s'
                                           % (sign_id, folder, iteminfo))
                        # 'National Highway' replace 'NH'
                        routenum = c.sub('NH', name)
                        info_list = routenum.split(' ')
                        length = len(info_list)
                        p = re.compile(r'\D+')
                        # len=2, 番号是数字,如：'National Highway 5'改为'NH 5'，并做成番号
                        if length == 2 and not p.findall(info_list[1]):
                            route_no = self._get_route_no(routenum, lang_code)
                        else:
                            # 把这种'National Highway 5 10 Km', 做成方面名称
                            # 由于距离被去掉，这里一般不会再执行
                            self.log.error('Error RouteNo. '
                                           'SignId=%s,Folder=%s,info=%s'
                                           % (sign_id, folder, iteminfo))
                            continue
                    if route_no:
                        sign_post.add_route_no(route_no)
                    else:
                        self.log.error('Get RouteNo Failed. '
                                       'SignId=%s,Folder=%s,type=%s,info=%s'
                                       % (sign_id, folder, itemtype, iteminfo))
                elif SIGN_POST_TYPE_EXIT_NO == sign_type:  # 出口番号
                    # 存在多个出口番号，其他出口番号做出口番号的别名
                    e_no = MultiLangName(lang_code, name, name_type)
                    if exit_no:
                        exit_no.add_alter(e_no)
                    else:  # 第一个出口番号
                        exit_no = e_no
                elif SIGN_POST_TYPE_NAME == sign_type:  # 方面名称
                    sign_name = MultiLangName(lang_code, name, name_type)
                    if signpost_name:  # 其他番号
                        signpost_name.add_alter(sign_name)
                    else:  # 第一个名称
                        signpost_name = sign_name
                else:
                    continue
            if route_no or exit_no or signpost_name:
                sign_post.set_exit_no(exit_no)
                sign_post.set_signpost_name(signpost_name)
                str_info = sign_post.to_string()
                if str_info:
                    self._store_name_to_temp_file(temp_file_obj, str_info)
                else:
                    self.log.error('No SignPost Info. '
                                   'Sign_id=%d, Folder=%s' % (sign_id, folder))
            else:
                self.log.warning('No route_no/exit_no/signpost_name.'
                                 'Sign_id=%d, Folder=%s' % (sign_id, folder))
        # ## 把名称导入数据库
        temp_file_obj.seek(0)
        self.pg.copy_from2(temp_file_obj, 'mid_temp_signpost_element')
        self.pg.commit2()
        # close file
        #temp_file_obj.close()
        cache_file.close(temp_file_obj,True)
        self.log.info('End Make SignPost element.')
        return 0

    def _store_name_to_temp_file(self, file_obj, str_info):
        if file_obj:
            file_obj.write('%s\n' % str_info)
        return 0

    def _get_route_no(self, routenum, lang_code=MMI_OFFICIAL_LANG_CODE):
        shield_obj = ShieldMmi()
        shield_id = shield_obj.convert_shield_id(routenum)
        if not shield_id:
            return None
        route_no = MultiLangShield(shield_id,
                                   routenum,
                                   lang_code
                                   )
        return route_no

    def _get_sign_type(self, infotyp):
        d = {1: SIGN_POST_TYPE_SHIELD_ID,
             2: SIGN_POST_TYPE_EXIT_NO,  # 出口番号
             3: SIGN_POST_TYPE_NAME,  # Street Name(做成名称)
             4: SIGN_POST_TYPE_NAME,  # Exit Name(做成名称)
             5: None,  # Pictogram(不收录)
             6: SIGN_POST_TYPE_NAME,  # Place Name(做成名称)
             7: SIGN_POST_TYPE_NAME  # Other Destination
             }
        # 值为空，不收录
        return d.get(infotyp)

    def _delete_distance_str(self, name):
        '''删除距离
           'Tribal Museum 2 Km' ==> 'Tribal Museum'
           'Koratagere 17.2 Kms' ==> 'Koratagere'
        '''
        rst_name = ' ' + name + ' '
        dist_list = re.findall(r'\s+\d*\.?\d+\s*[K\k][M|m][S|s]*\.?\s+',
                               rst_name)
        if dist_list:
            if len(dist_list) > 1:
                self.log.warning('Distance String More Than 1. name = %s'
                                 % name)
            dist_str = dist_list[0]
            if dist_str == rst_name:  # 整个名称都是距离
                return ''
            rst_name = rst_name.replace(dist_str, ' ')
            rst_name = rst_name.strip()
            # ## 几个连续的空白，变成一个空格
            rst_name = re.sub(r'\s+', ' ', rst_name)
            return rst_name
        # ## 距离数据前一个字符非空白
        dist_list = re.findall(r'\d*\.?\d+\s*[K\k][M|m][S|s]*\.?\s+', rst_name)
        if dist_list:
            self.log.warning('Prefix Char is not blank. name = %s' % name)
            dist_str = dist_list[0]
            rst_name = rst_name.replace(dist_str, ' ')
            rst_name = rst_name.strip()
            # ## 几个连续的空白，变成一个空格
            rst_name = re.sub(r'\s+', ' ', rst_name)
            return rst_name
        '''删除距离
           'Police Museum 20 Mtr' ==> 'Police Museum'
           'Jew Cementery 200 Mtr' ==> 'Jew Cementery'
        '''
        ##距离为Mtr
        dist_list = re.findall(r'\s+\d*\.?\d+\s*[M\m][T|t][R|r]*\.?\s+',
                               rst_name)
        if dist_list:
            if len(dist_list) > 1:
                self.log.warning('Distance String More Than 1. name = %s'
                                 % name)
            dist_str = dist_list[0]
            if dist_str == rst_name:  # 整个名称都是距离
                return ''
            rst_name = rst_name.replace(dist_str, ' ')
            rst_name = rst_name.strip()
            # ## 几个连续的空白，变成一个空格
            rst_name = re.sub(r'\s+', ' ', rst_name)
            return rst_name
        
        dist_list = re.findall(r'\d*\.?\d+\s*[M\m][T|t][R|r]*\.?\s+', rst_name)
        if dist_list:
            self.log.warning('Prefix Char is not blank. name = %s' % name)
            dist_str = dist_list[0]
            rst_name = rst_name.replace(dist_str, ' ')
            rst_name = rst_name.strip()
            # ## 几个连续的空白，变成一个空格
            rst_name = re.sub(r'\s+', ' ', rst_name)
            return rst_name
        return name  # 没有距离

    def _make_path_link(self):
        "找到SignPost对应的link序 (如：in_link_id, out_link_id, passlid, passlid_cnt)"
        # 同一个id中，
        # seqnr = 1的， 作为in_link_id
        # seqnr最大的，作为out_link_id
        # 处于以上两者之间的，作为pass link
        self.CreateTable2('mid_temp_signpost_passlink')
        self.CreateFunction2('mid_get_connect_junction')
        # self.CreateFunction2('mid_make_signpost_path_links')
        temp_file_obj = cache_file.open('signpost_passlink')  # 创建临时文件
        sqlcmd = """
        SELECT sign_id, link_ids, folder,
               mid_get_connect_junction(link_ids[1], link_ids[2])
          FROM (
                SELECT sign_id, array_agg(link_id) as link_ids, folder
                  from (
                        SELECT sign_id, seqnr,
                               edge_id::BIGINT as link_id, folder
                          FROM org_signpost_table_1
                          order by folder, sign_id, seqnr
                    ) as p
                  group by folder, sign_id
                  order by folder, sign_id
          ) AS A
        """
        pathes = self.get_batch_data(sqlcmd)
        for path_info in pathes:
            sign_id = path_info[0]
            link_ids = path_info[1]
            folder = path_info[2]
            node_id = path_info[3]
            if not sign_id:
                self.log.error('Error Sign ID.')
                continue
            if not node_id:
                self.log.error('Error Node ID.')
                continue
            if link_ids:
                in_link_id = link_ids[0]
                out_link_id = link_ids[-1]
                pass_link = '|'.join([str(p) for p in link_ids[1:-1]])
                pass_link_cnt = len(link_ids[1:-1])

                str_info = ('%s\t%d\t%d\t%d\t%d\t%s\t%d'
                            % (folder, sign_id, in_link_id, node_id,
                               out_link_id, pass_link, pass_link_cnt))
                self._store_name_to_temp_file(temp_file_obj, str_info)

        # ## 把名称导入数据库
        temp_file_obj.seek(0)
        self.pg.copy_from2(temp_file_obj, 'mid_temp_signpost_passlink')
        self.pg.commit2()
        # close file
        #temp_file_obj.close()
        cache_file.close(temp_file_obj,True)
        return 0

    def _merge_all_info(self):
        "把SignPost的数据合并起来(Name, Route_NO, Exit_No, Path_Link)"
        # 注：这里要把mid_temp_signpost_element放最前面, 因为有看板的无素没有收录。
        sqlcmd = """
        INSERT INTO signpost_uc_tbl(
                    id, nodeid, inlinkid,
                    outlinkid, passlid, passlink_cnt,
                    sp_name, route_no1, route_no2,
                    route_no3, route_no4, exit_no)
        (
                SELECT distinct e.sign_id, node_id, in_link_id,
                       out_link_id, passlid, passlink_cnt,
                       signpost_name, route_no1, route_no2,
                       route_no3, route_no4, exit_no
                  FROM mid_temp_signpost_element  as e
                  LEFT JOIN mid_temp_signpost_passlink as p
                  ON e.folder = p.folder and e.sign_id = p.sign_id
                  left join link_tbl as b
                  on p.in_link_id = b.link_id or 
                     p.out_link_id = b.link_id
                  where b.one_way_code <> 4 and b.link_id is not null                    
                  order by  e.sign_id
        );
        """
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()
        return 0
    def _cvt_name_type(self, sign_type):
        if SIGN_POST_TYPE_EXIT_NO == sign_type:
            return 'office_name'
        if (SIGN_POST_TYPE_SHIELD_ID == sign_type):
            return 'shield'
        if SIGN_POST_TYPE_NAME == sign_type:
            return 'office_name'
                    
    def _DoCheckValues(self):        
        self.log.info('begin Check signpost_uc Values..')
        sqlcmd = '''
                 select id
                 from signpost_uc_tbl
                 where sp_name is null and route_no1 is null and route_no2 is null
                    and route_no3 is null and  route_no4 is null and exit_no is null;
                 '''
        rows = self.GetRows(sqlcmd)
        if rows:
            self.log.error('all name is null!')
            return -1
                
        return 0
    
    def _DoCheckNumber(self):
        return 0
    
    def _DoContraints(self):
        self.log.info('begin Check signpost_uc Contraints..')
        sqlcmd = '''
                 select id
                 from signpost_uc_tbl as a
                 left join link_tbl as b
                 on a.inlinkid = b.link_id and a.nodeid in (b.s_node, b.e_node) and b.one_way_code <> 4
                 left join link_tbl as c
                 on a.outlinkid = c.link_id and c.one_way_code <> 4
                 left join node_tbl as d
                 on a.nodeid = d.node_id
                 where b.link_id is null or c.link_id is null or d.node_id is null
                 
                 union
                 
                 select id
                 from
                 (
                     select id,unnest(string_to_array(passlid,'|')::bigint[]) as pass_link
                     from signpost_uc_tbl
                     where passlid is not null
                 )as a
                 left join link_tbl as b
                 on a.pass_link = b.link_id and b.one_way_code <> 4
                 where b.link_id is null;
                 '''
        rows = self.GetRows(sqlcmd)
        if rows:
            self.log.warning('Contraints is error!')
            return -1
        return 0
        
    def _DoCheckLogic(self):       
        self.log.info('begin Check signpost_uc Logic..')
        sqlcmd = '''
                 select id
                 from signpost_uc_tbl
                 where inlinkid = outlinkid
                 
                 union
                 
                 select id 
                 from signpost_uc_tbl as a
                 left join link_tbl as b
                 on a.inlinkid = b.link_id and 
                    ((a.nodeid = b.s_node and b.one_way_code in (1,3)) or 
                    (a.nodeid = b.e_node and b.one_way_code in (1,2)))
                 where b.link_id is null;                 
                 '''
        rows = self.GetRows(sqlcmd)
        if rows:
            self.log.warning('CheckLogic is error!')
            return -1
        
        return 0

#######################################################################
# ## SignPostElement
#######################################################################
class SignPostElementMmi(object):
    def __init__(self, sign_id, folder):
        '''
        Constructor
        '''
        self._folder = folder
        self._exit_no = None  # 出口番号(MultLangName对象)        
        self.MAX_ROUTE_NO = 4  # 番号的最大数
        self._sign_id = sign_id
        self._dest_number = None
        self._signpost_name = None  # 方面名称, 一个方面可以有多个名称
        self._route_no = list()  # route番号()

    def set_exit_no(self, exit_no_obj):
        if exit_no_obj:
            self._exit_no = exit_no_obj
            
    def set_signpost_name(self, signpost_name):
        self._signpost_name = signpost_name
        
    def add_route_no(self, shield_obj):
        if shield_obj:
            self._route_no.append(shield_obj)
        
    def to_string(self):
        if self._folder:
            rst_str = '%s' % self._folder
        else:
            return None
        if self._sign_id:
            rst_str += '\t%d' % self._sign_id
        else:
            return None
        if self._signpost_name:
            json_sp_name = self._signpost_name.json_format_dump()
        else:
            json_sp_name = ''
        rst_str += '\t%s' % json_sp_name

        rn_cnt = 0
        while rn_cnt < len(self._route_no) and rn_cnt < self.MAX_ROUTE_NO:
            rst_str += '\t%s' % self._route_no[rn_cnt].json_format_dump()
            rn_cnt += 1

        while rn_cnt < self.MAX_ROUTE_NO:
            rst_str += '\t%s' % ''
            rn_cnt += 1
        # 加上出口番号
        if self._exit_no:
            json_exit_no = self._exit_no.json_format_dump()
            if not json_exit_no:
                json_exit_no = ''
        else:
            json_exit_no = ''
        rst_str += '\t%s' % json_exit_no
        return rst_str

    def is_empty(self):
        if(not self._signpost_name
           and not self._route_no
           and not self._exit_no):
            return True
        return False
    
