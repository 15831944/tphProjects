#! /usr/bin/python

import threading
import zmq
import Queue

class ServerMain(threading.Thread):
    def __init__(self):
        threading.Thread.__init__ (self)
        self.context = zmq.Context()
        self.workers = []
    
    def add_svr_worker(self,srv_worker):
        srv_worker.set_context(self.context)
        self.workers = []
        self.workers.append(srv_worker)

    def run(self):
        context = self.context
        frontend = context.socket(zmq.XREP)
        frontend.bind('tcp://*:5570')
        
        backend = context.socket(zmq.XREQ)
        backend.bind('tcp://*:5571')
        
        worker = self.workers[0]
        worker.start()
        
        poll = zmq.Poller()
        poll.register(frontend, zmq.POLLIN)
        poll.register(backend, zmq.POLLIN)
        
        while True:
            sockets = dict(poll.poll())
            if frontend in sockets:
                ident, msg = frontend.recv_multipart()
                backend.send_multipart([ident, msg])
                if msg == 'quit':
                    worker.join()
                    break
            if backend in sockets:
                ident, msg = backend.recv_multipart()
                frontend.send_multipart([ident, msg])
        
        frontend.close()
        backend.close()
        context.term()


class ServerWorker(threading.Thread):
    """ServerWorker"""
    def __init__(self):
        threading.Thread.__init__ (self)
        self.context = None
        self.worker = None
        self.work_queue = Queue.Queue(0)
    
    def set_context(self, _context):
        self.context = _context
    
    def get_work_queue(self):
        return self.work_queue
    
    def run(self):
        self.worker = self.context.socket(zmq.XREQ)
        self.worker.connect('tcp://localhost:5571')
        while True:
            ident, msg = self.worker.recv_multipart()
            if msg=='quit':
                break
            
            self.work_queue.put(msg)
            
            back_msg = b'OK'
            self.worker.send_multipart([ident, back_msg])
        
        self.worker.close()
    
    