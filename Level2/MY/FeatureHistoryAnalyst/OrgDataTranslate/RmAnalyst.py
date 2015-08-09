# -*- coding: cp936 -*-

import os
import os.path
import shutil
import psycopg2

class RmAnalyst(object):
    def __init__(self, connectStr):
        if connectStr == '' or connectStr == None:
            print """Connect string error: %s """ % connectStr
        self.conn = psycopg2.connect(connectStr)
        self.pg = self.conn.cursor()

    def __del__(self):
        return

    def get1(self, srcDir):
        sqlcmd = '''
                    select datet, 
                           (array_agg(open_pricet))[1] as open_pricet, 
                           (array_agg(close_pricet))[array_upper(array_agg(close_pricet), 1)] as close_pricet, 
                           array_agg(minutet) as minute_list, 
                           array_agg(highest_pricet) as highest_pricet_list, 
                           array_agg(lowest_pricet) as lowest_pricet_list
                    from 
                    (
	                    select * from rm01_minute order by datet, minutet
                    ) as t
                    group by datet;
                '''
        self.pg.execute(sqlcmd)
        rows = self.pg.fetchall()
        lastDayClosePricet = 0 # �����������̼�
        dict = {}
        for row in rows:
            datet = row[0]
            open_pricet = row[1]
            close_pricet = row[2]
            minute_list = row[3]
            highest_pricet_list = row[4]
            lowest_pricet_list = row[5]

            
            todayBuildPrice = 0    # ���½��콨�ּ�
            todayTarWinPrice = 0   # ���½���ֹӯ��
            todayTarLosePrice = 0  # ���½���ֹ���
            # ��Լ���н��׵ĵ�һ��û���������̼ۣ�������ڽ��տ��̼ۡ�
            if lastDayClosePricet == 0:
                lastDayClosePricet = open_pricet

            # �����տ��̸�����������ʱ�����ա�
            if open_pricet > lastDayClosePricet:
                todayBuildPrice = highest_pricet_list[5]   # ����ʱ����������ʱѡ����09:05����߼ۣ��˴��ɸ��ġ�
                todayTarWinPrice = lastDayClosePricet - 10 # Ŀ��ֹӯ�����������̼�-10����Ӯ10�������Ŀ�Ĵﵽ��
                todayTarLosePrice = todayBuildPrice + 20   # Ŀ��ֹ����ǽ��콨�ּ�+20������20����ֹ��
                for i in range(5, len(minute_list)):
                    if lowest_pricet_list[i] < todayTarWinPrice: # ����۸������ֹӯ�㣬����ӯ��10����Ŀ��ﵽ���������������
                        dict[datet] = 10
                        break

                    if 

        return 
if __name__ == "__main__":
    test = RmAnalyst("host=127.0.0.1 dbname=rm user=postgres password=pset123456")
    test.get1(r"""E:\features\tick""")





