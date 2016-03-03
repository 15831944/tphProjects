#encoding=utf-8
import Image
from os import path
import os


newImg = Image.new('RGBA', (530, 480), (255, 255, 255, 1))
img = Image.open(r"C:\Users\hexin\Desktop\20151216_image\TWNCA00110A.png")
newImg.paste(img, (0,200,530, 480))
newImg.save(r'C:\Users\hexin\Desktop\20151216_image\x.png',"PNG")