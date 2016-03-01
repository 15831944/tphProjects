# -*- coding: utf-8 -*-
def classFactory(iface):
    from .NostraIllustCheck import NostraIllustCheck
    return NostraIllustCheck(iface)
