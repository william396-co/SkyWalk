#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os
import sys
import shutil
from summary_tools import *
from descriptor import *
import plugins
import zipfile
import hashlib

ignoreList = []

def isInIgnoreList(fname):
    for i in ignoreList:
        iff = i
        if platform.system() == 'Windows':
            iff = i.decode('utf8').encode('gbk')
        if iff == fname :
            return True;
    return False

def generate_battle_version( xlspath ):
    filenames = []
    # battle filelist
    battlefile = open( xlspath + "/battle_filelist.txt", "r")
    filenames.extend(battlefile.readlines())
    battlefile.close()
    # json
    for filename in os.listdir( xlspath + '/json' ):
        filenames.append("json/"  + filename)
    filenames.sort()
    m = hashlib.md5()
    # 计算md5
    for filename in filenames:
        f = open( xlspath + '/' + filename.rstrip(), 'rb')
        m.update(f.read())
        f.close()
    return m.hexdigest()

def main() :
    print(sys.argv)
    xlspath = sys.argv[1]
    topath = sys.argv[2]
    full = sys.argv[3]
    tag = sys.argv[4]
    ver = sys.argv[5]
    verLua = sys.argv[6]
    print(len(sys.argv) <= 6, len(sys.argv))
    gen_name = "gen_lua_cut_ct" if len(sys.argv) <= 7 else sys.argv[7]
    
    # 检查目录是否存在
    if not os.path.exists( xlspath ) :
        print(xlspath, " is not exist !")
        exit(0)
    file_sumary = generate_file_sumary(xlspath)
    summary_path = "./a_localdata/" + tag + "_summary.txt"
    last_file_summary = {}
    if full == "FALSE":
        last_file_summary = read_file_summary(summary_path)
    summary_diff = compare_file_summary(file_sumary,last_file_summary)

    languageFilePath = xlspath + "/102-语言_language_programmer.xls"
    # if not os.path.exists( languageFilePath ) :
        # print(languageFilePath, " is not exist !")
        # exit(0)

    # 遍历xlspath
    files = []
    ignore_file_path = sys.path[0]+'/ignore'
    if not os.path.exists(ignore_file_path):
        print("File[ ignore ] not exits.")
    else:
        f = file(ignore_file_path, 'r')
        for line in f.readlines(): #依次读取每行  
            line = line.strip()   
            ignoreList.append(line)

    for k,v in summary_diff["updated"].items():
        if isInIgnoreList(k):
            continue
        files.append(v.path)

    print("\n\nstart gen .......................\n")
    code_gen_req = CodeGenerateRequest(files, ver, languageFilePath)
    
    for name in plugins.__all__:
        if name == gen_name:
            code_gen_response = CodeGenerateResponse(topath)
            code_gen_response_2 = CodeGenerateResponse(topath)
            plugin = getattr(plugins, name)
            try:
                gen_code = plugin.gen_code
                active = plugin.active
                type_name = plugin.type_name
                gen_configs = plugin.gen_configs
            except AttributeError:
                pass
            else:
                try:
                    gen_code(code_gen_req, code_gen_response, topath)
                    code_gen_response.saveToFile()
                    gen_configs(code_gen_req, code_gen_response_2, topath, generate_battle_version(xlspath))
                    code_gen_response_2.saveToFile()
                except:
                    print("Error Error Error Error Error Error Error Error Error ")
                    raise
    
    if full == "FALSE":
        write_file_summary(file_sumary,summary_path)
    print("\n\n\n\nDone..........................")

if __name__ == "__main__" :
    main()
