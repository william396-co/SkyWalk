#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os
import sys
import itertools
import xlrd
import xlwt
from xlutils.copy import copy


def get_value(worksheet, row, col) :
    cell = worksheet.cell(row, col)
    value = worksheet.cell_value(row, col)
    return value

class LangFile:
    
    def __init__(self, sheet, oriSheet, nrows, ncols):
        self.sheet = sheet
        self.oriSheet = oriSheet
        self.nrows = nrows
        self.ncols = ncols
        
    def title(self) :
        self.sheet.write(0, 0, "%")
        self.sheet.write(0, 1, "int")
        self.sheet.write(0, 2, "String")
        
        self.sheet.write(1, 0, "!")
        self.sheet.write(1, 1, "id")
        self.sheet.write(1, 2, "value")
        
        self.sheet.write(2, 0, "#")
        self.sheet.write(2, 1, "id")
        self.sheet.write(2, 2, "value")
        
    def replace(self, value, row) :
        print("replace", self.sheet.name, row, value)
        self.sheet.write(row, 0, 1)
        self.sheet.write(row, 2, value)
        self.sheet.write(row, 3, "")
        
    def delete(self, row) :
        print("delete", self.sheet.name, row)
        self.sheet.write(row, 0, 9999999)
        self.sheet.write(row, 3, "语言表中已删除")
        
    def add(self, key, value) :
        print("add", self.sheet.name, key, value)
        self.sheet.write(self.nrows, 0, 1)
        self.sheet.write(self.nrows, 1, key)
        self.sheet.write(self.nrows, 2, value)
        self.nrows = self.nrows + 1
    
    def getId(self, row) :
        if self.oriSheet == None:
            return 0
        return int(get_value(self.oriSheet, row, 1))
    
def main():
    path = sys.argv[1]
    
    if not os.path.exists(path) :
        print(path, " is not exist !")
        exit(0)
    
    oriFile =  path + "/100-语言包_language.xlsm"
    if not os.path.exists(oriFile) :
        print(oriFile, "is not exist")
        exit(0)
        
    oriIds = {}
    oriIdFile = path + "/102-语言_language_programmer.xls"
    
    startId = 100000
    oriWork = None
    if os.path.exists(oriIdFile) :
        oriWork = xlrd.open_workbook(oriIdFile)
        tblId = oriWork.sheet_by_name('language_id')
        if tblId != None :
            nrows = tblId.nrows
            ncols = tblId.ncols
            if nrows >= 3 and ncols >= 3:
                for i in range(3, nrows):
                    nId = int(get_value(tblId, i, 1))
                    value = get_value(tblId, i, 2)
                    oriIds[value] = nId
                    startId = max(startId, nId)
    
    langWork = xlrd.open_workbook(oriFile)
    sheetLang = langWork.sheet_by_name('language')
    if sheetLang == None:
        print(oriFile, " language is not exist")
        exit(0)
    
    nrows = sheetLang.nrows
    ncols = sheetLang.ncols
    if nrows < 3 or ncols < 3:
        print("language not have data")
        exit(0)
    nIdToIndex = {}
    for i in range(3, nrows):
        strId = get_value(sheetLang, i, 1)
        if strId in oriIds:
            nIdToIndex[oriIds[strId]] = i
        else:
            oriIds[strId] = startId
            nIdToIndex[startId] = i
            startId = startId + 1
    
    areas = {}
    areas["id"] = 1
    for i in range(2, ncols) :
        area = get_value(sheetLang, 1, i)
        if area != None and "_" in area:
            startIndex = area.find("_")
            area = area[startIndex + 1:len(area)]
            areas[area] = i
    
    langFiles = {}
    newWork = None

    if not os.path.exists(oriIdFile) :
        isNew = True
        newWork = xlwt.Workbook(encoding='utf-8')
        for area in areas :
            lang = LangFile(newWork.add_sheet("language_" + area), None, 3, 2)
            lang.title()
            langFiles[area] = lang
    else:
        book1 = xlrd.open_workbook(oriIdFile)
        newWork = copy(book1)
        tempSheets = {}
        worksheets = book1.sheets()
        for oriSheet in worksheets :
            sheet = newWork.get_sheet(oriSheet.name)
            area = sheet.name.replace("language_", "")
            if area in areas:
                tempSheets[area] = sheet
        for area in areas :
            lang = None
            if area not in tempSheets:
                sheet = newWork.add_sheet("language_" + area)
                lang = LangFile(sheet, None, 3, 2)
                lang.title()
            else:
                sheet = tempSheets[area]
                oriSheet = book1.sheet_by_name("language_" + area)
                lang = LangFile(sheet, oriSheet, oriSheet.nrows, oriSheet.ncols)
            
            langFiles[area] = lang
    
    writeDatas = {}
    for area in langFiles :
        lang = langFiles[area]
        nrows = lang.nrows
        ncols = lang.ncols
        if nrows > 2 :
            for i in range(3, nrows) :
                nId = lang.getId(i)
                if nId in nIdToIndex :
                    value = get_value(sheetLang, nIdToIndex[nId], areas[area])
                    lang.replace(value, i)
                else:
                    lang.delete(i)
                if nId not in writeDatas :
                    writeDatas[nId] = {}
                writeDatas[nId][area] = True
                
    for i in range(100000, int(startId)) :
        for area in areas :
            if i not in writeDatas or area not in writeDatas[i] :
                value = i
                if i in nIdToIndex :
                    index = nIdToIndex[i]
                    value = get_value(sheetLang, index, areas[area])
                langFiles[area].add(i, value)
                
    newWork.save(oriIdFile)

if __name__ == "__main__" :
    main()