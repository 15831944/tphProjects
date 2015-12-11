# -*- coding: utf-8 -*-
import os
from GuideInfoCommonDlg import GuideInfoCommonDlg
class BuildingStructureShowInfoDlg(GuideInfoCommonDlg):
    def __init__(self, theCanvas, theLayer, parent=None):
        super(BuildingStructureShowInfoDlg, self).__init__(theCanvas, theLayer, "Building Structure", parent)

    def initcomboBoxRecordNo(self):
        while(self.comboBoxRecordNo.count() > 0):
            self.comboBoxRecordNo.removeItem(0)
        for oneFeature in self.mTheLayer.selectedFeatures():
            if self.mIsMyFeature(oneFeature):
                self.comboBoxRecordNo.addItem("%.0f" % oneFeature.attribute('record_no'))

    def mIsMyFeature(self, theFeature):
        return RegulationShowInfoDlg.isMyFeature(theFeature)

    @staticmethod
    def isMyFeature(theFeature):
        try:
            guideinfo_id = theFeature.attribute('guideinfo_id')
            in_link_id = theFeature.attribute('in_link_id')
            in_link_id_t = theFeature.attribute('in_link_id_t')
            node_id = theFeature.attribute('node_id')
            node_id_t = theFeature.attribute('node_id_t')
            out_link_id = theFeature.attribute('out_link_id')
            out_link_id_t = theFeature.attribute('out_link_id_t')
            type_code = theFeature.attribute('type_code')
            type_code_priority = theFeature.attribute('type_code_priority')
            centroid_lontitude = theFeature.attribute('centroid_lontitude')
            centroid_lantitude = theFeature.attribute('centroid_lantitude')
            building_name_bak = theFeature.attribute('building_name_bak')
            order_id = theFeature.attribute('order_id')
            gid = theFeature.attribute('gid')
            building_name = theFeature.attribute('building_name')
        except KeyError, kErr:
            return False
        except Exception, ex:
            return False
        return True







