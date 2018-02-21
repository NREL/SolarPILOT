import os
dirs = [os.getcwd() + "\\..\\app", os.getcwd() + "..\\..\\..\\ssc\\solarpilot"]
topdir = os.getcwd()
count = 0
for dr in dirs:
    os.chdir(dr)
    objects = os.listdir(os.getcwd())

    for obj in objects:
        if obj.split(".")[-1] in ["cpp","h"]:
            count += len([line for line in open(obj,"r").readlines()])
        
print count

raw_input("Hit Enter...")
