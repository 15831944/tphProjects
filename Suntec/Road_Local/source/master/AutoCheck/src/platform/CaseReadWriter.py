# -*- coding: UTF8 -*-
'''
Created on 2012-11-29

@author: hongchenzai
'''
import os
import shutil

import common.Logger
import common.ConfigReader
###############################################################################
## 测试用例的读入写出类
###############################################################################
class CCaseReadWriter(object):
    '''测试用例的读入写出类。
    '''

    def __init__(self, model_path):
        '''
        Constructor
        '''
        self.__suiteInfoList = []          # 一张表对应一个CSuiteInfo实例
        self.__path          = self.__generateResultFile(model_path)        # excel的文件路径及名称
    
    def __generateResultFile(self, sourceFile):
        import os
        import time
        if os.path.isfile(sourceFile):
            (host, dbname) = common.ConfigReader.CConfigReader.instance().getDBInfo()
            sourcefileName = os.path.split(sourceFile)[1]
            pos = sourcefileName.rfind('.')
            targetFileName = sourcefileName[0:pos] + '_' + host + '_' + dbname + '_' +\
                        time.strftime('%Y%m%d%H%M%S',time.localtime(time.time())) + \
                        sourcefileName[pos:]
            targetFile = os.path.join(os.pardir, 'report', targetFileName)
            #open(targetFile, "wb").write(open(sourceFile, "rb").read())
            shutil.copyfile(sourceFile, targetFile)
        
        return targetFile
   
    def getSuiteInfoList(self):
        '''取得测试用例基本信息列表(不包含Summary表)。'''
        import copy
        if len(self.__suiteInfoList) == 0:
            # 从excel表载入用例信息
            self._loadCaseInfo()
        temp_suiteInfoList = []
        for suiteInfo in self.__suiteInfoList[:]:
            if suiteInfo.getSuiteName().lower() != 'summary':
                temp_suiteInfoList.append(suiteInfo)
        return copy.deepcopy(temp_suiteInfoList)
    
    def getSummarySuiteInfo(self):
        '''读取Summary信息表。'''
        if len(self.__suiteInfoList) == 0:
            # 从excel表载入用例信息
            self._loadCaseInfo()
            
        for suiteInfo in self.__suiteInfoList[:]:
            if suiteInfo.getSuiteName().lower() == 'summary':
                summaryInfoList = []
                # 把caseInfo转成SummaryInfo
                for caseinfo in suiteInfo.getCaseInfoList()[:]:
                    summaryInfoList.append(CSummaryInfo(caseinfo))
                return CSuiteInfo(suiteInfo.getSuiteName(), summaryInfoList)
        return None
    
    def saveCaseListReport(self, SuiteInfoList = []):
        '''把check的结果写到excel表'''
        if SuiteInfoList == None: 
            return
                
        objExcel  = CExcel2(self.__path)
        # 所有sheet
        for objSuiteInfo in SuiteInfoList:
            CCell_list = []
            # 一个sheet里，所有测试用例
            for objCaseinfo in objSuiteInfo.getCaseInfoList():
                # 一个用例里，值发生变更的单元格
                for ojbCCell in objCaseinfo.getValueChangedList():
                    CCell_list.append(ojbCCell)
            # 一个sheet, 写一次
            objExcel.writeCaseInfoList(objSuiteInfo.getSuiteName(), CCell_list)
        return
      
    def saveSummaryReport(self, SummarySuiteInfo):
        if SummarySuiteInfo == None: 
            return
        objExcel  = CExcel2(self.__path)
        CCell_list = []
        # 一个sheet里，所有测试用例
        for objSummaryInfo in SummarySuiteInfo.getCaseInfoList():
            # 一个用例里，值发生变更的单元格
            for ojbCCell in objSummaryInfo.getValueChangedList():
                CCell_list.append(ojbCCell)
        # 一个sheet, 写一次
        objExcel.writeCaseInfoList(SummarySuiteInfo.getSuiteName(), CCell_list)
        return
    
    def _loadCaseInfo(self):
        '''从excel表载入用例信息。'''
                
        objExcel = CExcel2(self.__path)
        # 取得所有sheet
        for sheet_name in objExcel.sheet_names():
            ojbCaseInfoList = []
            case_info_list  = objExcel.getCaseInfoes(sheet_name)
            if case_info_list != None:
                # 用例的字典key
                case_key = case_info_list[0]
            for case_info in case_info_list[1:]:
                info_dict = {}
                for i in range(len(case_key)):
                    key = case_key[i]                 # 种别是CCell
                    info_dict[key] = case_info[i]     # 种别是CCell 
                
                ojbCaseInfoList.append(CCaseInfo(sheet_name, info_dict))
            
            self.__suiteInfoList.append(CSuiteInfo(sheet_name, ojbCaseInfoList))
        return 0        
    
###############################################################################
## CSuiteInfo类
############################################################################### 
class CSuiteInfo(object):
    def __init__(self, strSuiteName="", caseinfolist=[]):
        self.strSuiteName = strSuiteName       # sheet name
        self.caseinfolist = caseinfolist
    
    def setSuiteName(self, strSuiteName):
        self.strSuiteName = strSuiteName
    
    def getSuiteName(self):
        return self.strSuiteName
    
    def addCaseInfo(self, caseinfo):
        self.caseinfolist.append(caseinfo)
    
    def getCaseInfoList(self):
        return self.caseinfolist

###############################################################################
## 测试用例基本信息类
###############################################################################    
class CCaseInfo(object):
    '''测试用例基本信息类
    '''
    
    def __init__(self, table_name = '', infoDict = {}):
        '''
        Constructor
        '''
        self.__table_name = table_name
        self.__InfoDict   = infoDict  # 字典的key和value分别用的是CCell对像
        #if self.getCaseID() == None:
        #    common.Logger.CLogger.instance().logger('CCaseInfo').warning("UserCase ID is None.")
        
    def getCaseID(self):
        return self.get(u'用例ID')
    
    def setCaseID(self, caseID):
        self.setValue(u'用例ID', caseID)
        
    def getDependCaseIDList(self):
        return self.get(u'依赖用例ID')
    
    def setDependCaseIDList(self, caseIDList):
        self.setValue(u'依赖用例ID', caseIDList)
    
    def isAuto(self):
        return self.get(u'是否自动化') == "Y"
    
    def isProj(self, name):

        if not self.get(u'项目'):
            return True
        
        if self.get(u'项目').find('-'+name) >= 0:
            return False
        
        if (self.get(u'项目').find('COMMON') >= 0 or self.get(u'项目').find(name) >= 0):
            return True
            
        return False

    def isCountry(self, name):

        if not self.get(u'国家'):
            return True
        
        if self.get(u'国家').find('-'+name) >= 0:
            return False
        
        if (self.get(u'国家').find('COMMON') >= 0 or self.get(u'国家').find(name) >= 0):
            return True
            
        return False
        
    def getCheckCode(self):
        return self.get(u'Check代码')
    
    def setCheckCode(self, codes):
        self.setValue(u'Check代码', codes)
    
    def getCheckResult(self):
        return self.get(u'测试结果')
    
    def setCheckResult(self, rVal):
        self.setValue(u'测试结果', rVal)
        
    def getMajorItem(self):
        return self.get(u'大项')
    
    def getCheckContent(self):
        return self.get(u'check内容')
    
    def getCheckMethod(self):
        return self.get(u'Check方法')
    
    def getTableName(self):
        return self.__table_name
    
    def getInfoDict(self):
        import copy
        return copy.deepcopy(self.__InfoDict)
    
    def get(self, key):
        for cell in self.__InfoDict.keys():
            if cell.getValue() == key:
                return self.__InfoDict.get(cell).getValue()
        return None
    
    def keys(self):
        #return self.__InfoDict.keys()
        return [cell.getValue() for cell in self.__InfoDict.keys()]
    
    def values(self):
        return [cell.getValue() for cell in self.__InfoDict.itervalues()]
    
    def __getKeyCell(self, key, row = 0, col = 0):
        for cell in self.__InfoDict.keys():
            if cell.getValue() == key:
                return cell
        return None
    
    def setValue(self, key, value):
        key_cell = self.__getKeyCell(key)
        if key_cell:
            self.__InfoDict[key_cell].setValue(value)
            #print self.__InfoDict[key_cell].getCellValue()
        else:
            common.Logger.CLogger.instance().\
            logger('CCaseInfo').warning('No Keyword <%s>', key)
            self.__InfoDict[CCell(key)] = CCell(value)
            #print self.__InfoDict[self.__getKeyCell(key)].getCellValue()
            
    def getValueChangedList(self):
        # 返回变更过的CCell
        objCCell_list = []
        for objCCell in self.__InfoDict.values():
            if objCCell.getChangedFlag() == True:
                objCCell_list.append(objCCell)
        return objCCell_list
        
###############################################################################
## Summary基本信息类
###############################################################################
class CSummaryInfo(CCaseInfo):
    def __init__(self, CaseInfo):
        '''
        Constructor
        '''
        CCaseInfo.__init__(self, 'Summary', CaseInfo.getInfoDict())  # 字典的key和value分别用的是CCell对像
    
    def getSuiteName(self):
        return self.get(u'表名')
      
    def getCaseNum(self):
        return self.get(u'用例总数')
    
    def setCaseNum(self, num):
        self.setValue(u'用例总数', num)
    
    def getAutoCaseNum(self):
        return self.get(u'自动化用例数')
    
    def setAutoCaseNum(self, num):
        self.setValue(u'自动化用例数', num)
        
    def getSucceedCaseNum(self):
        return self.get(u'自动化用例成功数')
        
    def setSucceedCaseNum(self, num):
        self.setValue(u'自动化用例成功数', num)
     
    def getFailedCaseNum(self):
        return self.get(u'自动化用例失败数')
        
    def setFailedCaseNum(self, num):
        self.setValue(u'自动化用例失败数', num)
     
    def getBlockCaseNum(self):
        return self.get(u'自动化用例阻塞数')
        
    def setBlockCaseNum(self, num):
        self.setValue(u'自动化用例阻塞数', num)

###############################################################################
## CCell
###############################################################################     
class CCell(object):
    def __init__(self, value = '', row = 0, col = 0):
        self.__value       = value  # 值
        self.__row         = row    # 对应所在 excel行号
        self.__col         = col    # 对应所在 excel列号
        self.__changed_flg = False  # 默认是False, 值变更一次之后设为True
        
    def getValue(self):
        return self.__value
    
    def setValue(self, value):
        self.__value       = value
        self.__changed_flg = True
        
    def getChangedFlag(self):
        return self.__changed_flg
    
    def getCellValue(self):
        return (self.__row, self.__col, self.__value)
        
###############################################################################
## CExcel
###############################################################################    
from xlrd import open_workbook, cellname, XL_CELL_NUMBER
class CExcel(object):
    '''Excel操作类
    '''
    def __init__(self, path = None):
        self.__path       = path         # excel路径
        
    def getCaseInfoes(self, sheet_name):
        '''读取用例信息'''
        import copy
        from mmap import mmap, ACCESS_READ
        
        caseinfo_list = []
        # 使用with，可以自动close
        with open(self.__path,'rb') as f:
            wb = open_workbook(file_contents=mmap(f.fileno(), 0, access=ACCESS_READ))
            
            sheet = wb.sheet_by_name(sheet_name)
            
            if sheet != None:
                title_row = self.__getTitleInfo(sheet)
                case_info = []
                for row_index in range(title_row, sheet.nrows):
                    case_info = []
                    for col_index in range(sheet.ncols):
                        cell = sheet.cell(row_index, col_index)
                        if cell.ctype == XL_CELL_NUMBER:
                            # 转成整数
                            case_info.append(CCell(int(cell.value), row_index, col_index)) # [值， 行， 列]
                        else:
                            case_info.append(CCell(cell.value, row_index, col_index))      # [值， 行， 列]
                    caseinfo_list.append(case_info)
            else:
                common.Logger.CLogger.instance().\
                logger('CExcel').warning("No sheet named <%r>.", sheet_name)       
        return caseinfo_list
    
    def sheet_names(self):
        '''取得所有sheet名称'''
        from mmap import mmap, ACCESS_READ
        # 使用with，可以自动close
        with open(self.__path,'rb') as f:
            wb = open_workbook(file_contents=mmap(f.fileno(), 0, access=ACCESS_READ))
            
        return wb.sheet_names()
    
    def __getTitleInfo(self, sheet):
        '''用例表单的标题信息'''
        from xlrd import empty_cell
        titleInfo = []
        for row_index in range(sheet.nrows):
            for col_index in range(sheet.ncols):
                c = sheet.cell(row_index, col_index)
                if c.value != empty_cell.value:
                    titleInfo.append(c.value) 
            if len(titleInfo) > 0:
                return row_index
            
        return None

    def writeCaseInfo(self, sheet_name, row, col, value):  # 值， 行， 列
        '''写入check结果。本这方法的缺点是: 不能保存Formulae, Names, anything ignored by xlrd'''
        from xlrd import open_workbook, XLRDError
        from xlutils.copy import copy
        from mmap import mmap, ACCESS_READ
        
        #from mmap import mmap, ACCESS_READ
        with open(self.__path,'rb') as f:
            rb = open_workbook(file_contents=mmap(f.fileno(), 0, access=ACCESS_READ), \
                               formatting_info = True)            
            # 取得sheet index
            try:
                sheetx = rb.sheet_names().index(sheet_name)   
            except ValueError:
                common.Logger.CLogger.instance().\
                logger('CExcel').warning("No sheet named <%r>.", sheet_name)
                #raise XLRDError('No sheet named <%r>' % self.sheet_name)
        
            # 通过xlutils.copy转换为可以写的格式  
            wb = copy(rb)
            
        # 通过序号获取的sheet  
        ws = wb.get_sheet(sheetx)
        ws.write(row, col, value)
        wb.save(self.__path)

        return 0
    
    def writeCaseInfoList(self, sheet_name, CCell_list = []):  # (值， 行， 列)
        '''写入check结果。本这方法的缺点是: 不能保存Formulae, Names, anything ignored by xlrd'''
        
        from xlrd import open_workbook, XLRDError
        from xlutils.copy import copy
        from xlwt import easyxf, XFStyle, Font
        from mmap import mmap, ACCESS_READ
        
        if CCell_list == []: 
            return 
        #from mmap import mmap, ACCESS_READ
        with open(self.__path,'rb') as f:
            rb = open_workbook(file_contents=mmap(f.fileno(), 0, access=ACCESS_READ), \
                               formatting_info = True)
            # 取得sheet index
            try:
                #print rb.sheet_names()
                #print sheet_name
                sheetx = rb.sheet_names().index(sheet_name)   
            except ValueError:
                common.Logger.CLogger.instance().\
                logger('CExcel').warning("No sheet named <%s>.", sheet_name)
                #raise XLRDError('No sheet named <%r>' % sheet_name)
                
            # 通过xlutils.copy转换为可以写的格式  
            wb = copy(rb)
            from xlutils.styles import Styles
            styles = Styles(rb)
        #print sheet_name, sheetx
        # 通过序号获取的sheet  
        ws = wb.get_sheet(sheetx)
        rs = rb.sheet_by_index(sheetx)
        for objCCell in CCell_list:
            row, col, value = objCCell.getCellValue()
            
            org_style = styles[rs.cell(row, col)]
            org_font = rb.font_list[org_style.xf.font_index]
            style    = self._getStyle(org_style, org_font)            

            ws.write(row, col, value, style)
            print dir(ws)
            #print row, col, value
        wb.save(self.__path)
        return 0
    
    def _getStyle(self, org_style, org_font):
        from xlwt import XFStyle, Font, Borders, Pattern
        # font
        font      = Font()
        font.name = org_font.name
        font.height = org_font.height
        font.italic = org_font.italic
        font.struck_out = org_font.struck_out
        font.outline = org_font.outline
        font.shadow = org_font.shadow
        font.colour_index = org_font.colour_index
        font.bold    = org_font.bold
        font._weight = org_font.weight
        font.escapement = org_font.escapement_type
        font.underline = org_font.underline_type
        font.family = org_font.family
        font.charset = org_font.character_set
        
        # border 
        borders = Borders()
        borders.left   = Borders.THIN #org_style.xf.border.left_line_style
        borders.right  = Borders.THIN #org_style.xf.border.right_line_style
        borders.top    = Borders.THIN #org_style.xf.border.top_line_style
        borders.bottom = Borders.THIN #org_style.xf.border.bottom_line_style
        #borders.diag   = self.NO_LINE

        borders.left_colour   = org_style.xf.border.left_colour_index
        borders.right_colour  = org_style.xf.border.right_colour_index
        borders.top_colour    = org_style.xf.border.top_colour_index
        borders.bottom_colour = org_style.xf.border.bottom_colour_index
        #borders.diag_colour   = org_style.xf.border.left_colour_index

        #borders.need_diag1 = self.NO_NEED_DIAG1
        #borders.need_diag2 = self.NO_NEED_DIAG2
        # Pattern
        pattern = Pattern()
        pattern.pattern             = org_style.xf.background.fill_pattern
        pattern.pattern_fore_colour = org_style.xf.background.pattern_colour_index
        pattern.pattern_back_colour = org_style.xf.background.background_colour_index
        #
        style         = XFStyle()
        style.borders = borders
        style.font    = font
        style.pattern = pattern
        
        return style




###############################################################################
## CExcel2
## CExcel2 is created by liuxinxing, it will replaces CExcel
###############################################################################
class CExcel2(object):
    def __init__(self, filepath = None):
        self.filepath = filepath
        try:
            self.objExcel = CExcel_Win32(filepath)
            self.objExcel.open()
        except:
            self.objExcel = CExcel_All_OS(filepath)
            self.objExcel.open()
    
    def __del__(self):
        self.objExcel.close()
    
    def sheet_names(self):
        return self.objExcel.getSheetNameList()
        
    def getCaseInfoes(self, sheet_name):
        caseinfo_list = []

        for row_index in range(1, self.objExcel.getRowCount(sheet_name)+1):
            case_info = []
            for col_index in range(1, self.objExcel.getColumnCount(sheet_name)+1):
                cellvalue = self.objExcel.getCellValue(sheet_name, row_index, col_index)
                if type(cellvalue) == type(1.0):
                    cellvalue = int(cellvalue)
                case_info.append(CCell(cellvalue, row_index, col_index))      # [值， 行， 列]
            caseinfo_list.append(case_info)
        return caseinfo_list

    def writeCaseInfoList(self, sheet_name, CCell_list = []):  # (值， 行， 列)
        if CCell_list == []: 
            return 

        for objCCell in CCell_list:
            row, col, value = objCCell.getCellValue()
            self.objExcel.setCellValue(sheet_name, row, col, value)
        
        self.objExcel.save()



class CExcel_All_OS:
    def __init__(self, filepath):
        self.filepath = filepath
    
    def open(self):
        import xlrd
        self.wb = xlrd.open_workbook(self.filepath, formatting_info=True)
    
    def save(self):
        import xlutils
        import xlutils.save
        xlutils.save.save(self.wb, self.filepath)
    
    def close(self):
        pass
    
    def getSheetCount(self):
        return len(self.wb.sheet_names())
    
    def getSheetNameList(self):
        return self.wb.sheet_names()
    
    def getRowCount(self, sheet_name):
        sheet = self.wb.sheet_by_name(sheet_name)
        return sheet.nrows
    
    def getColumnCount(self, sheet_name):
        sheet = self.wb.sheet_by_name(sheet_name)
        return sheet.ncols
    
    def getCellValue(self, sheet_name, row, col):
        sheet = self.wb.sheet_by_name(sheet_name)
        cell = sheet.cell(row-1, col-1)
        return cell.value
        
    def setCellValue(self, sheet_name, row, col, value):
        import xlrd.biffh
        sheet = self.wb.sheet_by_name(sheet_name)
        cell = sheet.cell(row-1, col-1)
        #print cell.ctype
        if cell.ctype == xlrd.biffh.XL_CELL_BLANK:
            if type(value) == type(1):
                cell.ctype = xlrd.biffh.XL_CELL_NUMBER
            else:
                cell.ctype = xlrd.biffh.XL_CELL_TEXT
        sheet.put_cell(row-1, col-1, cell.ctype, value, cell.xf_index)

class CExcel_Win32:
    def __init__(self, filepath):
        self.filepath = os.path.abspath(filepath)
    
    def open(self):
        import win32com.client
        self.xlApp = win32com.client.Dispatch('Excel.Application')
        self.xlBook = self.xlApp.Workbooks.Open(self.filepath)
    
    def save(self):
        self.xlBook.Save()

    def close(self):
        self.xlBook.Close(False)
    
    def getSheetCount(self):
        return self.xlBook.Sheets.Count
    
    def getSheetNameList(self):
        sheet_name_list = []
        for index in range(0, self.xlBook.Sheets.Count):
            sheet = self.xlBook.Worksheets[index]
            sheet_name_list.append(sheet.Name)
        return sheet_name_list
    
    def getRowCount(self, sheet_name):
        sheet = self.xlBook.Worksheets(sheet_name)
        return sheet.UsedRange.Rows.Count
    
    def getColumnCount(self, sheet_name):
        sheet = self.xlBook.Worksheets(sheet_name)
        return sheet.UsedRange.Columns.Count
    
    def getCellValue(self, sheet_name, row, col):
        sheet = self.xlBook.Worksheets(sheet_name)
        return sheet.Cells(row, col).Value
        
    def setCellValue(self, sheet_name, row, col, value):
        sheet = self.xlBook.Worksheets(sheet_name)
        sheet.Cells(row, col).Value = value
    