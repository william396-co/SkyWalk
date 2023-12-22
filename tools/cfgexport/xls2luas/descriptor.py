#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os
import sys
import xlrd
import aftertreatments

CONST_SHEET_ATTRIBUTE_TYPE_ROW = 0
CONST_SHEET_ATTRIBUTE_NAME_ROW = 1
CONST_SHEET_ATTRIBUTE_DEFAULT_ROW = 3
CONST_INFO_ROWS = 3
CONST_INFO_COLS = 1

CONST_VERSION_COLS = 0

def get_version_num(verStr):
    sver = verStr.split('.')
    if len(sver) > 4:
        print('error in version str')
        return 0
    i = 0
    version_num = 0
    t = [100000000, 1000000, 10000, 1];
    for v in sver:
        version_num += int(v) * t[i]
        i += 1

    return version_num

def cmp_version(src_ver, dst_ver):
    if get_version_num(src_ver) > get_version_num(dst_ver):
        return True
    return False


class DescriptorFile:
    def __init__(self,name):
        self.name = name
        self.basename = os.path.basename(name)
        self.configs = []

class DescriptorData:
    def __init__(self, version, key, content):
        self.version = version;
        self.key = key
        self.types = None
        self.content = content

class DescriptorAttr:
    def __init__(self, _name, _type, start_col):
        self.name = _name
        self.type = _type
        self.default = None
        self.defaultKey = None
        self.account = {}
        self.cols = [start_col]
        self.item_cols = 1

class DescriptorConfig:
    def __init__(self, name, nrows, ncols):
        self.name = name.lower()
        self.nrows = nrows
        self.ncols = ncols
        self.attrs = []
        self.attr_datas = {}
        self.key_type = "string"
        self.pageLineNum = 2000

class CodeGenerateRequest():
    patch_file = None
    fileName = None
    files = None
    single = False
    version = None
    sheets = None
    _workbook = None
    langIds = None
    searching_end_attr_desc = None
    def __init__(self, files, version, langFilePath):
        self.readLanguage(langFilePath)
        self.version = version
        self.files = []
        for f in files:
            self.add_file(f)

    def setSingleGen(self):
        self.single = True
        
    def readLanguage(self, langFilePath):
        self.langIds = {}
        # workbook = xlrd.open_workbook(langFilePath)
        # worksheet = workbook.sheet_by_name('language_id')
        # for i in range(3, worksheet.nrows) :
            # id = int(worksheet.cell_value(i, 1))
            # value = worksheet.cell_value(i, 2)
            # self.langIds[value] = id
        
    def add_file(self, f):
        print('--------parse xml file ' + f)
        
        file_name,file_suffix = os.path.splitext(os.path.basename(f))
        startIndex = file_name.find("_")
        if startIndex != -1:
            file_name = file_name[startIndex+1:]
        else:
            file_name = file_name
            
        self.fileName = file_name.replace(".", "_").lower()
        
        file_desc = DescriptorFile(f)

        _workbook = xlrd.open_workbook(f)
        self._workbook = _workbook

        worksheets = _workbook.sheets()
        for worksheet in worksheets :
            self.add_config(worksheet,file_desc)
            if self.searching_end_attr_desc != None:
                raise("array[] 不匹配")
        self.files.append(file_desc)

    def is_file_can_add(self, cfg_desc):
        if cfg_desc.nrows <= CONST_INFO_ROWS and cfg_desc.ncols <= 1:
            return False
        if len(cfg_desc.attrs) == 0:
            return False
        if not cfg_desc.attrs[0].type == '$':
            return False
        if cfg_desc.name == "con_100_language":
            return False
        return True

    def add_config(self, worksheet, file_desc):
        fileName = self.fileName
        sheetName = worksheet.name.lower()
        if self.fileName != sheetName:
            fileName = fileName + "_" + sheetName
        config_desc = DescriptorConfig(fileName, worksheet.nrows, worksheet.ncols)
        self.searching_end_attr_desc = None;

        if worksheet.nrows <= CONST_INFO_ROWS and worksheet.ncols <= 1:
            return
            
        if not worksheet.cell_value(0,0) == '$':
            return
            
        try:
            config_desc.pageLineNum = int(worksheet.cell_value(2,0))
        except:
            config_desc.pageLineNum = 2000

        aftertreatment = None

        afterFileName = worksheet.name
        
        if "option" in afterFileName.lower():
            afterFileName = "z_common_option"

        if afterFileName in aftertreatments.__all__:
            aftertreatment = getattr(aftertreatments, afterFileName)
            print(aftertreatment)
        print('------------parse xml sheet ' + worksheet.name, worksheet.nrows, worksheet.ncols)

        config_desc.key_type = worksheet.cell_value(CONST_SHEET_ATTRIBUTE_TYPE_ROW, CONST_INFO_COLS)
        
        for i in range(0,config_desc.ncols):
            attr_name = worksheet.cell_value(CONST_SHEET_ATTRIBUTE_NAME_ROW,i)
            attr_type_str = worksheet.cell_value(CONST_SHEET_ATTRIBUTE_TYPE_ROW,i)
            self.add_attr(attr_name, attr_type_str, i, config_desc)

        if self.is_file_can_add(config_desc):
                lastKey = None
                lastRow = None
                lastVersion = None
                                
                for i in range(CONST_INFO_ROWS, config_desc.nrows):
                    key = self.get_key(worksheet, i) 
                    
                    if key == None or key == '':
                        continue
                    
                    version = self.get_version(worksheet, i)
                    if version.strip() == '#':
                        continue
                        
                    if version.strip() == '' or cmp_version(version, self.version):
                        lastVersion = version
                        continue
                    
                    if lastKey == None:
                        lastKey = key
                        lastRow = i
                        lastVersion = version
                        continue
                        
                    if key == lastKey:
                        if lastVersion == None or lastVersion.strip() == '' or cmp_version(version, lastVersion):
                            lastRow = i
                        continue
                    
                    lastVersion = version
                        
                    config_desc.attr_datas[lastKey] = self.add_data(worksheet, lastRow, config_desc, aftertreatment)
                        
                    lastKey = key
                    lastRow = i
                
                if lastKey != None:
                    config_desc.attr_datas[lastKey] = self.add_data(worksheet, lastRow, config_desc, aftertreatment)
                
                if aftertreatment != None:
                    aftertreatment.restructureAttr(config_desc, DescriptorAttr)
                file_desc.configs.append(config_desc)

    def add_attr(self, _name, _type, col, config_desc):
        if not (self.searching_end_attr_desc == None):
            self.searching_end_attr_desc.cols.append(col)
            if _name == ']':
                self.searching_end_attr_desc = None
            return

        attr_desc = None
        if _name[-2:] == ':[':
            if _name[-4:-3] == ':':
                attr_desc = DescriptorAttr(_name[:-4], _type, col)
                attr_desc.item_cols = _name[-3:-2]
            else:
                attr_desc = DescriptorAttr(_name[:-2], _type, col)
            self.searching_end_attr_desc = attr_desc

        if attr_desc == None:
            attr_desc = DescriptorAttr(_name, _type, col)
        config_desc.attrs.append(attr_desc)

    def get_value(self, worksheet, row, col):
        cell = worksheet.cell(row, col)
        value = worksheet.cell_value(row, col)
        if cell.ctype == xlrd.XL_CELL_DATE :
            try:
                datetuple = xlrd.xldate_as_tuple(value, self._workbook.datemode)
                # time only no date component
                if datetuple[0] == 0 and datetuple[1] == 0 and datetuple[2] == 0:
                    value = "%02d:%02d:%02d" % datetuple[3:]
                # date only, no time
                elif datetuple[3] == 0 and datetuple[4] == 0 and datetuple[5] == 0:
                    value = "%04d/%02d/%02d" % datetuple[:3]
                else: # full date
                    value = "%04d/%02d/%02d %02d:%02d:%02d" % datetuple
            except Exception:
                print('exception: date-type error  row:' + str(row) + ' col:' + str(col) + ' value:' + str(value))
        return value
    
    def get_version(self, worksheet, row):
        version = str(worksheet.cell_value(row, CONST_VERSION_COLS))
        return version
    
    def get_key(self, worksheet, row):
        key = self.get_value(worksheet, row, CONST_INFO_COLS)
        try:
            key = int(key)
        except:
            key = key
        return key

    def add_data(self, worksheet, row, cfg_desc, aftertreatment):
        version = self.get_version(worksheet, row)
        if version.strip() == '':
            return None

        key = self.get_key(worksheet, row)
                
        contents = []
        for attr_desc in cfg_desc.attrs:
            content = None
            if len(attr_desc.cols) == 1:
                content = self.get_value(worksheet, row, attr_desc.cols[0])
                if attr_desc.type.lower() == "lang" :
                    if content in self.langIds:
                        content = self.langIds[content]
                    else:
                        content = 0
            else:
                content = []
                step = int(attr_desc.item_cols)
                if step > 1:
                    dis = len(attr_desc.cols);
                    col_s = attr_desc.cols[0]
                    if (divmod(dis,step)[1] == 0):
                        for i in range(0, int(dis/step)):
                            item_content = list();
                            for j in range(0, step):
                                tmp = self.get_value(worksheet, row, col_s + i * step + j)
                                if attr_desc.type.lower() == "lang" :
                                    if tmp in self.langIds:
                                        tmp = self.langIds[tmp]
                                    else:
                                        tmp = 0
                                item_content.append(tmp);
                            content.append(item_content)
                else:
                    for col in attr_desc.cols:
                        tmp = self.get_value(worksheet, row, col)
                        if attr_desc.type.lower() == "lang" :
                            if tmp in self.langIds:
                                tmp = self.langIds[tmp]
                            else:
                                tmp = 0
                        content.append(tmp)
            accountKey = str(content)
            if accountKey not in attr_desc.account:
                attr_desc.account[accountKey] = 1
            else:
                attr_desc.account[accountKey] = attr_desc.account[accountKey] + 1
                
            if attr_desc.defaultKey == None or attr_desc.account[accountKey] > attr_desc.account[attr_desc.defaultKey]:
                attr_desc.defaultKey = accountKey
                attr_desc.default = content
            
            contents.append(content)
        data = DescriptorData(version, key, contents)
        if aftertreatment != None:
            aftertreatment.restructureData(data, cfg_desc.attrs)
        return data

class ReturnFile():
    name = None
    content = None

class CodeGenerateResponse():
    topath = None
    mypath = None
    mysuffix = None
    files = None
    def __init__(self, topath):
        self.topath = topath
        self.files = []

    def addFile(self):
        newfile = ReturnFile();
        self.files.append(newfile)
        return newfile

    def saveToFile(self):
        gen_path = self.topath + self.mypath
        if not os.path.exists( gen_path ) :
            os.makedirs(gen_path)

        '''
        #clean up dir
        try:
            for filename in os.listdir(gen_path) :
                if os.path.isfile(gen_path+'/'+filename) :
                    ss = filename.split( '.' )
                    suffix = ss[len(ss)-1]
                    if suffix == self.mysuffix :
                        os.remove(gen_path+"/"+filename)
        except:
            print(u'清理文件过程，可能出现问题，跳过。')
            pass
        '''

        for _f in self.files:
            name = _f.name.lower()
            if os.path.exists(gen_path+'/'+name):
                os.remove(gen_path+"/"+name)
            f = open(gen_path+'/'+name,"wb")
            f.write(_f.content);
            f.close()


