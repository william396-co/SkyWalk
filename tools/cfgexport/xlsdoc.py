#!/usr/bin/env python
# -*- coding: utf-8 -*-

import xlrd

# 字段表
FIELD_TBALE = {
        'int' :     { 'type':'int32_t',       'isbuiltin':True,  'value':'0',     'desc':'整型数值,取值范围:-2147483648~2147483647' },
        'long' :    { 'type':'int64_t',       'isbuiltin':True,  'value':'0',     'desc':'整型数值,取值范围:-9223372036854775808~9223372036854775808' },
        'float' :   { 'type':'double',        'isbuiltin':True,  'value':'0.0f',  'desc':'浮点数值,慎用' },
        'string' :  { 'type':'std::string',   'isbuiltin':True,  'value':'',      'desc':'字符串' },
        'lang' :    { 'type':'std::string',   'isbuiltin':True,  'value':'',      'desc':'语言包' },
        'array' :   { 'type':'IntVec',        'isbuiltin':False, 'value':'',      'desc':'数值数组' },
        'fee' :     { 'type':'Fee',           'isbuiltin':False, 'value':'',      'desc':'费用' },
        'range' :   { 'type':'Range',         'isbuiltin':False, 'value':'',      'desc':'范围段' },
        'reward' :  { 'type':'Resource',      'isbuiltin':False, 'value':'',      'desc':'奖励两元组' },
        'vector' :  { 'type':'Vec2i',         'isbuiltin':False, 'value':'',      'desc':'位置坐标' },
        'attr':     { 'type':'AttribValue',   'isbuiltin':False, 'value':'',      'desc':'属性值' },
        'effect':   { 'type':'Effect',        'isbuiltin':False, 'value':'',      'desc':'技能效果' },
        'robot':    { 'type':'TrainingHero',  'isbuiltin':False, 'value':'',      'desc':'英雄培养信息' },
}

class XlsDocument() :
    def __init__( self, xlsfile, typeidx = 0, schemeidx = 1, commentidx = 2, dataidx = 3 ) :
        self._xlsfile = xlsfile
        self._typeidx = typeidx
        self._schemeidx = schemeidx
        self._commentidx = commentidx
        self._dataidx = dataidx
        self._workbook = xlrd.open_workbook( xlsfile )

    def get_sheets( self ) :
        return self._workbook.sheets()

    def get_sheet( self, name ) :
        return self._workbook.sheet_by_name( name )

    def get_colidx( self, sheet, field ) :
        for col in range( sheet.ncols ) :
            value = sheet.cell(self._schemeidx,col).value
            if value.lower() == field :
                return col
        return -1

    def get_scheme_keys( self, scheme ) :
        return [ scheme[0][0], scheme[1][0] ]

    def get_scheme( self, sheet ) :
        # 取出scheme
        scheme = []
        field = ''
        number = 0
        fromcol = 0
        for col in range( sheet.ncols ) :
            value = sheet.cell(self._schemeidx,col).value
            if value == "" :
                continue
            elif value == "]" :
                typev = str(self.format_value(sheet.cell(self._typeidx, fromcol))).strip()
                comment = str(self.format_value(sheet.cell(self._commentidx, fromcol)))
                scheme.append( (field,number,col-fromcol+1,typev, fromcol, comment) )
            else :
                field = ''
                number = 0
                fromcol = col
                fields = str( value )
                fields = fields.split( ':' )
                field = fields[0]
                if len( fields ) == 2 :
                    number = 1
                elif len( fields ) == 3 :
                    number = int( fields[1] )
                if number == 0 :
                    typev = str(self.format_value(sheet.cell(self._typeidx, col))).strip()
                    comment = str(self.format_value(sheet.cell(self._commentidx, col)))
                    scheme.append( (field,0,0,typev,col, comment ) )
        return scheme

    def isbuiltin( self, ftype ) :
        vt = ftype.lower();
        if vt in FIELD_TBALE.keys() :
            result = FIELD_TBALE[ vt ]
            return result['isbuiltin']
        return False

    def convert( self, ftype ) :
        vt = ftype.lower();
        if vt == '!' or vt == '$' or vt == 'skip' :
            return '', ''
        if vt in FIELD_TBALE.keys() :
            result = FIELD_TBALE[ vt ]
            return result[ 'type' ], result[ 'value' ]
        return ftype, ''

    # 是否需要跳过
    def is_skip( self, scheme ) :
        if scheme.upper() == 'SKIP' :
            return True
        return False

    # 是否是合法的表名
    def filter( self, sheet ) :
        # 过滤表头
        if sheet.nrows < self._dataidx : return False
        if sheet.nrows < self._schemeidx : return False
        # 单数字表名
        if sheet.name.isnumeric() : return False
        # 第一个单元格必须是$
        if self.format_value( sheet.cell(0, 0) ) != '$' : return False
        return True

    # number类型与date类型的特殊处理
    def format_value( self, cell ) :
        value = cell.value
        if cell.ctype == xlrd.XL_CELL_NUMBER :
            # 如果是整数的话, 直接取整
            if cell.value == int( cell.value ) :
                value = int( cell.value )
        elif cell.ctype == xlrd.XL_CELL_DATE :
            datetuple = xlrd.xldate_as_tuple(value, self._workbook.datemode)
            # time only no date component
            if datetuple[0] == 0 and datetuple[1] == 0 and datetuple[2] == 0:
                value = "%02d:%02d:%02d" % datetuple[3:]
            # date only, no time
            elif datetuple[3] == 0 and datetuple[4] == 0 and datetuple[5] == 0:
                value = "%04d/%02d/%02d" % datetuple[:3]
            else: # full date
                value = "%04d/%02d/%02d %02d:%02d:%02d" % datetuple
        return value
