class xxxx(object):
    def __init__(self, str):
        self.angle = int(str[0:str.find('xx')])
        a = str.find("xx")
        b = str.find("'")
        c = str[a+2: b]
        self.minute = int(c)
        
        d = str.find("'")
        e = str.find('"')
        f = str[d+1: e]
        self.second = float(f)
        return
    
    def getDecimal(self):
        return float(self.angle) - float(self.minute) / 60.0 - self.second / 3600.0

if __name__ == '__main__':
    filePath = r"C:\Users\hexin\Desktop\1.txt"
    outputPath = r"C:\Users\hexin\Desktop\2.txt"
    with open(filePath, "r") as iFStream:
        with open(outputPath, 'w') as oFStream:
            latLonList = iFStream.readlines()
            for oneLatLon in latLonList:
                lat = oneLatLon.split(' ')[0]
                lon = oneLatLon.split(' ')[1]
                latDecimal = xxxx(lat).getDecimal()
                lonDecimal = xxxx(lon).getDecimal()
                oFStream.write('%.5f,%.5f\n' % (latDecimal,lonDecimal))
            