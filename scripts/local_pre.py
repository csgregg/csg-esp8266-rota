import platform
import time

Import("env")

print("Extra Script (Pre): local_pre.py")

FILENAME_BUILDNO = '.version'
FILENAME_VERSION_H = 'include/Version.h'

build_no = 0
try:
    with open(FILENAME_BUILDNO) as f:
        build_no = int(f.readline()) + 1
except:
    print('Starting build number from 1..')
    build_no = 1
with open(FILENAME_BUILDNO, 'w+') as f:
    f.write(str(build_no))
    print('Build number: {}'.format(build_no))


timeStr = time.ctime()
print('Current Timestamp : ', timeStr)

hf = """
#define BUILD_NUMBER {}
#define BUILD_TIMESTAMP {}
""".format(build_no, timeStr)
with open(FILENAME_VERSION_H, 'w+') as f:
    f.write(hf)


## Could also do it this way but it makes it do full rebuild each time
# env.Append(CPPDEFINES=[
#   ("LOCAL_BUILD_NUMBER", build_no)
# ])


## Define platform name
print('Build platform: ' + platform.node())
env.Append(CPPDEFINES=[
  ("BUILD_ENV", platform.node())
])