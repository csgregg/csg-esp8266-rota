import os

Import("env", "projenv")

print("Extra Script - Post")

# Get build flags values from env
def get_build_flag_value(flag_name):
    build_flags = env.ParseFlags(env['BUILD_FLAGS'])
    flags_with_value_list = [build_flag for build_flag in build_flags.get('CPPDEFINES') if type(build_flag) == list]
    defines = {k: v for (k, v) in flags_with_value_list}
    return defines.get(flag_name)


# Change file system image name
def change_spiffs_name(*args, **kwargs):
    target = str(kwargs['target'][0])
    target_path = os.path.dirname(os.path.abspath(target))
    new_target = "%s-Fv%s.bin" % (get_build_flag_value("DEVICE_CODE"), get_build_flag_value("BUILD_TAG"))

    print("Target : %s" % target)
    print("New image file: %s" % new_target)

    os.rename(target, os.path.join(target_path, new_target))

env.AddPostAction("$BUILD_DIR/spiffs.bin", change_spiffs_name) 