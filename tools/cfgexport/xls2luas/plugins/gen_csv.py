#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys
import os
from io import BytesIO

INDENT = '    '
INDENT_LEN = len(INDENT)

_files = {}

def type_name():
    return 'csv'

def my_path():
    return '/csv/GameConfigs'

def my_suffix():
    return 'csv'

def active():
    return False

class Writer(object):
    def __init__(self, prefix=None):
        self.io = BytesIO()
        self.__indent = ''
        self.__prefix = prefix

    def getvalue(self):
        return self.io.getvalue()

    def getindent(self):
        return self.__indent

    def __enter__(self):
        self.__indent += INDENT
        return self

    def __exit__(self, type, value, trackback):
        self.__indent = self.__indent[:-INDENT_LEN]

    def __call__(self, data):
        self.io.write(self.__indent)
        if self.__prefix:
            self.io.write(self.__prefix)
        self.io.write(data)

def is_skip( scheme ) :
    if cmp( scheme, '' ) == 0 :
        return True
    elif cmp( scheme, '$' ) == 0 :
        return True
    elif cmp( scheme.upper(), 'SKIP' ) == 0 :
        return True
    return False

def write_array_csv(context,data):
    for d in data:
        try:
            context('%s,'%(str(d)))
        except:
            context(('%s,'%(d)).encode('utf-8'))
    context('\n')


def code_gen_datas(data_desc, config_desc, context):
    key = data_desc.key
    content = data_desc.content
    context_data = []
    for j in range(0, len(config_desc.attrs)):
        attrs = config_desc.attrs[j]
        attr_type = attrs.type
        attr_name = attrs.name
        if is_skip(attr_type):
            continue
        context_data.append(content[j])
    write_array_csv(context, context_data)

def code_gen_config(config_desc):
    context = Writer()
    for (k,v) in config_desc.attr_datas.items():
        code_gen_datas(v, config_desc, context)
    return context.getvalue()


def code_gen_file(file_desc):
    for config in file_desc.configs:
        context_value = code_gen_config(config)
        print(context_value)
        _files[config.name + '.' + my_suffix()] = context_value.decode('utf-8')

#-----------------------------------------------------------------------------------------
def gen_code(request, response, toPath):
    print('.........start gen type : '+type_name()+'..........')
    response.mysuffix = my_suffix()
    response.mypath = my_path()

    for file_desc in request.files:
       code_gen_file(file_desc)

    for k,v in _files.items():
        print('gen code [%s] file = %s'%(type_name(), k))
        file = response.addFile()
        file.name = k
        file.content = v.encode('utf-8-sig')

    print('.........end gen type : '+type_name()+'............')