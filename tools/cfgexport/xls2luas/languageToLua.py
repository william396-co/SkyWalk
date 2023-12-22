#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os
import sys
import xlrd
import math

def get_value(workbook, worksheet, row, col):
    cell = worksheet.cell(row, col)
    value = worksheet.cell_value(row, col)
    
    try:
        value = float(value)
        tempI = int(value)
        if value == tempI:
            value = tempI 
    except ValueError:
        return value
    
    return value

def writePageFile(workbook, worksheet, path, name, page, minIndex, maxIndex):
    filePath = path + str(page) + "_" + name + ".lua"
    desc = "local items = {\n"
    desc += '\t[0] = "''",\n'
    for i in range(minIndex, maxIndex) :
        id = int(get_value(workbook, worksheet, i, 1))
        value = str(get_value(workbook, worksheet, i, 2)).replace("\r", "\\n").replace("\n", "\\n").replace("\"", "\\\"")
        desc += '\t[' + str(id) + '] = "' + value + '",\n'
    desc += "}\n\n\n\n\n"
    desc += "return items"
    
    f = open(filePath, 'wb')
    f.write(str.encode(desc))
    f.close()

def main():
    path = sys.argv[1]
    outPath = sys.argv[2]
    
    if not os.path.exists(path) :
        print(path, " is not exist !")
        exit(0)
    
    file = path + "/102-语言_language_programmer.xls"

    if not os.path.exists(file) :
        print(file, " is not exist !")
        exit(0)

    if not os.path.exists(outPath + "/LanguagePackage") :
        os.makedirs(outPath + "/LanguagePackage")

    workbook = xlrd.open_workbook(file)
    for worksheet in workbook.sheets() :
        area = worksheet.name.replace("language_", "")
        filePath = outPath + "/LanguagePackage/" + area + "/Config/"
        fileName = "lan"
        modlePath = area + ".Config."
        if area == "id" :
            filePath = outPath + "/LanguagePackage/"
            fileName = "language_id"
            modlePath = ""
        elif not os.path.exists(outPath + "/LanguagePackage/" + area + "/Config") :
            os.makedirs(outPath + "/LanguagePackage/" + area + "/Config")
        
        desc = ""
        pageLineNum = 2000
        try:
            pageLineNum = int(worksheet.cell_value(2,0))
        except:
            pageLineNum = 2000
        lines = worksheet.nrows
        maxPage = math.ceil(lines / pageLineNum)
        
        if maxPage > 1 :
            loadText = "function data:load()\n"
            configNameText = "function data:getConfigName()\n"
            configNameText = configNameText + "\treturn {\n"
            for i in range(1, maxPage + 1):
                minIndex = 3 + (i - 1) * pageLineNum
                maxIndex = min(3 + i * pageLineNum, lines)
                writePageFile(workbook, worksheet, filePath, fileName, i, minIndex, maxIndex)
                configNameText += '\t\t"{1}{0}_{2}",\n'.format(i, modlePath, fileName)
                if i == 1 :
                    loadText += '\tlocal item{0} = require "{1}{0}_{2}"\n'.format(i, modlePath, fileName)
                else:
                    loadText += '\tlocal page{0} = require "{1}{0}_{2}"\n'.format(i, modlePath, fileName)
                    loadText += "\tlocal item{0} = setmetatable( page{0}, {{ __index = item{1}}})\n".format(i, i - 1)
           
            configNameText += "\t}\n"
            configNameText += "end\n\n"
            loadText += "\tdata.Items = item%s\n"%(maxPage)
            loadText += "\tdata.init = true\n"
            loadText += "end\n\n"
           
            desc += "local data = {init = false}\n\n"
            desc += configNameText
            desc += loadText
            desc += "function data:get(id)\n"
            desc += "\tif not self.init then data:load() end\n\n"
            desc += "\treturn self.Items[id]\n"
            desc += "end\n\n"
            desc += "return data"
            
            f = open(filePath + fileName + ".lua", 'wb')
            f.write(str.encode(desc))
            f.close()
        else:
            desc = "local items = {\n"
            desc += '\t[0] = "''",\n'
            
            for i in range(3, worksheet.nrows) :
                id = int(get_value(workbook, worksheet, i, 1))
                value = str(get_value(workbook, worksheet, i, 2)).replace("\r", "\\n").replace("\n", "\\n").replace("\"", "\\\"")
                desc += '\t[' + str(id) + '] = "' + value + '",\n'
            desc += "}\n\n\n\n\n"
            desc += "local data = {Items = items}\n"
            desc += "function data:get(id)\n"
            desc += "\treturn self.Items[id]\n"
            desc += "end\n\n"
            desc += "return data"
            f = open(filePath + fileName + ".lua", 'wb')
            f.write(str.encode(desc))
            f.close()
    
if __name__ == "__main__" :
    main()