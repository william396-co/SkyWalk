import os
import sys
import time
import platform
from stat import *
import pickle
import operator


class summary_info:
    def __init__(self):
        self.path = ""
        self.time = 0
    def __repr__(self):
        return "[time = %d,path = %s]\n"%(self.time,self.path)

def write_file_summary(summary,summary_path):
    f = open(summary_path, 'wb') 
    pickle.dump(summary,f)
    f.close()

def read_file_summary(summary_path):
    file_summary = {}
    print(summary_path)
    if not os.path.exists(summary_path):
        print("File[" + summary_path + "] not exits.")
    else:
        f = open(summary_path, 'rb') 
        file_summary = pickle.load(f) 
        f.close()
    return file_summary

def generate_file_sumary(excel_folder):
    file_sumary = {}
    onlyfiles = [ f for f in os.listdir(excel_folder) if os.path.isfile(os.path.join(excel_folder,f)) ]
    for f in onlyfiles:
        file_path = os.path.join(excel_folder, f) 
        file_name = os.path.basename(f)
        file_name,file_suffix = os.path.splitext(file_name)
        if operator.eq(file_name[:2],"~$"):
            print("Excel temp file:" + file_name)
        else:
            if file_suffix == ".xlsx" or file_suffix == ".xlsl" or file_suffix == ".xls" or file_suffix == ".xlsm":
                info = summary_info()
                info.time = os.stat(file_path)[ST_MTIME]
                info.path = file_path
                file_sumary[file_name] = info
    return file_sumary

# return the excel files modified
def compare_file_summary(cur_summary,last_summary):
    summary_diff = { }
    updated = {}
    unused = {}
    
    for k,v in cur_summary.items():
        if k not in last_summary or last_summary[k].time != v.time:
            updated[k] = v
    summary_diff["updated"] = updated
    
    
    for k,v in last_summary.items():
        if k in cur_summary:
            unused[k] = v
    summary_diff["unused"] = unused
    
    return summary_diff