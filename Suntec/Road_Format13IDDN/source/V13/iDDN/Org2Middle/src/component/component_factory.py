# -*- coding: cp936 -*-
'''
Created on 2012-3-22

@author: hongchenzai
'''
import base
import guideinfo_lane
import guideinfo_towardname
import guideinfo_signpost
import guideinfo_spotguide
import guideinfo_caution
import guideinfo_forceguide
import guideinfo_signpost_uc
import dictionary
import link
import jpn
import chn
import ramp_roadtype
import link_split
import link_merge
import node_move
import admin
import vics
import shield
import link_overlay_or_circle
import highway
import jdb
import import_sidefiles

from rdf import dictionary_rdf, linkname_rdf, guideinfo_lane_rdf, \
               guideinfo_towardname_rdf, link_rdf, node_rdf, rawdata_rdf, \
               regulation_rdf, linkname_rdf, trans_geom_rdf, \
               trans_srid_rdf, guideinfo_spotguide_rdf, admin_rdf, shield_rdf,park_rdf

from axf import regulation_axf, dictionary_axf, linkname_axf, guideinfo_spotguide_axf, \
               guideinfo_towardname_axf, guideinfo_signpost_axf, guideinfo_lane_axf, \
               link_axf, node_axf, newid_axf, admin_axf, shield_axf, park_axf,\
               highway_axf

from ta import dictionary_ta, linkname_ta, regulation_ta, \
               link_ta, node_ta, guideinfo_lane_ta, shield_ta, \
               guideinfo_towardname_ta, guideinfo_spotguide_ta, \
               guideinfo_signpost_uc_ta, admin_ta, import_jvfiles_ta,park_ta
from component.nostra import guideinfo_lane_nostra, guideinfo_spotguide_nostra
from component.rdf import import_csv_data_rdf
from component.msm import guideinfo_lane_msm, guideinfo_spotguide_msm, \
    guideinfo_forceguide_msm
from component.jdb.guideinfo_lane_arrow_optimize_jdb import comp_guideinfo_lane_arrow_optimize_jdb


########################################################################
# #  class comp_factory
########################################################################
class comp_factory(base.comp_factory_base):
    def __init__(self):
        '''
        Constructor
        '''

    def CreateOneComponent(self, comp_name):
        if comp_name == 'Guideinfo_Lane':
            return guideinfo_lane.comp_guideinfo_lane()

        if comp_name == 'Link':
            return  link.comp_link()

        return None

########################################################################
# #  class comp_factory_chn
########################################################################
class comp_factory_chn(comp_factory):
    def __init__(self):
        '''
        Constructor
        '''

    def CreateOneComponent(self, comp_name):
        if comp_name == 'Guideinfo_Lane':
            return chn.guideinfo_lane_chn.comp_guideinfo_lane_chn()

        return None

########################################################################
# #  class comp_factory_jpn
########################################################################
class comp_factory_jpn(comp_factory):
    def __init__(self):
        '''
        Constructor
        '''

    def CreateOneComponent(self, comp_name):
        if comp_name == 'forceguide_patch':
            return jpn.comp_import_forceguide_patch_jpn()

        if comp_name == 'sidefiles':
            return jpn.comp_import_sidefiles_jpn()

        if comp_name == 'Guideinfo_Lane':
            return jpn.comp_guideinfo_lane_jpn()

        if comp_name == 'Guideinfo_TowardName':
            return jpn.comp_guideinfo_towardname_jpn()
        
        if comp_name == 'park':
            return jpn.comp_park_jpn()

        if comp_name == 'Link':
            return jpn.comp_link_jpn()

        if comp_name == 'Split_Link':
            return jpn.comp_split_link_jpn()

        if comp_name == 'Trans_SRID':
            return jpn.comp_trans_srid_jpn()

        if comp_name == 'Node':
            return jpn.comp_node_jpn()

        if comp_name == 'Dictionary':
            return jpn.comp_dictionary_jpn()

        if comp_name == 'Regulation':
            return jpn.comp_regulation_jpn()

        if comp_name == 'Guideinfo_SignPost':
            return jpn.comp_guideinfo_signpost_jpn()

        if comp_name == 'Tile':
            return jpn.comp_tile_jpn()

        if comp_name == 'Guideinfo_ForceGuide':
            return jpn.com_guideinfo_forceguide_jpn()

        if comp_name == 'Guideinfo_SpotGuide':
            return jpn.comp_guideinfo_spotguide_jpn()

        if comp_name == 'Guideinfo_Etc':
            return jpn.comp_guideinfo_etc_jpn()

        if comp_name == 'Link_Name':
            return jpn.comp_linkname_jpn()

        if comp_name == 'Toll':
            return jpn.comp_node_toll_jpn()

        if comp_name == 'Admin':
            return jpn.comp_admin_jpn()

        if comp_name == 'Guideinfo_Caution':
            return jpn.comp_guideinfo_caution_jpn()

        if comp_name == 'Shield':
            return jpn.comp_shield_jpn()
        if comp_name == 'Ramp':
            return ramp_roadtype.comp_ramp_roadtype()

        if comp_name == 'Link_Split':
            return link_split.comp_link_split()

        if comp_name == 'Link_Merge':
            return link_merge.comp_link_merge()

        if comp_name == 'Node_Move':
            return node_move.comp_node_move()

        if comp_name == 'Tmc':
            return jpn.comp_vics_jpn()

        if comp_name == 'guideinfo_signpost_uc':
            return guideinfo_signpost_uc.comp_guideinfo_signpost_uc()
        if comp_name == 'fac_name':
            return jpn.comp_fac_name()

        if comp_name == 'Highway':
            return highway.comp_highway()

        if comp_name == 'MainNode':
            import component.mainnode
            return component.mainnode.comp_mainnode()
        return None

########################################################################
# #  class comp_factory_uc: 北美
########################################################################
class comp_factory_rdf(comp_factory):
    def __init__(self):
        '''北美
        '''

    def CreateOneComponent(self, comp_name):
        if comp_name == 'ImportJCV':
            return import_csv_data_rdf.import_junctionview_data_rdf()
            pass
        if comp_name == 'ImportCSV':
            return import_sidefiles.import_csv_data()
            pass        
        if comp_name == 'Guideinfo_Lane':
            return guideinfo_lane_rdf.comp_guideinfo_lane_rdf()
            pass
        if comp_name == 'Guideinfo_TowardName':
            # return guideinfo_towardname_rdf.comp_guideinfo_towardname_rdf()
            import component.guideinfo_towardname
            return component.guideinfo_towardname.comp_guideinfo_towardname()
            pass
        if comp_name == 'rawdata':
            return rawdata_rdf.comp_rawdata_rdf()
            pass
        if comp_name == 'park':
            return park_rdf.comp_park_rdf()
            pass
        if comp_name == 'Link':
            return link_rdf.comp_link_rdf()
            pass
        if comp_name == 'Node':
            return node_rdf.comp_node_rdf()
            pass
        if comp_name == 'Dictionary':
            return dictionary_rdf.comp_dictionary_rdf()
            pass
        if comp_name == 'Regulation':
            return regulation_rdf.comp_regulation_rdf()
        if comp_name == 'Guideinfo_SignPost':
            return guideinfo_signpost.comp_guideinfo_signpost()
            pass
        if comp_name == 'Guideinfo_SpotGuide':
            return guideinfo_spotguide_rdf.comp_guideinfo_spotguide_rdf()
            pass
        if comp_name == 'Link_Name':
            return linkname_rdf.comp_linkname_rdf()
        if comp_name == 'Trans_Geom':
            return trans_geom_rdf.comp_trans_geom_rdf()
        if comp_name == 'trans_Srid':
            return trans_srid_rdf.comp_trans_srid_rdf()
        if comp_name == 'Admin':
            return admin_rdf.comp_admin_rdf()
        if comp_name == 'Guideinfo_Caution':
            return guideinfo_caution.comp_guideinfo_caution()

        if comp_name == 'Ramp':
            return ramp_roadtype.comp_ramp_roadtype()
        if comp_name == 'Tmc':
            return vics.comp_vics()
        if comp_name == 'Link_Split':
            return link_split.comp_link_split()
        if comp_name == 'Link_Merge':
            return link_merge.comp_link_merge()
        if comp_name == 'Node_Move':
            return node_move.comp_node_move()
        if comp_name == 'guideinfo_signpost_uc':
            from component.rdf import guideinfo_singpost_uc_rdf
            return guideinfo_singpost_uc_rdf.comp_guideinfo_signpost_uc_rdf()
        if comp_name == 'Guideinfo_ForceGuide':
            from component.rdf import guideinfo_forceguide_rdf
            return guideinfo_forceguide_rdf.comp_guideinfo_forceguide_rdf()
        if comp_name == 'Shield':
            return shield_rdf.comp_shield_rdf()
        if comp_name == 'Highway':
            return highway.comp_highway()
        if comp_name == 'CrossName':
            import component.guideinfo_crossname
            return component.guideinfo_crossname.comp_guideinfo_crossname()

        if comp_name == 'MainNode':
            import component.mainnode
            return component.mainnode.comp_mainnode()
        return None

########################################################################
# #  class comp_factory_axf: 高德数据
########################################################################
class comp_factory_axf(comp_factory):
    def __init__(self):
        '''高德
        '''

    def CreateOneComponent(self, comp_name):
        if comp_name == 'Guideinfo_Lane':
            return guideinfo_lane_axf.comp_guideinfo_lane_axf()
        if comp_name == 'Guideinfo_TowardName':
            return guideinfo_towardname_axf.comp_guideinfo_towardname_axf()
        if comp_name == 'park':
            return park_axf.comp_park_axf()
        if comp_name == 'Link':
            return link_axf.comp_link_axf()
        if comp_name == 'Node':
            return node_axf.comp_node_axf()
        if comp_name == 'Dictionary':
            return dictionary_axf.comp_dictionary_axf()
        if comp_name == 'Regulation':
            return regulation_axf.comp_regulation_axf()
        if comp_name == 'Guideinfo_SignPost':
            return guideinfo_signpost_axf.comp_guideinfo_signpost_axf()
        if comp_name == 'Guideinfo_ForceGuide':
            return guideinfo_forceguide.com_guideinfo_forceguide()
        if comp_name == 'Guideinfo_SpotGuide':
            return guideinfo_spotguide_axf.comp_guideinfo_spotguide_axf()
        if comp_name == 'Link_Name':
            return linkname_axf.comp_linkname_axf()
        if comp_name == 'NewID':
            return newid_axf.comp_newid_axf()
        if comp_name == 'Admin':
            return admin_axf.comp_admin_axf()
        if comp_name == 'Guideinfo_Caution':
            return guideinfo_caution.comp_guideinfo_caution()

        if comp_name == 'Ramp':
            return ramp_roadtype.comp_ramp_roadtype()
        if comp_name == 'Tmc':
            return vics.comp_vics()
        if comp_name == 'Link_Split':
            return link_split.comp_link_split()
        if comp_name == 'Link_Merge':
            return link_merge.comp_link_merge()
        if comp_name == 'Node_Move':
            return node_move.comp_node_move()
        if comp_name == 'guideinfo_signpost_uc':
            return guideinfo_signpost_uc.comp_guideinfo_signpost_uc()

        if comp_name == 'Shield':
            return shield_axf.comp_shield_axf()

        if comp_name == 'Highway':
            return highway_axf.comp_highway_axf()
        if comp_name == 'CrossName':
            import component.guideinfo_crossname
            return component.guideinfo_crossname.comp_guideinfo_crossname()
        return None

########################################################################
# #  class comp_factory_ta: TomTom数据
########################################################################
class comp_factory_ta(comp_factory):
    def __init__(self):
        '''TomTom
        '''

    def CreateOneComponent(self, comp_name):
        if comp_name == 'sidefiles':
            return import_jvfiles_ta.comp_import_sidefiles_uc()
            pass
        if comp_name == 'Sidefiles_Phoneme':
            from component.ta import import_phoneme_ta
            return import_phoneme_ta.comp_import_phoneme_ta()
        if comp_name == 'Guideinfo_Lane':
            return guideinfo_lane_ta.comp_guideinfo_lane_ta()
            pass
        if comp_name == 'Guideinfo_TowardName':
            from component.guideinfo_towardname import comp_guideinfo_towardname
            return comp_guideinfo_towardname()
            pass
        if comp_name == 'park':
            return park_ta.comp_park_ta()
            pass
        if comp_name == 'Link':
            return link_ta.comp_link_ta()
            pass
        if comp_name == 'Node':
            return node_ta.comp_node_ta()
            pass
        if comp_name == 'Dictionary':
            return dictionary_ta.comp_dictionary_ta()
            pass
        if comp_name == 'Regulation':
            return regulation_ta.comp_regulation_ta()
            pass
        if comp_name == 'Guideinfo_SignPost':
            return guideinfo_signpost.comp_guideinfo_signpost()
            pass
        if comp_name == 'Guideinfo_SpotGuide':
            return guideinfo_spotguide_ta.comp_guideinfo_spotguide_ta()
            pass
        if comp_name == 'Link_Name':
            return linkname_ta.comp_linkname_ta()
            pass
        if comp_name == 'Shield':
            return shield_ta.comp_shield_ta()
        if comp_name == 'Admin':
            return admin_ta.comp_admin_ta()
        if comp_name == 'Guideinfo_Caution':
            return guideinfo_caution.comp_guideinfo_caution()

        if comp_name == 'guideinfo_signpost_uc':
            return guideinfo_signpost_uc_ta.comp_guideinfo_signpost_uc_ta()

        if comp_name == 'Guideinfo_ForceGuide':
            return guideinfo_forceguide.com_guideinfo_forceguide()

        if comp_name == 'Ramp':
            return ramp_roadtype.comp_ramp_roadtype()
        if comp_name == 'Tmc':
            return vics.comp_vics()
        if comp_name == 'Link_Split':
            return link_split.comp_link_split()
        if comp_name == 'Link_Merge':
            return link_merge.comp_link_merge()
        if comp_name == 'Node_Move':
            return node_move.comp_node_move()
        if comp_name == 'Link_overlay_or_circle':
            return link_overlay_or_circle.comp_link_dupli_or_circle()

        if comp_name == 'Highway':
            return highway.comp_highway()
        if comp_name == 'CrossName':
            import component.guideinfo_crossname
            return component.guideinfo_crossname.comp_guideinfo_crossname()

        if comp_name == 'MainNode':
            import component.mainnode
            return component.mainnode.comp_mainnode()
        return None

########################################################################
# #  class comp_factory_jdb.
########################################################################
class comp_factory_jdb(comp_factory):
    def __init__(self):
        '''
        deal with iPC RDB Data.
        '''

    def CreateOneComponent(self, comp_name):
        if comp_name == 'PreProcess':
            import component.jdb.preprocess
            return component.jdb.preprocess.comp_proprocess_jdb()
            pass

        if comp_name == 'sidefiles':
            import component.jdb.import_sidefiles
            return component.jdb.import_sidefiles.comp_import_sidefiles_jdb()
            pass

        if comp_name == 'Guideinfo_Lane':
            import component.jdb.guideinfo_lane_jdb
            return component.jdb.guideinfo_lane_jdb.comp_guideinfo_lane_jdb()
            pass

        if comp_name == 'Guideinfo_TowardName':
            import component.jdb.guideinfo_towardname_jdb
            return component.jdb.guideinfo_towardname_jdb.comp_guideinfo_towardname_jdb()
            pass

        if comp_name == 'Link':
            import component.jdb.link_jdb
            return component.jdb.link_jdb.comp_link_jdb()
            pass
        
        if comp_name == 'arrow_optimize':
            return comp_guideinfo_lane_arrow_optimize_jdb()
            pass
        
        if comp_name == 'Split_Link':
            import component.jdb.split_link
            return component.jdb.split_link.comp_split_link_jdb()
            pass

        if comp_name == 'Trans_SRID':
            return jdb.comp_trans_srid_jdb()
            pass

        if comp_name == 'Node':
            import component.jdb.node_jdb
            return component.jdb.node_jdb.comp_node_jdb()
            pass

        if comp_name == 'Dictionary':
            import component.dictionary
            return component.dictionary.comp_dictionary()

        if comp_name == 'Regulation':
            import component.jdb.regulation
            return component.jdb.regulation.comp_regulation_jdb()
            pass

        if comp_name == 'Guideinfo_SignPost':
            import component.jdb.guideinfo_signpost_jdb
            return component.jdb.guideinfo_signpost_jdb.comp_guideinfo_signpost_jdb()
            pass

        if comp_name == 'Tile':
            import component.jdb.tile_jdb
            return component.jdb.tile_jdb.comp_tile_jdb()
            pass

#         if comp_name == 'forceguide_patch':
#             import component.jdb.import_forceguide_patch
#             return component.jdb.import_forceguide_patch.comp_import_forceguide_patch_jdb()
#             pass
        if comp_name == 'Guideinfo_ForceGuide':
            import component.jdb.guideinfo_forceguide_jpn_jdb
            return component.jdb.guideinfo_forceguide_jpn_jdb.com_guideinfo_forceguide_jpn()
            pass

        if comp_name == 'Guideinfo_SpotGuide':
            import component.jdb.guideinfo_spotguide_jdb
            return component.jdb.guideinfo_spotguide_jdb.comp_guideinfo_spotguide_jdb()
            pass
        
        if comp_name == 'park':
            import component.jdb.park_jdb
            return component.jdb.park_jdb.comp_park_jdb()
            pass

        if comp_name == 'Guideinfo_Etc':
            # return jpn.comp_guideinfo_etc_jpn()
            pass

        if comp_name == 'Link_Name':
            # return jpn.comp_linkname_jpn()
            pass

        if comp_name == 'Toll':
            # return jpn.comp_node_toll_jpn()
            pass
        if comp_name == 'CrossName':
            import component.jdb.guideinfo_crossname_jdb
            return component.jdb.guideinfo_crossname_jdb.comp_guideinfo_crossname_jdb()
            pass
        if comp_name == 'Admin':
            import component.jdb.admin_jdb
            return component.jdb.admin_jdb.comp_admin_jdb()
            pass

        if comp_name == 'Guideinfo_Caution':
            import component.jdb.guideinfo_caution_jpn_jdb
            return component.jdb.guideinfo_caution_jpn_jdb.comp_guideinfo_caution_jpn()
            pass

        if comp_name == 'Shield':
            # return jpn.comp_shield_jpn()
            pass
        if comp_name == 'Ramp':
            return ramp_roadtype.comp_ramp_roadtype()
            pass

        if comp_name == 'Link_Split':
            return link_split.comp_link_split()
            pass

        if comp_name == 'Link_Merge':
            return link_merge.comp_link_merge()
            pass

        if comp_name == 'Node_Move':
            return node_move.comp_node_move()
            pass

        if comp_name == 'guideinfo_signpost_uc':
            return guideinfo_signpost_uc.comp_guideinfo_signpost_uc()
            pass
        if comp_name == 'fac_name':
            return jdb.comp_fac_name_jdb()
            pass
        if comp_name == 'fac_namenew':
            return jdb.comp_fac_name_new_jdb()
            pass
        if comp_name == 'Highway_Prepare':
            from component.jdb.hwy import hwy_prepare
            return hwy_prepare.HwyPrepare()

        if comp_name == 'Highway':
            from component.jdb.hwy.highway import Highway
            return Highway()
            pass
        if comp_name == 'Trans_DirGuide':
            return jdb.comp_trans_dirguide_jdb()
        if comp_name == 'Trans_HighwayNode':
            return jdb.comp_trans_highway_node_jdb()

        if comp_name == 'MainNode':
            import component.mainnode
            return component.mainnode.comp_mainnode()
#         if comp_name == 'ImportCaution':
#             import component.jdb.import_caution_data_jdb
#             return component.jdb.import_caution_data_jdb.import_org_caution_data_jdb()
            pass
        return None


########################################################################
# #  class comp_factory_nostra: 泰国数据
########################################################################
class comp_factory_nostra(comp_factory):
    def __init__(self):
        '''GlobeTech
        '''

    def CreateOneComponent(self, comp_name):
        if comp_name == 'Topo':
            from component.nostra import topo_nostra
            return topo_nostra.comp_topo_nostra()
            pass
        if comp_name == 'sidefiles':
            # return import_jvfiles_ta.comp_import_sidefiles_uc()
            pass
        if comp_name == 'illustname':
            import component.nostra.illustname_nostra
            return component.nostra.illustname_nostra.comp_illustname_nostra()
            pass
        if comp_name == 'Guideinfo_Lane':
            return guideinfo_lane_nostra.comp_guideinfo_lane_nostra()
            pass
        if comp_name == 'Guideinfo_SpotGuide':
            return guideinfo_spotguide_nostra.comp_guideinfo_spotguide_nostra()
            pass
        if comp_name == 'Guideinfo_TowardName':
            from component.guideinfo_towardname import comp_guideinfo_towardname
            return comp_guideinfo_towardname()
            pass
        if comp_name == 'Guideinfo_SignPost_UC':
            from component.nostra import guideinfo_signpost_uc_nostra as sp
            return sp.comp_guideinfo_signpost_uc_nostra()
        if comp_name == 'Guideinfo_SignPost':
            from component.guideinfo_signpost import comp_guideinfo_signpost
            return comp_guideinfo_signpost()
        if comp_name == 'Guideinfo_ForceGuide':
            from component.nostra.guideinfo_forceguide_nostra import comp_guideinfo_forceguide_nostra
            return comp_guideinfo_forceguide_nostra()
        if comp_name == 'Guideinfo_Caution':
            return guideinfo_caution.comp_guideinfo_caution()

        if comp_name == 'park':
            from component.nostra import park_nostra
            return park_nostra.comp_park_nostra()
            pass
        
        if comp_name == 'Link':
            from component.nostra import link_nostra
            return link_nostra.comp_link_nostra()
            pass
        if comp_name == 'Node':
            from component.nostra import node_nostra
            return node_nostra.comp_node_nostra()
            pass

        if comp_name == 'Dictionary':
            from component.nostra import dictionary_nostra
            return dictionary_nostra.comp_dictionary_nostra()
            pass

        if comp_name == 'Regulation':
            import component.nostra.regulation
            return component.nostra.regulation.comp_regulation_nostra()
            pass

        if comp_name == 'Admin':
            import component.nostra.admin
            return component.nostra.admin.comp_admin_nostra()
            pass
        if comp_name == 'Highway':
            return highway.comp_highway()

        if comp_name == 'Ramp':
            return ramp_roadtype.comp_ramp_roadtype()

        if comp_name == 'Tmc':
            return vics.comp_vics()

        if comp_name == 'Link_Split':
            return link_split.comp_link_split()

        if comp_name == 'Link_Merge':
            return link_merge.comp_link_merge()

        if comp_name == 'Node_Move':
            return node_move.comp_node_move()

        if comp_name == 'Link_overlay_or_circle':
            return link_overlay_or_circle.comp_link_dupli_or_circle()
        if comp_name == 'CrossName':
            import component.guideinfo_crossname
            return component.guideinfo_crossname.comp_guideinfo_crossname()
        if comp_name == 'importupdatejv':
            import component.nostra.import_update_junctionview_nostra
            return component.nostra.import_update_junctionview_nostra.import_org_update_junctionview_nostra()
        return None


########################################################################
# #  class comp_factory_mmi: 印度数据
########################################################################
class comp_factory_mmi(comp_factory):
    def __init__(self):
        '''MMI
        '''
        comp_factory.__init__(self)

    def CreateOneComponent(self, comp_name):
        if comp_name == 'sidefiles':
            from component.mmi.import_sidefile_mmi import \
                    comp_import_sidefile_mmi
            return comp_import_sidefile_mmi()
        if comp_name == 'Dictionary':
            from component.mmi.dictionary_mmi import comp_dictionary_mmi
            return comp_dictionary_mmi()
        if comp_name == 'Guideinfo_SignPost_UC':
            from component.mmi.guideinfo_signpost_uc_mmi import \
                comp_guideinfo_signpost_uc_mmi
            return comp_guideinfo_signpost_uc_mmi()
        if comp_name == 'Admin':
            from component.mmi.admin_mmi import comp_admin_mmi
            return comp_admin_mmi()
        if comp_name == 'park':
            from component.mmi.park_mmi import comp_park_mmi
            return comp_park_mmi()
        if comp_name == 'Link':
            from component.mmi.link_mmi import comp_link_mmi
            return comp_link_mmi()
        if comp_name == 'Node':
            from component.mmi.node_mmi import comp_node_mmi
            return comp_node_mmi()
        if comp_name == 'Regulation':
            import component.mmi.regulation
            return component.mmi.regulation.comp_regulation_mmi()
        if comp_name == 'MainNode':
            import component.mainnode
            return component.mainnode.comp_mainnode()
        if comp_name == 'spotguide_tbl':
            from component.mmi.guideinfo_spotguide_mmi import comp_guideinfo_spotguide_mmi
            return comp_guideinfo_spotguide_mmi()
        if comp_name == 'Guideinfo_Lane':
            from component.mmi.guideinfo_lane_mmi import comp_guideinfo_lane_mmi
            return comp_guideinfo_lane_mmi()
        if comp_name == 'Ramp':
            return ramp_roadtype.comp_ramp_roadtype()
        if comp_name == 'Link_Split':
            return link_split.comp_link_split()
        if comp_name == 'Link_Merge':
            return link_merge.comp_link_merge()
        if comp_name == 'Node_Move':
            return node_move.comp_node_move()
        
        if comp_name == 'Guideinfo_TowardName':
            from component.guideinfo_towardname import comp_guideinfo_towardname
            return comp_guideinfo_towardname()
        if comp_name == 'Guideinfo_SignPost':
            from component.guideinfo_signpost import comp_guideinfo_signpost
            return comp_guideinfo_signpost()
        if comp_name == 'Guideinfo_ForceGuide':
            return guideinfo_forceguide.com_guideinfo_forceguide()
        if comp_name == 'Guideinfo_Caution':
            return guideinfo_caution.comp_guideinfo_caution()
        if comp_name == 'Highway':
            return highway.comp_highway()
        if comp_name == 'CrossName':
            import component.guideinfo_crossname
            return component.guideinfo_crossname.comp_guideinfo_crossname()


########################################################################
# #  class comp_factory_msm: 马来西亚Malsing数据
########################################################################
class comp_factory_msm(comp_factory):
    def __init__(self):
        '''
            MalsingMap
        '''

    def CreateOneComponent(self, comp_name):
        if comp_name == 'sidefiles':
            from component.msm import import_sidefiles_msm
            return import_sidefiles_msm.comp_import_sidefiles_msm()
        if comp_name == 'Topo':
            from component.msm import topo_msm
            return topo_msm.comp_topo_msm()
            pass

        if comp_name == 'ORG_patch':
            from component.msm import org_patch_msm
            return org_patch_msm.comp_org_patch_msm()
            pass
        
        if comp_name == 'Link':
            from component.msm import link_msm
            return link_msm.comp_link_msm()
            pass
        
        if comp_name == 'Node':
            from component.msm import node_msm
            return node_msm.comp_node_msm()
            pass
        
        if comp_name == 'Regulation':
            from component.msm import regulation_msm
            return regulation_msm.comp_regulation_msm()
            pass
        
        if comp_name == 'Admin':
            from component.msm import admin_msm
            return admin_msm.comp_admin_msm()
            pass
        
        if comp_name == 'Dictionary':
            from component.msm import dictionary_msm
            return dictionary_msm.comp_dictionary_msm()
            pass
        
        if comp_name == 'Guideinfo_SignPost_UC':
            from component.msm import guideinfo_signpost_uc_msm
            return guideinfo_signpost_uc_msm.comp_guideinfo_signpost_uc_msn()
            pass
        
        if comp_name == 'Guideinfo_TowardName':
            from component.guideinfo_towardname import comp_guideinfo_towardname
            return comp_guideinfo_towardname()

        if comp_name == 'Guideinfo_Lane':
            return guideinfo_lane_msm.comp_guideinfo_lane_msm()
            pass
        if comp_name == 'Guideinfo_SpotGuide':
            return guideinfo_spotguide_msm.comp_guideinfo_spotguide_msm()
            pass

        if comp_name == 'Guideinfo_SignPost':
            from component.guideinfo_signpost import comp_guideinfo_signpost
            return comp_guideinfo_signpost()
            pass
        
        if comp_name == 'Guideinfo_ForceGuide':
            return guideinfo_forceguide_msm.comp_guideinfo_forceguide_msm()
            pass
        
        if comp_name == 'Guideinfo_Caution':
            return guideinfo_caution.comp_guideinfo_caution()
            pass
        if comp_name == 'park':
            from component.park import comp_park
            return comp_park()
            pass
        
        if comp_name == 'Highway':
            return highway.comp_highway()
            pass
        
        if comp_name == 'CrossName':
            import component.guideinfo_crossname
            return component.guideinfo_crossname.comp_guideinfo_crossname()
            pass
        
        if comp_name == 'Ramp':
            return ramp_roadtype.comp_ramp_roadtype()
            pass

        if comp_name == 'Link_overlay_or_circle':
            return link_overlay_or_circle.comp_link_dupli_or_circle()
        
        if comp_name == 'Link_Split':
            return link_split.comp_link_split()
            pass
        
        if comp_name == 'Link_Merge':
            return link_merge.comp_link_merge()
            pass
        
        if comp_name == 'MainNode':
            import component.mainnode
            return component.mainnode.comp_mainnode()
            pass
        return None
