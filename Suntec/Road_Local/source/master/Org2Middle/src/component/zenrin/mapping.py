import component.component_base

class comp_mapping_zenrin(component.component_base.comp_base):
    '''
    classdocs
    '''

    def __init__(self):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self, 'Mapping')

    def _DoCreateIndex(self):
        
        'create index.'
        self.CreateIndex2('temp_link_mapping_meshcode_linkno_idx')
        self.CreateIndex2('temp_node_mapping_meshcode_nodeno_idx')
        
    def _DoCreateTable(self):
        self.log.info('Make Mapping table.')
        
        self.CreateTable2('temp_node_mapping')
        self.CreateTable2('temp_link_mapping')
