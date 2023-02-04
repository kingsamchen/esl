
add_library(esl INTERFACE)

target_sources(esl
  PRIVATE
    byteswap.h
    file_util.h
    macros.h
    scope_guard.h
    strings.h
    unique_handle.h

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
  INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/../
)

target_compile_definitions(esl
  INTERFACE
    $<$<BOOL:${WIN32}>:
      _UNICODE
      UNICODE
      NOMINMAX
    >

    $<$<CONFIG:DEBUG>:
      _DEBUG
    >
)

get_target_property(esl_FILES esl SOURCES)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${esl_FILES})
