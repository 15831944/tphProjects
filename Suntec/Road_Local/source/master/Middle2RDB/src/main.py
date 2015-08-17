# -*- coding: cp936 -*-
'''
Created on 2011-12-5

@author: hongchenzai
'''
import common.log
from common import rdb_log
from common import rdb_common

from item import rdb_tile, rdb_guideinfo_safety_zone
from item import rdb_node
from item import rdb_link
from item import rdb_guideinfo_cross_name
from item import rdb_guideinfo_lane
from item import rdb_guideinfo_pic_blob_bytea
from item import rdb_guideinfo_signpost
from item import rdb_guideinfo_spotguide
from item import rdb_guideinfo_natural_guidence
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
from item import rdb_guideinfo_safety_alert
from item import rdb_guideinfo_road_structure
from item import rdb_region
from item import rdb_view
from item import rdb_guideinfo_signpost_uc
from item import rdb_link_shield
from item import rdb_admin
from item import rdb_guideinfo_forceguide
from item import rdb_vics
from item import rdb_forecast
from item import rdb_language
from item import rdb_link_with_all_attri_view
from item import rdb_node_with_all_attri_view
from item import rdb_link_org2rdb
from item import rdb_park
from item import rdb_node_height
from item.hwy import rdb_hwy
from item import rdb_slope
from item import rdb_guideinfo_hook_turn
from item import rdb_version

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
from item_client import rdb_guideinfo_natural_guidence_client
from item_client import rdb_guideinfo_towardname_client
from item_client import rdb_highway_facility_data_client
from item_client import rdb_guideinfo_building_structure_client
from item_client import rdb_guideinfo_caution_client
from item_client import rdb_guideinfo_road_structure_client
from item_client import rdb_guideinfo_signpost_uc_client
from item_client import rdb_link_shield_client
from item_client import rdb_admin_client
from item_client import rdb_guideinfo_forceguide_client
from item_client import rdb_guideinfo_hook_turn_client
import rdb_thread


def main():
    rdb_log.init()
    common.log.common_log.instance().init()
    
    try:
        rdb_log.log('Middle2RDB', 'Now it is converting Middle data to RDB format data. It will take 4 hour.', 'info')
        comp_list = [
					rdb_guideinfo_pic_blob_bytea.rdb_guideinfo_pic_blob_bytea()
					]
        comp_lis1 = [
                    rdb_tile.rdb_tile(),
                    rdb_language.rdb_language(),
                    rdb_guideinfo_cross_name.rdb_guideinfo_cross_name(),
                    rdb_guideinfo_lane.rdb_guideinfo_lane(),
                    rdb_guideinfo_pic_blob_bytea.rdb_guideinfo_pic_blob_bytea(),
                    rdb_guideinfo_signpost.rdb_guideinfo_signpost(),
                    rdb_guideinfo_spotguide.rdb_guideinfo_spotguide(),
                    rdb_guideinfo_natural_guidence.rdb_guideinfo_natural_guidence(),
                    rdb_guideinfo_towardname.rdb_guideinfo_towardname(),
                    rdb_highway_facility_data.rdb_highway_facility_data(),
                    rdb_guideinfo_building_structure.rdb_guideinfo_building_structure(),
                    rdb_guideinfo_caution.rdb_guideinfo_caution(),
                    rdb_guideinfo_road_structure.rdb_guideinfo_road_structure(),
                    rdb_guideinfo_signpost_uc.rdb_guideinfo_signpost_uc(),
                    rdb_guideinfo_forceguide.rdb_guideinfo_forceguide(),
                    rdb_guideinfo_safety_zone.rdb_guideinfo_safety_zone(),
                    rdb_guideinfo_safety_alert.rdb_guideinfo_safetyalert(),
                    rdb_guideinfo_hook_turn.rdb_guideinfo_hook_turn(),
                    rdb_cond_speed.rdb_cond_speed(),
                    rdb_link_add_info.rdb_link_add_info(),
                    rdb_linklane_info.rdb_linklane_info(),
    
                    rdb_regulation.rdb_regulation(),
    
                    rdb_node.rdb_node(),
                    rdb_link.rdb_link(),
                    rdb_link.rdb_link_wellshape_handle.instance(),
                    #rdb_link.rdb_link_simplify_axf_china(),
                    rdb_link.rdb_shape_optimize(),
    
                    rdb_admin.rdb_admin(),
                    
                    rdb_region.rdb_region.instance(),
                    
                    rdb_link_org2rdb.rdb_link_org2rdb.instance(),
    
                    rdb_vics.rdb_vics.instance(),
                    rdb_vics.rdb_traffic_area.instance(),
                    rdb_forecast.rdb_forecast.instance(),
                    
                    rdb_node_height.rdb_node_height(),
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
                    rdb_guideinfo_natural_guidence_client.rdb_guideinfo_natural_guidence_client(),
                    rdb_guideinfo_towardname_client.rdb_guideinfo_towardname_client(),
                    rdb_highway_facility_data_client.rdb_highway_facility_data_client(),
                    rdb_guideinfo_building_structure_client.rdb_guideinfo_building_structure_client(),
                    rdb_guideinfo_caution_client.rdb_guideinfo_caution_client(),
                    rdb_guideinfo_road_structure_client.rdb_guideinfo_road_structure_client(),
                    rdb_guideinfo_signpost_uc_client.rdb_guideinfo_signpost_uc_client(),
                    rdb_guideinfo_forceguide_client.rdb_guideinfo_forceguide_client(),
                    rdb_guideinfo_hook_turn_client.rdb_guideinfo_hook_turn_client(),
    
                    rdb_node_client.rdb_node_client(),
                    rdb_link_client.rdb_link_client(),
                    rdb_admin_client.rdb_admin_client(),
    
                    rdb_view.rdb_view(),
                    rdb_link_with_all_attri_view.rdb_link_with_all_attri_view(),
                    rdb_node_with_all_attri_view.rdb_node_with_all_attri_view(),
                    rdb_hwy.rdb_highway(),
                    rdb_version.rdb_version(),
    				rdb_guideinfo_pic_blob_bytea.rdb_guideinfo_pic_binary()
                    ]
    
        # 当本前继续往下做其他component
        for component in comp_list:
            component.Make()
    
        # Check
        for component in comp_list:
            component.Check()
            pass
        
        rdb_log.log('Middle2RDB', 'Convert Middle data to RDB format data succeeded. Please the check data.', 'info')
    except:
        common.log.common_log.instance().logger('System').exception('error happened in middle2rdb...')
        common.log.common_log.instance().end()
        rdb_log.end()
        
        import time
        time.sleep(1)#avoid same log file name to autocheck
        rdb_common.rdb_exit(1)
        
    common.log.common_log.instance().end()
    rdb_log.end()

    # 正常退硜E并发送mail
    rdb_common.rdb_exit(0)
    return 0

if __name__ == '__main__':
    main()
