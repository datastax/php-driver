project('php-cassandra', 'c', version: '0.3.0', default_options: [
  'b_lundef=false',
  'b_colorout=always',
  'b_pie=true',
  'c_std=c17',
  'cpp_std=c++20'
])


cmake = import('cmake')
cassandra_cpp_opt = cmake.subproject_options()


cassandra_cpp = cmake.subproject('cassandra_cpp', options: cassandra_cpp_opt)

cc = meson.get_compiler('c')
cc_clang = cc.get_id().contains('clang')
cc_gnu = cc.get_id().contains('gcc')

linux = host_machine.system() == 'linux'
apple = host_machine.system() == 'darwin'


php_include_dir = run_command('php-config', '--include-dir', check: true).stdout().strip()

php_include_dir_all = include_directories(php_include_dir)
php_include_dir_main = include_directories(php_include_dir + '/main')
php_include_dir_Zend = include_directories(php_include_dir + '/Zend')
php_include_dir_TSRM = include_directories(php_include_dir + '/TSRM')
php_include_dir_ext = include_directories(php_include_dir + '/ext')


incdir = [
  php_include_dir_all,
  php_include_dir_main,
  php_include_dir_Zend,
  php_include_dir_TSRM,
  php_include_dir_ext
]


SOURCES = []

libcassandra = cassandra_cpp.dependency('libcassandra')

library(
    'cassandra',
    sources: SOURCES,
    include_directories: incdir,
    name_prefix: '',
    dependencies: [libcassandra]
)
