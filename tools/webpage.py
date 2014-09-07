#!/usr/bin/env python
'''
Build the Oryol sample webpage.
'''

import os
import json
from string import Template
import shutil

GitHubSamplesURL = 'https://github.com/floooh/oryol/tree/master/code/Samples/'

#-------------------------------------------------------------------------------
def build() :
    if not os.path.exists('build/webpage') :
        os.makedirs('build/webpage')

    # load the websamples.json file, should have been created during the last build
    f = open('build/websamples.json')
    samples = json.loads(f.read())
    f.close

    # build the thumbnail gallery
    content = ''
    for sample in samples['samples'] :
        if sample['name'] != '__end__' :
            name    = sample['name']
            imgPath = sample['image']
            types   = sample['type'] 
            desc    = sample['desc']
            head, tail = os.path.split(imgPath)
            if tail == 'none' :
                imgFileName = 'dummy.jpg'
            else :
                imgFileName = tail
            content += '<div class="thumb">\n'
            content += '  <div class="thumb-title">{}</div>\n'.format(name)
            content += '  <div class="img-frame"><a href="{}.html"><img class="image" src="{}" title="{}"></img></a></div>\n'.format(name,imgFileName,desc)
            content += '  <div class="thumb-bar">\n'
            content += '    <ul class="thumb-list">\n'
            if 'emscripten' in types :
                content += '      <li class="thumb-item"><a class="thumb-link" href="{}.html">emsc</a></li>\n'.format(name)
            if 'pnacl' in types :
                content += '      <li class="thumb-item"><a class="thumb-link" href="{}_pnacl.html">pnacl</a></li>\n'.format(name)
            if 'android' in types :
                content += '      <li class="thumb-item"><a class="thumb-link" href="{}-debug.apk">apk</a></li>\n'.format(name)
            content += '    </ul>\n'
            content += '  </div>\n'
            content += '</div>\n'

    # populate the html template, and write to the build directory
    f = open('web/index.html')
    templ = Template(f.read())
    f.close
    html = templ.safe_substitute(samples=content)
    f = open('build/webpage/index.html', 'w')
    f.write(html)
    f.close()

    # copy other required files
    shutil.copy('web/style.css', 'build/webpage/style.css')
    shutil.copy('web/dummy.jpg', 'build/webpage/dummy.jpg')
    shutil.copy('web/emsc.js', 'build/webpage/emsc.js')
    shutil.copy('web/pnacl.js', 'build/webpage/pnacl.js')
    shutil.copy('web/about.html', 'build/webpage/about.html')    

    # generate emscripten HTML pages
    for sample in samples['samples'] :
        name = sample['name']
        if name != '__end__' and 'emscripten' in sample['type'] :
            print '> generate emscripten HTML page: {}'.format(name)
            for ext in ['js', 'html.mem'] :
                shutil.copy('bin/emsc/{}.{}'.format(name, ext), 'build/webpage')
            f = open('web/emsc.html')
            templ = Template(f.read())
            f.close()
            srcUrl = GitHubSamplesURL + sample['src'];
            html = templ.safe_substitute(name=name, source=srcUrl)
            f = open('build/webpage/{}.html'.format(name), 'w')
            f.write(html)
            f.close()

    # copy PNaCl HTML pages
    for sample in samples['samples'] :
        name = sample['name']
        if name != '__end__' and 'pnacl' in sample['type'] :
            print '> generate PNaCl HTML page: {}'.format(name)
            for ext in ['nmf', 'pexe'] :
                shutil.copy('bin/pnacl/{}.{}'.format(name, ext), 'build/webpage')
            f = open('web/pnacl.html')
            templ = Template(f.read())
            f.close()
            srcUrl = GitHubSamplesURL + sample['src'];
            html = templ.safe_substitute(name=name, source=srcUrl)
            f = open('build/webpage/{}_pnacl.html'.format(name), 'w')
            f.write(html)
            f.close()

    # copy the screenshots
    for sample in samples['samples'] :
        if sample['name'] != '__end__' :
            imgPath = sample['image']
            head, tail = os.path.split(imgPath)
            if tail != 'none' :
                print '> copy screenshot: {}'.format(tail)
                shutil.copy(imgPath, 'build/webpage/' + tail)

    # copy the Android sample files over
    for sample in samples['samples'] :
        if sample['name'] != '__end__' and 'android' in sample['type'] :
            print '> copy android sample files: {}'.format(sample['name'])
            shutil.copy('bin/android/{}-debug.apk'.format(sample['name']), 'build/webpage')

    print 'Generated Samples web page under build/webpage!'

