

set(command "make;install")
execute_process(
  COMMAND ${command}
  RESULT_VARIABLE result
  OUTPUT_FILE "C:/My/tphProjects/Level2/MY/caffe_vcproj/external/glog-prefix/src/glog-stamp/glog-install-out.log"
  ERROR_FILE "C:/My/tphProjects/Level2/MY/caffe_vcproj/external/glog-prefix/src/glog-stamp/glog-install-err.log"
  )
if(result)
  set(msg "Command failed: ${result}\n")
  foreach(arg IN LISTS command)
    set(msg "${msg} '${arg}'")
  endforeach()
  set(msg "${msg}\nSee also\n  C:/My/tphProjects/Level2/MY/caffe_vcproj/external/glog-prefix/src/glog-stamp/glog-install-*.log\n")
  message(FATAL_ERROR "${msg}")
else()
  set(msg "glog install command succeeded.  See also C:/My/tphProjects/Level2/MY/caffe_vcproj/external/glog-prefix/src/glog-stamp/glog-install-*.log\n")
  message(STATUS "${msg}")
endif()
