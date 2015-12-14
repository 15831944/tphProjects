#encoding=utf-8
import os
from PyQt4 import QtCore, QtGui, uic
from PyQt4.QtGui import QMessageBox, QGraphicsScene, QPixmap
from qgis.core import QgsDataSourceURI, QgsFeatureRequest, QgsFeature

FORM_CLASS, _ = uic.loadUiType(os.path.join(os.path.dirname(__file__),
                               'CautionShowInfoDlgDesign.ui'))

class CautionShowInfoDlg(QtGui.QDialog, FORM_CLASS):
    cautionInfoDict = {
        0 : u"未定義",
        1 : u"カーブ案内",
        2 : u"危険案内",
        3 : u"合流分岐案内",
        4 : u"県境案内",
        5 : u"向左急弯路",
        6 : u"向右急弯路",
        7 : u"反向弯路（左）",
        8 : u"反向弯路（右）",
        9 : u"连续弯路",
        10 : u"上陡坡",
        11 : u"下陡坡",
        12 : u"两侧变窄",
        13 : u"右侧变窄",
        14 : u"左侧变窄",
        15 : u"窄桥",
        16 : u"双向交通",
        17 : u"注意儿童",
        18 : u"注意牲畜",
        19 : u"注意落石（左）",
        20 : u"注意落石（右）",
        21 : u"注意横风",
        22 : u"易滑",
        23 : u"傍山险路（左）",
        24 : u"傍山险路（右）",
        25 : u"堤坝路（左）",
        26 : u"堤坝路（右）",
        27 : u"村庄",
        28 : u"驼峰桥",
        29 : u"路面不平",
        30 : u"过水路面",
        31 : u"有人看守铁路道口",
        32 : u"无人看守铁路道口",
        33 : u"左右绕行",
        34 : u"左侧绕行",
        35 : u"右侧绕行",
        36 : u"注意危险",
        37 : u"禁止超车",
        38 : u"解除禁止超车",
        39 : u"鸣喇叭",
        40 : u"连续下坡",
        41 : u"文字性警示标牌（现场为文字提示，且无法归类到国标危险信息标牌中）",
        42 : u"注意左侧合流",
        43 : u"注意右侧合流",
        44 : u"停车让行",
        45 : u"会车让行",
        46 : u"减速让行",
        47 : u"隧道开灯",
        48 : u"潮汐车道",
        49 : u"路面高凸",
        50 : u"路面低洼",
        51 : u"交通意外黑点"
    }
    
    cautionImageDict = {
        0 : ":/caution/0.png",
        1 : ":/caution/1.png",
        2 : ":/caution/2.png",
        3 : ":/caution/3.png",
        4 : ":/caution/4.png",
        5 : ":/caution/5.png",
        6 : ":/caution/6.png",
        7 : ":/caution/7.png",
        8 : ":/caution/8.png",
        9 : ":/caution/9.png",
        10 : ":/caution/10.png",
        11 : ":/caution/11.png",
        12 : ":/caution/12.png",
        13 : ":/caution/13.png",
        14 : ":/caution/14.png",
        15 : ":/caution/15.png",
        16 : ":/caution/16.png",
        17 : ":/caution/17.png",
        18 : ":/caution/18.png",
        19 : ":/caution/19.png",
        20 : ":/caution/20.png",
        21 : ":/caution/21.png",
        22 : ":/caution/22.png",
        23 : ":/caution/23.png",
        24 : ":/caution/24.png",
        25 : ":/caution/25.png",
        26 : ":/caution/26.png",
        27 : ":/caution/27.png",
        28 : ":/caution/28.png",
        29 : ":/caution/29.png",
        30 : ":/caution/30.png",
        31 : ":/caution/31.png",
        32 : ":/caution/32.png",
        33 : ":/caution/33.png",
        34 : ":/caution/34.png",
        35 : ":/caution/35.png",
        36 : ":/caution/36.png",
        37 : ":/caution/37.png",
        38 : ":/caution/38.png",
        39 : ":/caution/39.png",
        40 : ":/caution/40.png",
        41 : ":/caution/41.png",
        42 : ":/caution/42.png",
        43 : ":/caution/43.png",
        44 : ":/caution/44.png",
        45 : ":/caution/45.png",
        46 : ":/caution/46.png",
        47 : ":/caution/47.png",
        48 : ":/caution/48.png",
        49 : ":/caution/49.png",
        50 : ":/caution/50.png",
        51 : ":/caution/51.png"
    }

    def __init__(self, theCanvas, theLayer, category="Caution",  parent=None):
        super(CautionShowInfoDlg, self).__init__(parent)
        self.setupUi(self)
        self.mTheCanvas = theCanvas
        self.mTheLayer = theLayer
        self.mAllFeatureIds = []
        self.mCategory = category
        self.setWindowTitle(self.mCategory)
        self.graphicsViewCaution.setScene(QGraphicsScene())

        featureIter = self.mTheLayer.getFeatures(QgsFeatureRequest().setFlags(QgsFeatureRequest.NoGeometry))
        inti = 0
        theFeature = QgsFeature()
        while(featureIter.nextFeature(theFeature) and inti<512):
            inti += 1
            self.mAllFeatureIds.append(theFeature.id())
        self.spinBoxFeatureIndex.setValue(1)
        self.spinBoxFeatureIndex.setMinimum(1)
        self.spinBoxFeatureIndex.setMaximum(inti)

        errMsg = ['']
        self.initcomboBoxInlinkid()

        self.showFeatureDetail(errMsg, self.mTheLayer.selectedFeatures()[0])
        self.comboBoxInlinkid.setFocus()

        self.pushButtonPrev.clicked.connect(self.onPushButtonPrev)
        self.pushButtonNext.clicked.connect(self.onPushButtonNext)
        self.connect(self.comboBoxInlinkid, QtCore.SIGNAL('activated(QString)'), self.comboBoxInlinkChanged)

    def disableAllControls(self):
        self.pushButtonPrev.setEnabled(False)
        self.pushButtonPrev.setEnabled(False)
        self.comboBoxInlinkid.setEnabled(False)
        self.textEditFeatureInfo.setEnabled(False)
        return

    def showFeatureDetail(self, errMsg, theFeature):
        strFeatureInfo = self.getFeatureInfoString(theFeature)
        self.textEditFeatureInfo.setText(strFeatureInfo)
        self.lineEditCautionInfo.setText(CautionShowInfoDlg.cautionInfoDict[theFeature.attribute('data_kind')])
        scene = self.graphicsViewCaution.scene()
        for oneItem in scene.items():
            scene.removeItem(oneItem)
        scene.addPixmap(QPixmap(CautionShowInfoDlg.cautionImageDict[theFeature.attribute('data_kind')]))
        return
    
    def getFeatureInfoString(self, theFeature):
        fieldList = theFeature.fields()
        attrList = theFeature.attributes()
        strFeatureInfo = "field count: %d\n" % len(fieldList)
        for oneField, oneAttr in zip(fieldList, attrList):
            if isinstance(oneAttr, float):
                strFeatureInfo += "%s: %.0f\n" % (oneField.name(), oneAttr)
            else:
                strFeatureInfo += "%s: %s\n" % (oneField.name(), oneAttr)
        return strFeatureInfo

    def comboBoxInlinkChanged(self, txt):
        errMsg = ['']
        inti = self.comboBoxInlinkid.currentIndex()
        self.selectFeatureById(errMsg, self.mSelFeatureIds[inti])
        if errMsg[0] <> '':
            QMessageBox.information(self, "Lane Info", """error:\n%s"""%errMsg[0])
            return
        return

    def onPushButtonPrev(self):
        self.spinBoxFeatureIndex.setValue(self.spinBoxFeatureIndex.value()-1)
        prevFeatureId = self.mAllFeatureIds[self.spinBoxFeatureIndex.value()-1]
        errMsg = ['']
        self.selectFeatureById(errMsg, prevFeatureId)
        if errMsg[0] <> '':
            QMessageBox.information(self, "Lane Info", """error:\n%s"""%errMsg[0])
            return
        return

    def onPushButtonNext(self):
        self.spinBoxFeatureIndex.setValue(self.spinBoxFeatureIndex.value()+1)
        nextFeatureId = self.mAllFeatureIds[self.spinBoxFeatureIndex.value()-1]
        errMsg = ['']
        self.selectFeatureById(errMsg, nextFeatureId)
        if errMsg[0] <> '':
            QMessageBox.information(self, "Lane Info", """error:\n%s"""%errMsg[0])
            return
        return

    def selectFeatureById(self, errMsg, featureId):
        self.mTheLayer.removeSelection()
        self.mTheLayer.select(featureId)
        center = self.mTheCanvas.zoomToSelected(self.mTheLayer)
        self.mTheCanvas.refresh()
        self.showFeatureDetail(errMsg, self.mTheLayer.selectedFeatures()[0])
        if errMsg[0] <> '':
            return
        return

    def initcomboBoxInlinkid(self):
        self.labelContentOfCombobox.setText("Inlinkid:")
        while(self.comboBoxInlinkid.count() > 0):
            self.comboBoxInlinkid.removeItem(0)
        for oneFeatureId in self.featureIdList:
            featureIter = self.mTheLayer.getFeatures(QgsFeatureRequest(oneFeatureId).setFlags(QgsFeatureRequest.NoGeometry))
            theFeature = QgsFeature()
            if featureIter.nextFeature(theFeature) == False:
                return
            if self.mIsMyFeature(theFeature):
                self.comboBoxInlinkid.addItem(str(oneFeature.attribute('in_link_id')))

    def mIsMyFeature(self, theFeature):
        return CautionShowInfoDlg.isMyFeature(theFeature)

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
            type_code = theFeature.attribute('passlink_count')
            type_code_priority = theFeature.attribute('data_kind')
            centroid_lontitude = theFeature.attribute('voice_id')
            centroid_lantitude = theFeature.attribute('strtts')
            building_name_bak = theFeature.attribute('image_id')
            order_id = theFeature.attribute('order_id')
        except KeyError, kErr:
            return False
        except Exception, ex:
            return False
        return True







