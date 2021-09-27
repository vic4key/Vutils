@ECHO OFF

FOR /L %%A IN (0, 1, 4) DO ( REM [0, 4] -> 5 times
  START "Socket Client %%A" py3 ./Test.Socket.Client.py
  ECHO.
)