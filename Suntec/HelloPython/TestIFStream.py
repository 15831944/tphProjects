#encoding=utf-8

if __name__ == "__main__":
    with open(r"C:\Users\hexin\Desktop\1.txt", 'r') as iFStream:
        while(True):
            x = iFStream.readline().strip()
            if x == '':
                break
            print x
            
        
    inti = 1
    inti += 1