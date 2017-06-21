"""
Update the version number in files used to compile SolarPILOT

Files:
    src/GUI_main.cpp
    deploy/BuildInstaller_64bit.iss
    deploy/BuildInstaller_Win32.iss
"""

import re
import datetime


files = ["src/UI/GUI_main.cpp","deploy/BuildInstaller_64bit.iss","deploy/BuildInstaller_Win32.iss"]


files_data = {}

for f in files:
    files_data[f] = [line for line in open(f,'r').readlines()]

#Get the previous version number from the GUI_main.cpp file
for line in files_data[files[0]]:
    if "_software_version = \"" in line:
        g = re.search("\"(.*?)\"",line)
        try:
            lastdate = g.group(1)
        except:
            print "Problem parsing date. Text was:\n\"%s\""%line
            raw_input("Hit enter...")
            raise AttributeError

#we now have a date to replace
today = datetime.date.today()
ftoday = "%d.%d.%d"%(today.year, today.month, today.day)
print "\nReplacing original version number \"%s\" with today's date \"%s\"\n"%(lastdate, ftoday)
        


for f in files:
    for i,line in enumerate(files_data[f]):
        if lastdate in line:
            files_data[f][i] = files_data[f][i].replace(lastdate,ftoday)
            print "Replacing %s | %d | %s"%(f,i+1,line.replace("\n",""))


#write the revised files
for f in files:
    fo = open(f,"w")
    for line in files_data[f]:
        fo.write(line)
    fo.close()


