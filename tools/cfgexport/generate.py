#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os
import sys
import time
import xls2cpp

lang_keyword = "language"
patch_keyword = "Patch"

def main() :
    version = ""
    language = {}
    if len(sys.argv) != 3 :
        print( "%s <xlspath> <cpppath> ." % sys.argv[0] )
        exit( 0 )
    xlspath = sys.argv[1]
    cpppath = sys.argv[2]
    # 检查目录是否存在
    if not os.path.exists( xlspath ) :
        print( xlspath, " is not exist !" )
        exit(0)
    if not os.path.exists( cpppath ) :
        print(cpppath, " is not exist !")
        exit(0)
    # 遍历xlspath
    for filename in os.listdir(xlspath) :
        if filename.find( '~$' ) != -1 : continue
        if not os.path.isfile( xlspath + '/' + filename ) : continue
        name, suffix = os.path.splitext( filename )
        if suffix != '.xls' and suffix != '.xlsx' and suffix != '.xlsm' :
            continue
        pos = name.find('_')
        if pos < 0 : engname = name
        else : engname = name[pos+1:]
        if engname.find( lang_keyword ) != -1 or engname.find( patch_keyword ) != -1 :
            continue
        xlsfile = xlspath + '/' + filename
        cppfile = cpppath + '/' + engname + '.hpp'
        print('Generate Config Code(s) ' + xlsfile + ' To ' + cppfile + ' ...')
        opexport = xls2cpp.XlsExport2Cpp( xlsfile, cppfile )
        opexport.export()
    print('Generate Config Code(s) Succeed .')

if __name__ == "__main__" :
    main()
