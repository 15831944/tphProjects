# -*- coding: UTF8 -*-
'''
Created on 2012-12-10
CrossName��
@author: hongchenzai
'''
import rdb_common_check
    
class CCheckTowardNameAttr(rdb_common_check.CCheckClientColumn):
    '''检查名称属性和种别'''
    def _getCondition(self):
        # 一般server和 client字段同名
        condition = """
                (name_attr <> (multi_towardname_attr & 15)) or 
                (name_kind <> ((multi_towardname_attr >> 4) & 3)) or
                (passlink_count <> (multi_towardname_attr >> 6))
            """
        return condition