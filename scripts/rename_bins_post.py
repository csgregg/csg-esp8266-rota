import gzip
import shutil
import os
import subprocess

Import("env", "projenv")


# Get build flags values from env
def get_build_flag_value(flag_name):
    build_flags = env.ParseFlags(env['BUILD_FLAGS'])
    flags_with_value_list = [build_flag for build_flag in build_flags.get('CPPDEFINES') if type(build_flag) == list]
    defines = {k: v for (k, v) in flags_with_value_list}
    return defines.get(flag_name)



# Compress firmware using gzip for 'compressed OTA upload'
def compressFirmware(source):

    print("Compressing firmware: " + source)
    os.system("gzip -9 -k " + source)

    target_path = os.path.dirname(os.path.abspath(source))
    print("Dir: " + target_path )
    for dirfiles in os.listdir(target_path):
        print(dirfiles)

    if os.path.exists(source +'.bak'):
        ORG_FIRMWARE_SIZE = os.stat(source + '.bak').st_size
        GZ_FIRMWARE_SIZE = os.stat(source + '.gz').st_size

        print("Compression reduced firmware size by {:.0f}% (was {} bytes, now {} bytes)".format((GZ_FIRMWARE_SIZE / ORG_FIRMWARE_SIZE) * 100, ORG_FIRMWARE_SIZE, GZ_FIRMWARE_SIZE))



# Change file system image name and compress
def prepareBINs(*args, **kwargs):
    print("Extra Script (Post): rename_bins_post.py")

    fstarget = str(kwargs['target'][0])
    target_path = os.path.dirname(os.path.abspath(target))
    progtarget = os.path.join(target_path, env['PROGNAME']+".bin")

    new_fstarget = "%s-Fv%s.bin" % (get_build_flag_value("DEVICE_CODE"), get_build_flag_value("BUILD_TAG"))
    os.rename(fstarget, os.path.join(target_path, new_fstarget))

    compressFirmware(progtarget)
    compressFirmware(new_fstarget)



# Set up call after FS bin has been created
env.AddPostAction("$BUILD_DIR/littlefs.bin", prepareBINs) 