# -*- coding: UTF8 -*-

import component.default.dictionary
import common.cache_file

class Zenrinname():
    
    def __init__(self):
        self.name_id=1
        self.officename_flag=False
        self.name_id_array=[] 
        self.name_type_array=[]
        self.language_code_array=[]
        self.name_array=[]
        self.phonetic_lang_array=[]
        self.phonetic_string_array=[]
        
    def _add_name(self,name_type,language_code,name,phonetic_lang,phonetic_string):
        if name_type=='office_name':
            if self.officename_flag:
                name_type='alter_name'
            else:
                self.officename_flag=True
        self.name_id_array.append(self.name_id)
        self.name_id+=1
        self.name_type_array.append(name_type)
        self.language_code_array.append(language_code)
        self.name_array.append(self._convertstring(name, 1))
        self.phonetic_lang_array.append(phonetic_lang)
        self.phonetic_string_array.append(phonetic_string)

    def _isroutenum(self,name):
        if not name:
            return False
        if '鄉道' in name or '縣道' in name or '省道' in name or '國道' in name:
            return True
        return False

    def _convertstring(self,name,dir):#IF (asciival > 65280) AND (asciival < 65375) THEN  
        if dir==1:
            return ''.join(map(lambda x:chr(32) if ord(x)==12288 else x \
                               if ord(x)<=65280 or ord(x)>=65375 else chr(ord(x)-65248),unicode(name)))
        else:
            return ''.join(map(lambda x:unichr(12288) if ord(x)==32 else unichr(ord(x)+65248) \
                               if ord(x)<127 else x,name)) 

    def _make_shield_num(self,name,level=None):
        
        if not level:
            if '鄉道' in name:
                level=4165
            elif '縣道' in name:
                level=4164
            elif '省道' in name:
                level=4163
            elif '國道' in name:
                level=4161
            else:
                self.log.error('Not a route num! %s' % name)
        return str(level)+'\t'+name.replace('鄉道','').replace('縣道','').replace('省道','')\
            .replace('國道','').replace('號','').replace('（支線）','').replace('高速公路','')
     
class comp_dictionary_zenrin(component.default.dictionary.comp_dictionary):
    '''字典(zenrin)
    '''

    def __init__(self):
        '''
        Constructor
        '''
        component.default.dictionary.comp_dictionary.__init__(self)
    
    def _DoCreateIndex(self):
        self.CreateIndex2('temp_link_name_link_id_idx')
        self.CreateIndex2('temp_link_shield_link_id_idx')
        
    def _DoCreateTable(self):
        
        self.CreateTable2('temp_link_name')
        self.CreateTable2('temp_link_shield')
        
    def _Do(self):
        # 创建语言种别的中间表
        self._InsertLanguages()
        # 设置语言code
        self.set_language_code()

        self._make_link_shield()
        self._make_link_name()
        
    
    def _make_link_name(self):
        '''道路名称(包括显示用番号)'''
        self.log.info('Make Link Name.')
        
        
        sqlcmd='''
            select a.meshcode,a.linkno,
                   b.tcode,b.tname,b.tnamey,--隧道 名 或者 桥名 
                   c.name,c.bpmf,--地下道 名 
                   d.expname,d.routeno,--省道高速名称和番号
                   e.l4name||e.l5name||e.l6name,--某某巷子某某弄的名字，需要拼接在一起
                   f.sname,f.snamey,--官方名，以及除了国道之外的番号会存在这里
                   f.tname,f.tnamey,--别名，国道番号存在这里
                   g.link_id
            from org_road a
            left join org_tunnelname b on a.meshcode=b.meshcode and a.linkno=b.roadno
            left join org_underpass c on a.meshcode=c.meshcode and a.linkno=c.linkno
            left join org_prov_exp d on a.meshcode=substr(d.meshcode,2) and a.linkno=d.linkno
            left join org_l5l6name e on a.meshcode=e.meshcode and a.linkno=e.roadno
            left join org_rname_bpmf f on a.meshcode=f.meshcode and a.linkno=f.roadno
            left join temp_link_mapping g on a.meshcode=g.meshcode and a.linkno=g.linkno
        '''
        
        asso_recs = self.pg.get_batch_data2(sqlcmd)
        
        temp_file_obj = common.cache_file.open('road_name')
        for asso_rec in asso_recs:
            name=Zenrinname()
            link_id = asso_rec[14]
            if asso_rec[9]:
                name._add_name('office_name','CHT', asso_rec[9], None , None)
            if [name._isroutenum(asso_rec[10]) , name._isroutenum(asso_rec[12]) , bool(asso_rec[8])].count(True)>1:
                self.log.warning('more than 1 route num ! meshcode=%s and linkno=%d' % (asso_rec[0],asso_rec[1]))
                #不能continue
            if asso_rec[10]:
                if name._isroutenum(asso_rec[10]):
                    name._add_name('route_num','CHT',asso_rec[10],'PYT',asso_rec[11])
                else:
                    name._add_name('office_name','CHT',asso_rec[10],'PYT',asso_rec[11])
            if asso_rec[12]:
                if name._isroutenum(asso_rec[12]):
                    name._add_name('route_num','CHT',asso_rec[12],'PYT',asso_rec[13])
                else:
                    name._add_name('office_name','CHT',asso_rec[12],'PYT',asso_rec[13])
            if asso_rec[7]:
                name._add_name('office_name','CHT', asso_rec[7], None , None)
                name._add_name('route_num','CHT', asso_rec[8], None, None)
            if asso_rec[2]:
                if asso_rec[2]==2:
                    name._add_name('bridge_name', 'CHT', asso_rec[3], 'PYT', asso_rec[4])
                else:
                    name._add_name('office_name', 'CHT', asso_rec[3], 'PYT', asso_rec[4])
            if asso_rec[5]:
                name._add_name('office_name','CHT', asso_rec[5], 'PYT', asso_rec[6])
            if name.name_id_array:
                #print asso_rec
                #print self.name_id_array,self.name_type_array,self.language_code_array,self.name_array,self.phonetic_lang_array,self.phonetic_string_array
                json_name = component.default.multi_lang_name.MultiLangName.name_array_2_json_string_multi_phon(name.name_id_array, 
                                                                                                 name.name_type_array, 
                                                                                                 name.language_code_array, 
                                                                                                 name.name_array,
                                                                                                 name.phonetic_lang_array,
                                                                                                 name.phonetic_string_array)
                temp_file_obj.write('%d\t%s\n' % (link_id, json_name))
        
        temp_file_obj.seek(0)
        self.pg.copy_from2(temp_file_obj, 'temp_link_name')
        self.pg.commit2()
        common.cache_file.close(temp_file_obj,True)

    def _make_link_shield(self):
        # 盾牌及番号
        self.log.info('Make Link Shield.')
               
        sqlcmd = """
            select a.meshcode,a.linkno,b.routeno,c.sname,c.snamey,c.tname,c.tnamey,d.link_id
            from org_road a
            left join org_prov_exp b on a.meshcode=substr(b.meshcode,2) and a.linkno=b.linkno
            left join org_rname_bpmf c on a.meshcode=c.meshcode and a.linkno=c.roadno
            left join temp_link_mapping d on a.meshcode=d.meshcode and a.linkno=d.linkno
        """
        
        asso_recs = self.pg.get_batch_data2(sqlcmd)
        
        temp_file_obj = common.cache_file.open('road_shield')
        for asso_rec in asso_recs:
            shield=Zenrinname()
            link_id = asso_rec[7]
            if asso_rec[2]:
                if shield._isroutenum(asso_rec[3]) or shield._isroutenum(asso_rec[5]):
                    self.log.warning('more than 1 route num ! meshcode=%s and linkno=%d' % (asso_rec[0],asso_rec[1]))
                    continue
                shield_num=shield._make_shield_num(asso_rec[2],4162)
                shield._add_name('shield', 'CHT', shield_num, None, None)
            else:
                if shield._isroutenum(asso_rec[3]):
                    if shield._isroutenum(asso_rec[5]):
                        self.log.warning('more than 1 route num ! meshcode=%s and linkno=%d' % (asso_rec[0],asso_rec[1]))
                        continue
                    shield_num=shield._make_shield_num(asso_rec[3])
                    shield._add_name('shield', 'CHT', shield_num, 'PYT', asso_rec[4])
                if shield._isroutenum(asso_rec[5]):
                    shield_num=shield._make_shield_num(asso_rec[5])
                    shield._add_name('shield', 'CHT', shield_num, 'PYT', asso_rec[6])
            if shield.name_id_array:
                json_name = component.default.multi_lang_name.MultiLangName.name_array_2_json_string_multi_phon(shield.name_id_array, 
                                                                                                 shield.name_type_array, 
                                                                                                 shield.language_code_array, 
                                                                                                 shield.name_array,
                                                                                                 shield.phonetic_lang_array,
                                                                                                 shield.phonetic_string_array)
                temp_file_obj.write('%d\t%s\n' % (link_id, json_name))
        
        temp_file_obj.seek(0)
        self.pg.copy_from2(temp_file_obj, 'temp_link_shield')
        self.pg.commit2()
        common.cache_file.close(temp_file_obj,True)