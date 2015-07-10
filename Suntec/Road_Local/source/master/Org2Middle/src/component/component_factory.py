# -*- coding: UTF8 -*-
'''
Created on 2012-3-22

@author: hongchenzai
'''
import common.common_func


########################################################################
# #  class comp_factory
########################################################################
class comp_factory(object):
    def __init__(self):
        '''
        Constructor
        '''

    def CreateOneComponent(self, comp_name):
        
        if comp_name == 'Ramp':
            from component.default import ramp_roadtype
            return ramp_roadtype.comp_ramp_roadtype()

        if comp_name == 'Link_Split':
            from component.default import link_split
            return link_split.comp_link_split()

        if comp_name == 'Link_Merge':
            from component.default import link_merge
            return link_merge.comp_link_merge()

        if comp_name == 'Node_Move':
            from component.default import node_move
            return node_move.comp_node_move()
        
        if comp_name == 'Link_overlay_or_circle':
            from component.default import link_overlay_or_circle
            return link_overlay_or_circle.comp_link_dupli_or_circle()

        if comp_name == 'MainNode':
            from component.default import mainnode
            return mainnode.comp_mainnode()

        if comp_name == 'Highway':
            from component.default import highway
            return highway.comp_highway()
        
        if comp_name == 'ImportCSV':
            from component.default import import_sidefiles
            return import_sidefiles.import_csv_data()

        if comp_name == 'Dictionary':
            from component.default import dictionary
            return dictionary.comp_dictionary()
        
        if comp_name == 'Guideinfo_NaturalGuidence':
            from component.default import guideinfo_natural_guidence
            return guideinfo_natural_guidence.comp_natural_guidence()
        
        if comp_name == 'Guideinfo_Safety_Zone':
            from component.default.guideinfo_safety_zone import comp_guideinfo_safety_zone
            return comp_guideinfo_safety_zone()
        
        if comp_name == 'Guideinfo_Safety_Alert':
            from component.default.guideinfo_safety_alert import comp_guideinfo_safety_alert
            return comp_guideinfo_safety_alert()
        
        if comp_name == 'guideinfo_signpost_uc':
            from component.default import guideinfo_signpost_uc
            return guideinfo_signpost_uc.comp_guideinfo_signpost_uc()
        
        if comp_name == 'Guideinfo_TowardName':
            from component.default import guideinfo_towardname
            return guideinfo_towardname.comp_guideinfo_towardname()
        
        if comp_name == 'Guideinfo_SignPost':
            from component.default import guideinfo_signpost
            return guideinfo_signpost.comp_guideinfo_signpost()
        
        if comp_name == 'CrossName':
            from component.default import guideinfo_crossname
            return guideinfo_crossname.comp_guideinfo_crossname()
        
        if comp_name == 'Guideinfo_building':
            from component.default import guideinfo_building
            return guideinfo_building.comp_guideinfo_building()
        
        if comp_name == 'Guideinfo_ForceGuide':
            from component.default import guideinfo_forceguide
            return guideinfo_forceguide.com_guideinfo_forceguide()
        
        if comp_name == 'Guideinfo_Caution':
            from component.default import guideinfo_caution
            return guideinfo_caution.comp_guideinfo_caution()
        
        if comp_name == 'Tmc':
            from component.default import vics
            return vics.comp_vics()
        
        if comp_name == 'park':
            from component.default import park
            return park.comp_park()
        
        if comp_name == 'slope':
            from component.default import slope
            return slope.comp_slope()
        
        if comp_name == 'height':
            from component.default import height
            return height.comp_height()
        
        if comp_name == 'hook_turn':
            from component.default import hook_turn
            return hook_turn.comp_hook_turn()
        
        if comp_name == 'stopsign':
            from component.default import stopsign
            return stopsign.comp_stopsign()
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
            from component.chn import guideinfo_lane_chn
            return guideinfo_lane_chn.comp_guideinfo_lane_chn()

        return comp_factory.CreateOneComponent(self, comp_name)

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
            from component.jpn import import_forceguide_patch
            return import_forceguide_patch.comp_import_forceguide_patch_jpn()

        if comp_name == 'sidefiles':
            from component.jpn import import_sidefiles_jpn
            return import_sidefiles_jpn.comp_import_sidefiles_jpn()

        if comp_name == 'Guideinfo_Lane':
            from component.jpn import guideinfo_lane_jpn
            return guideinfo_lane_jpn.comp_guideinfo_lane_jpn()

        if comp_name == 'Guideinfo_TowardName':
            from component.jpn import guideinfo_towardname_jpn
            return guideinfo_towardname_jpn.comp_guideinfo_towardname_jpn()
        
        if comp_name == 'park':
            from component.jpn import park_jpn
            return park_jpn.comp_park_jpn()

        if comp_name == 'Link':
            from component.jpn import link_jpn
            return link_jpn.comp_link_jpn()

        if comp_name == 'Split_Link':
            from component.jpn import split_link
            return split_link.comp_split_link_jpn()

        if comp_name == 'Trans_SRID':
            from component.jpn import trans_srid
            return trans_srid.comp_trans_srid_jpn()

        if comp_name == 'Node':
            from component.jpn import node_jpn
            return node_jpn.comp_node_jpn()

        if comp_name == 'Dictionary':
            from component.jpn import dictionary_jpn
            return dictionary_jpn.comp_dictionary_jpn()

        if comp_name == 'Regulation':
            from component.jpn import regulation_jpn
            return regulation_jpn.comp_regulation_jpn()

        if comp_name == 'Guideinfo_SignPost':
            from component.jpn import guideinfo_signpost_jpn
            return guideinfo_signpost_jpn.comp_guideinfo_signpost_jpn()

        if comp_name == 'Tile':
            from component.jpn import tile_jpn
            return tile_jpn.comp_tile_jpn()

        if comp_name == 'Guideinfo_ForceGuide':
            from component.jpn import guideinfo_forceguide_jpn
            return guideinfo_forceguide_jpn.com_guideinfo_forceguide_jpn()

        if comp_name == 'Guideinfo_SpotGuide':
            from component.jpn import guideinfo_spotguide_jpn
            return guideinfo_spotguide_jpn.comp_guideinfo_spotguide_jpn()

        if comp_name == 'Guideinfo_Etc':
            from component.jpn import guideinfo_etc_jpn
            return guideinfo_etc_jpn.comp_guideinfo_etc_jpn()

        if comp_name == 'Link_Name':
            from component.jpn import linkname_jpn
            return linkname_jpn.comp_linkname_jpn()

        if comp_name == 'Toll':
            from component.jpn import node_toll_jpn
            return node_toll_jpn.comp_node_toll_jpn()

        if comp_name == 'Admin':
            from component.jpn import admin_jpn
            return admin_jpn.comp_admin_jpn()

        if comp_name == 'Guideinfo_Caution':
            from component.jpn import guideinfo_caution_jpn
            return guideinfo_caution_jpn.comp_guideinfo_caution_jpn()

        if comp_name == 'Shield':
            from component.jpn import shield_jpn
            return shield_jpn.comp_shield_jpn()

        if comp_name == 'Tmc':
            from component.jpn import vics_jpn
            return vics_jpn.comp_vics_jpn()
        
        if comp_name == 'fac_name':
            from component.jpn import fac_name
            return fac_name.comp_fac_name()
        
        return comp_factory.CreateOneComponent(self, comp_name)

########################################################################
# #  class comp_factory_rdf: Here
########################################################################
class comp_factory_rdf(comp_factory):
    def __init__(self):
        '''锟斤拷锟斤拷
        '''

    def CreateOneComponent(self, comp_name):        
        if comp_name == 'ImportJCV':
            from component.rdf import import_csv_data_rdf
            return import_csv_data_rdf.import_junctionview_data_rdf()
            pass
        if comp_name == 'Guideinfo_Lane':
            from component.rdf import guideinfo_lane_rdf
            return guideinfo_lane_rdf.comp_guideinfo_lane_rdf()
            pass
        if comp_name == 'Guideinfo_TowardName':
            #from component.rdf import guideinfo_towardname_rdf
            # return guideinfo_towardname_rdf.comp_guideinfo_towardname_rdf()
            pass
        if comp_name == 'rawdata':
            from component.rdf import rawdata_rdf
            return rawdata_rdf.comp_rawdata_rdf()
            pass
        if comp_name == 'park':
            from component.rdf import park_rdf
            return park_rdf.comp_park_rdf()
            pass
        if comp_name == 'slope':
            from component.rdf import slope_rdf
            return slope_rdf.comp_slope_rdf()
            pass       
        if comp_name == 'Link':
            from component.rdf import link_rdf
            return link_rdf.comp_link_rdf()
            pass
        if comp_name == 'Node':
            from component.rdf import node_rdf
            return node_rdf.comp_node_rdf()
            pass
        if comp_name == 'Node_Height':
            from component.rdf import node_height_rdf
            return node_height_rdf.comp_node_height_rdf()
            pass
        if comp_name == 'Dictionary':
            if common.common_func.JudgementCountry('hkg','rdf') \
              or common.common_func.JudgementCountry('mac','rdf'):               
                from component.rdf import dictionary_rdf_hkgmac
                return dictionary_rdf_hkgmac.comp_dictionary_rdf_hkg_mac()
            else:  
                from component.rdf import dictionary_rdf
                return dictionary_rdf.comp_dictionary_rdf()
            pass
        if comp_name == 'Regulation':
            from component.rdf import regulation_rdf
            return regulation_rdf.comp_regulation_rdf()
        if comp_name == 'Guideinfo_SpotGuide':
            from component.rdf import guideinfo_spotguide_rdf
            return guideinfo_spotguide_rdf.comp_guideinfo_spotguide_rdf()
            pass
        if comp_name == 'Link_Name':
            from component.rdf import linkname_rdf
            return linkname_rdf.comp_linkname_rdf()
        if comp_name == 'Trans_Geom':
            from component.rdf import trans_geom_rdf
            return trans_geom_rdf.comp_trans_geom_rdf()
        if comp_name == 'trans_Srid':
            from component.rdf import trans_srid_rdf
            return trans_srid_rdf.comp_trans_srid_rdf()
        if comp_name == 'Admin':
            from component.rdf import admin_rdf
            return admin_rdf.comp_admin_rdf()
        if comp_name == 'Guideinfo_Caution':
            from component.rdf import guideinfo_caution_rdf
            return guideinfo_caution_rdf.comp_guideinfo_caution_rdf()
        
        if comp_name == 'Guideinfo_Caution_highaccident':
            from component.rdf import guideinfo_caution_highaccident_rdf
            return guideinfo_caution_highaccident_rdf.comp_guideinfo_caution_highaccident_rdf()
        
        if comp_name == 'Guideinfo_Safety_Zone':
            from component.rdf.guideinfo_safety_zone_rdf import comp_guideinfo_safety_zone_rdf
            return comp_guideinfo_safety_zone_rdf()
        if comp_name == 'GuideInfo_SignPost':
            from component.rdf.guideinfo_signpost_rdf import comp_guideinfo_signpost_rdf
            return comp_guideinfo_signpost_rdf()
        
        if comp_name == 'guideinfo_signpost_uc':
            from component.rdf import guideinfo_singpost_uc_rdf
            return guideinfo_singpost_uc_rdf.comp_guideinfo_signpost_uc_rdf()
        if comp_name == 'Guideinfo_ForceGuide':
            from component.rdf import guideinfo_forceguide_rdf
            return guideinfo_forceguide_rdf.comp_guideinfo_forceguide_rdf()
        if comp_name == 'Guideinfo_NaturalGuidence':
            from component.rdf import guideinfo_natural_guidence
            return guideinfo_natural_guidence.comp_natural_guidence()
        if comp_name == 'Guideinfo_building':
            from component.rdf import guideinfo_building_rdf
            return guideinfo_building_rdf.comp_guideinfo_building_rdf()
            pass
        if comp_name == 'Shield':
            from component.rdf import shield_rdf
            return shield_rdf.comp_shield_rdf()
        if comp_name == 'Highway':
            from component.rdf.hwy.highway_rdf import HighwayRDF
            return HighwayRDF()
        if comp_name == 'Highway_Prepare':
            from component.rdf.hwy.hwy_prepare_rdf import HwyPrepareRDF
            return HwyPrepareRDF()
        if comp_name == 'stopsign':
            from component.rdf import stopsign_rdf
            return stopsign_rdf.comp_stopsign_rdf()
            pass
        
        return comp_factory.CreateOneComponent(self, comp_name)


########################################################################
# #  class comp_factory_axf: 中国高德
########################################################################
class comp_factory_axf(comp_factory):
    def __init__(self):
        '''锟竭碉拷
        '''

    def CreateOneComponent(self, comp_name):        
        if comp_name == 'Guideinfo_Lane':
            from component.axf import guideinfo_lane_axf
            return guideinfo_lane_axf.comp_guideinfo_lane_axf()
        if comp_name == 'Guideinfo_TowardName':
            from component.axf import guideinfo_towardname_axf
            return guideinfo_towardname_axf.comp_guideinfo_towardname_axf()
        if comp_name == 'park':
            from component.axf import park_axf
            return park_axf.comp_park_axf()
        if comp_name == 'Link':
            from component.axf import link_axf
            return link_axf.comp_link_axf()
        if comp_name == 'Node':
            from component.axf import node_axf
            return node_axf.comp_node_axf()
        if comp_name == 'Dictionary':
            from component.axf import dictionary_axf
            return dictionary_axf.comp_dictionary_axf()
        if comp_name == 'Regulation':
            from component.axf import regulation_axf
            return regulation_axf.comp_regulation_axf()
        if comp_name == 'Guideinfo_SignPost':
            from component.axf import guideinfo_signpost_axf
            return guideinfo_signpost_axf.comp_guideinfo_signpost_axf()
        if comp_name == 'Guideinfo_SpotGuide':
            from component.axf import guideinfo_spotguide_axf
            return guideinfo_spotguide_axf.comp_guideinfo_spotguide_axf()
        if comp_name == 'Link_Name':
            from component.axf import linkname_axf
            return linkname_axf.comp_linkname_axf()
        if comp_name == 'NewID':
            from component.axf import newid_axf
            return newid_axf.comp_newid_axf()
        if comp_name == 'Admin':
            from component.axf import admin_axf
            return admin_axf.comp_admin_axf()

        if comp_name == 'Shield':
            from component.axf import shield_axf
            return shield_axf.comp_shield_axf()

        if comp_name == 'Highway':
            from component.axf import highway_axf
            return highway_axf.comp_highway_axf()
        
        return comp_factory.CreateOneComponent(self, comp_name)

########################################################################
# #  class comp_factory_ta: TomTom锟斤拷锟?
########################################################################
class comp_factory_ta(comp_factory):
    def __init__(self):
        '''TomTom
        '''

    def CreateOneComponent(self, comp_name):        
        if comp_name == 'sidefiles':
            if common.common_func.JudgementCountry('AUS'):
                from component.ta import import_jvfiles_sensis
                print 'Current proj is austrilia of tomtom!'
                return import_jvfiles_sensis.comp_import_siedfiles_sensis()
            else:
                from component.ta import import_jvfiles_ta
                return import_jvfiles_ta.comp_import_sidefiles_uc()
            pass
        if comp_name == 'Sidefiles_Phoneme':
            from component.ta import import_phoneme_ta
            return import_phoneme_ta.comp_import_phoneme_ta()
        if comp_name == 'Guideinfo_Lane':
            from component.ta import guideinfo_lane_ta
            return guideinfo_lane_ta.comp_guideinfo_lane_ta()
            pass
        if comp_name == 'Guideinfo_Safety_Alert':
            from component.ta import guideinfo_safetyalert_ta
            return guideinfo_safetyalert_ta.comp_guideinfo_safety_alert_ta()
            pass
        if comp_name == 'Guideinfo_Safety_Zone':
            from component.ta import guideinfo_safetyzone_ta
            return guideinfo_safetyzone_ta.comp_guideinfo_safety_zone_ta()
            pass
        if comp_name == 'park':
            from component.ta import park_ta
            return park_ta.comp_park_ta()
            pass
        if comp_name == 'Link':
            from component.ta import link_ta
            return link_ta.comp_link_ta()
            pass
        if comp_name == 'Node':
            from component.ta import node_ta
            return node_ta.comp_node_ta()
            pass
        if comp_name == 'Dictionary':
            from component.ta import dictionary_ta
            return dictionary_ta.comp_dictionary_ta()
            pass
        if comp_name == 'Regulation':
            from component.ta import regulation_ta
            return regulation_ta.comp_regulation_ta()
            pass
        if comp_name == 'Guideinfo_SpotGuide':
            if common.common_func.JudgementCountry('AUS'):
                from component.ta import guideinfo_spotguide_sensis
                return guideinfo_spotguide_sensis.comp_guideinfo_spotguide_sensis()
            else:
                from component.ta import guideinfo_spotguide_ta
                return guideinfo_spotguide_ta.comp_guideinfo_spotguide_ta()
                pass
        if comp_name == 'Link_Name':
            from component.ta import linkname_ta
            return linkname_ta.comp_linkname_ta()
            pass
        if comp_name == 'school_zone':
            from component.ta import school_zone_ta
            return school_zone_ta.comp_school_zone_ta()
            pass
        if comp_name == 'hook_turn':
            from component.ta import hook_turn_ta
            return hook_turn_ta.comp_hook_turn_ta()
            pass        
        if comp_name == 'Shield':
            from component.ta import shield_ta
            return shield_ta.comp_shield_ta()
        if comp_name == 'Admin':
            from component.ta import admin_ta
            return admin_ta.comp_admin_ta()
        if comp_name == 'guideinfo_signpost_uc':
            from component.ta import guideinfo_signpost_uc_ta
            return guideinfo_signpost_uc_ta.comp_guideinfo_signpost_uc_ta()
        if comp_name == 'Highway_Prepare':
            from component.ta.hwy.hwy_prepare_ta import HwyPrepareTa
            return HwyPrepareTa()
        if comp_name == 'Highway':
            from component.ta.hwy.highway_ta import HighwayTa
            return HighwayTa()
        if comp_name == 'Guideinfo_building':
            from component.ta.guideinfo_building_ta import comp_guideinfo_building_ta
            return comp_guideinfo_building_ta()
        if comp_name == 'stopsign':
            from component.ta import stopsign_ta
            return stopsign_ta.comp_stopsign_ta()
            pass
        return comp_factory.CreateOneComponent(self, comp_name)


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
            from component.jdb import preprocess
            return preprocess.comp_proprocess_jdb()
            pass

        if comp_name == 'sidefiles':
            from component.jdb import import_sidefiles
            return import_sidefiles.comp_import_sidefiles_jdb()
            pass

        if comp_name == 'Guideinfo_Lane':
            from component.jdb import guideinfo_lane_jdb
            return guideinfo_lane_jdb.comp_guideinfo_lane_jdb()
            pass

        if comp_name == 'Guideinfo_TowardName':
            from component.jdb import guideinfo_towardname_jdb
            return guideinfo_towardname_jdb.comp_guideinfo_towardname_jdb()
            pass

        if comp_name == 'Link':
            from component.jdb import link_jdb
            return link_jdb.comp_link_jdb()
            pass
        
        if comp_name == 'arrow_optimize':
            from component.jdb import guideinfo_lane_arrow_optimize_jdb
            return guideinfo_lane_arrow_optimize_jdb.comp_guideinfo_lane_arrow_optimize_jdb()
            pass
        
        if comp_name == 'Split_Link':
            from component.jdb import split_link
            return split_link.comp_split_link_jdb()
            pass

        if comp_name == 'Trans_SRID':
            from component.jdb import trans_srid_jdb
            return trans_srid_jdb.comp_trans_srid_jdb()
            pass

        if comp_name == 'Node':
            from component.jdb import node_jdb
            return node_jdb.comp_node_jdb()
            pass
        
        if comp_name == 'Regulation':
            from component.jdb import regulation
            return regulation.comp_regulation_jdb()
            pass

        if comp_name == 'Guideinfo_SignPost':
            from component.jdb import guideinfo_signpost_jdb
            return guideinfo_signpost_jdb.comp_guideinfo_signpost_jdb()
            pass

        if comp_name == 'Tile':
            from component.jdb import tile_jdb
            return tile_jdb.comp_tile_jdb()
            pass

#         if comp_name == 'forceguide_patch':
#             import component.jdb.import_forceguide_patch
#             return component.jdb.import_forceguide_patch.comp_import_forceguide_patch_jdb()
#             pass
        if comp_name == 'Guideinfo_ForceGuide':
            from component.jdb import guideinfo_forceguide_jdb
            return guideinfo_forceguide_jdb.com_guideinfo_forceguide_jdb()
            pass

        if comp_name == 'Guideinfo_SpotGuide':
            from component.jdb import guideinfo_spotguide_jdb
            return guideinfo_spotguide_jdb.comp_guideinfo_spotguide_jdb()
            pass
        
        if comp_name == 'park':
            from component.jdb import park_jdb
            return park_jdb.comp_park_jdb()
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
            from component.jdb import guideinfo_crossname_jdb
            return guideinfo_crossname_jdb.comp_guideinfo_crossname_jdb()
            pass
        if comp_name == 'Admin':
            from component.jdb import admin_jdb
            return admin_jdb.comp_admin_jdb()
            pass

        if comp_name == 'Guideinfo_Caution':
            from component.jdb import guideinfo_caution_jdb
            return guideinfo_caution_jdb.comp_guideinfo_caution_jdb()
            pass
        
        if comp_name == 'Guideinfo_building':
            from component.jdb import guideinfo_building_jdb
            return guideinfo_building_jdb.comp_guideinfo_building_jdb()
            pass
    
        if comp_name == 'Shield':
            # return jpn.comp_shield_jpn()
            pass

        if comp_name == 'fac_name':
            from component.jdb import fac_name_jdb
            return fac_name_jdb.comp_fac_name_jdb()
            pass
        if comp_name == 'fac_namenew':
            from component.jdb import fac_name_jdb_new
            return fac_name_jdb_new.comp_fac_name_new_jdb()
            pass
        if comp_name == 'Highway_Prepare':
            from component.jdb.hwy import hwy_prepare
            return hwy_prepare.HwyPrepare()

        if comp_name == 'Highway':
            from component.jdb.hwy.highway import Highway
            return Highway()
            pass
        if comp_name == 'Trans_DirGuide':
            from component.jdb import trans_org_data
            return trans_org_data.comp_trans_dirguide_jdb()
        if comp_name == 'Trans_HighwayNode':
            from component.jdb import trans_org_data
            return trans_org_data.comp_trans_highway_node_jdb()

#         if comp_name == 'ImportCaution':
#             import component.jdb.import_caution_data_jdb
#             return component.jdb.import_caution_data_jdb.import_org_caution_data_jdb()
            pass

        if comp_name == 'update_patch_sapa_link':
            from component.jdb import update_patch
            return update_patch.comp_update_patch_jdb()
        
        return comp_factory.CreateOneComponent(self, comp_name)


########################################################################
# #  class comp_factory_nostra: 秦国
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
            from component.nostra import illustname_nostra
            return illustname_nostra.comp_illustname_nostra()
            pass
        if comp_name == 'Guideinfo_Lane':
            from component.nostra import guideinfo_lane_nostra
            return guideinfo_lane_nostra.comp_guideinfo_lane_nostra()
            pass
        if comp_name == 'Guideinfo_SpotGuide':
            from component.nostra import guideinfo_spotguide_nostra
            return guideinfo_spotguide_nostra.comp_guideinfo_spotguide_nostra()
            pass
        if comp_name == 'Guideinfo_SignPost_UC':
            from component.nostra import guideinfo_signpost_uc_nostra
            return guideinfo_signpost_uc_nostra.comp_guideinfo_signpost_uc_nostra()
        if comp_name == 'Guideinfo_ForceGuide':
            from component.nostra.guideinfo_forceguide_nostra import comp_guideinfo_forceguide_nostra
            return comp_guideinfo_forceguide_nostra()

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
            from component.nostra import regulation
            return regulation.comp_regulation_nostra()
            pass
        if comp_name == 'Admin':
            from component.nostra import admin
            return admin.comp_admin_nostra()
            pass

        if comp_name == 'importupdatejv':
            from component.nostra import import_update_junctionview_nostra
            return import_update_junctionview_nostra.import_org_update_junctionview_nostra()

        return comp_factory.CreateOneComponent(self, comp_name)


########################################################################
# #  class comp_factory_mmi: 印度
########################################################################
class comp_factory_mmi(comp_factory):
    def __init__(self):
        '''MMI
        '''
        comp_factory.__init__(self)

    def CreateOneComponent(self, comp_name):        
        if comp_name == 'sidefiles':
            from component.mmi import import_sidefile_mmi
            return import_sidefile_mmi.comp_import_sidefile_mmi()
        if comp_name == 'Dictionary':
            from component.mmi import dictionary_mmi
            return dictionary_mmi.comp_dictionary_mmi()
        if comp_name == 'Guideinfo_SignPost_UC':
            from component.mmi import guideinfo_signpost_uc_mmi
            return guideinfo_signpost_uc_mmi.comp_guideinfo_signpost_uc_mmi()
        if comp_name == 'Admin':
            from component.mmi import admin_mmi
            return admin_mmi.comp_admin_mmi()
        if comp_name == 'park':
            from component.mmi import park_mmi
            return park_mmi.comp_park_mmi()
        if comp_name == 'Link':
            from component.mmi import link_mmi
            return link_mmi.comp_link_mmi()
        if comp_name == 'Node':
            from component.mmi import node_mmi
            return node_mmi.comp_node_mmi()
        if comp_name == 'Regulation':
            from component.mmi import regulation
            return regulation.comp_regulation_mmi()
        if comp_name == 'spotguide_tbl':
            from component.mmi import guideinfo_spotguide_mmi
            return guideinfo_spotguide_mmi.comp_guideinfo_spotguide_mmi()
        if comp_name == 'Guideinfo_Lane':
            from component.mmi import guideinfo_lane_mmi
            return guideinfo_lane_mmi.comp_guideinfo_lane_mmi()
        if comp_name == 'Highway_Prepare':
            from component.ta.hwy.hwy_prepare_ta import HwyPrepareTa
            return HwyPrepareTa()  # Create a empty table: mid_all_highway_node
        if comp_name == 'Highway':
            from component.mmi.hwy.highway_mmi import HighwayMMi
            return HighwayMMi()
        
        if comp_name == 'Guideinfo_building':
            from component.mmi import guideinfo_building_mmi
            return guideinfo_building_mmi.comp_guideinfo_building_mmi()
            pass
        if comp_name == 'Guideinfo_NaturalGuidence':
            from component.mmi import guideinfo_natural_guidence_mmi
            return guideinfo_natural_guidence_mmi.comp_natural_guidence()
        return comp_factory.CreateOneComponent(self, comp_name)


########################################################################
# #  class comp_factory_msm: 马来西亚
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

        if comp_name == 'Guideinfo_Lane':
            from component.msm import guideinfo_lane_msm
            return guideinfo_lane_msm.comp_guideinfo_lane_msm()
            pass
        if comp_name == 'Guideinfo_SpotGuide':
            from component.msm import guideinfo_spotguide_msm
            return guideinfo_spotguide_msm.comp_guideinfo_spotguide_msm()
            pass
        
        if comp_name == 'Guideinfo_ForceGuide':
            from component.msm import guideinfo_forceguide_msm
            return guideinfo_forceguide_msm.comp_guideinfo_forceguide_msm()
            pass
        
        return comp_factory.CreateOneComponent(self, comp_name)
