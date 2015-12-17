from PyQt4 import QtCore, QtGui
from PyQt4.QtCore import *
from PyQt4.QtGui import *

if __name__ == "__main__":
    import sys
    basePath = r"C:\Users\hexin\.qgis2\python\plugins\QtGraphicViewTest\icons"
    app = QApplication(sys.argv)
    mainView = QGraphicsView()
    scene = QGraphicsScene()
    scene2 = QGraphicsScene()
    scene.addPixmap(QPixmap(basePath+r"\1.png"))
    scene.addPixmap(QPixmap(basePath+r"\2.png"))
    mainView.setScene(scene)
    scene.addPixmap(QPixmap(basePath+r"\4.png"))
#    for oneItem in scene.items():
#        scene.removeItem(oneItem)
    mainView.setScene(scene2)
    mainView.setScene(scene)
    mainView.show()
    mainView.setWindowTitle("aoe")
    app.exec_()



