# -*- coding: utf-8 -*-
import os
from GuideInfoCommonDlg import GuideInfoCommonDlg
from qgis.core import QgsFeatureRequest, QgsFeature

class BuildingStructureShowInfoDlg(GuideInfoCommonDlg):
    def __init__(self, theCanvas, theLayer, selFeatureIds, parent=None):
        self.labelOutlinkid.setText("Node ID:")
        super(BuildingStructureShowInfoDlg, self).__init__(theCanvas, theLayer, selFeatureIds, "Building Structure", parent)

    def initComboBoxOutlinkid(self):
        while(self.comboBoxOutlinkid.count() > 0):
            self.comboBoxOutlinkid.removeItem(0)
        for oneFeatureId in self.mSelFeatureIds:
            featureIter = self.mTheLayer.getFeatures(QgsFeatureRequest(oneFeatureId).setFlags(QgsFeatureRequest.NoGeometry))
            theFeature = QgsFeature()
            if featureIter.nextFeature(theFeature) == False:
                return
            if self.mIsMyFeature(theFeature):
                self.comboBoxOutlinkid.addItem(str(theFeature.attribute('node_id')))

    def mIsMyFeature(self, theFeature):
        return BuildingStructureShowInfoDlg.isMyFeature(theFeature)

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







