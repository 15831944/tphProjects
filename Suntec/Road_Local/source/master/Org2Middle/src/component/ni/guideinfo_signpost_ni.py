# -*- coding: cp936 -*-
'''
Created on 2015-5-6
@author: tangpinghui
'''
from common import common_func
from component.component_base import comp_base

class comp_guideinfo_signpost_ni(comp_base):
    def __init__(self):
        comp_base.__init__(self, 'GuideInfo_SignPost')
        return
    def _DoCreateTable(self):
        self.CreateTable2('signpost_tbl')
        return 

    def _DoCreatIndex(self):
        return 0

    def _DoCreateFunction(self):
        return 0

    def _Do(self):
        self.log.info("ni generate signpost_tbl begin.")
        
        signpost_tbl_insert_sqlcmd = '''
                                        insert into signpost_tbl(nodeid, inlinkid, outlinkid,
                                                                 passlid, passlink_cnt, patternno, 
                                                                 arrowno)
                                        VALUES (%s, %s, %s,
                                                %s, %s, %s, 
                                                %s)
                                    '''
        if 1:
            # ����Y:\�ĵ�\3���ݸ�ʽ�ĵ�\3.5 ��ά\NIMIF-G-14win_����_����\NIMIF-G-14win-GreatChina(���򿴰�).pdf
            # ���ɵ�demo�汾
            # ���廹��Ҫ��signpostЭ�鵽���Ժ���е���
            rows = self.get_batch_data('''
                                        SELECT nodeid, inlinkid, outlinkid, patternno, 
                                               arrowno, passlid, passlid2, folder
                                        FROM org_signboard
                                       ''')
            for row in rows:     
                nodeid = row[0]
                inlinkid = row[1]
                outlinkid = row[2]
                patternno = row[3]
                arrowno = row[4]
                passlid = row[5]
                passlid2 = row[6]
                folder = row[7]
                
                totalPasslid = passlid
                if(passlid and passlid2):
                    totalPasslid = totalPasslid + '|' + passlid2
                    
                # ������֤������folder�¿��ܴ���ͬ����patternno��arrownoͼƬ��
                # ����20150508��������ά�й������д������������
                # patternͼ���У�beijing\40210371.png �� guangzhou\40210371.png����ͬ��ͼƬ
                # arrowͼ���У�beijing\224019.png �� guangzhou\224019��png����ͬ��ͼƬ
                # Ŀǰ����������жϣ�������ͬ��patternͼ��arrowͼ����ͬ�ġ�
                # Ϊ��Ԥ�������п��ܴ��ڲ�ͬ����ת����ʱ��folder�ӳɺ�׺��������ͼʱ������ѭ�˹���
                fixedPatternno = patternno + '_' + folder
                fixedArrowno = arrowno + '_' + folder
                
                totalPasslidCount = common_func.CountPassLink(totalPasslid)
                               
                self.pg.execute2(signpost_tbl_insert_sqlcmd, 
                                 (nodeid, inlinkid, outlinkid,
                                  totalPasslid, totalPasslidCount, fixedPatternno, 
                                  fixedArrowno))
        self.pg.commit2()
        self.log.info("ni generate signpost_tbl end.")
        return 0
    
        