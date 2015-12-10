

set(command "C:/Program Files (x86)/CMake/bin/cmake.exe;--build;C:/My/tphProjects/Level2/MY/caffe_vcproj/external/gflags-prefix/src/gflags-build;--config;${config};--target;install")
execute_process(
  COMMAND ${command}
  RESULT_VARIABLE result
  OUTPUT_FILE "C:/My/tphProjects/Level2/MY/caffe_vcproj/external/gflags-prefix/src/gflags-stamp/gflags-install-out.log"
  ERROR_FILE "C:/My/tphProjects/Level2/MY/caffe_vcproj/external/gflags-prefix/src/gflags-stamp/gflags-install-err.log"
  )
if(result)
  set(msg "Command failed: ${result}\n")
  foreach(arg IN LISTS command)
    set(msg "${msg} '${arg}'")
  endforeach()
  set(msg "${msg}\nSee also\n  C:/My/tphProjects/Level2/MY/caffe_vcproj/external/gflags-prefix/src/gflags-stamp/gflags-install-*.log\n")
  message(FATAL_ERROR "${msg}")
else()
  set(msg "gflags install command succeeded.  See also C:/My/tphProjects/Level2/MY/caffe_vcproj/external/gflags-prefix/src/gflags-stamp/gflags-install-*.log\n")
  message(STATUS "${msg}")
endif()
