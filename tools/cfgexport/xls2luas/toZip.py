#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os
import sys
import shutil
from summary_tools import *
from descriptor import *
import plugins
import zipfile

ignoreList = []

def isInIgnoreList(fname):
    for i in ignoreList:
        iff = i
        if platform.system() == 'Windows':
            iff = i.decode('utf8').encode('gbk')
        if iff == fname :
            return True;
    return False

def main() :
    xlspath = sys.argv[1]
    topath = sys.argv[2]
    ver = sys.argv[3]
    verLua = sys.argv[4]

    
    print("\n\n\n\nDo Zip..........................")

    zip_archive_path = topath + '/archive/'
    if not os.path.exists(zip_archive_path):
        os.mkdir(zip_archive_path) 

    file_zip = zipfile.ZipFile(zip_archive_path+'config_lua.zip','w',zipfile.ZIP_DEFLATED) 
    startdir = topath + '/lua/'
    for dirpath, dirnames, filenames in os.walk(startdir): 
        for filename in filenames: 
            tozipfile = os.path.relpath(os.path.join(dirpath,filename), startdir)  
            print('zipfile ' + tozipfile)
            file_zip.write(os.path.join(dirpath,filename), tozipfile) 
    file_zip.close() 

    #write_file_summary(file_sumary,summary_path)

if __name__ == "__main__" :
    main()
