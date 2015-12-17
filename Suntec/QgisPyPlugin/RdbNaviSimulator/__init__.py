# -*- coding: utf-8 -*-
def classFactory(iface):
    from .RdbViewer import RdbViewer
    return RdbViewer(iface)
