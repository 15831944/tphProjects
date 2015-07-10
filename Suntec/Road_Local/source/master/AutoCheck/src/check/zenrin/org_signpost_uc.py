# coding:utf-8
import platform.TestCase

'''
********************************************************************************
以下检查针对表org_cross_name
元数据协议：《汎用シェープ台湾版_仕様書.pdf》或《Specification_TaiwanShape.pdf》第2.1.16
********************************************************************************
'''
# 确认org_cross_name表不为空。
class CCheckTblOrgCrossNameNotNull(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
                '''
        return True

# 确认org_cross_name.fromnodeno在node表中存在
class CCheckOrgCrossNameFromNodeNoExists(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
                '''
        return True

# 确认org_cross_name.tonodeno在node表中存在
class CCheckOrgCrossNameToNodeNoExists(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
                '''
        return True
    
# 确认当org_cross_name.nextflg标记了有多条name或reading时，必定可以找到每条记录
class CCheckOrgCrossName_Name_Reading(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
                '''
        return True

'''
********************************************************************************
以下检查针对表org_dest_guide
元数据协议：《汎用シェープ台湾版_仕様書.pdf》或《Specification_TaiwanShape.pdf》第2.1.17
********************************************************************************
'''
# 确认org_dest_guide表不为空。
class CCheckTblOrgDestGuideNotNull(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
                '''
        return True

# 确认org_dest_guide.stotlinkno在link表中存在
class CCheckOrgDestGuideSTotLinkNoExists(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
                '''
        return True

# 确认org_dest_guide.ttoelinkno在link表中存在
class CCheckOrgDestGuideTToeLinkNoExists(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
                '''
        return True
    
# 确认org_dest_guide.snodeno在node表中存在
class CCheckOrgDestGuideSNodeNoExists(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
                '''
        return True

# 确认org_dest_guide.tnodeno在node表中存在
class CCheckOrgDestGuideTNodeNoExists(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
                '''
        return True

# 确认org_dest_guide.enodeno在node表中存在
class CCheckOrgDestGuideENodeNoExists(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
                '''
        return True

# 确认当org_dest_guide.nextflg标记了有多条name或reading时，必定可以找到每条记录
class CCheckOrgDestGuide_Name_Reading(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
                '''
        return True
    
# 台湾元数据提供了inlink和outlink，检查这两条link能够走通。
class CCheckOrgDestGuide_InlinkOutlinkConnected(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
                '''
        return True













