# Header-only common utilities
add_library(plag_common INTERFACE)
target_include_directories(plag_common INTERFACE ${PROJECT_SOURCE_DIR}/src)
target_compile_features(plag_common INTERFACE cxx_std_17)
