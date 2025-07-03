include(${ESL_CMAKE_DIR}/utils.cmake)

option(ESL_USE_MSVC_PARALLEL_BUILD "If enabled, build multiple files in parallel." ON)
option(ESL_USE_WIN32_LEAN_AND_MEAN "If enabled, define WIN32_LEAN_AND_MEAN" ON)

if(ESL_NOT_SUBPROJECT)
  message(STATUS "esl compiler MSVC global conf is in active")

  # Force generating debugging symbols in Release build.
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Zi")
  set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /DEBUG /OPT:REF /OPT:ICF")
endif()

message(STATUS "ESL_USE_MSVC_PARALLEL_BUILD = ${ESL_USE_MSVC_PARALLEL_BUILD}")
message(STATUS "ESL_USE_WIN32_LEAN_AND_MEAN = ${ESL_USE_WIN32_LEAN_AND_MEAN}")

function(esl_common_compile_configs TARGET)
  get_target_type(${TARGET} TARGET_TYPE)

  if(NOT TARGET_TYPE STREQUAL "INTERFACE_LIBRARY")
    target_compile_definitions(${TARGET}
      PUBLIC
        _UNICODE
        UNICODE
        NOMINMAX

        $<$<CONFIG:DEBUG>:
          _DEBUG
        >

        $<$<BOOL:ESL_USE_WIN32_LEAN_AND_MEAN>:WIN32_LEAN_AND_MEAN>
    )

    target_compile_options(${TARGET}
      PRIVATE
        /W4
        /wd4819 # source characters not in current code page.

        /Zc:inline            # Have the compiler eliminate unreferenced COMDAT functions and data before emitting the object file.
        /Zc:rvalueCast        # Enforce the standard rules for explicit type conversion.
        /Zc:strictStrings     # Don't allow conversion from a string literal to mutable characters.
        /Zc:threadSafeInit    # Enable thread-safe function-local statics initialization.

        /permissive-  # Be mean, don't allow bad non-standard stuff (C++/CLI, __declspec, etc. are all left intact).

        $<$<BOOL:ESL_USE_MSVC_PARALLEL_BUILD>:/MP>
    )
  else()
    target_compile_definitions(esl
      INTERFACE
        _UNICODE
        UNICODE
        NOMINMAX

        $<$<CONFIG:DEBUG>:
          _DEBUG
        >

        $<$<BOOL:ESL_USE_WIN32_LEAN_AND_MEAN>:WIN32_LEAN_AND_MEAN>
    )
  endif()
endfunction()
