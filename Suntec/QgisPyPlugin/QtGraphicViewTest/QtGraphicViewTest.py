import sys
from PyQt4 import QtCore
from PyQt4.QtGui import QApplication, QGraphicsView, QGraphicsScene, QPixmap

def main():
    app = QApplication(sys.argv)
    mainView = QGraphicsView()
    scene = QGraphicsScene()
    pixmap = QPixmap(r":/8a130311.png")
    scene.addPixmap(pixmap.scaled(100, 100, 
                                  QtCore.Qt.IgnoreAspectRatio,
                                  QtCore.Qt.SmoothTransformation))
    mainView.setScene(scene)
    mainView.show()
    app.exec_()

if __name__ == "__main__":
    main()



