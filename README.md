**Dstore is a self-contained, independently compilable and testable database storage engine component.**

> [中文](README_CN.md) | **English**

---

# Environment & Build

## Docker (Recommended)

The repository ships a `Dockerfile` that builds the complete toolchain (GCC 7.3, lz4, cjson, gtest, securec, mockcpp) inside an Ubuntu 20.04 image. The deploy script mounts your local source tree into the container so edits on the host are reflected immediately.

### Prerequisites

- Docker installed and running
- ~5 GB free disk space (image build compiles GCC from source)

### 1. Build the image and start the container

```bash
bash docker_deploy.sh           # build image (first run) then start container
bash docker_deploy.sh --build   # force rebuild the image
bash docker_deploy.sh --shell   # start container and open a shell
bash docker_deploy.sh --stop    # stop and remove the container
```

Your source directory is bind-mounted at `/opt/project/dstore` inside the container.

### 2. Enter the container

```bash
docker exec -it dstore-dev bash
```

### 3. Full build (inside the container)

```bash
# The dstore-build alias is pre-configured in /root/.bashrc:
dstore-build

# Or run the steps manually:
source ${BUILD_ROOT}/buildenv
cd ${BUILD_ROOT}/utils && bash build.sh -m debug
cd ${BUILD_ROOT}       && bash build.sh -m debug -tm ut
```

After a successful build:
- `utils/output/lib/libgsutils.so` — utility library
- `output/lib/libdstore.so` and `output/lib/libdstore.a` — storage engine

### 4. Incremental rebuild (inside the container)

After the first full build, use the `dstore-rebuild` alias for fast incremental compilation:

```bash
dstore-rebuild
# equivalent to: cd ${BUILD_ROOT}/tmp_build && make install -j$(nproc)
```

---

## IDE Integration (VS Code / CLion)

The `.devcontainer/devcontainer.json` configures VS Code Remote Containers and JetBrains Gateway to attach to the `dstore:latest` image with the source mounted automatically.

Open the repository folder in VS Code → **Reopen in Container**.

---

## Manual Setup (without Docker)

If you prefer a native build, install the dependencies below and update `buildenv`.

### 1. Environment Configuration

The following dependencies and recommended versions are required:

```
XXX/
├── dstore/ -------------------- # project root
└── local_libs/ ---------------- # dependency directory
    ├── buildtools/ ------------ # build tools
    │   └── gcc7.3/ ------------ # recommended: v7.3
    │       ├── gcc/
    │       ├── gmp/
    │       ├── isl/
    │       ├── mpc/
    │       └── mpfr/
    ├── secure/ ---------------- # recommended: v3.0.9
    │   ├── include/
    │   └── lib/
    ├── lz4/ ------------------- # recommended: v1.10.0
    │   ├── include/
    │   └── lib/
    ├── cjson/ ----------------- # recommended: v1.7.17
    │   ├── include/
    │   └── lib/
    └── gtest/ ----------------- # recommended: v1.10.0
        ├── include/
        └── lib/
```

#### 1.1 Configure dependencies

Download and compile each dependency at the specified version to avoid compatibility issues.

##### secure
**Download**: https://gitcode.com/opengauss/openGauss-third_party/tree/master/platform/Huawei_Secure_C
**Build**: `./build.sh -m all`

##### cjson
**Download**: https://gitcode.com/opengauss/openGauss-third_party/tree/master/dependency/cJSON
**Build**: `./build.sh -m all`

##### lz4
**Download**: https://github.com/lz4/lz4/releases
**Build**: `make -j$(nproc) && make install PREFIX=xxx/output`

##### gtest
**Download**: https://github.com/google/googletest/releases
**Build**:
```
mkdir output && cd output
cmake \
-DCMAKE_INSTALL_PREFIX=xxx/output \
-DCMAKE_BUILD_TYPE=Debug \
-DBUILD_SHARED_LIBS=OFF \
-DCMAKE_CXX_COMPILER=g++ \
-DCMAKE_C_COMPILER=gcc \
..
make -j$(nproc) && make install
```

After building, organize each library's output under its named directory. For example, `local_libs/secure` should contain `include/` and `lib/` subdirectories.

#### 1.2 Update and load the environment file

Set `BUILD_ROOT` in `dstore/buildenv` to the project path on your machine, e.g. `BUILD_ROOT=/opt/project/dstore`

```bash
source dstore/buildenv
```

### 2. Build

#### 2.1 Build the utils module first

```bash
cd dstore/utils
bash build.sh -m release   # or: debug
```

Verify that `libgsutils.so` is present under `utils/output/lib/`.

#### 2.2 Build dstore

```bash
cd dstore
bash build.sh -m release   # or: debug
```

After a successful build, `libdstore.so` and `libdstore.a` will be present under `dstore/output/lib/`.

---

# Test

## 1. Unit Tests

### 1.1 Build with unit tests enabled

```bash
cd dstore
bash build.sh -m debug -tm ut
```

The build produces a `unittest` binary under `tmp_build/bin/`.

### 1.2 Run tests

All targets are invoked from the `tmp_build/` directory:

```bash
cd ${BUILD_ROOT}/tmp_build
```

| Target | Coverage |
|---|---|
| `make run_dstore_ut_all` | All unit tests |
| `make run_dstore_buffer_unittest` | Buffer pool, page writer, dirty page queue |
| `make run_dstore_xact_unittest` | Transactions, CSN, undo (level0) |
| `make run_dstore_index_unittest` | B-tree index (level0) |
| `make run_dstore_lock_unittest` | Lock manager (level0) |
| `make run_dstore_ha_unittest` | WAL, replication, backup/restore, recovery |
| `make run_dstore_framework_unittest` | Memory context, common algorithms, task manager |
| `make run_dstore_datamanager_unittest` | Heap, tablespace, control file, VFS, catalog |

Example:

```bash
make run_dstore_buffer_unittest
```

### 1.3 Run with AddressSanitizer (ASan)

```bash
make run_dstore_ut_asan
```

---

## 2. TPCC Test

### 2.1 Build

```bash
cd dstore
bash build.sh -m release
```

### 2.2 Run tpcctest

```bash
cd ${BUILD_ROOT}/tmp_build
make run_dstore_tpcctest
```
