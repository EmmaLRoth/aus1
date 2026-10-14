add_executable(semestral_project)

target_sources(semestral_project PRIVATE
    main.cpp
)

target_link_libraries(semestral_project PRIVATE ds heap-monitor)

set_target_properties(semestral_project PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)
