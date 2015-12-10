

set(command "env;CFLAGS= /DWIN32 /D_WINDOWS /W3;CXXFLAGS= /DWIN32 /D_WINDOWS /W3 /GR /EHsc;C:/My/tphProjects/Level2/MY/caffe_vcproj/external/glog-prefix/src/glog/configure;--prefix=C:/My/tphProjects/Level2/MY/caffe_vcproj/external/glog-install;--enable-shared=no;--enable-static=yes;--with-gflags=C:/My/tphProjects/Level2/MY/caffe_vcproj/external/gflags-install/lib/..")
execute_process(
  COMMAND ${command}
  RESULT_VARIABLE result
  OUTPUT_FILE "C:/My/tphProjects/Level2/MY/caffe_vcproj/external/glog-prefix/src/glog-stamp/glog-configure-out.log"
  ERROR_FILE "C:/My/tphProjects/Level2/MY/caffe_vcproj/external/glog-prefix/src/glog-stamp/glog-configure-err.log"
  )
if(result)
  set(msg "Command failed: ${result}\n")
  foreach(arg IN LISTS command)
    set(msg "${msg} '${arg}'")
  endforeach()
  set(msg "${msg}\nSee also\n  C:/My/tphProjects/Level2/MY/caffe_vcproj/external/glog-prefix/src/glog-stamp/glog-configure-*.log\n")
  message(FATAL_ERROR "${msg}")
else()
  set(msg "glog configure command succeeded.  See also C:/My/tphProjects/Level2/MY/caffe_vcproj/external/glog-prefix/src/glog-stamp/glog-configure-*.log\n")
  message(STATUS "${msg}")
endif()
