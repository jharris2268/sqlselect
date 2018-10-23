from distutils.core import setup, Extension
from distutils.command.build_ext import build_ext
from distutils.sysconfig import customize_compiler
import shutil, os.path, subprocess

def call_pkgconfig(args):
    ans = subprocess.check_output(args).decode('utf8')
    return ans.split()
cflags = ['-std=c++14']
libs =['-L/usr/local/lib', '-loqt',]
libs += call_pkgconfig(['mapnik-config','--libs'])


mp_cflags=call_pkgconfig(['mapnik-config','--cflags'])
mp_cflags=[c for c in mp_cflags if not c in ('-std=c++11','-Wshadow')]
cflags += mp_cflags

cflags += ['-DPICOJSON_USE_INT64',]

#libs.append('-lboost_thread')

srcs=['../src/'+f for f in (
    "feature.cpp",
    "result.cpp",
    "table.cpp",
    "where.cpp",
    
)]
srcs.append('src/sqlselect.cpp')
srcs.append('src/geojson.cpp')
srcs.append('src/mvt.cpp')


class my_build_ext(build_ext):
    def build_extensions(self):
        customize_compiler(self.compiler)
        try:
            self.compiler.compiler_so.remove("-Wstrict-prototypes")
        except (AttributeError, ValueError):
            pass
        build_ext.build_extensions(self)

ext_modules = [
    Extension(
        'sqlselect._sqlselect',
        srcs,
        include_dirs=[
            '/usr/local/include',
            '../include/',            
        ],
        extra_link_args=libs,
        extra_compile_args=cflags,
        
    ),
    
    
]



setup(
    name='sqlselect',
    packages=['sqlselect'],
    version='0.0.1',
    long_description='',
    ext_modules=ext_modules,
    zip_safe=False,
    cmdclass = {'build_ext': my_build_ext},
)

