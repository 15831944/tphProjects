# -*- coding: utf-8 -*-
import os
from GuideInfoCommonDlg import GuideInfoCommonDlg
from qgis.core import QgsFeatureRequest, QgsFeature

class ForceGuideShowInfoDlg(GuideInfoCommonDlg):
    def __init__(self, theCanvas, theLayer, selFeatureIds, parent=None):
        super(ForceGuideShowInfoDlg, self).__init__(theCanvas, theLayer, selFeatureIds, "Force Guide", parent)
        self.labelOutlinkid.setText("Outlink:")

    def initComboBoxOutlinkid(self):
        while(self.comboBoxOutlinkid.count() > 0):
            self.comboBoxOutlinkid.removeItem(0)
        for oneFeatureId in self.mSelFeatureIds:
            featureIter = self.mTheLayer.getFeatures(QgsFeatureRequest(oneFeatureId).setFlags(QgsFeatureRequest.NoGeometry))
            theFeature = QgsFeature()
            if featureIter.nextFeature(theFeature) == False:
                return
            if self.mIsMyFeature(theFeature):
                self.comboBoxOutlinkid.addItem(str(theFeature.attribute('out_link_id')))

    def mIsMyFeature(self, theFeature):
        return ForceGuideShowInfoDlg.isMyFeature(theFeature)

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
            passlink_count = theFeature.attribute('passlink_count')
            guide_type = theFeature.attribute('guide_type')
            position_type = theFeature.attribute('position_type')
            #order_id = theFeature.attribute('order_id') # jdb 15 autumn has no this column
        except KeyError, kErr:
            return False
        except Exception, ex:
            return False
        return True







