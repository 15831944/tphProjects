# -*- coding: UTF-8 -*-
'''
Created on 2014-3-27

@author: hongchenzai
'''
from component.jdb.hwy.hwy_def import IC_TYPE_RAMP
from component.jdb.hwy.hwy_def import IC_TYPE_IC
from component.jdb.hwy.hwy_def import IC_TYPE_SMART_IC
from component.jdb.hwy.hwy_def import IC_TYPE_SA
from component.jdb.hwy.hwy_def import IC_TYPE_PA
from component.jdb.hwy.hwy_def import IC_TYPE_JCT
from component.jdb.hwy.hwy_def import INOUT_TYPE_IN
from component.jdb.hwy.hwy_data_mng import HwyDataMng
from component.jdb.hwy.hwy_def import IC_TYPE_TRUE
from component.jdb.hwy.hwy_def import IC_DEFAULT_VAL
from component.jdb.hwy.hwy_def import ROAD_CODE_NO_TOLL
from component.jdb.hwy.hwy_graph import repeat_cycle
ROAD_CODE_DIFF = 400000  # 无料区间番号和有料番号的差值


#==============================================================================
# HwyFacilPathFactory
#==============================================================================
class HwyFacilPathFactory():

    def __init__(self):
        ''''''

    @staticmethod
    def create_facil_path(hwy, toll_flag, node_id,
                          road_code, road_point, updown,
                          facil_cls, inout, facil_name):
        ic_path = None
        if toll_flag:  # 有料
            if hwy.is_ic_in(facil_cls, inout):
                ic_path = HwyIcInPath(node_id, road_code, road_point,
                                      updown, facil_cls, inout,
                                      facil_name)
            elif hwy.is_ic_out(facil_cls, inout):
                ic_path = HwyIcOutPath(node_id, road_code, road_point,
                                       updown, facil_cls, inout,
                                       facil_name)
            elif hwy.is_sapa_out(facil_cls, inout):
                ic_path = HwySaPaOutPath(node_id, road_code, road_point,
                                         updown, facil_cls, inout,
                                         facil_name)
            elif hwy.is_sapa_in(facil_cls, inout):
                ic_path = HwySaPaInPath(node_id, road_code, road_point,
                                        updown, facil_cls, inout,
                                        facil_name)
            elif hwy.is_jct_out(facil_cls, inout):
                ic_path = HwyJCTPath(node_id, road_code, road_point,
                                     updown, facil_cls, inout,
                                     facil_name)
        else:  # 无料区间
            if hwy.is_ic_in(facil_cls, inout):
                ic_path = HwyIcInPathNoToll(node_id, road_code, road_point,
                                            updown, facil_cls, inout,
                                            facil_name)
            elif hwy.is_ic_out(facil_cls, inout):
                ic_path = HwyIcOutPathNoToll(node_id, road_code, road_point,
                                             updown, facil_cls, inout,
                                             facil_name)
            elif hwy.is_sapa_out(facil_cls, inout):
                ic_path = HwySaPaOutPathNoToll(node_id, road_code, road_point,
                                               updown, facil_cls, inout,
                                               facil_name)
            elif hwy.is_sapa_in(facil_cls, inout):
                ic_path = None
                #ic_path = HwySaPaInPathNoToll(node_id, road_code, road_point,
                #                              updown, facil_cls, inout,
                #                              facil_name, toll_flag)
            elif hwy.is_jct_out(facil_cls, inout):
                ic_path = HwyJCTPathNoToll(node_id, road_code, road_point,
                                           updown, facil_cls, inout,
                                           facil_name)
        return ic_path


#==============================================================================
# HwyFacilityPath
#==============================================================================
class HwyFacilityPath(object):
    '''
    classdocs
    '''

    def __init__(self, node_id, road_code, road_point,
                 updown, facilcls, inout,
                 facil_name, toll_flag=IC_TYPE_TRUE):
        '''
        Constructor
        '''
        self.node_id = node_id
        self.road_code = road_code
        self.road_point = road_point
        self.updown = updown
        self.facilcls = facilcls
        self.inout = inout
        self.facil_name = facil_name
        # {(path):(facil_info, path_type)}, path_type: UTURN, JCT, IC, SAPA
        self.pathes_dict = {}
        # self.uturn_pathes_dict = {}
        self.toll_flag = toll_flag
        hwy_data = HwyDataMng.instance()
        if toll_flag:
            self.G = hwy_data.get_graph()
        else:
            self.G = hwy_data.get_graph_no_toll()

    def cal_path(self):
        target_facils = self.get_target_facils()
        for target in target_facils:
            target_node = target.node_id
            pathes = self.get_paths_node_2_node(target_node)
            for path in pathes:
                # 设施所在位置到出入口(一般道)
                out_pathes = self.get_paths_2_inout(target_node)
                if out_pathes:
                    for out_path in out_pathes:
                        self._add_path([path + out_path[1:]], target, 'IC')
                else:
                    self._add_path([path], target, 'IC')
        if not target_facils:
            no_target_fac = ('八草東ＩＣ', '浜松いなさ北ＩＣ', '観音ランプ')  # 无设施所在位置的IC
            if self.facil_name not in no_target_fac:
                print ('No target node, node_id=%s, name=%s'
                       % (self.node_id, self.facil_name))
            # 设施到高速出入口(一般道)
            pathes = self.get_paths_2_inout(self.node_id)
            self._add_path(pathes, None, 'IC')
        # 其他路径(通常是高速到无料区间，这是数据没有提供设施所在点)
        self.cal_other_path()

    def cal_uturn_path(self):
        hwy_data = HwyDataMng.instance()
        # 取得对面的设施
        opp_facils = hwy_data.get_hwy_opposite_in_facils(self.road_code,
                                                         self.road_point,
                                                         self.updown)
        for opposite_facil in opp_facils:
            pathes = self.G.all_paths_uturn(self.node_id,
                                            opposite_facil.node_id)
            if pathes:
                # 求link最短的路径
                # pathes = sorted(pathes, key=len)
                # 求长度最短
                pathes = sorted(pathes, key=self.G.get_length_for_path)
                self._add_path(pathes[0:1], opposite_facil, 'UTURN')

    def get_paths_node_2_node(self, target):
        return []

    def get_paths_2_inout(self, source):
        return []

    def get_target_facils(self):
        hwy_data = HwyDataMng.instance()
        pos_infos = hwy_data.get_pos_infos(self.road_code,
                                           self.road_point,
                                           self.updown,
                                           self.facilcls,
                                           self.inout)
        return pos_infos

    def _add_path(self, pathes, target_facil_info, path_type):
        for path in pathes:
            t_path = tuple(path)
            if t_path not in self.pathes_dict:
                self.pathes_dict[t_path] = (target_facil_info, path_type)
            else:
                if self.pathes_dict[t_path] != (target_facil_info, path_type):
                    road_code = self.pathes_dict[t_path][0].road_code
                    road_point = self.pathes_dict[t_path][0].road_point
                    facilcls = self.pathes_dict[t_path][0].facilcls
                    if(not target_facil_info or
                       (road_code == target_facil_info.road_code and
                        road_point == target_facil_info.road_point and
                        facilcls == target_facil_info.road_point)
                       ):
                        print ('Path repeat for.'
                               'node_id=%s, path=%s, pathes_dict=%s'
                               % (self.node_id, path, self.pathes_dict))

    def get_inlinkid(self):
        inlinks = self.G.get_main_inlinkids(self.node_id)
        if inlinks:
            return inlinks[0]
        else:
            return None

    def get_outlinkid(self, t_nodeid):
        outlinks = self.G.get_main_outlinkids(t_nodeid)
        if outlinks:
            return outlinks[0]
        else:
            return None

    def get_path_linkids(self, path):
        return self.G.get_linkids(path)

    def _delete_same_roadcode_path(self, pathes, roadcode):
        rst_pathes = []
        for path in pathes:
            if not self._is_same_roadcode(path, roadcode):
                rst_pathes.append(path)
            else:
                jct = ('和田山ＪＣＴ',)  # 已知有本线JCT路径的JCT
#                 if self.facilcls == 3 and self.facil_name not in jct:  # JCT
#                     print 'Path is Same RoadCode for JCT. Path=%s' % path
        return rst_pathes

    def _is_same_roadcode(self, path, roadcode):
        '''整条path都在同一条高速(本线)上'''
        for u, v in zip(path[0:-1], path[1:]):
            data = self.G.get_edge_data(u, v)
            if data:
                if data.get('road_code') != roadcode:
                    return False
            else:
                print 'No Data for edge(%s, %s).' % (u, v)
                return False
        return True

    def cal_other_path(self):
        pass

    def _delete_repeat_path(self, other_pathes):
        temp_pathes = []
        for path in other_pathes:
            if tuple(path) not in self.pathes_dict:
                temp_pathes.append(path)
        rst_pathes = []
        for path in temp_pathes:
            last_node = path[-1]
            repeat_flag = False
            for key_path in self.pathes_dict.iterkeys():
                if last_node in key_path:
                    repeat_flag = True
                    continue
            if not repeat_flag:
                rst_pathes.append(path)
        return rst_pathes

    def _delete_exist_position_path(self, other_pathes):
        '''删除存设施所在位置的路径'''
        rst_pathes = []
        for path in other_pathes:
            position_flag = False
            for node in path[1:]:
                if self.G.is_ic_position(node):
                    position_flag = True
                    break
            if not position_flag:
                rst_pathes.append(path)
        return rst_pathes

    def _delete_repeat_cycle(self, pathes):
        '''点在无料区间本线上。'''
        rst_pathes = []
        for path in pathes:
            if repeat_cycle(path):
                rst_pathes.append(path)
        return rst_pathes

    def _is_on_no_toll_main_link(self, node_id):
        '''点在无料区间本线上。'''
        hwy_data = HwyDataMng.instance()
        no_toll_G = hwy_data.get_graph_no_toll()  # 无料区间
        if node_id not in no_toll_G:
            return False
        # inlink是有料高速本线
        for parent, node, data in no_toll_G.in_edges_iter(node_id, True):
            if data.get('road_code') > ROAD_CODE_NO_TOLL:
                return True
        for parent, node, data in no_toll_G.out_edges_iter(node_id, True):
            if data.get('road_code') > ROAD_CODE_NO_TOLL:
                return True
        return False


#==============================================================================
# HwyIcOutPath
#==============================================================================
class HwyIcOutPath(HwyFacilityPath):
    def __init__(self, node_id, road_code, road_point,
                 updown, facil_cls, inout, facil_name,
                 toll_flag=IC_TYPE_TRUE):
        '''
        Constructor
        '''
        HwyFacilityPath.__init__(self, node_id, road_code, road_point,
                                 updown, facil_cls, inout, facil_name,
                                 toll_flag)

    def get_paths_node_2_node(self, target):
        if self.node_id != target and target in self.G:
            pathes = self.G.all_paths_ic_out(self.node_id, target)
        else:
            pathes = self.G.all_paths_2_out(self.node_id)
        return pathes

    def get_paths_2_inout(self, source):
        return self.G.all_paths_2_out(source)

    def cal_other_path(self):
        '''计算直接到达其他非高速道(通常是到过高速无料区间)的路径 '''
        other_pathes = self.G.all_paths_2_out(self.node_id)
        # 删除重复的路径（主要是和前方设施路径相同的）
        other_pathes = self._delete_repeat_path(other_pathes)
        other_pathes = self._delete_exist_position_path(other_pathes)
        for path in other_pathes:
            last_node = path[-1]
            # 判断最后一个落在高速无料区间本线上
            if not self._is_on_no_toll_main_link(last_node):
                print ('last Node dose not in no_toll highway main link. '
                       'node_id=%s, name=%s' % (self.node_id, self.facil_name))
            else:
                pass
                # print ('last Node is in no_toll highway main link. '
                #       'node_id=%s, name=%s, last_node=%s'
                #       % (self.node_id, self.facil_name, last_node))
            self._add_path([path], None, 'IC')


#==============================================================================
# HwyIcInPath
#==============================================================================
class HwyIcInPath(HwyFacilityPath):
    def __init__(self, node_id, road_code, road_point,
                 updown, facil_cls, inout, facil_name,
                 toll_flag=IC_TYPE_TRUE):
        '''
        Constructor
        '''
        HwyFacilityPath.__init__(self, node_id, road_code, road_point,
                                 updown, facil_cls, inout, facil_name,
                                 toll_flag)

    def get_paths_node_2_node(self, target):
        if self.node_id != target and target in self.G:
            pathes = self.G.all_paths_ic_in(self.node_id, target)
        else:
            pathes = self.G.all_paths_2_in(self.node_id)
        return pathes

    def get_paths_2_inout(self, source):
        return self.G.all_paths_2_in(source)

    def get_path_linkids(self, path):
        temp_path = list(path)
        temp_path.reverse()
        link_lid = self.G.get_linkids(temp_path)
        return link_lid[::-1]

    def cal_uturn_path(self):
        return

    def cal_other_path(self):
        '''计算直接到达其他非高速道(通常是到过高速无料区间)的路径 '''
        other_pathes = self.G.all_paths_2_in(self.node_id)
        # 删除重复的路径（主要是和前方设施路径相同的）
        other_pathes = self._delete_repeat_path(other_pathes)
        other_pathes = self._delete_exist_position_path(other_pathes)
        for path in other_pathes:
            last_node = path[-1]
            # 判断最后一个落在高速无料区间本线上
            if not self._is_on_no_toll_main_link(last_node):
                print ('last Node dose not in no_toll highway main link.'
                       'node_id=%s, name=%s' % (self.node_id, self.facil_name))
            self._add_path([path], None, 'IC')


#==============================================================================
# HwyIcJCTPath
#==============================================================================
class HwyJCTPath(HwyFacilityPath):
    def __init__(self, node_id, road_code, road_point,
                 updown, facil_cls, inout, facil_name,
                 toll_flag=IC_TYPE_TRUE):
        '''
        Constructor
        '''
        HwyFacilityPath.__init__(self, node_id, road_code, road_point,
                                 updown, facil_cls, inout, facil_name,
                                 toll_flag)

    def cal_path(self):
        hwy_data = HwyDataMng.instance()
        target_facils = hwy_data.get_hwy_forward_branches(self.node_id,
                                                          self.road_code,
                                                          self.road_point,
                                                          self.updown,
                                                          self.inout)
        pathes = []
        if target_facils:
            for target in target_facils:
#                 if (target.facil_name != self.facil_name and
#                     target.facilcls == self.facilcls):
#                     print ('JCT Name different, node=%s,name=%s,'
#                            'target_node=%s, target_name=%s'
#                            % (self.node_id, self.facil_name,
#                               target.node_id, target.facil_name))
                target_node = target.node_id
                pathes = self.G.all_paths_jct(self.node_id, target_node)
                pathes = self._delete_repeat_cycle(pathes)
                if pathes:
                    # pathes = sorted(pathes, key=len)
                    self._add_path(pathes, target, 'JCT')
        if not target_facils:
            print ('No Branch for node%s' % self.node_id)
        # 讲算其他JCT路径
        # 数据中存前方JCT给少的情况，如[鳥栖ＪＣＴ，road_code=204020,road_seq=180]
        self.cal_other_path()

    def cal_other_path(self):
        '''计算直接到达其他JCT入口(不在前方设施里)的路径 '''
        if self._exist_same_jct():  # 当前JCT存在并列的JCT设施
            return
        other_pathes = self.G.all_paths_2_jct(self.node_id)
        # 删除重复的路径（主要是和前方设施路径相同的）
        other_pathes = self._delete_repeat_to_node(other_pathes)
        # 删除本线删路径
        other_pathes = self._delete_same_roadcode_path(other_pathes,
                                                       self.road_code)
        for path in other_pathes:
            to_node = path[-1]
            facil_info = self._get_other_facil_info(to_node)
            if facil_info:
                if(self.road_code == facil_info.road_code):
                    continue
                self._add_path([path], facil_info, 'JCT')
#                 print ('存在其他JCT.node_id=%s,name=%s,path=%s'
#                        % (self.node_id, facil_info.facil_name, path))

    def _exist_same_jct(self):
        '''当前JCT存在并列的JCT设施'''
        jct_num = 0
        hwy_data = HwyDataMng.instance()
        facil_infos = hwy_data.get_hwy_facils_by_nodeid(self.node_id)
        for facil_info in facil_infos:
            if facil_info.facilcls == IC_TYPE_JCT:
                jct_num += 1
        if jct_num > 1:
            return True
        else:
            return False

    def _delete_repeat_to_node(self, other_pathes):
        '''删除重复的路径'''
        rst_pathes = []
        for path in other_pathes:
            to_node = path[-1]
            del_flag = False
            for key in self.pathes_dict.iterkeys():
                if to_node in key:
                    del_flag = True
                    break
            if not del_flag:
                rst_pathes.append(path)
        return rst_pathes

    def _get_other_facil_info(self, to_node):
        '''获取其他设施的设施情报。to_node是JCT or SAPA的入口'''
        rst_facil_infos = []
        hwy_data = HwyDataMng.instance()
        facil_infos = hwy_data.get_hwy_facils_by_nodeid(to_node)
        for facil_info in facil_infos:
            if(facil_info.facilcls == IC_TYPE_JCT
               and facil_info.inout == INOUT_TYPE_IN):
                rst_facil_infos.append(facil_info)
        if rst_facil_infos:
            if len(rst_facil_infos) > 1:
                print 'Other than One JCT.'
                exit(1)
            return rst_facil_infos[0]
        # ## SAPA
        for facil_info in facil_infos:
            if(facil_info.facilcls in (IC_TYPE_SA, IC_TYPE_PA)
               and facil_info.inout == INOUT_TYPE_IN):
                return facil_info


#==============================================================================
# HwyIcSaPaOut
#==============================================================================
class HwySaPaOutPath(HwyFacilityPath):
    def __init__(self, node_id, road_code, road_point,
                 updown, facil_cls, inout, facil_name,
                 toll_flag=IC_TYPE_TRUE):
        '''
        Constructor
        '''
        HwyFacilityPath.__init__(self, node_id, road_code, road_point,
                                 updown, facil_cls, inout, facil_name,
                                 toll_flag)
        # self.jct_path_dict = {}  # SA出口到JCT入口：发生在SA和JCT并列，且点不同
        # self.ic_path_dict = {}  # SA出口到高速出入口(一般道): 发生SA和IC并列，且点不同

    def cal_path(self):
        hwy_data = HwyDataMng.instance()
        target_facils = self.get_target_facils()
        pathes = []
        if target_facils:
            for target in target_facils:
                target_node = target.node_id
                pathes = self.G.all_paths_sapa(self.node_id, target_node)
                pathes = self._delete_same_roadcode_path(pathes,
                                                         self.road_code)
                self._add_path(pathes, target, 'SAPA')
        else:
            # print 'No target node, node_id=%s' % self.node_id
            pathes = self.G.all_paths_2_sapa_in(self.node_id)
            for path in pathes:
                target_node = path[-1]
                facils = hwy_data.get_hwy_facils_by_nodeid(target_node,
                                                           self.facilcls)
                # 同侧(同上下行)， 同road_code
                for facil_info in facils:
                    if (self.road_code == facil_info.road_code and
                        self.facilcls == facil_info.facilcls and
                        self.updown == facil_info.updown):
                        temp_pathes = self._delete_same_roadcode_path([path],
                                                              self.road_code)
                        self._add_path(temp_pathes, facil_info, 'SAPA')
        self.cal_jct_path()
        self.cal_ic_path()

    def cal_jct_path(self):
        hwy_data = HwyDataMng.instance()
        target_facils = hwy_data.get_hwy_forward_branches(self.node_id,
                                                          self.road_code,
                                                          self.road_point,
                                                          self.updown,
                                                          self.inout)
        pathes = []
        if target_facils:
            for target in target_facils:
                target_node = target.node_id
                pathes = self.G.all_paths_jct(self.node_id, target_node)
                if pathes:
                    # 按node数据从少到多排序
                    # pathes = sorted(pathes, key=len)
                    self._add_path(pathes, target, 'JCT')
                    jct_path_sapa = ('草津ＰＡ', )  # 已知的有JCT分歧路径的SAPA
                    if self.facil_name not in jct_path_sapa:
                        print ('SaPa have JCT Out. node=%s, name=%s'
                               % (self.node_id, self.facil_name))
        else:
            return

    def cal_ic_path(self):
        '''SA出口到IC所在位置'''
        hwy_data = HwyDataMng.instance()
        same_facils = hwy_data.get_same_facil(self.road_code, self.road_point,
                                              self.updown, self.inout)
        for same in same_facils:
            ic_types = (IC_TYPE_IC, IC_TYPE_SMART_IC, IC_TYPE_RAMP)
            if(same.facilcls not in ic_types
               or same.node_id == self.node_id
               or same.inout != self.inout):
                continue
            hwy_ic = HwyIcOutPath(self.node_id, same.road_code,
                                  same.road_point, same.updown,
                                  same.facilcls, same.inout,
                                  same.facil_name)
            same_path = []
            has_sapa_ic = False
            hwy_ic.cal_path()
            for path, val in hwy_ic.pathes_dict.iteritems():
                target_facil_info, path_type = val
                if target_facil_info:
                    target_node = target_facil_info.node_id
                else:
                    target_node = path[-1]
                same_path = self.G.all_paths_ic_out(same.node_id, target_node)
                # IC出口到IC所在位置，必须是通的
                if same_path:
                    self._add_path([path], target_facil_info, path_type)
                    has_sapa_ic = True
            if has_sapa_ic:
                ic_path_sapa = ('亀山ＰＡ',)  # 有IC路径的SAPA
                if self.facil_name not in ic_path_sapa:
                    print ('SaPa have IC Out. node=%s, name=%s'
                           % (self.node_id, self.facil_name))

    def get_target_facils(self):
        hwy_data = HwyDataMng.instance()
        targets = hwy_data.get_hwy_in_facils(self.road_code,
                                             self.road_point,
                                             self.updown,
                                             self.inout)
        return targets


#==============================================================================
# HwyIcSaPaIn
#==============================================================================
class HwySaPaInPath(HwySaPaOutPath):
    def __init__(self, node_id, road_code, road_point,
                 updown, facil_cls, inout, facil_name,
                 toll_flag=IC_TYPE_TRUE):
        '''
        Constructor
        '''
        HwySaPaOutPath.__init__(self, node_id, road_code, road_point,
                                updown, facil_cls, inout, facil_name,
                                toll_flag)

    def cal_path(self):
        self.cal_ic_path()
        return

    def cal_ic_path(self):
        '''SA入口到IC所在位置'''
        hwy_data = HwyDataMng.instance()
        same_facils = hwy_data.get_same_facil(self.road_code, self.road_point,
                                              self.updown, self.inout)
        for same in same_facils:
            ic_types = (IC_TYPE_IC, IC_TYPE_SMART_IC, IC_TYPE_RAMP)
            if(same.facilcls not in ic_types
               or same.node_id == self.node_id
               or same.inout != self.inout):
                continue
            hwy_ic = HwyIcInPath(self.node_id, same.road_code,
                                 same.road_point, same.updown,
                                 same.facilcls, same.inout,
                                 same.facil_name)
            hwy_ic.cal_path()
            same_path = []
            has_sapa_ic = False
            for path, val in hwy_ic.pathes_dict.iteritems():
                target_facil_info, path_type = val
                if target_facil_info:
                    target_node = target_facil_info.node_id
                else:
                    target_node = path[-1]
                same_path = self.G.all_paths_ic_in(same.node_id,
                                                   target_node)
                # IC入口到IC所在位置，必须是通的
                # case: road_code=301060, road_seq=30, direction_c=2
                if same_path:
                    self._add_path([path], target_facil_info, path_type)
                    has_sapa_ic = True
            if has_sapa_ic:
                ic_path_sapa = ('亀山ＰＡ',)
                if self.facil_name not in ic_path_sapa:
                    print ('SaPa have IC In. node=%s, name=%s'
                           % (self.node_id, self.facil_name))

    def cal_uturn_path(self):
        return

    def get_path_linkids(self, path):
        temp_path = list(path)
        temp_path.reverse()
        link_lid = self.G.get_linkids(temp_path)
        return link_lid[::-1]


#==============================================================================
# 无料区间设施路径
#==============================================================================

#==============================================================================
# 无料区间IC出口
#==============================================================================
class HwyIcOutPathNoToll(HwyIcOutPath):
    def __init__(self, node_id, road_code, road_point,
                 updown, facil_cls, inout, facil_name):
        '''
        Constructor
        '''
        HwyIcOutPath.__init__(self, node_id, road_code, road_point,
                              updown, facil_cls, inout, facil_name,
                              IC_DEFAULT_VAL)

    def cal_path(self):
        target_facils = self.get_target_facils()
        hwy_data = HwyDataMng.instance()
        for target in target_facils:
            target_node = target.node_id
            hwy_data.load_link_by_expand_box(self.G, self.node_id, target_node)
            pathes = []
            if self.node_id != target_node:
                pathes = self.G.all_paths_ic_out(self.node_id, target_node)
            else:
                print 'node_id == target_node. node_id=%s' % self.node_id
            for path in pathes:
                self._add_path([path], target, 'IC')

    def cal_uturn_path(self):
        return


#==============================================================================
# 无料区间IC入口
#==============================================================================
class HwyIcInPathNoToll(HwyIcInPath):
    def __init__(self, node_id, road_code, road_point,
                 updown, facil_cls, inout, facil_name):
        '''
        Constructor
        '''
        HwyIcInPath.__init__(self, node_id, road_code, road_point,
                             updown, facil_cls, inout, facil_name,
                             IC_DEFAULT_VAL)

    def cal_path(self):
        target_facils = self.get_target_facils()
        hwy_data = HwyDataMng.instance()
        for target in target_facils:
            target_node = target.node_id
            hwy_data.load_link_by_expand_box(self.G, self.node_id, target_node)
            pathes = []
            if self.node_id != target_node:
                pathes = self.G.all_paths_ic_in(self.node_id, target_node)
            else:
                print 'node_id == target_node. node_id=%s' % self.node_id
            for path in pathes:
                self._add_path([path], target, 'IC')

    def cal_uturn_path(self):
        return


#==============================================================================
# 无料JCT
#==============================================================================
class HwyJCTPathNoToll(HwyJCTPath):
    def __init__(self, node_id, road_code, road_point,
                 updown, facil_cls, inout, facil_name):
        '''
        Constructor
        '''
        HwyJCTPath.__init__(self, node_id, road_code, road_point,
                            updown, facil_cls, inout, facil_name,
                            IC_DEFAULT_VAL)

    def cal_path(self):
        hwy_data = HwyDataMng.instance()
        target_facils = hwy_data.get_hwy_forward_branches(self.node_id,
                                                          self.road_code,
                                                          self.road_point,
                                                          self.updown,
                                                          self.inout)
        pathes = []
        if target_facils:
            for target in target_facils:
                target_node = target.node_id
                hwy_data.load_link_by_expand_box(self.G,
                                                 self.node_id,
                                                 target_node)
                pathes = self.G.all_paths_jct(self.node_id, target_node)
                if pathes:
                    self._add_path(pathes, target, 'JCT')
                    hwy_data = HwyDataMng.instance()
                    hwy_graph = hwy_data.get_graph()
                    # 判断目标点落在高速上
                    if target_node in hwy_graph:
                        print ('Target JCT is on HWY. node_id=%s,name=%s'
                               % (target_node, target.facil_name))
        # 讲算其他JCT路径
        self.cal_other_path()
        # ## 无branches情报, 如: 和田山ＪＣＴ
#         if not target_facils:
#             no_branch_jct = ('和田山ＪＣＴ',)  # 已知无分歧情报JCT
#             if self.facil_name not in no_branch_jct:
#                 print ('No Branch for node=%s,name=%s'
#                        % (self.node_id, self.facil_name))
#             hwy_data.load_link_by_expand_box(self.G,
#                                              self.node_id,
#                                              self.node_id)
#             pathes = self.G.all_paths_2_jct(self.node_id)
#             # 删除本线删路径
#             pathes = self._delete_same_roadcode_path(pathes,
#                                                      self.road_code)
#             if pathes:
#                 self._add_path(pathes, None, 'JCT')

    def cal_uturn_path(self):
        return

    def _is_same_roadcode(self, path, roadcode):
        '''整条path都在同一条高速(本线)上'''
        for u, v in zip(path[0:-1], path[1:]):
            data = self.G.get_edge_data(u, v)
            if data:
                temp_road_code = data.get('road_code')
                if temp_road_code != (roadcode + ROAD_CODE_DIFF):
                    return False
                if temp_road_code == roadcode:
                    print data.get('link_id')
            else:
                print 'No Data for edge(%s, %s).' % (u, v)
                return False
        return True


#==============================================================================
# 无料区间SAPA
#==============================================================================
class HwySaPaOutPathNoToll(HwySaPaOutPath):
    def __init__(self, node_id, road_code, road_point,
                 updown, facil_cls, inout, facil_name):
        '''
        Constructor
        '''
        HwySaPaOutPath.__init__(self, node_id, road_code, road_point,
                                updown, facil_cls, inout, facil_name,
                                IC_DEFAULT_VAL)

    def cal_path(self):
        hwy_data = HwyDataMng.instance()
        target_facils = self.get_target_facils()
        pathes = []
        if target_facils:
            for target in target_facils:
                target_node = target.node_id
                hwy_data.load_link_by_expand_box(self.G,
                                                 self.node_id,
                                                 target_node)
                pathes = self.G.all_paths_sapa(self.node_id, target_node)
                # 删除本线删路径
                pathes = self._delete_same_roadcode_path(pathes,
                                                         self.road_code)
                self._add_path(pathes, target, 'SAPA')
                hwy_data = HwyDataMng.instance()
                hwy_graph = hwy_data.get_graph()
                # 目标点落在高速上
                if target_node in hwy_graph:
                    print ('Target SAPA is on HWY. node_id=%s,name=%s'
                           % (target_node, target.facil_name))
        else:
            print 'No target node, node_id=%s' % self.node_id

    def _is_same_roadcode(self, path, roadcode):
        '''整条path都在同一条高速(本线)上'''
        for u, v in zip(path[0:-1], path[1:]):
            data = self.G.get_edge_data(u, v)
            if data:
                temp_road_code = data.get('road_code')
                if temp_road_code != (roadcode + ROAD_CODE_DIFF):
                    return False
                if temp_road_code == roadcode:
                    print data.get('link_id')
            else:
                print 'No Data for edge(%s, %s).' % (u, v)
                return False
        return True

    def cal_uturn_path(self):
        return
