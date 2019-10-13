import os
import gzip
import shutil

Import("env")

print("Extra Script - Pre")

try:
    os.mkdir("data")
except OSError:
    print ("<data> folder exists")
else:
    print ("Created <data> folder")

# Get build flags values from env
def get_build_flag_value(flag_name):
    build_flags = env.ParseFlags(env['BUILD_FLAGS'])
    flags_with_value_list = [build_flag for build_flag in build_flags.get('CPPDEFINES') if type(build_flag) == list]
    defines = {k: v for (k, v) in flags_with_value_list}
    return defines.get(flag_name)

# Gzip files in one folder to another
def deflate_www(sourceFolder,destFolder):

    p_sourceFolder = os.path.abspath(env.subst("$PROJECT_DIR") + "/" + sourceFolder)
    p_destFolder = os.path.abspath(env.subst("$PROJECT_DIR") + "/" + destFolder)

    print('Deflating www files in ' + p_sourceFolder)
    print('To ' + p_destFolder)

    for folder, subfolders, files in os.walk(p_sourceFolder):
        for file in files:
            print(os.path.join(folder, file))
            with open(os.path.join(folder, file), 'rb') as f_in:
                with gzip.open(os.path.join(p_destFolder, file) + '.gz', 'wb') as f_out:
                    shutil.copyfileobj(f_in, f_out)


# Current details
print("Device Code: %s" % get_build_flag_value("DEVICE_CODE"))
print("Build tag: %s" % get_build_flag_value("BUILD_TAG"))

# Deflate www into data
deflate_www("www","data")

# Change build file name
new_name = "%s-Pv%s" % (get_build_flag_value("DEVICE_CODE"), get_build_flag_value("BUILD_TAG"))
print("Changing build file name to: %s" % new_name)
env.Replace(PROGNAME=new_name)
