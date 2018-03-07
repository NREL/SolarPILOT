# -*- coding: utf-8 -*-
"""
Created on Tue Mar 06 21:55:52 2018

@author: mwagner

This script pulls solar resource files from SAM (if installed) and copies locations
that exceed a desired annual DNI threshold.

"""

#DNI screening

import os
import shutil

source = "../../sam/deploy/solar_resource"
dest = "../deploy/climate_files"

dnidir = []

#process thte SAM solar resource files to calculate annual DNI
for f in os.listdir(source):
    dni = 0.
    for line in open(os.path.join(source,f), 'r').readlines()[3:]:
        dat = line.split(',')
        dni +=  float(dat[4])
    
    dnidir.append([f, dni])
    
dnidir.sort(key=lambda x: x[1], reverse=True)

"""
#plot sorted list DNI values to get a picture of distribution
import matplotlib.pyplot as plt
plt.plot(range(len(dnidir)), [x[1] for x in dnidir], '.-')
plt.show()
"""

#delete existing weather files in the solarpilot directory
for f in os.listdir(dest):
    os.unlink(os.path.join(dest, f))

threshold = 1.7e6 #w-h/m2

for f,x in dnidir:
    
    if x < threshold:
        break
    print "Copying " + f
    shutil.copyfile(os.path.join(source,f), os.path.join(dest, f))
    
