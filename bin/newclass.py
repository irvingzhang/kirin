#!/bin/env python

import sys
import os
import re
from conf import *

def getFileName(class_name):
    file_name=''
    last_pos = 0
    class_name_len = len(class_name)
    for i in range(0, class_name_len):
        if i > 0 and class_name[i].isupper():
            file_name += class_name[last_pos:i].lower()
            file_name += '_'
            last_pos=i
        else:
            continue
    file_name += class_name[last_pos:class_name_len]
    return file_name.lower()

def getHeaderPath(path):
    print 'path', path
    binFile = os.path.abspath(__file__)
    rootPath = os.path.dirname(os.path.dirname(binFile))
    if not rootPath.endswith('/'):
        rootPath += "/"

    if path.endswith('/test'):
        path = path[:-5]
        pass
    header_path = path[len(rootPath):]
    return header_path

def createClassH(path, classname, filename):
    print "begin to create class h"
    hp = getHeaderPath(path)
    print "head path:", hp
    micro_path = hp.replace('/', '_')
    sub_ns = hp[hp.index('/') + 1:]
    if sub_ns.find('/') > 0:
        sub_ns = sub_ns[:sub_ns.index('/')]
        pass
    print "sub_ns is:", sub_ns

    context = ""
    context = context + "#ifndef " + micro_path.upper() + "_" + classname.upper() + "_H" + "\n"
    context = context + "#define " + micro_path.upper() + "_" + classname.upper() + "_H" + "\n"
    context = context + "" + "\n"
    context = context + "#include \"" + processor_name + "/common/common.h\"" + "\n"
    context = context + "#include \"" + processor_name + "/common/log.h\"" + "\n"
    context = context + "" + "\n"
    context = context + "BEGIN_" + processor_name.upper() + "_NS(" + sub_ns + ");" + "\n"
    context = context + "" + "\n"
    context = context + "class " + classname + "\n"
    context = context + "{" + "\n"
    context = context + "public:" + "\n"
    context = context + "    " + classname + "();" + "\n"
    context = context + "    ~" + classname + "();" + "\n"
    context = context + "private:" + "\n"
    context = context + "    " + classname + "(const " + classname + " &);" + "\n"
    context = context + "    " + classname + "& operator=(const " + classname + " &);" + "\n"
    context = context + "public:" + "\n"
    context = context + "" + "\n"
    context = context + "private:" + "\n"
    context = context + "    KIRIN_LOG_DECLARE();" + "\n"
    context = context + "};" + "\n"
    context = context + "" + "\n"
    context = context + "END_" + processor_name.upper() + "_NS(" + sub_ns + ");" + "\n"
    context = context + "" + "\n"
    context = context + "#endif //" + micro_path.upper() + "_" + classname.upper() + "_H" + "\n"
    f = file(path + "/" + filename + ".h", "w")
    f.write(context)
    f.close()
    return True

def createClassCPP(path, classname, filename):
    print "\nbegin to create class cpp"
    hp = getHeaderPath(path)
    print "head path:", hp
    micro_path = hp.replace('/', '_')
    sub_ns = hp[hp.index('/') + 1:]
    if sub_ns.find('/') > 0:
        sub_ns = sub_ns[:sub_ns.index('/')]
        pass
    print "sub_ns is:", sub_ns

    context = ""
    if (not re.match(".*/test", path)):
        context = context + "#include \"" + hp + "/" + filename + ".h\"" + "\n"
    else:
        context = context + "#include \"" + hp + "/test/" + filename + ".h\"" + "\n"
    context += "\n"
    context += "BEGIN_" + processor_name.upper() + "_NS(" + sub_ns + ");" + "\n"
    context += "KIRIN_LOG_SETUP(" + sub_ns + ", " + classname + ");" + "\n"
    context += "\n"
    context += classname + "::" + classname + "() \n{ " + "\n"
    context += "}" + "\n"
    context += "\n"
    context += classname + "::~" + classname + "() \n{ " + "\n"
    context += "}" + "\n"
    context += "\n"
    context += "END_" + processor_name.upper() + "_NS(" + sub_ns + ");" + "\n"
    context += "\n"
    f = file(path + "/" + filename + ".cpp", "w")
    f.write(context)
    f.close()
    return True

def createClass(path, classname, filename):
    ret = 0
    if os.path.exists(path + "/" + filename + ".h"):
        print "header file " + path + "/" + filename + ".h exists!"
    else:
        if createClassH(path, classname, filename):
            print "create header file " + path + "/" + filename + ".h succeed."
            ret = ret + 1
        else:
            print "create header file " + path + "/" + filename + ".h failed."
    if os.path.exists(path + "/" + filename + ".cpp"):
        print "source file " + path + "/" + filename + ".cpp exists!"
    else:
        if createClassCPP(path, classname, filename):
            print "create source file " + path + "/" + filename + ".cpp succeed."
            ret = ret + 1
        else:
            print "create source file " + path + "/" + filename + ".cpp failed."
    return ret

def insertMakefile(makefile, name):
    TIP = "source_list"
    TIPLEN = len(TIP)
    TIP2 = "EXTRA_DIST"
    TIP2LEN = len(TIP2)
    fin = file(makefile, "r")
    fbak = file(makefile + ".bak", "w")
    context = fin.read()
    fin.close()
    fbak.write(context)
    fbak.close()
    fout = file(makefile, "w")
    fin = file(makefile + ".bak", "r")
    while True:
        l = fin.readline()
        if not l:
            break
        if len(l) > TIPLEN and l[:TIPLEN] == TIP:
            l = l + "    " + name + ".cpp\\\n"
#      n = l.find("=")
#      if n != -1:
#    l = l[:n+1] + " " + name + ".cpp " + l[n+1:]
            if len(l) > TIP2LEN and l[:TIP2LEN] == TIP2:
                l = l + "    " + name + ".h\\\n"
#      n = l.find("=")
#      if n != -1:
#    l = l[:n+1] + " " + name + ".h " + l[n+1:]
                fout.write(l)
                fin.close()
                fout.close()
                print name + " has been inserted into " + makefile


def insertSConscript(scon_name, name):
    hasFindSource = False
    hasWritten = False
    os.system("mv " + scon_name + " " + scon_name + ".bak")
    TIP = ur"_sources\s*=\s*\["
    fin = file(scon_name + ".bak", "r");
    fout = file(scon_name, "w");
    while True:
        l = fin.readline()
        if not l:
            break;
        if not hasFindSource and not hasWritten:
            if re.search(TIP, l):
                hasFindSource = True
        elif not hasWritten and l.find(']') != -1:
            l = "    \'" + name + ".cpp\',\n" + l
            hasFindSource = False
            hasWritten = True
        fout.write(l)
    fin.close()
    fout.close()
    print name + " has been inserted into " + scon_name
    os.system("rm " + scon_name + ".bak")

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print >> sys.stderr, "Usage:", sys.argv[0], "<classname>"
        sys.exit(1)
    path = os.getcwd()
    class_name = sys.argv[1];
    file_name = getFileName(class_name)
    if createClass(path, class_name, file_name) == 2:
        insertSConscript(path + "/SConscript", file_name)
