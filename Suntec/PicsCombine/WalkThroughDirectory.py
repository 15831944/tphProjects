#encoding=utf-8
import os.path
import struct
import sys

def combine_images(srcDir):
    for curDir,dirNames,fileNames in os.walk(srcDir):
        for file in fileNames:
            print file
            
if __name__ == '__main__':
    combine_images("C:\\My\\work\\20150409\\2DJ_2015Q1_ASIA\\EGYPT\\DRIVER_VIEW\\LANDSCAPE\\ASPECT_RATIO_5x3")
    i = 100
    i += 1