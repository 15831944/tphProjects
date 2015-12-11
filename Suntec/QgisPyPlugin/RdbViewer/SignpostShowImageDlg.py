# -*- coding: utf-8 -*-
import os
from SpotguideShowImageDlg import SpotguideShowImageDlg
class SignpostShowImageDlg(SpotguideShowImageDlg):

    def __init__(self, theCanvas, theLayer, parent=None):
        super(SignpostShowImageDlg, self).__init__(theCanvas, theLayer, parent)

    def mIsMyFeature(self, theFeature):
        return SignpostShowImageDlg.isMyFeature(theFeature)

    @staticmethod
    def isMyFeature(theFeature):
        try:
            gid = theFeature.attribute('gid')
            in_link_id = theFeature.attribute('in_link_id')
            in_link_id_t = theFeature.attribute('in_link_id_t')
            node_id = theFeature.attribute('node_id')
            node_id_t = theFeature.attribute('node_id_t')
            out_link_id = theFeature.attribute('out_link_id')
            out_link_id_t = theFeature.attribute('out_link_id_t')
            type = theFeature.attribute('sp_name')
            passlink_count = theFeature.attribute('passlink_count')
            is_exist_sar = theFeature.attribute('is_pattern')
            pattern_id = theFeature.attribute('pattern_id')
            arrow_id = theFeature.attribute('arrow_id')
            order_id = theFeature.attribute('order_id')
        except KeyError, kErr:
            return False
        except Exception, ex:
            return False
        return True



