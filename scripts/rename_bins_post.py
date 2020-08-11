import gzip
import shutil
import os

Import("env", "projenv")

print("Extra Script (Post): rename_bins_post.py")

# Get build flags values from env
def get_build_flag_value(flag_name):
    build_flags = env.ParseFlags(env['BUILD_FLAGS'])
    flags_with_value_list = [build_flag for build_flag in build_flags.get('CPPDEFINES') if type(build_flag) == list]
    defines = {k: v for (k, v) in flags_with_value_list}
    return defines.get(flag_name)



# Compress firmware using gzip for 'compressed OTA upload'
def compressFirmware(source):

    if not os.path.exists(source +'.bak'):
        print("Compressing firmware: " + source)
        shutil.move(source, source + '.bak')
        with open(source + '.bak', 'rb') as f_in:
            with gzip.open(source + '.gz', 'wb') as f_out:
                shutil.copyfileobj(f_in, f_out)

    if os.path.exists(source+'.bak'):
        ORG_FIRMWARE_SIZE = os.stat(source + '.bak').st_size
        GZ_FIRMWARE_SIZE = os.stat(source).st_size

        print("Compression reduced firmware size by {:.0f}% (was {} bytes, now {} bytes)".format((GZ_FIRMWARE_SIZE / ORG_FIRMWARE_SIZE) * 100, ORG_FIRMWARE_SIZE, GZ_FIRMWARE_SIZE))



# Change file system image name
def change_littleFS_name(*args, **kwargs):
    target = str(kwargs['target'][0])
    target_path = os.path.dirname(os.path.abspath(target))

    print("Dir: " + target_path )
    for dirfiles in os.listdir(target_path):
        print(dirfiles)

    progtarget = os.path.join(target_path, env['PROGNAME']+".bin")

    print("Compress prog")
    print(progtarget)
    compressFirmware(progtarget)

    print("Compress FS")
    print(target)
    compressFirmware(target)

    new_target = "%s-Fv%s.bin" % (get_build_flag_value("DEVICE_CODE"), get_build_flag_value("BUILD_TAG"))

    print("Target : %s" % target)
    print("New image file: %s" % new_target)

    os.rename(target, os.path.join(target_path, new_target))



env.AddPostAction("$BUILD_DIR/littlefs.bin", change_littleFS_name) 