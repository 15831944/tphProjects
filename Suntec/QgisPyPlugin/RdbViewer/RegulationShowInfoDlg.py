# -*- coding: utf-8 -*-
import os
from GuideInfoCommonDlg import GuideInfoCommonDlg
class RegulationShowInfoDlg(GuideInfoCommonDlg):
    def __init__(self, theCanvas, theLayer, parent=None):
        super(RegulationShowInfoDlg, self).__init__(theCanvas, theLayer, "Regulation", parent)

    def initComboBoxOutlinkid(self):
        self.labelOutlinkid.setText("Record No.:")
        while(self.comboBoxOutlinkid.count() > 0):
            self.comboBoxOutlinkid.removeItem(0)
        for oneFeature in self.mTheLayer.selectedFeatures():
            if self.mIsMyFeature(oneFeature):
                self.comboBoxOutlinkid.addItem("%.0f" % oneFeature.attribute('record_no'))

    def mIsMyFeature(self, theFeature):
        return RegulationShowInfoDlg.isMyFeature(theFeature)

    @staticmethod
    def isMyFeature(theFeature):
        try:
            record_no = theFeature.attribute('record_no')
            regulation_id = theFeature.attribute('regulation_id')
            regulation_type = theFeature.attribute('regulation_type')
            is_seasonal = theFeature.attribute('is_seasonal')
            first_link_id = theFeature.attribute('first_link_id')
            first_link_id_t = theFeature.attribute('first_link_id_t')
            last_link_id = theFeature.attribute('last_link_id')
            last_link_dir = theFeature.attribute('last_link_dir')
            last_link_id_t = theFeature.attribute('last_link_id_t')
            link_num = theFeature.attribute('link_num')
            key_string = theFeature.attribute('key_string')
            order_id = theFeature.attribute('order_id')
        except KeyError, kErr:
            return False
        except Exception, ex:
            return False
        return True







