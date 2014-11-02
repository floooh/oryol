'''
Code generator script for python files, called during build process. 
'''
import sys

#=== entry point
if len(sys.argv) > 1 :
    for arg in sys.argv[1:] :
        print 'Processing: {}'.format(arg)
        # processFile(arg)
else :
    print 'Needs full path to a generator py file!'

