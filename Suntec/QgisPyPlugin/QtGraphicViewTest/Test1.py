import os
import datetime
import copy
import heapq
import random
ISOTIMEFORMAT='%Y-%m-%d %X'

def qsort(L):
    if len(L) <= 1:
        return L  
    return qsort([lt for lt in L[1:] if lt < L[0]]) + [L[0]] + qsort([ge for ge in L[1:] if ge >= L[0]])

def main():
    list1 = []
    for i in range(0, 100000):
        list1.append(random.randint(1, 100000))
    list2 = copy.deepcopy(list1)
    print datetime.datetime.now()
    x1 = [lt for lt in list1[1:] if lt < list1[0]]
    print datetime.datetime.now()
    print '-----------------------------------------------'
    print datetime.datetime.now()
    list1.sort()
    print datetime.datetime.now()
    print '-----------------------------------------------'
    print datetime.datetime.now()
    x = qsort(list2)
    print datetime.datetime.now()
    return


if __name__ == '__main__':
    main()


