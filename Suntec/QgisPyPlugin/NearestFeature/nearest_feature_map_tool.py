# -*- coding: utf-8 -*-
from qgis.gui import QgsMapTool
from qgis.core import QgsMapLayer, QgsMapToPixel, QgsFeature, QgsFeatureRequest, QgsGeometry
from PyQt4.QtGui import QCursor, QPixmap, QMessageBox
from PyQt4.QtCore import Qt, QCoreApplication
from myDbManager import myDbManager


class NearestFeatureMapTool(QgsMapTool):
    
    def __init__(self, canvas):
        
        super(QgsMapTool, self).__init__(canvas)
        self.canvas = canvas
        self.cursor = QCursor(Qt.ArrowCursor)
        
    def activate(self):
        self.canvas.setCursor(self.cursor)
    
    def screenToLayerCoords(self, screenPos, layer):
        transform = self.canvas.getCoordinateTransform()
        canvasPoint = QgsMapToPixel.toMapCoordinates(transform, screenPos.x(), screenPos.y())
        # Transform if required
        layerEPSG = layer.crs().authid()
        projectEPSG = self.canvas.mapRenderer().destinationCrs().authid()
        if layerEPSG != projectEPSG:
            renderer = self.canvas.mapRenderer()
            layerPoint = renderer.mapToLayerCoordinates(layer, canvasPoint)
        else:
            layerPoint = canvasPoint
        
        # Convert this point (QgsPoint) to a QgsGeometry
        return QgsGeometry.fromPoint(layerPoint)


    def canvasReleaseEvent(self, mouseEvent):
        """ 
        Each time the mouse is clicked on the map canvas, perform 
        the following tasks:
            Loop through all visible vector layers and for each:
                Ensure no features are selected
                Determine the distance of the closes feature in the oneLayer to the mouse click
                Keep track of the oneLayer id and id of the closest feature
            Select the id of the closes feature 
        """
        theLayer = self.canvas.currentLayer()
        if theLayer is None:
            return
        if theLayer.type() != QgsMapLayer.VectorLayer:
            return
        if theLayer.featureCount() == 0:
            return
                
        theLayer.removeSelection()
        
        # Determine the location of the click in real-world coords
        layerPoint = self.toLayerCoordinates(theLayer, mouseEvent.pos())
        
        # Loop through all features in the oneLayer
        mouseClickGeom = QgsGeometry.fromPoint(layerPoint)
        theFeature = None
        for oneFeature in theLayer.getFeatures():
            dist = oneFeature.geometry().distance(mouseClickGeom)
            if dist < 0.0002:
                theFeature = oneFeature
                break

        if theFeature is None:
            return
        
        # Select the feature
        theLayer.select(theFeature.id())
        strFeatureInfo = self.getFeatureInfoString(theFeature)

        errMsg = ['']
        dbManager = myDbManager()
        imageData = dbManager.getPictureBinaryData(errMsg, theLayer, theFeature)
        if errMsg[0] != '':
            QMessageBox.information(self.canvas, "warnning", errMsg[0])
            return

        from show_image_dialog import ShowImageDialog
        dlg = ShowImageDialog(imageData, strFeatureInfo)
        dlg.show()
        result = dlg.exec_()
        if result:
            pass
        else:
            pass
        return

    def getFeatureInfoString(self, theFeature):
        fieldList = theFeature.fields()
        attrList = theFeature.attributes()
        strFeatureInfo = "field count: %d\n" % len(fieldList)
        for oneField, oneAttr in zip(fieldList, attrList):
            strFeatureInfo += "%s: %s\n" % (oneField.name(), oneAttr)
        return strFeatureInfo
        















