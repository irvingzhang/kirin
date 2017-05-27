# -*- mode: python -*-
import sys, os, os.path, platform, re, time
from SCons.Tool.ascons.PathListVariable import PathListVariable
# The root environment
### Place to store some options which influence the compiler
 
env = Environment(tools=['default', 'ascons'])
env.requireVersion(6)
env['heapchecktype'] = 'tcmalloc'
env['enable_strip'] = False

vars = Variables()
vars.Add('version', 'version used to compile rpm', '0.0.1')
vars.Update(env)

env['project_name'] = 'kirin'
env['package_name'] = env['project_name']

env.SConscript(
    dirs='.',
    exports='env',
    variant_dir=env['BUILD_DIR'],
    duplicate=1
    )

# insatll .h for making rpm
env.aInstallHeader(env['BUILD_DIR'] + '/kirin/common/*.h', 'kirin/common');
env.aInstallHeader(env['BUILD_DIR'] + '/kirin/test/*.h', 'kirin/test');
