'''
Created on 2012-3-21

@author: zhangliang
'''
import base
import subproj
import component

class proj_factory(object):
    '''
       This is project factory , it's determined which sub-project is running
    '''
    __instance = None

    def __init__(self):
        '''
        Constructor
        '''

    @staticmethod
    def instance(proj_name):
        if proj_factory.__instance is None:
            if proj_name is not None:
                if proj_name == 'proj_chn' :
                    proj_factory.__instance = subproj.proj_chn.proj_chn_module(
                                                                               component.comp_factory_chn()
                                                                               )
                    pass
                elif proj_name == 'proj_jpn' :
                    proj_factory.__instance = subproj.proj_jpn.proj_jpn_module(
                                                                               component.comp_factory_jpn()
                                                                               )
                    pass
                elif proj_name == 'proj_jdb' :
                    proj_factory.__instance = subproj.proj_jdb.proj_jdb_module(
                                                                               component.comp_factory_jdb()
                                                                               )
                    pass
                elif proj_name == 'proj_rdf' :
                    proj_factory.__instance = subproj.proj_rdf.proj_rdf_module(
                                                                               component.comp_factory_rdf()
                                                                               )
                elif proj_name == 'proj_axf' :
                    proj_factory.__instance = subproj.proj_axf.proj_axf_module(
                                                                               component.comp_factory_axf()
                                                                               )
                elif proj_name == 'proj_ta' :
                    proj_factory.__instance = subproj.proj_ta.proj_ta_module(
                                                                               component.comp_factory_ta()
                                                                               )
                elif proj_name == 'proj_nostra':
                    from component import component_factory
                    from subproj.proj_nostra import proj_nostra_module
                    proj_factory.__instance = proj_nostra_module(
                                                                 component_factory.comp_factory_nostra()
                                                                 )
                elif proj_name == 'proj_mmi':
                    from component.component_factory import comp_factory_mmi
                    from subproj.proj_mmi import proj_mmi_module
                    proj_factory.__instance = proj_mmi_module(comp_factory_mmi())
                elif proj_name == 'proj_msm':  # Malsing
                    from component.component_factory import comp_factory_msm
                    from subproj.proj_msm import proj_msm_module
                    comp_factory = comp_factory_msm()
                    proj_factory.__instance = proj_msm_module(comp_factory)
                else:  # proj_base
                    proj_factory.__instance = base.proj_base.proj_base_module()

        return proj_factory.__instance
