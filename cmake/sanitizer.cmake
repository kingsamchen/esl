
set(ESL_USE_SANITIZER "" CACHE STRING
  "Options are, case-insensitive: ASAN, UBSAN, TSAN. use ; to separate multiple sanitizer")
message(STATUS "ESL_USE_SANITIZER = ${ESL_USE_SANITIZER}")

set(ESL_SANITIZER_COMPILE_FLAGS "")
set(ESL_SANITIZER_LINK_FLAGS "")

if(ESL_USE_SANITIZER)
  if(MSVC)
    # See https://learn.microsoft.com/en-us/cpp/sanitizers/asan-known-issues?#incompatible-options
    string(REGEX REPLACE "/RTC[1scu]" "" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
    string(REGEX REPLACE "/RTC[1scu]" "" CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
    list(APPEND ESL_SANITIZER_LINK_FLAGS "/INCREMENTAL:NO")

    foreach(SANITIZER_ORIG IN LISTS ESL_USE_SANITIZER)
      string(TOUPPER "${SANITIZER_ORIG}" SANITIZER)

      if("${SANITIZER}" STREQUAL "ASAN")
        list(APPEND ESL_SANITIZER_COMPILE_FLAGS "/fsanitize=address")
      else()
        message(FATAL_ERROR "Unsupported sanitizer=${SANITIZER}")
      endif()
    endforeach()
  else()
    list(APPEND ESL_SANITIZER_COMPILE_FLAGS "-fno-omit-frame-pointer")

    foreach(SANITIZER_ORIG IN LISTS ESL_USE_SANITIZER)
      string(TOUPPER "${SANITIZER_ORIG}" SANITIZER)

      if("${SANITIZER}" STREQUAL "ASAN")
        list(APPEND ESL_SANITIZER_COMPILE_FLAGS "-fsanitize=address")
        list(APPEND ESL_SANITIZER_LINK_FLAGS "-fsanitize=address")
      elseif("${SANITIZER}" STREQUAL "UBSAN")
        list(APPEND ESL_SANITIZER_COMPILE_FLAGS "-fsanitize=undefined")
        list(APPEND ESL_SANITIZER_LINK_FLAGS "-fsanitize=undefined")
      elseif("${SANITIZER}" STREQUAL "TSAN")
        list(APPEND ESL_SANITIZER_COMPILE_FLAGS "-fsanitize=thread")
        list(APPEND ESL_SANITIZER_LINK_FLAGS "-fsanitize=thread")
      else()
        message(FATAL_ERROR "Unsupported sanitizer=${SANITIZER}")
      endif()
    endforeach()
  endif()
endif()

function(esl_apply_sanitizer TARGET)
  message(STATUS "Apply esl sanitizer for ${TARGET}")

  target_compile_options(${TARGET}
    PRIVATE
      ${ESL_SANITIZER_COMPILE_FLAGS}
  )

  target_link_options(${TARGET}
    PRIVATE
      ${ESL_SANITIZER_LINK_FLAGS}
  )
endfunction()
