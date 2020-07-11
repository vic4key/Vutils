from socket import *

with socket(AF_INET, SOCK_STREAM, IPPROTO_IP) as s:
  print("created")

  s.connect(("127.0.0.1", 1609))
  _, p = s.getsockname()
  print(f"connected {p}")

  # input("enter to send ...") # for test listen blocking on server
  # s.sendall(b"hello from client")
  # print("sent")

  d = s.recv(1024)
  print("received", d)

  s.sendall(b"hello from client\x00")
  print("sent")

  s.close()
  print("closed")