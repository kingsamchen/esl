CPMAddPackage("gh:onqtam/doctest#v2.4.9")

add_executable(esl_test)

target_sources(esl_test
  PRIVATE
    test_main.cpp
    stringification.h

    byteswap_test.cpp
    scope_guard_test.cpp
    strings_join_test.cpp
    strings_match_test.cpp
    strings_split_test.cpp
    strings_trim_test.cpp
    unique_handle_test.cpp
)

target_link_libraries(esl_test
  PRIVATE
    esl
    doctest
)

esl_apply_common_compile_options(esl_test)

if(MSVC)
  if(ESL_USE_MSVC_PARALLEL_BUILD)
    esl_apply_msvc_parallel_build(esl_test)
  endif()
  if(ESL_USE_MSVC_STATIC_ANALYSIS)
    esl_apply_msvc_static_analysis(esl_test)
  endif()
else()
  if(ESL_USE_SANITIZER)
    esl_apply_sanitizer(esl_test)
  endif()
endif()

get_target_property(test_FILES esl_test SOURCES)
source_group("esl_test" FILES ${test_FILES})

add_test(NAME tests COMMAND esl_test)
