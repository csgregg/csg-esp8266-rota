import platform
import time

Import("env")

print("Extra Script (Pre): travis_pre.py")


timeStr = time.ctime()
print('Current Timestamp : ', timeStr)


env.Append(CPPDEFINES=[
  ("BUILD_TIMESTAMP", timeStr)
])
