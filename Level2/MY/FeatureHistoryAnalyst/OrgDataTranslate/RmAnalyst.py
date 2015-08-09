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
        lastDayClosePricet = 0 # 记下昨日收盘价
        dict = {}
        for row in rows:
            datet = row[0]
            open_pricet = row[1]
            close_pricet = row[2]
            minute_list = row[3]
            highest_pricet_list = row[4]
            lowest_pricet_list = row[5]

            
            todayBuildPrice = 0    # 记下今天建仓价
            todayTarWinPrice = 0   # 记下今天止盈价
            todayTarLosePrice = 0  # 记下今天止损价
            # 合约进行交易的第一天没有昨日收盘价，令其等于今日开盘价。
            if lastDayClosePricet == 0:
                lastDayClosePricet = open_pricet

            # 当今日开盘高于昨日收盘时，做空。
            if open_pricet > lastDayClosePricet:
                todayBuildPrice = highest_pricet_list[5]   # 做空时机待定，暂时选的是09:05的最高价，此处可更改。
                todayTarWinPrice = lastDayClosePricet - 10 # 目标止盈点是昨日收盘价-10，即赢10个点今天目的达到。
                todayTarLosePrice = todayBuildPrice + 20   # 目标止损点是今天建仓价+20，即输20个点止损。
                for i in range(5, len(minute_list)):
                    if lowest_pricet_list[i] < todayTarWinPrice: # 如果价格跌到了止盈点，今天盈利10个点目标达到，今天任务结束。
                        dict[datet] = 10
                        break

                    if 

        return 
if __name__ == "__main__":
    test = RmAnalyst("host=127.0.0.1 dbname=rm user=postgres password=pset123456")
    test.get1(r"""E:\features\tick""")





