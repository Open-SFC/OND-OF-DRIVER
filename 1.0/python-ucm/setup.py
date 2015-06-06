from distutils.core import setup, Extension

module1 = Extension('_ucm',
        define_macros = [('MAJOR_VERSION', '1'),('MINOR_VERSION', '1')],
        include_dirs = ['/usr/local/include','../controller/ofcli/include/infra/transport', '../controller/ofcli/include/infra/dm','../controller/ofcli/include/infra/je','../controller/ofcli/include/common','../controller/include','../controller/ofcli/include/lxos','../controller/ofproto/src/include','../ext/urcu','../ext/futex','../controller/ofcli/include/utils/netutil','../controller/ofcli/include/utils/dslib','../controller/ofcli/infra/je/include/', '../cminfra/include/common', '../cminfra/include/lxos','../cminfra/include/utils/netutil','../cminfra/include/utils/dslib','../cminfra/include/infra/dm','../cminfra/include/infra/transport','../cminfra/include/infra/je','../cminfra/infra/je/include'],
        libraries = ['ucmtrchl','ctlrutils','lxos','utils'],
        library_dirs = ['/usr/local/lib','../controller/ofproto/build/lib-x86-64bit'],
        sources = ['ucm-override.c'])

setup (name = 'UCM',
        version = '1.1',
        description = 'This is a UCM package',
        author = 'Purandhar Sairam Mannidi',
        author_email = 'sairam.mp@freescale.com',
        url = 'http://docs.python.org/extending/building',
        long_description = '''
        This is Python UCM Bindings.
        ''',
        ext_modules = [module1])
