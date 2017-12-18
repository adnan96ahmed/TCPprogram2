#!/usr/bin/python
import sys
import os
import subprocess
from math import *

if ((len(sys.argv) < 4) or (len(sys.argv) > 5)):
    sys.exit("Please enter the number of clients to run as the second arguement,\n'server-IP-address:port-number' as the third argument,\nthe filename to read from as the fourth argument,\nand an optional buffer length as the fifth argument")

else:
    nClients = sys.argv[1]
    serverInfo = sys.argv[2]
    fileName = sys.argv[3]
    if (len(sys.argv) == 5):
        optionalBuffer = sys.argv[4]

singleCommand = "./client " + serverInfo + " " + fileName
if (len(sys.argv)==5):
    singleCommand = singleCommand + " " + optionalBuffer

fullCommand = singleCommand
if (nClients > 1):
    for x in range(1, int(nClients)):
        fullCommand = fullCommand + ";" + singleCommand

#following code taken from: https://stackoverflow.com/questions/17742789/running-multiple-bash-commands-with-subprocess

def subprocess_cmd(command):
    process = subprocess.Popen(command,stdout=subprocess.PIPE, shell=True)
    proc_stdout = process.communicate()[0].strip()
    print proc_stdout

subprocess_cmd(fullCommand)
