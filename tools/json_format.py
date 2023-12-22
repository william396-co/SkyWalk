#!/usr/bin/env python
# -*- coding: utf-8 -*-


import sys
import json
import pyperclip

#fp = open( "json.txt", "r+")
#data = fp.read()
data = pyperclip.paste()
data = data.replace( '\n', '' )
json_dict = json.loads( data )
format_data = json.dumps(json_dict, indent=4)

pyperclip.copy(format_data)

#fp.truncate( 0 )
#fp.seek( 0, 0 )
#fp.write( format_data )
#fp.close()
