# src/common/ -- shared header-only tools owned by Anas, FROZEN after Week 1.
# Header-only, but an INTERFACE target so every area can link it and inherit
# the include path. Adding a header here never edits a shared build file.
add_library(plag_common INTERFACE)
target_include_directories(plag_common INTERFACE ${PROJECT_SOURCE_DIR}/src)
target_compile_features(plag_common INTERFACE cxx_std_17)
