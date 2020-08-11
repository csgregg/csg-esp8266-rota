import os

Import("env", "projenv")


# Get build flags values from env
def get_build_flag_value(flag_name):
    build_flags = env.ParseFlags(env['BUILD_FLAGS'])
    flags_with_value_list = [build_flag for build_flag in build_flags.get('CPPDEFINES') if type(build_flag) == list]
    defines = {k: v for (k, v) in flags_with_value_list}
    return defines.get(flag_name)



# Change file system image name and compress
def prepareBINs(*args, **kwargs):
    print("Extra Script (Post): process_bins_post.py")

    fstarget = str(kwargs['target'][0])
    target_path = os.path.dirname(os.path.abspath(fstarget))
    progtarget = os.path.join(target_path, env['PROGNAME']+".bin")

    new_fstarget = "%s-Fv%s.bin" % (get_build_flag_value("DEVICE_CODE"), get_build_flag_value("BUILD_TAG"))
    os.rename(fstarget, os.path.join(target_path, new_fstarget))

    os.system("gzip -9 -k " + progtarget)
    os.system("gzip -9 -k " + new_fstarget)

    print("Dir: " + target_path )
    for dirfiles in os.listdir(target_path):
        print(dirfiles)



# Set up call after FS bin has been created
env.AddPostAction("$BUILD_DIR/littlefs.bin", prepareBINs) 