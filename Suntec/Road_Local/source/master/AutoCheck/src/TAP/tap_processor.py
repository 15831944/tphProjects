# -*- coding: utf-8 -*-

import re

TAP_NG = 0
TAP_OK = 1
TAP_SKIP = 2
TAP_TODO = 3

class TAP_Processor(object):
    #===============================================================================
    #                 |-----result------|
    # _resultList格式：［［msgType，message］，［msgType ,message］.......］
    #                     ^                        ^                      
    #                   TAP_NG                   TAP_OK                   
    #===============================================================================   
    def __init__(self):
        self._resultList = []
    
    def clear(self):
        self._resultList =  []
    
    #add操作， by list
    def addResultList(self , result_List):   
        self._resultList.extend(result_List)
     
    #add操作， by simple result      
    def addResult(self ,msgType , message):
        self._resultList.append([msgType, message])
    
    #load操作， by tap file
    def loadTap(self , tapFile):
        with open(tapFile , 'rb') as f:
            for line in f:
                if re.match(r'\S+', line):
                    if re.match(r'[^\d+..\d+]',line):
                        self._resultList.append(self.tapLine2result(line))
    #dump操作 to file
    def dump2file(self, outPathName):   
        result_list = self.dump2strList()
        
        with open(outPathName , 'wb') as f:
            f.writelines(result_list)
      
    #dump操作 to string list       
    def dump2strList(self):
        result_list = ['1..%d\n'%len(self._resultList)]
        
        for i in range(0, len(self._resultList)):
            result_list.append(self.result2tapLine(self._resultList[i], i+1)+'\n')
            
        return result_list
           
    #获取所有result
    def getAllResult(self):
        result_list = self._resultList
        
        return result_list
      
    #获取所有result,转成字典类型
    def getAllResultDict(self):
        result_list = self._resultList
        result_dict = {}
        result_map = ["NG","OK","Not executed","NG"]
        for item in result_list:
            result_dict[item[1]] = result_map[item[0]]
            
        return result_dict
    
    #search操作 all success｜fail|skip|TODO result
    def searchByType(self, resultType):
        result_list = []
        
        for result in self._resultList:
            if result[0] == resultType:
                result_list.append(result)
        
        return result_list
    
    #search操作 message match one REGEX
    def searchByMessage(self , REGEX):
        result_list = []
        
        for result in self._resultList:
            if re.search(REGEX, result[1]):
                result_list.append(result)
                
        return result_list     

    #result to tapLine
    def result2tapLine(self, result , i):
        tapPrintLine = ''
        
        if result[0] == TAP_NG:
            tapPrintLine = "not ok %d - %s"%(i,result[1])
        elif result[0] == TAP_OK:
            tapPrintLine = "ok %d - %s"%(i,result[1])
        elif result[0] == TAP_SKIP:
            tapPrintLine = "not ok %d - %s # SKIP skip this test"%(i,result[1])
        elif result[0] == TAP_TODO:
            tapPrintLine = "not ok %d - %s # TODO need redo, source list error"%(i,result[1])
        else:
            raise NameError 
        
        return tapPrintLine
    
    #tapLine to result 
    def tapLine2result(self , tapLine):        
        result = []
        pattern = re.compile(r'(\s+$)|(^\s+)')
        
        if re.match('^ok', tapLine) :  
            message = re.search(r'(?<=-)(.*)', tapLine).group()
            message = pattern.sub('', message)
            result = [TAP_OK,message]  
        elif re.match('^not.*#\sSKIP\s.*', tapLine):
            message = re.search(r'(?<=-)(.*)(?=#)', tapLine).group()
            message = pattern.sub('', message)               
            result = [TAP_SKIP,message]
        elif re.match('^not.*#\sTODO\s.*', tapLine):
            message = re.search(r'(?<=-)(.*)(?=#)', tapLine).group()
            message = pattern.sub('', message)
            result = [TAP_TODO,message]
        elif re.match('^not',tapLine):
            message = re.search(r'(?<=-)(.*)', tapLine).group()
            message = pattern.sub('', message)
            result = [TAP_NG,message]
        else:
            raise NameError
                
        return result
    
 
  
        
                
    
    


                
    
    

                

                
                
                
    
                
            
                
         
                
                
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        