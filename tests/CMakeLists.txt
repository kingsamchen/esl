CPMAddPackage("gh:onqtam/doctest#v2.4.11")

add_executable(esl_test)

target_sources(esl_test
  PRIVATE
    test_main.cpp

    stringification.h
    test_util.h

    byteswap_test.cpp
    file_util_test.cpp
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

if(ESL_RUN_CLANG_TIDY_ON_BUILD)
  esl_apply_clang_tidy_on_build(esl_test)
endif()

if(ESL_USE_SANITIZER)
  esl_apply_sanitizer(esl_test)
endif()

if(MSVC)
  if(ESL_USE_MSVC_PARALLEL_BUILD)
    esl_apply_msvc_parallel_build(esl_test)
  endif()
endif()

get_target_property(test_FILES esl_test SOURCES)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${test_FILES})

add_test(NAME tests COMMAND esl_test)
