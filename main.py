from picamera2 import Picamera2

import RPi.GPIO as GPIO
import cv2
import numpy as np
import imutils

PIN_ROSSO = 5  # 29
PIN_VERDE = 6  # 31

GPIO.setmode(GPIO.BCM)
GPIO.setup(PIN_ROSSO,GPIO.OUT)
GPIO.setup(PIN_VERDE,GPIO.OUT)

cv2.startWindowThread()  # permette l'aggiornamento di cv2.imshow()
camera = Picamera2()  # assegna la videocamera e assegna il video a camera
camera.configure(
camera.create_preview_configuration(main={"format": 'XRGB8888', "size": (160, 120)}))  # configura la videocamera
camera.controls.Brightness = 0
camera.set_controls({"ExposureTime": 10000, "AnalogueGain": 1.0, "AeEnable": 0})  # controllo esposizione
camera.start()  # avvia la videocamera

def trova_colore(img, lower, upper):
    hsv = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)    
    mask = cv2.inRange(hsv, lower, upper)
    return mask 

def trova_verde(immagine):
    lower = np.array([45,15,0])
    upper = np.array([80,255,255])
    return trova_colore(immagine, lower, upper)

def trova_rosso(immagine):
    lower1 = np.array([0,200,0])
    upper1 = np.array([8,255,255])
    lower2 = np.array([170,200,0])
    upper2 = np.array([180,255,255])
    mask1 = trova_colore(immagine, lower1, upper1)
    mask2 = trova_colore(immagine, lower2, upper2)
    return cv2.bitwise_or(mask1,mask2)

def bounding_rect_area(mask):
    cnts = cv2.findContours(mask.copy(), cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
    cnts = imutils.grab_contours(cnts)
    if len(cnts) != 0:
        c = max(cnts, key=cv2.contourArea)
        x, y, w, h = cv2.boundingRect(c)
        return w * h
    return -1
    

def main():
    while True:
        frame = camera.capture_array()
        #frame = cv2.flip(frame, 0) #decommentare in caso che la videocamera è al contrario
        filter_rosso = trova_rosso(frame)
        #cv2.imshow("MASK ROSSO", filter_rosso)
        filter_verde = trova_verde(frame)
        #cv2.imshow("MASK VERDE", filter_verde)
        
        obs_rosso = bounding_rect_area(filter_rosso)
        obs_verde = bounding_rect_area(filter_verde)
        
        if obs_rosso > 1000 or obs_verde > 1000:
            if obs_rosso > obs_verde:
                GPIO.output(PIN_VERDE,GPIO.LOW)
                GPIO.output(PIN_ROSSO,GPIO.HIGH)
            elif obs_verde > obs_rosso:
                GPIO.output(PIN_ROSSO,GPIO.LOW)
                GPIO.output(PIN_VERDE,GPIO.HIGH)
        else:
            GPIO.output(PIN_VERDE,GPIO.LOW)
            GPIO.output(PIN_ROSSO,GPIO.LOW)

if __name__ == "__main__":
    main()