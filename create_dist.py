"""
This copies the contents of the "deploy" folder that are needed to run the program
into a "distributable" folder. This allows quick exclusion of .svn files, debug files,
and all of the unzipped weather files.
"""


import os
import shutil
import time

cd = os.getcwd()

sourcedir = cd+"\\deploy"

destdir = cd+"\\distribution"

if os.path.exists(destdir):
    shutil.rmtree(destdir)

exists = os.path.exists(destdir)
#print exists, destdir
if not exists:
    os.mkdir(destdir)

time.sleep(0.5)

def rec_copy(sdir, ddir, excludes=".svn;.tm2;.tm3;.epw;.smw;.swrf;_Debug.exe;.ilk;.suo;install_tools"):
    """
    Recursively copy the source directory to the destination directory
    while excluding items with "exclude" attributes.
    """
    exs = excludes.split(";")
    
    items = os.listdir(sdir)
    
    for item in items:
        #check if any exclusions match
        skip = False
        for ex in exs:
            if ex in item:
                if item != "CA Daggett.tm2":
                    skip = True
        if skip: continue
        
        #if it's a file copy it from source to destination
        #use the rule that if '.' appears, it's a file
        if "." in item:
            print "Copying "+sdir+"\\"+item+" to "+ddir+"\\"+item
            shutil.copyfile(sdir+"\\"+item, ddir+"\\"+item)
        else:   #it's a path
            print "Creating directory "+ddir+"\\"+item
            os.mkdir(ddir+"\\"+item)
            rec_copy(sdir+"\\"+item, ddir+"\\"+item, excludes)
            
    
    
rec_copy(sourcedir, destdir)
    
print "Complete"
    