
add_library(esl INTERFACE)

target_sources(esl
  PRIVATE

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
