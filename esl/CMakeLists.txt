
add_library(esl INTERFACE)

add_library(esl::esl ALIAS esl)

target_sources(esl
  PRIVATE
    byteswap.h
    file_util.h
    ignore_unused.h
    macros.h
    scope_guard.h
    strings.h
    unique_handle.h
    utility.h

    detail/files.h
    detail/secure_crt.h
    detail/strings_join.h
    detail/strings_match.h
    detail/strings_split.h

    $<$<BOOL:${WIN32}>:
    >

    $<$<NOT:$<BOOL:${WIN32}>>:
    >
)

target_include_directories(esl
  INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

esl_common_compile_configs(esl)

get_target_property(esl_FILES esl SOURCES)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${esl_FILES})
