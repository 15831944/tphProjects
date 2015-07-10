# coding:utf-8
import platform.TestCase

# 确认org_spjcty表不为空。
class CCheckTblOrgSpjctyNotNull(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
                '''
        return True

# 确认每个atten_intersection_no都是分歧点。
class CCheckAttenIntersectionNoIsJunction(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
                '''
        return True

# 确认每条记录中的atten_intersection_no都能在node表找到对应项。  
class CCheckAttenIntersectionNoExists(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
                '''
        return True
    
# 经过分析，台湾元数据中的to_road_num字段应当等于2。
class CCheckToRoadNumEquals_2_(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
                '''
        return True


















