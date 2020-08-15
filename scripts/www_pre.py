import os
import gzip
import shutil
import requests

import html

# Install packages
os.system("python -m pip install --upgrade pip")
os.system("python -m pip install beautifulsoup4")
from bs4 import BeautifulSoup

Import("env")

print("Extra Script (Pre): www_pre.py")

# Create folders
try:
    os.mkdir("data")
except OSError:
    print ("<data> folder exists")
else:
    print ("Created <data> folder")

try:
    os.mkdir("data/tmp")
except OSError:
    print ("<data/tmp> folder exists")
else:
    print ("Created <data/tmp> folder")

try:
    os.mkdir("data/www")
except OSError:
    print ("<data/www> folder exists")
else:
    print ("Created <data/www> folder")


# Get build flags values from env
def get_build_flag_value(flag_name):
    build_flags = env.ParseFlags(env['BUILD_FLAGS'])
    flags_with_value_list = [build_flag for build_flag in build_flags.get('CPPDEFINES') if type(build_flag) == list]
    defines = {k: v for (k, v) in flags_with_value_list}
    return defines.get(flag_name)


def minify_file(file,url):
    print("Minifying: " + str(file))
    data = {'input': open(file, 'rb').read()}
    response = requests.post(url, data=data)
    minified = response.content
    with open(file, 'wb') as f_out:
        f_out.write(minified)

            

# Replace build codes and move to new folder
def parse_replace(sourceFolder,destFolder):
    p_sourceFolder = os.path.abspath(env.subst("$PROJECT_DIR") + "/" + sourceFolder)
    p_destFolder = os.path.abspath(env.subst("$PROJECT_DIR") + "/" + destFolder)

    print('Parsing html files in ' + p_sourceFolder)

    for folder, subfolders, files in os.walk(p_sourceFolder):
        for file in files:
            if file.endswith('.html'):
                with open(os.path.join(folder, file), 'rb') as f_in:
                    content = f_in.read()
                    soup = BeautifulSoup(content, 'html')
                    for item in soup.find_all(class_='bs-replace'):
                        if str(item) != 'None':
                            flag = item.string
                            print('Replacing ' + flag + ' with \'' + get_build_flag_value(flag) + '\' in ' + file )
                            item.string = get_build_flag_value(flag) 
                with open(os.path.join(p_destFolder,file),'w') as f_out:
                    f_out.write(str(soup))
                minify_file(os.path.join(p_destFolder,file),'https://html-minifier.com/raw')
            elif file.endswith('.js'):
                shutil.copyfile(os.path.join(folder, file), os.path.join(p_destFolder,file))
                minify_file(os.path.join(p_destFolder,file),'https://javascript-minifier.com/raw')
            elif file.endswith('.css'):
                shutil.copyfile(os.path.join(folder, file), os.path.join(p_destFolder,file))
                minify_file(os.path.join(p_destFolder,file),'https://cssminifier.com/raw')
            else:
                shutil.copyfile(os.path.join(folder, file), os.path.join(p_destFolder,file))


# Empty folder
def deletecontents(folder):
    for root, dirs, files in os.walk(folder):
        for file in files:
            os.remove(os.path.join(root, file))


# Gzip files in one folder to another
def deflate_www(sourceFolder,destFolder):

    p_sourceFolder = os.path.abspath(env.subst("$PROJECT_DIR") + "/" + sourceFolder)
    p_destFolder = os.path.abspath(env.subst("$PROJECT_DIR") + "/" + destFolder)

    print('Deflating www files in ' + p_sourceFolder)
    print('To ' + p_destFolder)

    for folder, subfolders, files in os.walk(p_sourceFolder):
        for file in files:
            with open(os.path.join(folder, file), 'rb') as f_in:
                with gzip.open(os.path.join(p_destFolder, file) + '.gz', 'wb') as f_out:
                    shutil.copyfileobj(f_in, f_out)



# Clean up
deletecontents("data/tmp")
deletecontents("data")

# Parse and replace 
parse_replace("www","data/tmp")

# Deflate www into data
deflate_www("data/tmp","data/www")

# Clean up
#shutil.rmtree("data/tmp")
