Build arangodb on Windows
=========================

In GitUI:
    checkout to /cygdrive/c/obi/arangodb

In terminal:
    cd /cygdrive/c/obi/arangodb/build
    ./scripts/build-nsis.sh --buildDir /cygdrive/c/obi/arangodb/build

In Explorer:
    copy "dlls" into /cygdrive/c/obi/arangodb/build/RelWithDebInfo

In Visual Studio (arangod properties):
    set working dir to /cygdrive/c/obi/arangodb/etc/relative
    add commandline arguments: --console -c /cygdrive/c/obi/arangodb/etc/relative/arango.conf

