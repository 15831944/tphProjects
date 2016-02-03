#encoding=utf-8
from PyQt4 import QtCore, QtGui, uic
from PyQt4.QtGui import QMessageBox, QGraphicsScene, QPixmap, QGraphicsPixmapItem, QPainter, QPen, QColor, QBrush
from PyQt4.QtCore import QRectF, Qt

class LanePixmapWithTopLeftXY(object):
    def __init__(self, x, y, pixmap):
        self.x = x
        self.y = y
        self.pixmap = pixmap
        return

class LaneShowImageWidget(QtGui.QWidget):
    def __init__(self, parent=None):
        super(LaneShowImageWidget, self).__init__(parent)
        self.lanePixmapList = []

    def initLanePixmaps(self, grayItemList, highlightItemList):
        self.lanePixmapList = []
        p = QPixmap(":/lane/0.png")
        width = p.width()
        height = p.height()
        totalLaneCount = grayItemList[0].lane_num

        blankPixmap = QPixmap(":/lane/lane_background.png")
        for i in range(0, totalLaneCount):
            x = (width+2)*(totalLaneCount-i-1)
            self.lanePixmapList.append(LanePixmapWithTopLeftXY(x, 0, blankPixmap))

        for grayItem in grayItemList:
            for i in range(0, 16):
                if grayItem.lane_info & 2**i:
                    for picPath in grayItem.getPicPath_gray():
                        tempPixmap = QPixmap(picPath)
                        x = (width+2)*(totalLaneCount-i-1)
                        self.lanePixmapList.append(LanePixmapWithTopLeftXY(x, 0, tempPixmap))

        for highlightItem in highlightItemList:
            for i in range(0, 16):
                if highlightItem.lane_info & 2**i:
                    for picPath in highlightItem.getPicPath_highlight():
                        tempPixmap = QPixmap(picPath)
                        x = (width+2)*(totalLaneCount-i-1)
                        self.lanePixmapList.append(LanePixmapWithTopLeftXY(x, 0, tempPixmap))

    def paintEvent(self, event):
        painter = QPainter(self)
        for onePixMap in self.lanePixmapList:
            painter.drawPixmap(onePixMap.x, onePixMap.y, onePixMap.pixmap)
        return
