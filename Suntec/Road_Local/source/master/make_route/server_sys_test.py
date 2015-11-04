# -*- coding: utf-8 -*-

import win32serviceutil   
import win32service   
import win32event
import logging   
import os
import zmq
import threading
import run_route

DB1 = '172.26.179.201'
DB2 = '172.26.179.194'
DB3 = '172.26.179.190'
DB4 = '172.26.179.187'
DB5 = '172.26.179.184'

DB_dict = {DB1:0,DB2:0,DB3:0,DB4:0,DB5:0}

class ServerMain(threading.Thread):
    def __init__(self):
        threading.Thread.__init__ (self)
        self.context = zmq.Context()
        self.workers = []
#        self._logger = logger
#        self._logger.info("1")
    
    def add_svr_worker(self,srv_worker):
        srv_worker.set_context(self.context)
        self.workers = []
        self.workers.append(srv_worker)

    def run(self):
        context = self.context
        frontend = context.socket(zmq.XREP)
        frontend.bind('tcp://*:5500')
        
        backend = context.socket(zmq.XREQ)
        backend.bind('tcp://*:5501')
        
        worker = self.workers[0]
        worker.start()
        
        poll = zmq.Poller()
        poll.register(frontend, zmq.POLLIN)
        poll.register(backend, zmq.POLLIN)
        
        try:          
            while True:
                sockets = dict(poll.poll())
                if frontend in sockets:
                    ident, msg = frontend.recv_multipart()
                    backend.send_multipart([ident, msg])
#                    self._logger.info("ServerMain" + ident)
#                    self._logger.info("ServerMain" + msg)
                    if msg == 'quit':
                        worker.join()
                        break
                if backend in sockets:
                    ident, msg = backend.recv_multipart()
                    frontend.send_multipart([ident, msg])
        except zmq.ZMQError,zerr:
            raise zerr
        
        frontend.close()
        backend.close()
        context.term()


class ServerWorker(threading.Thread):
    """ServerWorker"""
    def __init__(self,logger):
        threading.Thread.__init__ (self)
        self.context = None
        self.worker = None
        self._logger = logger
        self._logger.info("2")
    
    def set_context(self, _context):
        self.context = _context
    
    def run(self):
        self.worker = self.context.socket(zmq.XREQ)
        self.worker.connect('tcp://localhost:5501')
        try:
            while True:
                ident, msg = self.worker.recv_multipart()
                if msg=='quit':
                    break
                self._logger(ident)
                self._logger(msg)
                self.action(self.worker, ident, msg)
        except zmq.ZMQError,zerr:
            raise zerr
        
        self.worker.close()
    
    def action(self, worker, rev_ident, rev_msg):
        
        print('Worker received %s from %s' % (rev_msg, rev_ident))
        #do build work
        back_msg = None
        if rev_ident is None or rev_msg is None :
            worker.send_multipart([rev_ident, back_msg])
            
        back_msg = "NG"
        self._logger.info("asa")  
        for current, flages in DB_dict.items():
            if flages == 0 :
                DB_dict[current] = 1
#                back_msg = run_route.run_route_com(DB1,rev_msg).run()
                DB_dict[current] = 0
                break
        print rev_ident, back_msg
        worker.send_multipart([rev_ident, back_msg])
          
class PythonService(win32serviceutil.ServiceFramework):   
    #服务名   
    _svc_name_ = "PythonService"  
    #服务显示名称   
    _svc_display_name_ = "Python Service Demo"  
    #服务描述   
    _svc_description_ = "Python service demo."  
  
    def __init__(self, args):   
        win32serviceutil.ServiceFramework.__init__(self, args)   
        self.hWaitStop = win32event.CreateEvent(None, 0, 0, None)  
        self.logger = self._getLogger()  
        self.isAlive = True 
          
    def _getLogger(self):  

          
        logger = logging.getLogger('[PythonService]')  
          
        #this_file = inspect.getfile(inspect.currentframe())  
        #dirpath = os.path.abspath(os.path.dirname(this_file))  
        handler = logging.FileHandler("C:\\zhaojie\\log\\service.log")  
          
        formatter = logging.Formatter('%(asctime)s %(name)-12s %(levelname)-8s %(message)s')  
        handler.setFormatter(formatter)  
          
        logger.addHandler(handler)  
        logger.setLevel(logging.INFO)  
          
        return logger  
  
    def SvcDoRun(self):  
        import time  
        self.logger.info("svc do run....")
        pg_server = ServerMain()
        pg_server.add_svr_worker(ServerWorker(self.logger))
        pg_server.start()
        self.logger.info("start!")
             
        while self.isAlive:  
#            self.logger.info("I am alive.")
            time.sleep(100)  
        # 等待服务被停止
        #win32event.WaitForSingleObject(self.hWaitStop, win32event.INFINITE)    
              
    def SvcStop(self):   
        # 先告诉SCM停止这个过程    
        self.logger.info("svc do stop....")  
        self.ReportServiceStatus(win32service.SERVICE_STOP_PENDING)   
        # 设置事件    
        win32event.SetEvent(self.hWaitStop)   
        self.isAlive = False
  
if __name__=='__main__':   
    win32serviceutil.HandleCommandLine(PythonService)