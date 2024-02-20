import asyncio
import cv2
from PIL import Image
from io import BytesIO
import numpy as np
from websockets.sync.client import connect

def hello():
    with connect("ws://smart-garden/ws") as websocket:
        websocket.send("Hello world!")
        message = websocket.recv()
        print(f"Received: {message}")

def capture():
    with connect("ws://smart-garden/ws") as websocket:
        websocket.send("capture")
        message = websocket.recv()
        file = open("RX_Image.jpg", "wb")
        file.write(message)
        file.close()

def stream():
    with connect("ws://smart-garden/ws") as websocket:
        websocket.send("stream")
        while True:
            msg = websocket.recv()
            img = np.asarray(Image.open(BytesIO(msg)))
            cv2.imshow("Image", img)
            key = cv2.waitKey(1) & 0xFF
            if key == ord('q'):
                websocket.close()

stream()