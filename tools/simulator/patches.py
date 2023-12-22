import os
import sys
import codecs
from configparser import ConfigParser

def patch_prototype():
    config = ConfigParser(inline_comment_prefixes=';')
    config.read_file(codecs.open('config.ini', 'r'))
    prototype_file = config.get('Global', 'protocol')
    os.system("find %s/ -name '*.proto' | xargs protoc -I=%s/ --python_out=message" % (prototype_file, prototype_file) )
    sys.modules['prototype'] = __import__('message')
