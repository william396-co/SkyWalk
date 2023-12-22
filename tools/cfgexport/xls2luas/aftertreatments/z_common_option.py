
def try_fromat_int( data ) :
    try:
        if type(data) == type(list()):
            for i in range(len(data)):
                data[i] = try_fromat_int(data[i])
            return data

        array_value = eval(data)
        return array_value
    except:
        a_int_value = 0
        try:
            a_int_value = int(float(data))
        except:
            return 0
        return a_int_value
        
    return 0

def try_format_array( data ) :
    try:
        if type(data) == type(list()):
            return data
        else:
            array_value = eval(data)
            return array_value
    except:
        return list()

    return list()
    
def restructureData(data_des, attrs):
    data_des.types = {}
    valueTypeIndex = -1
    valueIndex = -1
    for i in range(len(attrs) - 1, -1, -1):
        if "valueType" == attrs[i].name:
            valueTypeIndex = i
        elif "value" == attrs[i].name:
            valueIndex = i
    
    if valueTypeIndex != -1 and valueIndex != -1:
        data_des.types[valueIndex] = data_des.content[valueTypeIndex]
    
def restructureAttr(config_desc, DescriptorAttr):
    pass