#!/usr/bin/env python

# Will split a png cubemap/skymap image produced by blender into 6 separated image files for use in a skybox within unity
# Requires Python Imaging Library > http://www.pythonware.com/products/pil/

import os
import sys

from PIL import Image

path = os.path.abspath("") + "/"
processed = False


def processImage(path, name):
    name = name[2:]
    img = Image.open(os.path.join(path, name))
    size = img.size[0] / 4
    
    splitAndSave(img, 0, size, size, addToFilename(name, "_left"))
    splitAndSave(img, size * 2, size, size, addToFilename(name, "_right"))
    splitAndSave(img, size * 3, size, size, addToFilename(name, "_back"))
    splitAndSave(img, size, size * 2, size, addToFilename(name, "_bottom"))
    splitAndSave(img, size, 0, size, addToFilename(name, "_top"))
    splitAndSave(img, size, size, size, addToFilename(name, "_front"))


def addToFilename(name, add):
    name = name.split('.')
    return name[0] + add + "." + name[1]


def splitAndSave(img, startX, startY, size, name):
    area = (startX, startY, startX + size, startY + size)
    saveImage(img.crop(area), path, name)


def saveImage(img, path, name):
    try:
        img.save(os.path.join(path, name))
    except:
        print("Error: Could not convert image.")
        pass


for arg in sys.argv:

    if ".png" in arg or ".jpg" in arg:
        processImage(path, arg)
        processed = True

if not processed:
    print("Error: No image")
    print("Usage: 'python SkyboxSplitter.py --image-name.png'")
