"""fips verb to build the oryol samples webpage"""

import os
import yaml 
import shutil
import subprocess
import glob
from string import Template

from mod import log, util, project, config
from tools import texexport

GitHubSamplesURL = 'https://github.com/floooh/oryol/tree/master/code/Samples/'

#-------------------------------------------------------------------------------
def deploy_webpage(fips_dir, proj_dir, webpage_dir) :
    """builds the final webpage under under .fips-deploy/oryol-webpage"""
    ws_dir = util.get_workspace_dir(fips_dir)

    # load the websamples.yml file, should have been created during the last build
    with open(webpage_dir + '/websamples.yml', 'r') as f :
        samples = yaml.load(f.read())

    # build the thumbnail gallery
    content = ''
    for sample in samples :
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
    with open(proj_dir + '/web/index.html', 'r') as f :
        templ = Template(f.read())
    html = templ.safe_substitute(samples=content)
    with open(webpage_dir + '/index.html', 'w') as f :
        f.write(html)

    # copy other required files
    for name in ['style.css', 'dummy.jpg', 'emsc.js', 'pnacl.js', 'about.html', 'favicon.png'] :
        log.info('> copy file: {}'.format(name))
        shutil.copy(proj_dir + '/web/' + name, webpage_dir + '/' + name)

    # generate emscripten HTML pages
    emsc_deploy_dir = '{}/.fips-deploy/oryol/emsc-make-release'.format(ws_dir)
    for sample in samples :
        name = sample['name']
        if name != '__end__' and 'emscripten' in sample['type'] :
            log.info('> generate emscripten HTML page: {}'.format(name))
            for ext in ['js', 'html.mem'] :
                shutil.copy('{}/{}.{}'.format(emsc_deploy_dir, name, ext), webpage_dir)
            with open(proj_dir + '/web/emsc.html', 'r') as f :
                templ = Template(f.read())
            src_url = GitHubSamplesURL + sample['src'];
            html = templ.safe_substitute(name=name, source=src_url)
            with open('{}/{}.html'.format(webpage_dir, name), 'w') as f :
                f.write(html)

    # copy PNaCl HTML pages
    pnacl_deploy_dir = '{}/.fips-deploy/oryol/pnacl-make-release'.format(ws_dir)
    for sample in samples :
        name = sample['name']
        if name != '__end__' and 'pnacl' in sample['type'] :
            log.info('> generate PNaCl HTML page: {}'.format(name))
            for ext in ['nmf', 'pexe'] :
                shutil.copy('{}/{}.{}'.format(pnacl_deploy_dir, name, ext), webpage_dir)
            with open(proj_dir + '/web/pnacl.html', 'r') as f :
                templ = Template(f.read())
            src_url = GitHubSamplesURL + sample['src'];
            html = templ.safe_substitute(name=name, source=src_url)
            with open('{}/{}_pnacl.html'.format(webpage_dir, name), 'w') as f :
                f.write(html)

    # copy the screenshots
    for sample in samples :
        if sample['name'] != '__end__' :
            img_path = sample['image']
            head, tail = os.path.split(img_path)
            if tail != 'none' :
                log.info('> copy screenshot: {}'.format(tail))
                shutil.copy(img_path, webpage_dir + '/' + tail)

    # copy the Android sample files over
    # android_deploy_dir = '{}/.fips-deploy/oryol/android-make-release'.format(ws_dir)
    # for sample in samples :
    #     if sample['name'] != '__end__' and 'android' in sample['type'] :
    #         log.info('> copy android sample files: {}'.format(sample['name']))
    #         shutil.copy('{}/{}-debug.apk'.format(android_deploy_dir, sample['name']), webpage_dir)

#-------------------------------------------------------------------------------
def export_assets(fips_dir, proj_dir, webpage_dir) :

    tex_srcdir = proj_dir + '/data'
    tex_dstdir = webpage_dir
    texexport.configure(proj_dir, tex_srcdir, tex_dstdir)
    texexport.exportSampleTextures()
    for dataFile in glob.glob(proj_dir + '/data/nanovg/*') :
        shutil.copy(dataFile, webpage_dir)
    for dataFile in glob.glob(proj_dir + '/data/*.txt') :
        shutil.copy(dataFile, webpage_dir)
    
#-------------------------------------------------------------------------------
def build_deploy_webpage(fips_dir, proj_dir) :
    # if webpage dir exists, clear it first
    ws_dir = util.get_workspace_dir(fips_dir)
    webpage_dir = '{}/.fips-deploy/oryol-webpage'.format(ws_dir)
    if os.path.isdir(webpage_dir) :
        shutil.rmtree(webpage_dir)
    os.makedirs(webpage_dir)

    # compile emscripten, pnacl and android samples
    project.build(fips_dir, proj_dir, 'emsc-make-release')
    project.build(fips_dir, proj_dir, 'pnacl-make-release')
    # project.build(fips_dir, proj_dir, 'android-make-release')
    
    # export sample assets
    export_assets(fips_dir, proj_dir, webpage_dir)

    # deploy the webpage
    deploy_webpage(fips_dir, proj_dir, webpage_dir)

    log.colored(log.GREEN, 'Generated Samples web page under {}.'.format(webpage_dir))

#-------------------------------------------------------------------------------
def serve_webpage(fips_dir, proj_dir) :
    ws_dir = util.get_workspace_dir(fips_dir)
    webpage_dir = '{}/.fips-deploy/oryol-webpage'.format(ws_dir)
    p = config.get_host_platform()
    if p == 'osx' :
        try :
            subprocess.call(
                    ['open http://localhost:8000 ; python {}/mod/httpserver.py'.format(fips_dir)],
                    cwd = webpage_dir, shell=True)
        except KeyboardInterrupt :
            pass
    elif p == 'win':
        log.error("FIXME: Windows not supported yet")
    elif p == 'linux':
        log.error("FIXME: Linux not supported yet")

#-------------------------------------------------------------------------------
def run(fips_dir, proj_dir, args) :
    if len(args) > 0 :
        if args[0] == 'build' :
            build_deploy_webpage(fips_dir, proj_dir)
        elif args[0] == 'serve' :
            serve_webpage(fips_dir, proj_dir)
        else :
            log.error("Invalid param '{}', expected 'build' or 'serve'".format(args[0]))
    else :
        log.error("Param 'build' or 'serve' expected")

#-------------------------------------------------------------------------------
def help() :
    log.info(log.YELLOW +
             'fips webpage build\n' +
             'fips webpage serve\n' +
             log.DEF +
             '    build oryol samples webpage')

