cmake_minimum_required(VERSION 3.5)

project(teg LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(${PROJECT_NAME}
  src/main.cpp
  src/TEG.h src/TEG.cpp
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

install(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

  target_link_libraries(${PROJECT_NAME} ncurses)

  add_custom_target(ProjectRelatedFiles SOURCES
      .astylerc
  )
