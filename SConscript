# -*- mode: python -*-

# Inherit the environment from my parent.
Import('env')

# Make a copy of the environment, so my changes are limited in this directory and sub-directories.
env = env.Clone()

env['kirin_libs'] = ['kirin', 'dl']

env['all_libs'] = env['kirin_libs'] 
env['test_libs'] = env['kirin_libs'] 

env.Append(ACSUBST = {'BUILDDIR': env['RELATIVE_BUILD_DIR']})
env.Append(ACSUBST = {'TOP_BUILDDIR': env['BUILD_DIR']})
env.Append(ACSUBST = {'TOP_DIR': env['TOP_DIR']})
env.Append(ACSUBST = {'VERSION': env['version']})

env['ABS_BUILD_DIR'] = env['TOP_DIR'] + '/' + env['BUILD_DIR'].replace('#', '')

env.aACSubst(target = ['kirin/test/test.h'],
             source = ['kirin/test/test.h.in'])

env.aACSubst(target = ['kirin/common/common.h'],
             source = ['kirin/common/common.h.in'])

env.aCheckLibrary('alog')

# List my sub-directories.
env.aDirs('kirin') 
