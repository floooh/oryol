'''
Code generator script for python files, called during build process. 
'''
import sys
import os
import imp

def processFile(absPyPath) :
    # dynamically load (and execute) the generator module
    path, script = os.path.split(absPyPath)
    moduleName, ext = os.path.splitext(script)
    fp, pathname, description = imp.find_module(moduleName, [path])
    module = imp.load_module(moduleName, fp, pathname, description)
    module.generate(path + '/', moduleName)

#=== entry point
if len(sys.argv) > 1 :
    for arg in sys.argv[1:] :
        print '## processing: {}'.format(arg)
        processFile(arg)
else :
    print 'Needs full path to a generator py file!'

