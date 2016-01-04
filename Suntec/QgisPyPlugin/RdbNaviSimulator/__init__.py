# -*- coding: utf-8 -*-

def classFactory(iface):
    from .RdbNaviSimulator import RdbNaviSimulator
    return RdbNaviSimulator(iface)
