#encoding=utf-8

def containsRoadClass(strRoadKind, strFilterClass):
    strRoadKindList = strRoadKind.split('|')
    for oneStrRoadKind in strRoadKindList:
        if oneStrRoadKind[0:2] == strFilterClass:
            return True
    return False

def getRoadClassDetail(strRoadClass):
    strRoadClassDetail = ''
    if containsRoadClass(strRoadClass, '00'):
        strRoadClassDetail = strRoadClassDetail + '|' + '高速路'

    if containsRoadClass(strRoadClass, '01'):
        strRoadClassDetail = strRoadClassDetail + '|' + '都市高速路'

    if containsRoadClass(strRoadClass, '02'):
        strRoadClassDetail = strRoadClassDetail + '|' + '国道'

    if containsRoadClass(strRoadClass, '03'):
        strRoadClassDetail = strRoadClassDetail + '|' + '省道'

    if containsRoadClass(strRoadClass, '04'):
        strRoadClassDetail = strRoadClassDetail + '|' + '县道'

    if containsRoadClass(strRoadClass, '06'):
        strRoadClassDetail = strRoadClassDetail + '|' + '乡镇村道'

    if containsRoadClass(strRoadClass, '08'):
        strRoadClassDetail = strRoadClassDetail + '|' + '其它道路'

    if containsRoadClass(strRoadClass, '09'):
        strRoadClassDetail = strRoadClassDetail + '|' + '九级路'

    if containsRoadClass(strRoadClass, '0a'):
        strRoadClassDetail = strRoadClassDetail + '|' + '轮渡'

    if containsRoadClass(strRoadClass, '0b'):
        strRoadClassDetail = strRoadClassDetail + '|' + '行人道路'

    return strRoadClassDetail.lstrip('|')

if __name__ == "__main__":
    print getRoadClassDetail('0102')