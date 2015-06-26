# -*- coding: UTF8 -*-
'''
Created on 2012-3-8

@author: hongchenzai
'''

import smtplib                          # Import smtplib for the actual sending function
from email.mime.text import MIMEText    # Import the email modules we'll need
import Logger
import database

class CMail(object):
    '''mail类: 把log信息发送给相关人员
    '''
    def __init__(self):
        '''
        Constructor
        '''
        self.logfile      =  Logger.CLogger.instance().get_logfile()
        self.from_who     = 'hongcz@pset.suntec.net'
        self.data_version =  database.pg_client.instance().Get_Version()
        self.to_list      = ['hongcz@pset.suntec.net',
                             'zhangliang@pset.suntec.net',
                             'liuxinxing@pset.suntec.net',
                             'yuanrui@pset.suntec.net'
                             ]
        
    def send_mail(self):
        
        # Open a plain text file for reading.  For this example, assume that
        # the text file contains only ASCII characters.
        fp = open(self.logfile, 'rb')
        # Create a text/plain message
        fp.seek(0)
        msg = MIMEText(fp.read())
        fp.close()
        
        # me == the sender's email address
        # you == the recipient's email address
        COMMASPACE = ', '
        toaddrs = COMMASPACE.join(self.to_list)
        msg['Subject'] = '[webgis] The RDB version is ' + self.data_version + ", the Log information is..."
        msg['From']    = self.from_who
        msg['To']      = toaddrs
        
        # Send the message via our own SMTP server, but don't include the
        # envelope header.
        s = smtplib.SMTP()
        s.connect("mail.pset.suntec.net", "25")
        s.login("hongcz", "111111Aa")
        s.sendmail(self.from_who, self.to_list, msg.as_string())
        s.quit()

        return 0
    
    def set_logfile(self, logfile_path = ''):
        '''设log文件的路径'''
        self.logfile = logfile_path
        return 0
    
    def set_data_version(self, version = '1.000'):
        '''设置当前作制的数据的版本号'''
        self.data_version = version
        return 0
    
    def set_from(self, from_who = 'hongcz@pset.suntec.net'):
        self.from_who = from_who
        return 0
    
    def add_to_list(self, to_who = 'hongcz@pset.suntec.net'):
        self.to_list.add(to_who)
        return 0
        
    