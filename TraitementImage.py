import numpy as np
import matplotlib.pyplot as plt
import cv2

def loadImage(chemin):
    img = cv2.imread(chemin)
    img = cv2.cvtColor(img,cv2.COLOR_BGR2RGB)
    red,green,blue = cv2.split(img)
    plt.figure(figsize=(16,4))
    plt.subplot(141)
    plt.title('Originale')
    plt.imshow(img)   
    plt.subplot(142)
    plt.title('Rouge')
    plt.imshow(red,cmap='gray')
    plt.subplot(143)
    plt.title('Vert')
    plt.imshow(green,cmap='gray')
    plt.subplot(144)
    plt.title('Bleu')
    plt.imshow(blue,cmap='gray')
    plt.show()
    return img,red,green,blue

def seuillage(image):
    seuil=21
    seuil, img = cv2.threshold(image, seuil, 255, cv2.THRESH_BINARY_INV)
    plt.imshow(img,cmap = 'gray')
    plt.show()
    return img

def segmentation(image):
    noyau = np.ones((5,5),np.int8)
    plt.figure(figsize=(12,4))
    plt.subplot(131)
    plt.imshow(image,cmap='gray') 
    img = cv2.dilate(image,noyau,1)
    plt.subplot(132)
    plt.imshow(img,cmap='gray') 
    img = cv2.erode(img,noyau,1)
    plt.subplot(133)
    plt.imshow(img,cmap='gray') 
    plt.show()
    return img

def rechercheContour(imageOrigine, imageSegmente):
    img2,contours, hierarchy = cv2.findContours(imageSegmente,cv2.RETR_TREE,cv2.CHAIN_APPROX_SIMPLE)
    if hierarchy.any():
        cnt=contours[0]
        rect = cv2.minAreaRect(cnt)
        box = cv2.boxPoints(rect)
        box = np.int0(box)
        cv2.drawContours(imageOrigine,[box],0,(0,255,0),2)
        plt.imshow(imageOrigine)
        plt.show()


if __name__ == "__main__":
    imageOrigine,imageRed,imageGreen,imageBlue = loadImage("resistance.png")
    imageSeuil = seuillage(imageGreen)
    imageSegmente = segmentation(imageSeuil)
    rechercheContour(imageOrigine,imageSegmente)


