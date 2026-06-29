# Tests - split into per-directory CMakeLists.txt files
# Original monolithic file backed up as tests/CMakeLists.txt.monolithic-bak

add_subdirectory(unit)
add_subdirectory(integration)
add_subdirectory(performance)

# ── Release Smoke Tests (labels must be set in subdirectories) ──
# Note: Test targets are defined in subdirectory CMakeLists.txt files.
# The RELEASE_SMOKE_TESTS variable and labels must be set where targets are defined.
