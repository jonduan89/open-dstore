# AGENTS.md - DStore Database Storage Engine

This document provides essential information for agentic coding agents working on the DStore database storage engine.

## Build System

### Primary Build Commands

**Main Build:**
```bash
./build.sh                    # Build in release mode (default)
./build.sh -m debug          # Build with debug symbols
./build.sh -m release        # Optimized production build
./build.sh -m memcheck       # Build with AddressSanitizer
./build.sh -m coverage       # Build with code coverage
./build.sh -vb               # Verbose build output
```

**Testing:**
```bash
# Unit Tests (requires paths to dependencies)
./tests/build_and_run_ut.sh -t <third_lib_path> -u <utils_path> [options]
./tests/build_and_run_ut.sh -t /path/to/third_lib -u /path/to/utils -g UTWal*.* -r false

# TPCC Benchmark Tests  
./tests/build_and_run_tpcctest.sh -t <local_lib_path> -u <utils_path> [options]

# Single Test Execution (after build)
cd tmp_build/bin && ./unittest --gtest_filter="TestSuite.TestName"
```

**CMake Options:**
```bash
cmake .. -DENABLE_UT=ON          # Enable unit tests
cmake .. -DENABLE_TPCC=ON        # Enable TPCC benchmark
cmake .. -DENABLE_FUZZ=ON        # Enable fuzz testing
cmake .. -DDSTORE_USE_ASSERT_CHECKING=ON  # Enable runtime asserts
```

### Build Requirements
- **CMake:** Minimum version 3.16
- **Compiler:** GCC 7.3.0 (recommended)
- **Environment:** Set `LOCAL_LIB_PATH` and `UTILS_PATH` variables
- **Dependencies:** lz4, cjson, secure, gtest

## Code Style Guidelines

### Language Standards
- **Primary:** C++14 with extensive C compatibility
- **Standard:** All code must compile cleanly with extensive warning flags
- **Platform:** Linux x86_64/aarch64 with cross-compilation support

### Naming Conventions

**Classes:**
- `PascalCase` for class names
- Example: `WalManager`, `DstoreName`, `BufferManager`

**Functions:**
- `camelCase` for function names  
- Example: `getVersionStrFromGit`, `initWalManager`, `writeWalRecord`

**Variables:**
- `camelCase` with descriptive names
- Hungarian notation hints for data types
- Example: `memoryContext`, `walStream`, `fileDescriptor`

**Constants:**
- `SCREAMING_SNAKE_CASE` for constants and macros
- Example: `MAX_WAL_SIZE`, `DEFAULT_BUFFER_POOL_SIZE`

**File Names:**
- `snake_case` for all source files
- Example: `dstore_wal_utils.cpp`, `heap_insert_handler.cpp`

### Header Organization

**File Header Template:**
```cpp
/*
 * Copyright (C) 2026 Huawei Technologies Co.,Ltd.
 *
 * dstore is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * ---------------------------------------------------------------------------------------
 *
 * filename.h
 *
 * Description:
 * Brief description of file purpose and contents.
 * ---------------------------------------------------------------------------------------
 */
```

**Include Guards:**
```cpp
#ifndef FILENAME_H
#define FILENAME_H
// ... content ...
#endif  // FILENAME_H
```

### Documentation Standards

**Comment Blocks:**
- Use consistent comment block format shown above
- Include file purpose and key responsibilities
- Mark APIs with clear parameter/return documentation

**Function Documentation:**
```cpp
/*
 * Brief explanation of function purpose.
 *
 * @param param1 Description of first parameter
 * @param param2 Description of second parameter  
 * @return Description of return value
 * @note Any important notes about usage or limitations
 */
RetStatus FunctionName(Type1 param1, Type2 param2);
```

### Import and Organization

**Namespaces:**
- Use `DSTORE::` primary namespace
- Module-specific sub-namespaces where appropriate
- Example: `DSTORE::Wal`, `DSTORE::Transaction`, `DSTORE::Lock`

**Header Includes:**
- Group includes logically: system, external project, local
- Use forward declarations where possible to minimize dependencies
- Order: C library → C++ library → External project → Local project

### Error Handling

**Return Codes:**
- Use `RetStatus` enum for function returns
- Consistent error code mapping via error code modules
- Example: `SUCCESS`, `ERROR_INVALID_PARAMETER`, `ERROR_OUT_OF_MEMORY`

**Exception Safety:**
- Codebase avoids exceptions (C-style error handling)
- Always check return values from function calls
- Use proper cleanup RAII patterns

### Memory Management

**Memory Contexts:**
- Use `DstoreMemoryContext` for all memory allocations
- Track and manage memory contexts properly
- Avoid raw memory allocations outside designated managers

**Smart Pointers:**
- Use `std::unique_ptr` and `std::shared_ptr` where appropriate
- Follow RAII principles for resource management

### Concurrency and Threading

**Thread Safety:**
- All shared data requires proper locking
- Use appropriate lock types (spinlocks, mutexes)
- Follow lock ordering to prevent deadlocks

**Atomic Operations:**
- Use atomic primitives for simple counter/index operations
- Memory barriers where necessary for ordering guarantees

### Performance Guidelines

**Optimization:**
- Architecture-specific optimizations (x86_64, aarch64)
- Profile-guided optimization enabled for release builds
- Avoid premature optimization; focus on algorithmic improvements

**Memory Layout:**
- Cache-conscious data structures
- Avoid false sharing in multi-threaded code
- Use padding for alignment-sensitive operations

## Module Structure

```
src/
├── buffer/         # Buffer management and cache
├── catalog/        # Database catalog and metadata  
├── common/         # Common utilities and infrastructure
├── config/         # Configuration management
├── heap/           # Heap table implementation
├── index/          # Index structures and operations
├── lock/           # Lock management
├── transaction/    # Transaction processing (MVCC)
├── wal/            # Write-ahead logging
└── ...

interface/
├── common/         # Common utilities and types
├── heap/           # Heap table interfaces
├── transaction/    # Transaction interfaces
└── ...

tests/
├── unittest/       # Unit tests (Google Test)
├── tpcctest/       # TPCC benchmark
└── utilities/      # Test helpers
```

## Testing Framework

### Unit Tests
- **Framework:** Google Test/Google Mock
- **Test Binaries:** Built to `tmp_build/bin/unittest`
- **Running Tests:** `./unittest --gtest_filter="Pattern"`
- **Example:** `./unittest --gtest_filter="UTWal*.*"`

### Test Patterns
- Follow AAA pattern: Arrange, Act, Assert
- Use mock objects for external dependencies
- Test both success and error cases
- Include performance tests for critical paths

### Code Coverage
- Use LCOV for coverage analysis (`-m coverage` build)
- Target minimum 80% coverage for new features
- Critical paths should have comprehensive coverage

## Development Workflow

1. **Setup Environment:**
   ```bash
   export LOCAL_LIB_PATH=/path/to/libs
   export UTILS_PATH=/path/to/utils
   ```

2. **Build Project:**
   ```bash
   ./build.sh -m debug
   ```

3. **Run Tests:**
   ```bash
   ./tests/build_and_run_ut.sh -t $LOCAL_LIB_PATH -u $UTILS_PATH
   ```

4. **Verify Code Quality:**
   - Build completes without errors
   - All tests pass
   - No new warnings introduced

5. **Commit Changes:**
   - Follow conventional commit messages
   - Include appropriate test coverage
   - Update documentation for API changes

## Quality Assurance

### Static Analysis
- Extensive compiler warnings enabled (`-Wall -Werror`)
- AddressSanitizer for memory debugging (`-m memcheck`)
- ThreadSanitizer for data race detection
- Custom fault injection framework

### Security Hardening
- Stack protection enabled (`-fstack-protector`)
- Position-independent executables
- Relocation read-only (RELRO) protection
- No-executable stack flags

### Performance Validation
- Benchmarks critical code paths
- Profile with production-like workloads
- Monitor memory usage and allocation patterns

This document should be kept up-to-date as the codebase evolves.