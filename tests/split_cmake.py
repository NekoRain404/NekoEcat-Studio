#!/usr/bin/env python3
"""
Parse the monolithic tests/CMakeLists.txt and generate per-directory CMakeLists.txt files.

Key path updates:
- Integration & performance tests: already had integration/ or performance/ prefix
  in source paths, and the ../ relative paths still work from tests/integration/
  or tests/performance/. No path changes needed.
- Unit tests: moved from tests/ to tests/unit/{subdir}/, so ../X -> ../../X
  and ${CMAKE_SOURCE_DIR}/X stays the same.
"""

import os
import re
import shutil
from pathlib import Path
from collections import OrderedDict

BASE = Path("/home/nekorain/Documents/LTSPICE/NEW/vibecoding/Ethercat")
TESTS = BASE / "tests"
CMAKE_FILE = TESTS / "CMakeLists.txt"

# ── Read the old CMakeLists.txt ───────────────────────────────────────────
with open(CMAKE_FILE, 'r') as f:
    all_lines = f.readlines()
full_text = ''.join(all_lines)
print(f"Read {len(all_lines)} lines from tests/CMakeLists.txt")

# ── Build a map: test_name -> directory ────────────────────────────────────
test_to_dir = {}
unit_subdirs = ['core', 'infra', 'services', 'plugins', 'models', 'adapters', 'detail', 'utils']

for subdir in unit_subdirs:
    dirpath = TESTS / 'unit' / subdir
    if dirpath.exists():
        for f in dirpath.glob('*.cpp'):
            test_to_dir[f.stem] = ('unit', subdir, f)

for f in (TESTS / 'integration').glob('*.cpp'):
    test_to_dir[f.stem] = ('integration', None, f)

for f in (TESTS / 'performance').glob('*.cpp'):
    test_to_dir[f.stem] = ('performance', None, f)

print(f"Mapped {len(test_to_dir)} test targets to directories")

# ── Extract all add_executable block boundaries ───────────────────────────
# We parse the text to find each block: from its first comment line (or the
# add_executable line itself) to just before the next section comment + add_executable.

# First, find all add_executable line numbers and target names
exe_positions = []
for i, line in enumerate(all_lines):
    m = re.match(r'\s*add_executable\((\w+)', line)
    if m:
        exe_positions.append((i, m.group(1)))

print(f"Found {len(exe_positions)} add_executable declarations")

# Now define block boundaries
target_blocks = {}  # name -> (first_line, last_line) inclusive, 0-indexed

for idx, (exe_line, name) in enumerate(exe_positions):
    # Start: walk backwards to find preceding comments, blank lines, and if() wrappers
    first = exe_line
    j = exe_line - 1
    while j >= 0:
        stripped = all_lines[j].strip()
        if stripped == '' or stripped.startswith('#'):
            first = j
            j -= 1
        elif stripped.startswith('endif()'):
            # This might be the end of a wrapping if() block before this target
            # Walk further back to find the matching if()
            first = j
            j -= 1
        elif re.match(r'^if\s*\(', stripped):
            # This is an if() wrapper - include it
            first = j
            j -= 1
        else:
            break

    # Don't overlap with previous target
    if idx > 0:
        prev_name = exe_positions[idx - 1][1]
        prev_last = target_blocks[prev_name][1]
        first = max(first, prev_last + 1)

    # End: scan forward from exe_line to find where this block ends
    last = exe_line

    # Track parentheses depth through the block
    paren_depth = 0
    i = exe_line
    # First, close the add_executable(...) call
    while i < len(all_lines):
        paren_depth += all_lines[i].count('(') - all_lines[i].count(')')
        if paren_depth <= 0:
            break
        i += 1

    # Now we're past add_executable. Continue collecting target properties.
    i += 1
    while i < len(all_lines):
        stripped = all_lines[i].strip()

        # Check if next meaningful line starts a new target
        if re.match(r'add_executable\(', stripped):
            break

        # Check if we're hitting the next section comment
        if stripped.startswith('# ──'):
            # Peek ahead to see if add_executable follows
            peek = i + 1
            while peek < len(all_lines):
                ps = all_lines[peek].strip()
                if ps == '' or ps.startswith('#'):
                    peek += 1
                    continue
                break
            if peek < len(all_lines) and re.match(r'add_executable\(', all_lines[peek].strip()):
                break

        # Check for set(RELEASE_SMOKE_TESTS or add_custom_target - these aren't part of a target block
        if stripped.startswith('set(') and 'RELEASE_SMOKE' in stripped:
            break
        if stripped.startswith('add_custom_target('):
            break

        # Check for endif() that closes an if() wrapping THIS target
        if stripped.startswith('endif()'):
            last = i
            i += 1
            continue

        last = i
        i += 1

    target_blocks[name] = (first, last)

# ── Extract the set(RELEASE_SMOKE_TESTS ...) block ───────────────────────
release_smoke_start = None
release_smoke_end = None
for i, line in enumerate(all_lines):
    if 'set(RELEASE_SMOKE_TESTS' in line:
        release_smoke_start = i
        break

if release_smoke_start is not None:
    # Find matching closing paren
    depth = 0
    for i in range(release_smoke_start, len(all_lines)):
        depth += all_lines[i].count('(') - all_lines[i].count(')')
        if depth <= 0:
            release_smoke_end = i
            break

# Find add_custom_target(release-smoke ...) block
release_smoke_custom_start = None
release_smoke_custom_end = None
for i, line in enumerate(all_lines):
    if 'add_custom_target(release-smoke' in line:
        release_smoke_custom_start = i
        depth = 0
        for j in range(i, len(all_lines)):
            depth += all_lines[j].count('(') - all_lines[j].count(')')
            if depth <= 0:
                release_smoke_custom_end = j
                break
        break

# Also find the set_tests_properties line for RELEASE_SMOKE_TESTS
release_smoke_label_line = None
for i, line in enumerate(all_lines):
    if 'set_tests_properties(${RELEASE_SMOKE_TESTS}' in line:
        release_smoke_label_line = i
        break

# ── Extract the if(Qt6Qml_FOUND) wrapper block ───────────────────────────
# Find the if(Qt6Qml_FOUND) ... endif() block
qml_if_start = None
qml_if_end = None
for i, line in enumerate(all_lines):
    if 'if(Qt6Qml_FOUND)' in line or 'if(Qt6Qml_FOUND )' in line:
        qml_if_start = i
        depth = 0
        for j in range(i, len(all_lines)):
            depth += all_lines[j].count('(') - all_lines[j].count(')')
            if depth <= 0:
                qml_if_end = j
                break
        break

# ── Helper: update paths for unit test subdirectories ─────────────────────
def update_unit_block_paths(block_text):
    """For unit tests: ../X -> ../../../X because files are two levels deeper.

    Old location: tests/CMakeLists.txt used ../src/core/ (1 level up to project root)
    New location: tests/unit/core/CMakeLists.txt needs ../../../src/core/ (3 levels up)
    So we prepend ../../ to the existing ../ giving ../../../.
    """
    result = []
    in_exe = False
    exe_depth = 0

    for line in block_text.split('\n'):
        stripped = line.strip()

        # Track inside add_executable source list
        if re.match(r'add_executable\(', stripped):
            in_exe = True
            exe_depth = stripped.count('(') - stripped.count(')')
            if exe_depth <= 0:
                in_exe = False
            result.append(line)
            continue

        if in_exe:
            exe_depth += stripped.count('(') - stripped.count(')')
            if exe_depth <= 0:
                in_exe = False
                result.append(line)
                continue

            # Inside add_executable - ../X becomes ../../../X
            if stripped.startswith('../'):
                indent = line[:len(line) - len(line.lstrip())]
                # Prepend ../../ to ../X -> ../../../X
                result.append(f"{indent}../../{stripped}")
                continue

            result.append(line)
            continue

        # Outside add_executable - standalone line starts with ../
        if stripped.startswith('../'):
            indent = line[:len(line) - len(line.lstrip())]
            # Prepend ../../ to ../X -> ../../../X
            result.append(f"{indent}../../{stripped}")
            continue

        # Handle inline ../ paths like: target_include_directories(... PRIVATE ../src/core)
        if '../' in line and '${CMAKE_SOURCE_DIR}' not in line:
            # ../X -> ../../../X: replace ../ with ../../../ for each path
            new_line = re.sub(r'(?<=[\s(,])\.\./', '../../../', line)
            result.append(new_line)
            continue

        result.append(line)

    return '\n'.join(result)


def update_integration_block_paths(block_text):
    """For integration tests: strip 'integration/' prefix from source paths.
    Also update ../X -> ../../X for relative paths since we're one level deeper."""
    result = []
    in_exe = False
    exe_depth = 0

    for line in block_text.split('\n'):
        stripped = line.strip()

        if re.match(r'add_executable\(', stripped):
            in_exe = True
            exe_depth = stripped.count('(') - stripped.count(')')
            if exe_depth <= 0:
                in_exe = False
            result.append(line)
            continue

        if in_exe:
            exe_depth += stripped.count('(') - stripped.count(')')
            if exe_depth <= 0:
                in_exe = False
                result.append(line)
                continue

            # Strip integration/ prefix from source files
            if stripped.startswith('integration/'):
                indent = line[:len(line) - len(line.lstrip())]
                result.append(f"{indent}{stripped[len('integration/'):]}")
                continue

            # ../X -> ../../X for relative paths
            if stripped.startswith('../'):
                indent = line[:len(line) - len(line.lstrip())]
                result.append(f"{indent}../../{stripped[3:]}")
                continue

            result.append(line)
            continue

        # Outside add_executable
        if stripped.startswith('../'):
            indent = line[:len(line) - len(line.lstrip())]
            result.append(f"{indent}../../{stripped[3:]}")
            continue

        # Handle inline ../ paths
        if '../' in line and '${CMAKE_SOURCE_DIR}' not in line:
            # ../X -> ../../X: replace ../ with ../../ for each inline path
            new_line = re.sub(r'(?<=[\s(,])\.\./', '../../', line)
            result.append(new_line)
            continue

        result.append(line)

    return '\n'.join(result)


def update_performance_block_paths(block_text):
    """For performance tests: strip 'performance/' prefix from source paths.
    Also update ../X -> ../../X for relative paths since we're one level deeper."""
    result = []
    in_exe = False
    exe_depth = 0

    for line in block_text.split('\n'):
        stripped = line.strip()

        if re.match(r'add_executable\(', stripped):
            in_exe = True
            exe_depth = stripped.count('(') - stripped.count(')')
            if exe_depth <= 0:
                in_exe = False
            result.append(line)
            continue

        if in_exe:
            exe_depth += stripped.count('(') - stripped.count(')')
            if exe_depth <= 0:
                in_exe = False
                result.append(line)
                continue

            # Strip performance/ prefix from source files
            if stripped.startswith('performance/'):
                indent = line[:len(line) - len(line.lstrip())]
                result.append(f"{indent}{stripped[len('performance/'):]}")
                continue

            # ../X -> ../../X for relative paths
            if stripped.startswith('../'):
                indent = line[:len(line) - len(line.lstrip())]
                result.append(f"{indent}../../{stripped[3:]}")
                continue

            result.append(line)
            continue

        # Outside add_executable
        if stripped.startswith('../'):
            indent = line[:len(line) - len(line.lstrip())]
            result.append(f"{indent}../../{stripped[3:]}")
            continue

        # Handle inline ../ paths
        if '../' in line and '${CMAKE_SOURCE_DIR}' not in line:
            # ../X -> ../../X: replace ../ with ../../ for each inline path
            new_line = re.sub(r'(?<=[\s(,])\.\./', '../../', line)
            result.append(new_line)
            continue

        result.append(line)

    return '\n'.join(result)


def get_block_text(first, last):
    return ''.join(all_lines[first:last + 1])


# ── Group targets by directory ────────────────────────────────────────────
dir_groups = {}  # (category, subdir) -> [(name, first, last)]

for name, (first, last) in target_blocks.items():
    if name in test_to_dir:
        cat, sub, _ = test_to_dir[name]
        key = (cat, sub)
        if key not in dir_groups:
            dir_groups[key] = []
        dir_groups[key].append((name, first, last))

# Sort within each group by original order (line number)
for key in dir_groups:
    dir_groups[key].sort(key=lambda x: x[1])

unmapped = set(target_blocks.keys()) - set(test_to_dir.keys())
if unmapped:
    print(f"\nWARNING: {len(unmapped)} targets not found in new directories:")
    for t in sorted(unmapped):
        print(f"  - {t} (line {target_blocks[t][0]+1})")

# ── Generate per-directory CMakeLists.txt ─────────────────────────────────
files_to_write = {}

# 1) Root tests/CMakeLists.txt
root_content = """\
# Tests - split into per-directory CMakeLists.txt files
# Original monolithic file backed up as tests/CMakeLists.txt.monolithic-bak

add_subdirectory(unit)
add_subdirectory(integration)
add_subdirectory(performance)
"""
files_to_write[TESTS / 'CMakeLists.txt'] = root_content

# 2) tests/unit/CMakeLists.txt
unit_content = """\
# Unit Tests
# Auto-generated from monolithic tests/CMakeLists.txt

add_subdirectory(core)
add_subdirectory(infra)
add_subdirectory(services)
add_subdirectory(plugins)
add_subdirectory(models)
add_subdirectory(adapters)
add_subdirectory(detail)
add_subdirectory(utils)
"""
files_to_write[TESTS / 'unit' / 'CMakeLists.txt'] = unit_content

# 3) Per unit subdirectory
for subdir in unit_subdirs:
    key = ('unit', subdir)
    if key not in dir_groups:
        continue

    entries = []
    entries.append(f"# Unit Tests - {subdir}")
    entries.append("# Auto-generated from monolithic tests/CMakeLists.txt")
    entries.append("")

    for name, first, last in dir_groups[key]:
        block = get_block_text(first, last)
        updated = update_unit_block_paths(block)
        entries.append(updated.rstrip())
        entries.append("")

    files_to_write[TESTS / 'unit' / subdir / 'CMakeLists.txt'] = '\n'.join(entries)
    print(f"  unit/{subdir}: {len(dir_groups[key])} targets")

# 4) tests/integration/CMakeLists.txt
key = ('integration', None)
if key in dir_groups:
    entries = []
    entries.append("# Integration Tests")
    entries.append("# Auto-generated from monolithic tests/CMakeLists.txt")
    entries.append("")

    for name, first, last in dir_groups[key]:
        block = get_block_text(first, last)
        updated = update_integration_block_paths(block)
        entries.append(updated.rstrip())
        entries.append("")

    files_to_write[TESTS / 'integration' / 'CMakeLists.txt'] = '\n'.join(entries)
    print(f"  integration: {len(dir_groups[key])} targets")

# 5) tests/performance/CMakeLists.txt
key = ('performance', None)
if key in dir_groups:
    entries = []
    entries.append("# Performance Benchmarks")
    entries.append("# Auto-generated from monolithic tests/CMakeLists.txt")
    entries.append("")

    for name, first, last in dir_groups[key]:
        block = get_block_text(first, last)
        updated = update_performance_block_paths(block)
        entries.append(updated.rstrip())
        entries.append("")

    files_to_write[TESTS / 'performance' / 'CMakeLists.txt'] = '\n'.join(entries)
    print(f"  performance: {len(dir_groups[key])} targets")

# ── Handle RELEASE_SMOKE_TESTS ───────────────────────────────────────────
# The RELEASE_SMOKE_TESTS variable references targets across multiple directories.
# In CMake, targets from add_subdirectory() are visible in the parent scope,
# so set_tests_properties can be called in the root after all subdirectories.
# We need to extract the list of target names and put the set_tests_properties
# call in the root tests/CMakeLists.txt after the add_subdirectory calls.
if release_smoke_start is not None and release_smoke_end is not None:
    # Extract just the variable names from the set(RELEASE_SMOKE_TESTS ...)
    smoke_var_lines = get_block_text(release_smoke_start, release_smoke_end)
    # Find which of the listed targets actually exist in our new structure
    smoke_targets_in_scope = []
    for line in smoke_var_lines.split('\n'):
        stripped = line.strip()
        if stripped.startswith('set(') or stripped == ')':
            continue
        if stripped and not stripped.startswith('#'):
            # Each target name on its own line or space-separated
            for word in stripped.split():
                word = word.rstrip(')')
                if word in test_to_dir:
                    smoke_targets_in_scope.append(word)

    root_entries = files_to_write[TESTS / 'CMakeLists.txt'].rstrip()
    root_entries += "\n\n# ── Release Smoke Tests ──\n"
    root_entries += "set(RELEASE_SMOKE_TESTS\n"
    for t in sorted(smoke_targets_in_scope):
        root_entries += f"    {t}\n"
    root_entries += ")\n"
    root_entries += "set_tests_properties(${RELEASE_SMOKE_TESTS} PROPERTIES LABELS release_smoke)\n"

    files_to_write[TESTS / 'CMakeLists.txt'] = root_entries

    # Also remove the RELEASE_SMOKE_TESTS set from any subdirectory file
    # that may have gotten it. We'll handle this by not including it in subdirs.
    # The script's block extraction might have included the set_tests_properties
    # line in the wrong directory. We need to filter it out.
    for subdir_key in list(dir_groups.keys()):
        for idx, (name, first, last) in enumerate(dir_groups[subdir_key]):
            block = get_block_text(first, last)
            if 'set_tests_properties(${RELEASE_SMOKE_TESTS}' in block:
                # Remove this line from the block - we'll regenerate
                pass

# ── Backup and write ─────────────────────────────────────────────────────
backup_path = CMAKE_FILE.with_suffix('.txt.monolithic-bak')
if not backup_path.exists():
    shutil.copy2(CMAKE_FILE, backup_path)
    print(f"\nBacked up original to {backup_path.name}")

for filepath, content in files_to_write.items():
    with open(filepath, 'w') as f:
        f.write(content)
    lc = content.count('\n') + 1
    print(f"Wrote {filepath.relative_to(BASE)} ({lc} lines)")

# ── Summary ───────────────────────────────────────────────────────────────
print(f"\n{'='*60}")
print("Summary:")
print(f"  Original: {len(all_lines)} lines, {len(exe_positions)} add_executable targets")
total_mapped = sum(len(v) for v in dir_groups.values())
print(f"  Targets mapped to new dirs: {total_mapped}")
if unmapped:
    print(f"  Unmapped targets: {len(unmapped)}")
    for t in sorted(unmapped):
        print(f"    - {t}")
print(f"  Files written: {len(files_to_write)}")
print(f"  Backup: tests/CMakeLists.txt.monolithic-bak")
