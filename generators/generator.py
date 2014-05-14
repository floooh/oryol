'''
The main generator.py script. This is integrated into the build process
as build tool for *.xml files. At compile time, it parses the
xml file, reads the 'generator' attr which names the actual
generator script and hands off processing to this script.

FIXME: this needs some sort of Generator Version in the header of
the generated files, which must be used additionally to the file
timestamps to decide whether processing must happen
'''

import sys
import os
import string
import imp
import xml.etree.ElementTree as ET

def processFile(absXmlPath) :
    '''
    Main entry point, parses the provided xml file and hands off
    processing to an actual generator script.
    '''
    absSourcePath = string.replace(absXmlPath, '.xml', '.cc')
    absHeaderPath = string.replace(absXmlPath, '.xml', '.h')

    # first check if this is a compatible file
    print "Checking '{}' ...".format(absXmlPath)
    xmlTree = ET.parse(absXmlPath)
    xmlRoot = xmlTree.getroot()
    if xmlRoot.tag == 'Generator' and xmlRoot.get('type', 'notype') != 'notype' :
        # check existance and modification time whether we should skip processing
        xmlModifiedTime = os.path.getmtime(absXmlPath)
        needsProcessing = False
        if os.path.isfile(absSourcePath) :
            # source file exists, check time stamp
            if xmlModifiedTime > os.path.getmtime(absSourcePath) :
                needsProcessing = True
        else :
            # generated source file doesn't exist yet
            needsProcessing = True

        if os.path.isfile(absHeaderPath) :
            # header file exists, check time stamp
            if xmlModifiedTime > os.path.getmtime(absHeaderPath) :
                needsProcessing = True
        else :
            # generated header file doesn't exist yet
            needsProcessing = True

        # dynamically load the generator module
        moduleName = xmlRoot.get('type')
        fp, pathname, description = imp.find_module(moduleName)
        module = imp.load_module(moduleName, fp, pathname, description)

        if not needsProcessing :
            needsProcessing = module.isDirty(xmlTree, absXmlPath, absSourcePath, absHeaderPath)
        if needsProcessing :
            print "  Generating source file from '{}'".format(absXmlPath)
            module.generate(xmlTree, absXmlPath, absSourcePath, absHeaderPath)
        else :
            print "  Nothing to do for '{}'".format(absXmlPath)

    else :
        print 'Not a generator xml file, ignored'

#=== entry point
if len(sys.argv) > 1 :
    for arg in sys.argv[1:] :
        print 'Processing: {}'.format(arg)
        processFile(arg)
else :
    print 'Needs full path to a generator xml file!'
