#!/usr/bin/env python
# encoding: utf-8

import sys
import os
from io import BytesIO
import json

_files = {}

def type_name():
    return 'json'

def my_path():
    return '/json/GameConfigs'

def my_suffix():
    return 'json'

def active():
    return False

def is_skip( scheme ) :
    if cmp( scheme, '' ) == 0 :
        return True
    elif cmp( scheme, '$' ) == 0 :
        return True
    elif cmp( scheme.upper(), 'SKIP' ) == 0 :
        return True
    return False

def try_fromat_int( data ) :
    try:
        if type(data) == type(list()):
            for i in range(len(data)):
                if type(data[i]) == type(list()):
                    data[i] = try_fromat_int(data[i])
                else:
                    data[i] = int(data[i])
            return data

        array_value = eval(data);
        return array_value;
    except:
        a_int_value = 0
        try:
            a_int_value = int(float(data))
        except:
            return 0
        return a_int_value
        
    return 0

def try_format_string( data ) :
    try:
        if not data.find('[') == -1:
            array_value = eval(data);
            return array_value;
        else:
            try:
                a_str_value = str(data)
                return a_str_value
            except:
                pass
    except:
        a_str_value = ''
        try:
            a_str_value = str(data)
            return a_str_value
        except:
            pass
    return data

def try_format_array( data ) :
    try:
        array_value = eval(data)
        for i in range(len(array_value)):
            if type(array_value[i]) == type('s'):
                array_value[i] = array_value[i].decode('utf8')
        return array_value;
    except:
        return list();

    return list();

def try_format_value( typeStr, nameStr, data ) :
    if typeStr.lower() == "int":
        return try_fromat_int(data)
    elif typeStr.lower() == "string":
        return try_format_string(data)
    elif typeStr.lower() == "lang":
        return try_format_string(data)
    elif typeStr.lower() == "array":
        return try_format_array(data)

def code_gen_datas(data_desc, config_desc, context):
    key = data_desc.key
    content = data_desc.content
    context_data = {}
    for j in range(0, len(config_desc.attrs)):
        attrs = config_desc.attrs[j]
        attr_type = attrs.type
        attr_name = attrs.name
        if is_skip(attr_type):
            continue
        context_data[attr_name] = try_format_value(attr_type, attr_name, content[j])
    context[key] = context_data

def code_gen_config(config_desc):
    context = {}
    for (k,v) in config_desc.attr_datas.items():
        code_gen_datas(v, config_desc, context)
    return context


def code_gen_file(file_desc):
    for config in file_desc.configs:
        context_value = json.dumps(code_gen_config(config), ensure_ascii = False, sort_keys=True, indent=4)
        _files[config.name + '.' + my_suffix()] = context_value.encode('utf-8')

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
        file.content = v

    print('.........end gen type : '+type_name()+'............')