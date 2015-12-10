

set(command "C:/Program Files (x86)/CMake/bin/cmake.exe;-P;C:/My/tphProjects/Level2/MY/caffe_vcproj/external/gflags-prefix/tmp/gflags-gitclone.cmake")
execute_process(
  COMMAND ${command}
  RESULT_VARIABLE result
  OUTPUT_FILE "C:/My/tphProjects/Level2/MY/caffe_vcproj/external/gflags-prefix/src/gflags-stamp/gflags-download-out.log"
  ERROR_FILE "C:/My/tphProjects/Level2/MY/caffe_vcproj/external/gflags-prefix/src/gflags-stamp/gflags-download-err.log"
  )
if(result)
  set(msg "Command failed: ${result}\n")
  foreach(arg IN LISTS command)
    set(msg "${msg} '${arg}'")
  endforeach()
  set(msg "${msg}\nSee also\n  C:/My/tphProjects/Level2/MY/caffe_vcproj/external/gflags-prefix/src/gflags-stamp/gflags-download-*.log\n")
  message(FATAL_ERROR "${msg}")
else()
  set(msg "gflags download command succeeded.  See also C:/My/tphProjects/Level2/MY/caffe_vcproj/external/gflags-prefix/src/gflags-stamp/gflags-download-*.log\n")
  message(STATUS "${msg}")
endif()
