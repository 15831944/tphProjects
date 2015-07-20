def image_path_comparation(image1, image2):
    image1Split = image1.split('_')
    image2Split = image2.split('_')
    
    # we define two images are similar only if:
    # the 4th segment is different.
    # the other segments are the same. 
    leftPart1 = '_'.join([image1Split[0], image1Split[1], image1Split[2]])
    leftPart2 = '_'.join([image2Split[0], image2Split[1], image2Split[2]])
    if leftPart1 < leftPart2:
        return True
    elif leftPart1 == leftPart2:
        rightPart1 = '_'.join([image1Split[4], image1Split[5]])
        rightPart2 = '_'.join([image2Split[4], image2Split[5]])
        if rightPart1 < rightPart2:
            return True
        elif rightPart1 == rightPart2:
            midPart1 = '_'.join([image1Split[3]])
            midPart2 = '_'.join([image2Split[3]])
            if midPart1 < midPart2:
                return True
            else:
                return False
        else:
            return False
    else:
        return False

def main():
    images = ['1_1_1_1_1_1', '1_1_1_1_1_2', '1_1_1_2_1_1', '1_1_1_2_1_2']
    images2 = []
    images2.append(object)
    images.sort(key = image_path_comparation)
    len = len(images)
    len += 1
    
main()