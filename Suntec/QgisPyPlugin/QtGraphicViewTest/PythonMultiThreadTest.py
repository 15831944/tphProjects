import threading
import time

num = 0
mu = threading.Lock()
class MyThread(threading.Thread):
    def run(self):
        global num
        time.sleep(1)
        if mu.acquire():
            num = num + 1
            msg = self.name + ' num is ----' + str(num)
            print msg + '\n'
            mu.release()

def test():
    for i in range(20):
        t = MyThread()
        t.start()

if __name__ == '__main__':
    test()
    inti = 1
    inti += 1
