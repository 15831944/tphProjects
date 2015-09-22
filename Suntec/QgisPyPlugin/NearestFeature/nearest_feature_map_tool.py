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
        
        layerData = []
        for oneLayer in self.canvas.layers():
            if oneLayer.type() != QgsMapLayer.VectorLayer:
                # Ignore this oneLayer as it's not a vector
                continue
            
            if oneLayer.featureCount() == 0:
                # There are no features - skip
                continue
            
            oneLayer.removeSelection()
            
            # Determine the location of the click in real-world coords
            layerPoint = self.toLayerCoordinates(oneLayer, mouseEvent.pos())
            shortestDistance = float("inf")
            featureIdInLayer = -1
            
            # Loop through all features in the oneLayer
            mouseClickGeom = QgsGeometry.fromPoint(layerPoint)
            for oneFeature in oneLayer.getFeatures():
                dist = oneFeature.geometry().distance(mouseClickGeom)
                if dist < shortestDistance:
                    shortestDistance = dist
                    featureIdInLayer = oneFeature.id()
            
            info = (oneLayer, featureIdInLayer, shortestDistance)
            layerData.append(info)
                
        if len(layerData) <= 0:
            # Looks like no vector layers were found - do nothing
            return
        
        # Sort the oneLayer information by shortest distance
        layerData.sort(key=lambda element: element[2])
        
        # Select the closest feature
        theLayer, featureIdInLayer, shortestDistance = layerData[0]
        theLayer.select(featureIdInLayer)
        theLayer.select()
        
        
        QMessageBox.information(self.canvas, 
                                QCoreApplication.translate('HelloWorld', "HelloWorld"), 
                                QCoreApplication.translate('HelloWorld', "HelloWorld"))
        















