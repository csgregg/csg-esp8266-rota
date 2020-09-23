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

    new_fstarget = os.path.join(target_path, "%s-Fv%s.bin" % (get_build_flag_value("DEVICE_CODE"), get_build_flag_value("BUILD_TAG")))

    print(target_path)
    os.system("ls -las " + target_path)

    os.rename(fstarget, new_fstarget)

    os.system("ls -las " + target_path)
    
    os.system("gzip -9 -k " + progtarget)
    os.system("gzip -9 -k " + new_fstarget)

    os.system("ls -las " + target_path)



# Set up call after FS bin has been created
env.AddPostAction("$BUILD_DIR/littlefs.bin", print(os.path.dirname(os.path.abspath(str(kwargs['target'][0]))))) 
env.AddPostAction("$BUILD_DIR/${PROGNAME}.bin", prepareBINs) 