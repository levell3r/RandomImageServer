from re import X
import numpy as np
import cv2
import os
import random
from flask import Flask, Response
from utils import prepare_image



#Random Image Picture with new hardcoded paths
img_path = random.choice(os.listdir("/home/dan/miniwebserver/images/"))
logo_img_orig = cv2.imread("/home/dan/miniwebserver/images/" + img_path, cv2.IMREAD_GRAYSCALE)

#Prep Image dimensions & 64b
logo_img, logo_img_width, logo_img_height = prepare_image(logo_img_orig)


#Setup the Flask App
app = Flask(__name__)
i = 0

@app.route("/image")
def image():
    global i
    if i % 1 == 0:
        response = Response(
            response=logo_img.tobytes(),
            headers={
                'Image-Width': logo_img_width,
                'Image-Height': logo_img_height
            }
        )
    i += 1
    return response

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=12345),


