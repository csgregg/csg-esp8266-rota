Import("env")


print("Extra Script (Pre): common_pre.py")

# Get build flags values from env
def get_build_flag_value(flag_name):
    build_flags = env.ParseFlags(env['BUILD_FLAGS'])
    flags_with_value_list = [build_flag for build_flag in build_flags.get('CPPDEFINES') if type(build_flag) == list]
    defines = {k: v for (k, v) in flags_with_value_list}
    return defines.get(flag_name)


# Current details
print("Device Code: %s" % get_build_flag_value("DEVICE_CODE"))
print("Build tag: %s" % get_build_flag_value("BUILD_TAG"))

# Change build file name
new_name = "%s-Pv%s" % (get_build_flag_value("DEVICE_CODE"), get_build_flag_value("BUILD_TAG"))
print("Changing build file name to: %s" % new_name)
env.Replace(PROGNAME=new_name)
