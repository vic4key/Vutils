from socket import *

with socket(AF_INET, SOCK_STREAM, IPPROTO_IP) as s:
  print("created")

  s.connect(("127.0.0.1", 1609))
  _, p = s.getsockname()
  print(f"connected {p}")

  d = s.recv(0)
  print("received", d)

  s.close()
  print("closed")