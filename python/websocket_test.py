
import socket
import hashlib
import base64
import asyncio
from time import sleep
from websockets.server import serve

async def rx(websocket):
    async for message in websocket:
        print(message)
        
async def program():
    print("hahaha")

async def main():
    async with serve(rx, "", 65432):
        await asyncio.Future()  # run forever
    async with program():
        await asyncio.Future()

asyncio.run(main())

# HOST = ''  # Standard loopback interface address (localhost)
# PORT = 65432  # Port to listen on (non-privileged ports are > 1023)

# with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
#     s.bind((HOST, PORT))
#     s.listen()
#     conn, addr = s.accept()
#     with conn:
#         print(f"Connected by {addr}")
#         rx = conn.recv(1024)
#         rx = rx.decode()
#         key = rx.split("\r\n")[6] 
#         key = key.split(": ")[1]
#         print(f"Received Key: {key}")
#         new_key = key + "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"
#         new_key = new_key.encode()
#         new_key = hashlib.sha1(new_key)
#         new_key = new_key.digest()
#         new_key = base64.b64encode(new_key)
#         new_key = new_key.decode()
#         print(new_key)
#         data = f"Upgrade: websocket\r\nConnection: Upgrade\r\nSec-WebSocket-Accept: {new_key}\r\n\r\n".encode()
#         conn.sendall(data)
#         while True:
#             #data = conn.recv(1024)
#             data = "Hello World\n"
#             if not data:
#                 break
#             data = data.encode()
#             conn.sendall(data)