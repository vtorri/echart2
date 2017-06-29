project('echart', 'c', version : '0.0.1')

cc = meson.get_compiler('c')

### Library

dep_lib = [dependency('eina'),
           dependency('evas'),
           dependency('eina')]

src_lib = ['src/lib/Echart.h',
           'src/lib/echart_chart.c',
           'src/lib/echart_data.c',
           'src/lib/echart_line.c',
           'src/lib/echart_main.c',
           'src/lib/echart_chart.h',
           'src/lib/echart_data.h',
           'src/lib/echart_line.h',
           'src/lib/echart_private.h']

cflags = ['-Wall',
          '-Wextra',
          '-Wshadow',
          '-Wdeclaration-after-statement',
          '-Wmissing-prototypes',
          '-Wstrict-prototypes',
          '-Wpointer-arith',
          '-Wno-missing-field-initializers']

cflags_lib = ['-DECHART_BUILD',
              '-DHAVE_CONFIG_H',
              cflags]

libm = cc.find_library('m', required : true)

dep_bin = shared_library('echart',
                         src_lib,
                         c_args : cflags_lib,
                         dependencies : [dep_lib, libm])

### Binary

cflags_bin = [cflags]
inc_bin = include_directories('src/lib')
dep_bin = declare_dependency(link_with : dep_bin,
                             include_directories : inc_bin)
src_bin = ['src/bin/echart.c']

executable('echart',
           src_bin,
           c_args : cflags_bin,
           dependencies : [dep_bin,
                           dependency('ecore-evas'),
                           dependency('ecore'),
                           dep_lib])

### config.h

as_name = meson.project_name()
as_version = meson.project_version()
conf = configuration_data()
conf.set_quoted('VERSION', as_version)
conf.set_quoted('PACKAGE', as_name)
configure_file(output : 'config.h',
               configuration : conf)
