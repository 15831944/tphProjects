from PyQt4 import QtCore, QtGui
from PyQt4.QtCore import *
from PyQt4.QtGui import *

if __name__ == "__main__":
    import sys
    basePath = r"C:\My\20151027_ni_15summer_illust\HKMC\2D\pattern\8a130311.png"
    app = QApplication(sys.argv)
    mainView = QGraphicsView()
    scene = QGraphicsScene()
    scene2 = QGraphicsScene()
    scene.addPixmap(QPixmap(basePath).scaled(100, 100))
    mainView.setScene(scene)
#    for oneItem in scene.items():
#        scene.removeItem(oneItem)
    mainView.setScene(scene2)
    mainView.setScene(scene)
    mainView.show()
    app.exec_()

