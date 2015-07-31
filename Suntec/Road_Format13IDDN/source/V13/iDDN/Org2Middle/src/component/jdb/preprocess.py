'''
Created on 2012-2-23

@author: sunyifeng
'''

import base

class comp_proprocess_jdb(base.component_base.comp_base):
    
    def __init__(self):
        '''
        Constructor
        '''
        base.component_base.comp_base.__init__(self, 'PreProcess') 
            
    def _Do(self):
        # common functions
        self.CreateFunction2('mid_get_json_string_for_name_japan')
        self.CreateFunction2('mid_get_json_string_for_routeno_japan')
        self.CreateFunction2('mid_get_json_string_for_japan_name')
        self.CreateFunction2('mid_get_json_string_for_japan_names')
        self.CreateFunction2('mid_get_json_string_for_japan_routeno')

        self.CreateFunction2('mid_get_json_string_for_name')
        self.CreateFunction2('mid_get_json_string_for_list')
                
        return 0
