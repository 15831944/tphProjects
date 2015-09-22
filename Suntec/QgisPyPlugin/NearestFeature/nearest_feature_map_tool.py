# -*- coding: utf-8 -*-
"""
/***************************************************************************
 NearestFeature
                                 A QGIS plugin
 Selects the nearest feature.
                              -------------------
        begin                : 2014-10-15
        git sha              : $Format:%H$
        copyright            : (C) 2014 by Peter Wells for Lutra Consulting
        email                : info@lutraconsulting.co.uk
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
"""
from qgis.gui import QgsMapTool
from qgis.core import QgsMapLayer, QgsMapToPixel, QgsFeature, QgsFeatureRequest, QgsGeometry
from PyQt4.QtGui import QCursor, QPixmap, QMessageBox
from PyQt4.QtCore import Qt, QCoreApplication
import time

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
        time1 = time.time()
        
        theLayer = self.canvas.currentLayer()
        if theLayer.type() != QgsMapLayer.VectorLayer:
            return
        if theLayer.featureCount() == 0:
            return
                
        theLayer.removeSelection()
        
        # Determine the location of the click in real-world coords
        layerPoint = self.toLayerCoordinates(theLayer, mouseEvent.pos())
        shortestDistance = float("inf")
        
        # Loop through all features in the oneLayer
        mouseClickGeom = QgsGeometry.fromPoint(layerPoint)
        theFeature = None
        for oneFeature in theLayer.getFeatures():
            dist = oneFeature.geometry().distance(mouseClickGeom)
            if dist < shortestDistance:
                shortestDistance = dist
                theFeature = oneFeature
            if shortestDistance < 0.001:
                break
        
        time2 = time.time()
        
        # Select the closest feature
        theLayer.select(theFeature.id())
        time3 = time.time()
        
        fieldList = theFeature.fields()
        attrList = theFeature.attributes()
        strFields = ''
        for oneField, oneQVariant in zip(fieldList, attrList):
            strFields += "%s: %s\n" % (oneField.name(), str(oneQVariant))
        QMessageBox.information(self.canvas, "Feature Fields", strFields)
        
        strFmt = "feature id: %d\ntime1: %s\ntime2: %s, delta: %s\ntime3: %s, delta: %s." 
        strTime = strFmt%(theFeature.id(), time1, time2, time2-time1, time3, time3-time2)
        QMessageBox.information(self.canvas, "Time Statistic", strTime)
        















