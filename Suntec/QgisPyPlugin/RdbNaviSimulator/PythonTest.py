import heapq

class x(object):
    def __init__(self):
        self.dict1 = {}
        self.dict2 = {}
        return
    
    def __getitem__(self, n):
        return self.dict1[n]
    
    
def main():
    xObj = x()
    xObj.dict1[1]='a1'
    xObj.dict1[2]='a2'
    
    xObj.dict2['a'] = 'b1'
    
    print xObj[1]
    
def heapTest():
    aList = []
    for i in ((1,500), (2,400), (4,200), (5,100)):
        heapq.heappush(aList, i)
    
    while (1):
        print '%s, %s' % heapq.heappop(aList)
        
    return

if __name__=='__main__':
    heapTest()