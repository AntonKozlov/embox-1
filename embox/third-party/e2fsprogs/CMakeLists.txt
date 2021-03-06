
cmake_minimum_required(VERSION 2.8)

include(ExternalProject)
ExternalProject_Add(
   e2fsprogs
   PREFIX .
   DOWNLOAD_DIR $ENV{EMBOX_ROOT}/download
   URL http://downloads.sourceforge.net/project/e2fsprogs/e2fsprogs/v1.42.8/e2fsprogs-1.42.8.tar.gz
   
   URL_MD5 8ef664b6eb698aa6b733df59b17b9ed4
   
   UPDATE_COMMAND ""
   
   PATCH_COMMAND
      cd $ENV{EMBOX_ROOT}/third-party/e2fsprogs/tree
      #&& bash -c "cp -l -u -f --parents -t <SOURCE_DIR>/ `find . -not -path '*/.*' -and -type f`"
      && patch -p1 -d <SOURCE_DIR> -N --input=$ENV{EMBOX_ROOT}/third-party/e2fsprogs/patch.txt 
      && cd <SOURCE_DIR> && autoconf && cd -
   
   LOG_CONFIGURE 1
   CONFIGURE_COMMAND
      <SOURCE_DIR>/configure --prefix=<INSTALL_DIR>
      ""CC=$ENV{EMBOX_DERIVED_CC}""
      ""CFLAGS=$ENV{EMBOX_DERIVED_CFLAGS}""
      ""CXXFLAGS=$ENV{EMBOX_DERIVED_CXXFLAGS}""
      ""LDFLAGS=$ENV{EMBOX_DERIVED_LDFLAGS}""
      --host=$ENV{EMBOX_DERIVED_TARGET}
      --disable-tls
      --disable-backtrace
      --disable-elf-shlibs
      --disable-bsd-shlibs

   BUILD_COMMAND
      make
         RDYNAMIC=
         all

   INSTALL_COMMAND
      make install-libs
   LOG_BUILD 1
)
SET (CMAKE_BUILD_TYPE "Debug")
