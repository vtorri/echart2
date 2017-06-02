project('echart', 'c')
srclib = ['src/lib/Echart.h', 'src/lib/echart_main.c']
einadep = dependency('eina')
evasdep = dependency('evas')
shared_library('libechart', srclib, c_args : '-DECHART_BUILD', dependencies : [einadep, evasdep])
