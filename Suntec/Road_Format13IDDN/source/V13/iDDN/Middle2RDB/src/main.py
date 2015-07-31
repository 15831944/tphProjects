# -*- coding: cp936 -*-
'''
Created on 2011-12-5

@author: hongchenzai
'''
import common.log
from common import rdb_log
from common import rdb_common

from item import rdb_tile
from item import rdb_node
from item import rdb_link
from item import rdb_guideinfo_cross_name
from item import rdb_guideinfo_lane
from item import rdb_guideinfo_pic_blob_bytea
from item import rdb_guideinfo_signpost
from item import rdb_guideinfo_spotguide
from item import rdb_guideinfo_towardname
from item import rdb_link_add_info
from item import rdb_linklane_info
from item import rdb_line_name
from item import rdb_poi_name
from item import rdb_cond_speed
from item import rdb_kind_code
from item import rdb_regulation
from item import rdb_dictionary
from item import rdb_highway_facility_data
from item import rdb_guideinfo_building_structure
from item import rdb_guideinfo_caution
from item import rdb_guideinfo_road_structure
from item import rdb_region
from item import rdb_view
from item import rdb_guideinfo_signpost_uc
from item import rdb_link_shield
from item import rdb_admin
from item import rdb_guideinfo_forceguide
from item import rdb_vics
from item import rdb_language
from item import rdb_link_with_all_attri_view
from item import rdb_node_with_all_attri_view
from item import rdb_link_org2rdb
from item import rdb_park
from item.hwy import rdb_hwy
from item import rdb_slope

from item_client import rdb_link_client
from item_client import rdb_node_client
from item_client import rdb_cond_speed_client
from item_client import rdb_link_add_info_client
from item_client import rdb_linklane_info_client
from item_client import rdb_regulation_client
from item_client import rdb_guideinfo_cross_name_client
from item_client import rdb_guideinfo_lane_client
from item_client import rdb_guideinfo_signpost_client
from item_client import rdb_guideinfo_spotguide_client
from item_client import rdb_guideinfo_towardname_client
from item_client import rdb_highway_facility_data_client
from item_client import rdb_guideinfo_building_structure_client
from item_client import rdb_guideinfo_caution_client
from item_client import rdb_guideinfo_road_structure_client
from item_client import rdb_guideinfo_signpost_uc_client
from item_client import rdb_link_shield_client
from item_client import rdb_admin_client
from item_client import rdb_guideinfo_forceguide_client
import rdb_thread


def main():
    rdb_log.init()
    common.log.common_log.instance().init()
    rdb_log.log('Middle2RDB', 'Now it is converting Middle data to RDB format data. It will take 4 hour.', 'info')

    comp_list = [
                rdb_tile.rdb_tile(),
                rdb_language.rdb_language(),
                rdb_guideinfo_cross_name.rdb_guideinfo_cross_name(),
                rdb_guideinfo_lane.rdb_guideinfo_lane(),
                rdb_guideinfo_pic_blob_bytea.rdb_guideinfo_pic_blob_bytea(),
                rdb_guideinfo_signpost.rdb_guideinfo_signpost(),
                rdb_guideinfo_spotguide.rdb_guideinfo_spotguide(),
                rdb_guideinfo_towardname.rdb_guideinfo_towardname(),
                rdb_highway_facility_data.rdb_highway_facility_data(),
                rdb_guideinfo_building_structure.rdb_guideinfo_building_structure(),
                rdb_guideinfo_caution.rdb_guideinfo_caution(),
                rdb_guideinfo_road_structure.rdb_guideinfo_road_structure(),
                rdb_guideinfo_signpost_uc.rdb_guideinfo_signpost_uc(),
                rdb_guideinfo_forceguide.rdb_guideinfo_forceguide(),

                rdb_cond_speed.rdb_cond_speed(),
                rdb_link_add_info.rdb_link_add_info(),
                rdb_linklane_info.rdb_linklane_info(),

                rdb_regulation.rdb_regulation(),

                rdb_node.rdb_node(),
                rdb_link.rdb_link(),
                #rdb_link.rdb_link_wellshape_handle_rdf(),
                #rdb_link.rdb_link_simplify_axf_china(),
                rdb_link.rdb_shape_optimize(),

                rdb_admin.rdb_admin(),

                #rdb_region.rdb_region_ipc_japen(),
                #rdb_region.rdb_region_axf_china(),
                #rdb_region.rdb_region_ta_europe(),
                rdb_region.rdb_region_ta_vietnam(),
                #rdb_region.rdb_region_rdf_ap(),
                #rdb_region.rdb_region_rdf_north_america(),
                #rdb_region.rdb_region_rdf_brazil(),
                #rdb_region.rdb_region_nostra_thailand(),
                #rdb_region.rdb_region_mmi_india(),

                
                #rdb_link_org2rdb.rdb_link_org2rdb_msm(),
#                rdb_link_org2rdb.rdb_link_org2rdb_nostra(),
#                rdb_link_org2rdb.rdb_link_org2rdb_rdf(),
                rdb_link_org2rdb.rdb_link_org2rdb_mmi(),
                #rdb_link_org2rdb.rdb_link_org2rdb_tomtom(),
#                rdb_link_org2rdb.rdb_link_org2rdb_axf(),
#                rdb_link_org2rdb.rdb_link_org2rdb_jpn(),
#                rdb_link_org2rdb.rdb_link_org2rdb_jdb(),

                #rdb_region.rdb_region_malsing_mys(),

                #rdb_vics.rdb_vics_jpn(),
                #rdb_vics.rdb_vics_axf(),
                rdb_vics.rdb_traffic_rdf_ap(),
                rdb_vics.rdb_traffic_nostra(),
                rdb_vics.rdb_traffic_area(),
                rdb_park.rdb_park(),
                rdb_slope.rdb_slope(),
                
                # client data and view
                rdb_cond_speed_client.rdb_cond_speed_client(),
                rdb_link_add_info_client.rdb_link_add_info_client(),
                rdb_linklane_info_client.rdb_linklane_info_client(),
                rdb_regulation_client.rdb_regulation_client(),

                rdb_guideinfo_cross_name_client.rdb_guideinfo_cross_name_client(),
                rdb_guideinfo_lane_client.rdb_guideinfo_lane_client(),
                rdb_guideinfo_signpost_client.rdb_guideinfo_signpost_client(),
                rdb_guideinfo_spotguide_client.rdb_guideinfo_spotguide_client(),
                rdb_guideinfo_towardname_client.rdb_guideinfo_towardname_client(),
                rdb_highway_facility_data_client.rdb_highway_facility_data_client(),
                rdb_guideinfo_building_structure_client.rdb_guideinfo_building_structure_client(),
                rdb_guideinfo_caution_client.rdb_guideinfo_caution_client(),
                rdb_guideinfo_road_structure_client.rdb_guideinfo_road_structure_client(),
                rdb_guideinfo_signpost_uc_client.rdb_guideinfo_signpost_uc_client(),
                rdb_guideinfo_forceguide_client.rdb_guideinfo_forceguide_client(),

                rdb_node_client.rdb_node_client(),
                rdb_link_client.rdb_link_client(),
                rdb_admin_client.rdb_admin_client(),

                rdb_view.rdb_view(),
                rdb_link_with_all_attri_view.rdb_link_with_all_attri_view(),
                rdb_node_with_all_attri_view.rdb_node_with_all_attri_view(),
                rdb_hwy.rdb_highway()
                ]

    # ����ǰ��������������component
    for component in comp_list:
        component.Make()

    # Check
    for component in comp_list:
        component.Check()
        pass
    blob_dump_object = rdb_guideinfo_pic_blob_bytea.rdb_guideinfo_pic_blob_bytea()
    blob_dump_object.RenameAndSaveIllust()
    blob_dump_object.RenameAndSaveSapaIllust()
    blob_dump_object.RenameAndSaveSignPost()
    rdb_log.log('Middle2RDB', 'Convert Middle data to RDB format data succeeded. Please the check data.', 'info')
    common.log.common_log.instance().end()
    rdb_log.end()

    # �����˳�E������mail
    rdb_common.rdb_exit(0)
    return 0

if __name__ == '__main__':
    main()
