project('echart', 'c', version : '0.0.1')

as_name = meson.project_name()
as_version = meson.project_version()
conf = configuration_data()
conf.set_quoted('VERSION', as_version)
conf.set_quoted('PACKAGE', as_name)
configure_file(output : 'config.h',
               configuration : conf)

deplib = [dependency('eina'),
          dependency('evas')]

srclib = ['src/lib/Echart.h',
          'src/lib/echart_chart.c',
          'src/lib/echart_data.c',
          'src/lib/echart_main.c',
          'src/lib/echart_data.h',
          'src/lib/echart_private.h']

cflags = ['-Wall',
          '-Wextra',
          '-Wshadow',
          '-Wdeclaration-after-statement',
          '-Wmissing-prototypes',
          '-Wstrict-prototypes',
          '-Wpointer-arith',
          '-Wno-missing-field-initializers']

cflagslib = ['-DECHART_BUILD',
             '-DHAVE_CONFIG_H',
             cflags]

libbin = shared_library('echart',
                        srclib,
                        c_args : cflagslib,
                        dependencies : deplib)

cflagsbin = [cflags]
incbin = include_directories('src/lib')
depbin = declare_dependency(link_with : libbin,
                            include_directories : incbin)
srcbin = ['src/bin/echart.c']

executable('echart',
           srcbin,
           c_args : cflagsbin,
           dependencies : [depbin,
                           dependency('ecore-evas'),
                           dependency('evas'),
                           dependency('ecore'),
                           dependency('eina')])
