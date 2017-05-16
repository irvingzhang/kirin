# -*- mode: python -*-
import sys, os, os.path, platform, re, time
from SCons.Tool.ascons.PathListVariable import PathListVariable
# The root environment
### Place to store some options which influence the compiler
 
env = Environment(tools=['default', 'ascons'])
env.requireVersion(6)
env['heapchecktype'] = 'tcmalloc'
env['enable_strip'] = False
'''
env['system_includedir'].append(env['TOP_DIR'] + '/kirin/thirdparty/googletest/googletest/')
env['system_includedir'].append(env['TOP_DIR'] + '/kirin/thirdparty/googletest/googletest/src/')
env['system_includedir'].append(env['TOP_DIR'] + '/kirin/thirdparty/googletest/googletest/include/')
env['system_includedir'].append(env['TOP_DIR'] + '/kirin/thirdparty/googletest/googletest/gtest/')
env.updateOption('system_includedir', env['system_includedir'])
'''
##env.addOptions(PathListVariable('apsara_boost_path', 'path to apsara and apsara boost path',
##                                [env['depend_prefix'][0] + '/include/apsara/',
##                                env['depend_prefix'][0] + '/include/apsara/boost/tr1/']))

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

