class RdbInformation(object):
    def __init__(self, theP):
        self.mTheP = theP
        self.mTheVolumn = 0
        self.mVolDistribution = {}
        return

class RdbInformationPool(object):
    def __init__(self):
        self.mRdbInfoList = []
        for i in range(1500, 2500):
            self.mRdbInfoList.append(RdbInformation(i))
        return

    def newTickCome(self, theTick):
        theTick
        return



