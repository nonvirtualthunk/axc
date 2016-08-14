

set(command "/usr/local/Cellar/cmake/3.5.1/bin/cmake;-P;/Users/sambock/Code/experiment/axc/catch/tmp/catch-gitclone.cmake")
execute_process(
  COMMAND ${command}
  RESULT_VARIABLE result
  OUTPUT_FILE "/Users/sambock/Code/experiment/axc/catch/src/catch-stamp/catch-download-out.log"
  ERROR_FILE "/Users/sambock/Code/experiment/axc/catch/src/catch-stamp/catch-download-err.log"
  )
if(result)
  set(msg "Command failed: ${result}\n")
  foreach(arg IN LISTS command)
    set(msg "${msg} '${arg}'")
  endforeach()
  set(msg "${msg}\nSee also\n  /Users/sambock/Code/experiment/axc/catch/src/catch-stamp/catch-download-*.log")
  message(FATAL_ERROR "${msg}")
else()
  set(msg "catch download command succeeded.  See also /Users/sambock/Code/experiment/axc/catch/src/catch-stamp/catch-download-*.log")
  message(STATUS "${msg}")
endif()
