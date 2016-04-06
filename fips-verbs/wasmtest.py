"""fips verb to build the oryol samples webpage"""

import os
import yaml 
import shutil
import subprocess
import glob
from string import Template

from mod import log, util, project, emscripten
from tools import texexport

BuildWasm = True
ExportAssets = True

#-------------------------------------------------------------------------------
def copy_build_files(fips_dir, proj_dir, webpage_dir) :
    # copy all files from the deploy dir to the webpage_dir
    ws_dir = util.get_workspace_dir(fips_dir)
    src_dir = '{}/fips-deploy/oryol/wasmasmjs-make-release'.format(ws_dir)
    dst_dir = webpage_dir
    if os.path.isdir(dst_dir) :
        shutil.rmtree(dst_dir)
    shutil.copytree(src_dir, dst_dir)
    shutil.copy('{}/web/wasmsuite-readme.md'.format(proj_dir), '{}/README.md'.format(dst_dir))
    shutil.copy('{}/LICENSE'.format(proj_dir), '{}/LICENSE'.format(dst_dir))

#-------------------------------------------------------------------------------
def export_assets(fips_dir, proj_dir, webpage_dir) :
    tex_srcdir = proj_dir + '/data'
    tex_dstdir = webpage_dir + '/data'
    texexport.configure(proj_dir, tex_srcdir, tex_dstdir)
    texexport.exportSampleTextures(['dds'])
    for dataFile in glob.glob(proj_dir + '/data/*.txt') :
        shutil.copy(dataFile, webpage_dir)
    tbui_from = '{}/data/tbui'.format(proj_dir)
    tbui_to   = '{}/data/tbui'.format(webpage_dir)
    shutil.copytree(tbui_from, tbui_to)

#-------------------------------------------------------------------------------
def build_deploy_webpage(fips_dir, proj_dir) :
    # if webpage dir exists, clear it first
    ws_dir = util.get_workspace_dir(fips_dir)
    webpage_dir = '{}/fips-deploy/oryol-wasm-buildsuite'.format(ws_dir)
    if os.path.isdir(webpage_dir) :
        shutil.rmtree(webpage_dir)
    os.makedirs(webpage_dir)

    config = 'wasmasmjs-make-release'
    project.clean(fips_dir, proj_dir, config) 
    project.gen(fips_dir, proj_dir, config)
    project.build(fips_dir, proj_dir, config)
    
    copy_build_files(fips_dir, proj_dir, webpage_dir)
    if ExportAssets :
        export_assets(fips_dir, proj_dir, webpage_dir)

    log.colored(log.GREEN, 'Done. ({})'.format(webpage_dir))

#-------------------------------------------------------------------------------
def serve_webpage(fips_dir, proj_dir) :
    ws_dir = util.get_workspace_dir(fips_dir)
    webpage_dir = '{}/fips-deploy/oryol-wasm-buildsuite'.format(ws_dir)
    p = util.get_host_platform()
    if p == 'osx' :
        try :
            subprocess.call(
                'open http://localhost:8000 ; python {}/mod/httpserver.py'.format(fips_dir),
                cwd = webpage_dir, shell=True)
        except KeyboardInterrupt :
            pass
    elif p == 'win':
        try:
            subprocess.call(
                'cmd /c start http://localhost:8000 && python {}/mod/httpserver.py'.format(fips_dir),
                cwd = webpage_dir, shell=True)
        except KeyboardInterrupt:
            pass
    elif p == 'linux':
        try:
            subprocess.call(
                'xdg-open http://localhost:8000; python {}/mod/httpserver.py'.format(fips_dir),
                cwd = webpage_dir, shell=True)
        except KeyboardInterrupt:
            pass

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
             'fips wasmtests build\n' +
             'fips wasmtests serve\n' +
             log.DEF +
             '    build WebAssembly build-suite samples (https://github.com/WebAssembly/build-suite)')

