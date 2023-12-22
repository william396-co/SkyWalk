#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os
import sys
import time
import xls2xml
import hashlib

lang_keyword = "language"
patch_keyword = "Patch"

# 生成svn版本文件
def create_version_file( xmlpath, revision, metaversion, gitversion, exclversion, exclpatchversion ) :
    versionfile = xmlpath + "/" + "exportversion.xml"
    doc = xls2xml.XmlDocument( versionfile )
    doc.add( "revision", revision )
    doc.add( "excelversion", exclversion )
    doc.add( "excelpatchversion", exclpatchversion )
    doc.add( "timestamp", str(int(time.time())) )
    subnode = doc.add( "battleversion" );
    doc.add2( subnode, "meta", metaversion );
    doc.add2( subnode, "script", gitversion );
    doc.save2()
    print( "Create VersionFile(Revision:%s) '%s' ." % ( revision, versionfile ) )

# 搜索语言包文件
def search_langfiles( xlspath, lang = "" ) :
    filenames = []
    full_lang_keyword = lang_keyword
    if lang != "" :
        full_lang_keyword = lang_keyword + lang.upper()
    for filename in os.listdir(xlspath) :
        if filename.find( '~$' ) != -1 : continue
        if not os.path.isfile( xlspath + '/' + filename ) : continue
        name, suffix = os.path.splitext( filename )
        if suffix != '.xls' and suffix != '.xlsx' and suffix != '.xlsm' : continue
        pos = name.find('_')
        if pos < 0 : continue
        engname = name[pos+1:]
        if engname.find( full_lang_keyword ) != -1 : filenames.append(filename)
    return filenames

def search_patchfiles( xlspath, lang = "" ) :
    full_patch_keyword = lang + patch_keyword
    for filename in os.listdir(xlspath) :
        if filename.find( '~$' ) != -1 : continue
        if not os.path.isfile( xlspath + '/' + filename ) : continue
        name, suffix = os.path.splitext( filename )
        if suffix != '.xls' and suffix != '.xlsx' and suffix != '.xlsm' : continue
        pos = name.find('_')
        if pos < 0 : continue
        engname = name[pos+1:]
        if engname.find( full_patch_keyword ) != -1 : return filename
    return ""

def import_language( xlspath, lang ) :
    language = {}
    filenames = search_langfiles( xlspath )
    for filename in filenames :
        langfilepath = xlspath + '/' + filename
        input = xls2xml.XlsExport2Xml( langfilepath, "" )
        tmp = input.importlang( lang )
        language.update( tmp )
        print('Import Language(%s) From "%s", Found %d Entries .' % ( lang, filename, len(tmp) ) )
    return language

def generate_battle_version( xlspath ):
    filenames = []
    # battle_filelist
    battlefile = open( xlspath + '/battle_filelist.txt', 'r' )
    filenames.extend( battlefile.readlines() )
    battlefile.close()
    # json
    for filename in os.listdir( xlspath + '/json' ):
        filenames.append( 'json/' + filename )
    filenames.sort()
    m = hashlib.md5()
    # 计算md5
    filenames.sort()
    for filename in filenames:
        f = open( xlspath + '/' + filename.rstrip(), 'rb' )
        m.update( f.read() )
        f.close()
    return m.hexdigest()

def main() :
    version = ""
    language = {}
    if len(sys.argv) != 7 and len(sys.argv) != 8 :
        print( "%s <xlspath> <xmlpath> <language> <svnversion> <gitrevision> <excelversion> [excelpatchversion] ." % sys.argv[0] )
        exit( 0 )
    xlspath = sys.argv[1]
    xmlpath = sys.argv[2]
    explang = sys.argv[3]
    svnrevision = sys.argv[4]
    gitrevision = sys.argv[5]
    version = sys.argv[6]
    patchversion = version
    if len(sys.argv) == 8 : patchversion = sys.argv[7]
    # 检查目录是否存在
    if not os.path.exists( xlspath ) :
        print( xlspath, " is not exist !" )
        exit(0)
    if not os.path.exists( xmlpath ) :
        print(xmlpath, " is not exist !")
        exit(0)
    # 解析语言包
    language = import_language( xlspath, explang )
    # 生成版本文件
    if svnrevision != "-1" :
        battleversion = generate_battle_version( xlspath )
        create_version_file( xmlpath, svnrevision, battleversion, gitrevision, version, patchversion )
    # 分支补丁文件
    patchfile = search_patchfiles( xlspath, explang )
    if patchfile != "" :
        print('Found PatchFile(%s) "%s" .' % ( explang, patchfile) )
        patchfile = '%s/%s' % (xlspath, patchfile)
    # 遍历xlspath
    for filename in os.listdir(xlspath) :
        if filename.find( '~$' ) != -1 : continue
        if not os.path.isfile( xlspath + '/' + filename ) : continue
        name, suffix = os.path.splitext( filename )
        if suffix != '.xls' and suffix != '.xlsx' and suffix != '.xlsm' :
            continue
        pos = name.find('_')
        if pos < 0 :
           engname = name
        else :
            engname = name[pos+1:]
        if engname.find( lang_keyword ) != -1 or engname.find( patch_keyword ) != -1 :
            continue
        xlsfile = xlspath + '/' + filename
        xmlfile = xmlpath + '/' + engname + '.xml'
        print('Convert ' + xlsfile + ' To ' + xmlfile + ' ...')
        opexport = xls2xml.XlsExport2Xml( xlsfile, xmlfile )
        opexport.export( language, patchfile, version, patchversion )
    print('Export Language:"%s", SVNRevision:%s, GitRevision:%s, Version:%s, Patch:%s, Output:"%s/" Succeed .' % (explang, svnrevision, gitrevision, version, patchversion, os.path.realpath(xmlpath)) )

if __name__ == "__main__" :
    main()
