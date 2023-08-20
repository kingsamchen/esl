
include(CMakePackageConfigHelpers)

set(ESL_INSTALL_CMAKE_DIR ${CMAKE_INSTALL_LIBDIR}/cmake/esl CACHE STRING
  "Installation directory for the library, in relative path")

configure_package_config_file(
  ${ESL_CMAKE_DIR}/esl-config.cmake.in
  ${PROJECT_BINARY_DIR}/esl-config.cmake
  INSTALL_DESTINATION ${ESL_INSTALL_CMAKE_DIR}
)

install(TARGETS esl
  EXPORT esl-targets
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(EXPORT esl-targets
  NAMESPACE esl::
  DESTINATION ${ESL_INSTALL_CMAKE_DIR}
)

if(WIN32)
  set(ESL_HEADER_EXCLUDED "(_linux\\.h)$")
else()
  set(ESL_HEADER_EXCLUDED "(_win\\.h)$")
endif()

# Install headers while keeping folder structure.
install(DIRECTORY
  ${ESL_DIR}/esl/
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/esl
  FILES_MATCHING PATTERN "*.h"
  PATTERN REGEX ${ESL_HEADER_EXCLUDED} EXCLUDE
)

install(FILES
  ${PROJECT_BINARY_DIR}/esl-config.cmake
  DESTINATION ${ESL_INSTALL_CMAKE_DIR}
)
