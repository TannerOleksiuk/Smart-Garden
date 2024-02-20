# Tinker Foundry
# Using websockets with ESP32 to transfer image data...
# Reference... https://shawnhymel.com/1675/arduino-websocket-server-using-an-esp32/
# also https://www.dfrobot.com/blog-1194.html?tracking=5cc027fb1dc4e
import websocket # pip install websocket-client
import numpy as np
import matplotlib.pyplot as plt
 
# Image dimensions - this needs to match our setting on the ESP32 CAM board
IMG_X = 320 # QVGA 320x240
IMG_Y = 240

ws = websocket.WebSocket()
ws.connect("ws://smart-garden/ws") # Use the IP address from the ESP32 board - printed to the serial monitor

while(1):
    # Ask the user for some input and transmit it
    str = input("Say something: ")
    # sent the input string across the socket
    ws.send('capture')
    binResp = ws.recv_frame() # receiving binary image data (we assume grey scale) from camera
    # we are going to store the received binary image data in an array... 
    img_array = np.zeros((IMG_Y,IMG_X))
    x = 0
    y = 0
    count = 0
    # traverse the captured data one byte at a time and populate image array
    for byte in bytearray(binResp.data):
       #print (byte)
       y = count // IMG_X
       x = count % IMG_X
       img_array[y,x] = byte
       count = count + 1
    # display captured image
    plt.figure('Capture Image 1')
    plt.imshow(img_array)
    plt.set_cmap('gray')
    plt.show(block=False)

# Gracefully close WebSocket connection
ws.close()