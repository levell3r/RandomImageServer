import cv2
import numpy as np


def preprocessImage(img, max_width, max_height):
  
    cols, rows = img.shape
    brightness = np.sum(img) / (255 * cols * rows)
    minimum_brightness = 0.56
    ratio = brightness / minimum_brightness
    
    ## adjust brightness to get the target brightness
    img = cv2.convertScaleAbs(img, alpha = 1 / ratio, beta = 50 * (minimum_brightness - brightness))
        
    ## brightness adjustment attempt 1  - TOO CONTRASTY :(
    #img = cv2.addWeighted(img, 1, np.zeros(img.shape, img.dtype), 0, 0)

    ##Generic img resize logic
    ratio = float(img.shape[1]) / float(img.shape[0])
    ratio_resize = float(max_width) / float(max_height)
    if ratio < ratio_resize:
        target_width = int(max_height * ratio)
        if target_width % 2 == 1:
            target_width = target_width + 1
        img = cv2.resize(img, (target_width, max_height))
    else:
        # we can do max height
        target_height = int(max_width / ratio)
        if target_height % 2 == 1:
            target_height = target_height + 1
        img = cv2.resize(img, (max_width, target_height))
    return img



def prepare_image(img):
    ##Adjust Resolution for LilyGo T5 4.7 of 960x540
    HOR_RES, VERT_RES = 960, 540
    img2 = preprocessImage(img, HOR_RES, VERT_RES)
    img3 = img2.reshape(-1)
    img3 = (img3 / 16).astype(np.uint8)
    img4 = np.empty((img3.shape[0] // 2,), np.uint8)
    for i in range(0, img3.shape[0], 2):
        img4[i // 2] = img3[i+1] << 4 | img3[i]
    return img4, img2.shape[1], img2.shape[0]

