# -*- coding: utf-8 -*-
"""
/***************************************************************************
 NearestFeature
                                 A QGIS plugin
 Selects the nearest feature.
                             -------------------
        begin                : 2014-10-15
        copyright            : (C) 2014 by Peter Wells for Lutra Consulting
        email                : info@lutraconsulting.co.uk
        git sha              : $Format:%H$
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
 This script initializes the plugin, making it known to QGIS.
"""


# noinspection PyPep8Naming
def classFactory(iface):  # pylint: disable=invalid-name
    """Load NearestFeature class from file NearestFeature.

    :param iface: A QGIS interface instance.
    :type iface: QgsInterface
    """
    #
    from .nearest_feature import NearestFeature
    return NearestFeature(iface)
