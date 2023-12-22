# coding:utf-8
import importlib
import glob
from google.protobuf import message
from google.protobuf import descriptor
from google.protobuf import text_format
from prototype.role_pb2 import *
from prototype.type_pb2 import *


class MessageManager:
    '''
    protobuf field type
    TYPE_DOUBLE = 1
    TYPE_FLOAT = 2
    TYPE_INT64 = 3
    TYPE_UINT64 = 4
    TYPE_INT32 = 5
    TYPE_FIXED64 = 6
    TYPE_FIXED32 = 7
    TYPE_BOOL = 8
    TYPE_STRING = 9
    TYPE_GROUP = 10
    TYPE_MESSAGE = 11
    TYPE_BYTES = 12
    TYPE_UINT32 = 13
    TYPE_ENUM = 14
    TYPE_SFIXED32 = 15
    TYPE_SFIXED64 = 16
    TYPE_SINT32 = 17
    TYPE_SINT64 = 18
    '''

    int_types = [3, 4, 5, 6, 7, 13, 14]
    labels = {1: 'optional', 2: 'required', 3: 'repeated'}


    def __init__(self):
        self._message_ids = {}
        self._message_classes = {}
        self._fuzzy_search = {}
        self._errors = {}
        self._ignore_messages = []
        self._skip_fields = []

    def get_message( self, module, descrip ) :
        cls = getattr( module, descrip.name )
        self._message_classes[ descrip.full_name ] = cls
        for desc in descrip.nested_types :
            self.get_message( cls, desc )

    def scan_messages(self, files):
        for pb_file in glob.glob(files):
            pb_file = pb_file[:-3] # 去除结尾的'.py'
            pb_file = pb_file.replace('/', '.')
            module = importlib.import_module(pb_file)
            module_descriptor = getattr(module, 'DESCRIPTOR', None)
            if module_descriptor is None:
                continue
            for message_name, message_descriptor in module_descriptor.message_types_by_name.items():
                if message_name in self._ignore_messages:
                    continue
                self.get_message( module, message_descriptor )
                self._fuzzy_search.setdefault(message_name, [])
                self._fuzzy_search[message_name].append(message_descriptor.full_name)
                try:
                    message_id = message_descriptor.GetOptions().Extensions[msgid]
                    self._message_ids[message_id] = getattr( module, message_name )
                except:
                    pass

            for error_field in dir(module):
                if error_field.startswith('Errorcode'):
                    self._errors[getattr(module, error_field)] = error_field

    def build_message(self, full_name, prefix=''):
        message_cls = self._message_classes[full_name]
        print("%s%s?(y/N)y" % (prefix, full_name))
        message = message_cls()
        for field_descriptor in message_cls.DESCRIPTOR.fields:
            if field_descriptor.label == descriptor.FieldDescriptor.LABEL_REPEATED:
                field = getattr(message, field_descriptor.name)
                if field_descriptor.message_type is None:
                    while True:
                        value = input('%s%s(%s)(%s) : ' % (prefix, field_descriptor.name, self.labels[field_descriptor.label], field))
                        if value == '':
                            break
                        if field_descriptor.type in self.int_types: # 整形
                            field.append(int(value))
                        else: # 其他
                            field.append(unicode(value, 'utf-8'))
                else:
                    info = "%s%s(%s)?(y/N)" % (prefix, field_descriptor.name, self.labels[field_descriptor.label])
                    while (input(info) == "y"):
                        _message = self.build_message(field_descriptor.message_type.full_name, prefix='\t')
                        _field_message = field.add()
                        _field_message.CopyFrom(_message)
            else:
                value = ''
                if field_descriptor.message_type is None:
                    if field_descriptor.has_default_value:
                        if field_descriptor.name in self._skip_fields:
                            print("%s%s(%s)(%s) : Skip" % (prefix, field_descriptor.name, self.labels[field_descriptor.label], hex(field_descriptor.default_value)))
                            value = field_descriptor.default_value
                        else:
                            value = input("%s%s(%s)(%s) : " % (prefix, field_descriptor.name,
                                self.labels[field_descriptor.label], hex(field_descriptor.default_value)))
                            if not value:
                                value = field_descriptor.default_value
                    else:
                        while True:
                            value = input("%s%s(%s) : " % (prefix, field_descriptor.name, self.labels[field_descriptor.label]))
                            if field_descriptor.label == descriptor.FieldDescriptor.LABEL_OPTIONAL or value:
                                break
                    if value:
                        if field_descriptor.type in self.int_types: # 整形
                            setattr(message, field_descriptor.name, int(value))
                        else: # 其他
                            setattr(message, field_descriptor.name, unicode(value, 'utf-8'))
                else:
                    field = getattr(message, field_descriptor.name)
                    field.CopyFrom(self.build_message(field_descriptor.message_type.full_name, prefix='\t'))

        return message


def main():
    manager = MessageManager()
    manager.scan_messages('message/*.py')
    while True:
        message_name = input('message (full) name : ')
        if message_name == '':
            break
        if message_name in manager._fuzzy_search.keys():
            message_full_names = manager._fuzzy_search[message_name]
            if len(message_full_names) > 1:
                print("%s in %s means:%s" % (message_name, len(message_full_names), message_full_names))
            message_name = message_full_names[0]
        message = manager.build_message(message_name)
        print(text_format.MessageToString(message))

if __name__ == '__main__':
    main()
