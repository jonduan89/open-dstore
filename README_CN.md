**Dstore是一个可独立编译，独立测试的数据库存储引擎组件。**

> **中文** | [English](README.md)

---

# 环境配置与编译

## Docker（推荐）

项目提供了一个 `Dockerfile`，可在 Ubuntu 20.04 镜像中构建完整工具链（GCC 7.3、lz4、cjson、gtest、securec、mockcpp）。部署脚本将本地源码目录挂载到容器中，在宿主机上的修改可立即在容器内生效。

### 前置条件

- 已安装并运行 Docker
- 约 5 GB 可用磁盘空间（首次构建需从源码编译 GCC）

### 1. 构建镜像并启动容器

```bash
bash docker_deploy.sh           # 首次运行：构建镜像并启动容器
bash docker_deploy.sh --build   # 强制重新构建镜像
bash docker_deploy.sh --shell   # 启动容器并进入 shell
bash docker_deploy.sh --stop    # 停止并删除容器
```

源码目录将以绑定挂载的方式映射到容器内的 `/opt/project/dstore`。

### 2. 进入容器

```bash
docker exec -it dstore-dev bash
```

### 3. 完整构建（容器内）

```bash
# /root/.bashrc 中已预配置 dstore-build 别名：
dstore-build

# 或手动执行以下步骤：
source ${BUILD_ROOT}/buildenv
cd ${BUILD_ROOT}/utils && bash build.sh -m debug
cd ${BUILD_ROOT}       && bash build.sh -m debug -tm ut
```

构建成功后的产物：
- `utils/output/lib/libgsutils.so` — 工具库
- `output/lib/libdstore.so` 和 `output/lib/libdstore.a` — 存储引擎

### 4. 增量编译（容器内）

首次完整构建完成后，使用 `dstore-rebuild` 别名进行快速增量编译：

```bash
dstore-rebuild
# 等价于：cd ${BUILD_ROOT}/tmp_build && make install -j$(nproc)
```

---

## IDE 集成（VS Code / CLion）

`.devcontainer/devcontainer.json` 已为 VS Code Remote Containers 和 JetBrains Gateway 配置好，可自动挂载源码并连接到 `dstore:latest` 镜像。

在 VS Code 中打开项目目录，选择 **Reopen in Container** 即可。

---

## 手动配置（不使用 Docker）

如果希望在宿主机上直接构建，请按照以下步骤安装依赖并更新 `buildenv`。

### 1. 环境配置

如下所示为编译 Dstore 所需依赖库及建议版本：

```
XXX/
├── dstore/ -------------------- # 项目入口
└── local_libs/ ---------------- # 依赖库目录
    ├── buildtools/ ------------ # 编译工具
    │   └── gcc7.3/ ------------ # 建议版本：v7.3
    │       ├── gcc/
    │       ├── gmp/
    │       ├── isl/
    │       ├── mpc/
    │       └── mpfr/
    ├── secure/ ---------------- # 建议版本：v3.0.9
    │   ├── include/
    │   └── lib/
    ├── lz4/ ------------------- # 建议版本：v1.10.0
    │   ├── include/
    │   └── lib/
    ├── cjson/ ----------------- # 建议版本：v1.7.17
    │   ├── include/
    │   └── lib/
    └── gtest/ ----------------- # 建议版本：v1.10.0
        ├── include/
        └── lib/
```

#### 1.1 配置依赖库

请按照上述指定版本下载并编译各依赖库，以避免版本不一致引发的兼容性问题。

##### secure
**下载地址**：https://gitcode.com/opengauss/openGauss-third_party/tree/master/platform/Huawei_Secure_C
**编译命令**：`./build.sh -m all`

##### cjson
**下载地址**：https://gitcode.com/opengauss/openGauss-third_party/tree/master/dependency/cJSON
**编译命令**：`./build.sh -m all`

##### lz4
**下载地址**：https://github.com/lz4/lz4/releases
**编译命令**：`make -j$(nproc) && make install PREFIX=xxx/output`

##### gtest
**下载地址**：https://github.com/google/googletest/releases
**编译命令**：
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

依赖库编译完成后，请按库名称调整目录结构。例如，`local_libs/secure` 下应包含对应的 `include` 和 `lib` 子目录。

#### 1.2 更新并加载环境配置文件

修改 `dstore/buildenv` 中 `BUILD_ROOT` 的值为当前项目路径，例如：`BUILD_ROOT=/opt/project/dstore`

```bash
source dstore/buildenv
```

### 2. 编译

#### 2.1 编译前置 utils 模块

```bash
cd dstore/utils
bash build.sh -m release   # 或：debug
```

确认 `utils/output/lib/` 路径下有 `libgsutils.so` 生成。

#### 2.2 编译 dstore

```bash
cd dstore
bash build.sh -m release   # 或：debug
```

编译成功后，`dstore/output/lib/` 路径下应有 `libdstore.so` 与 `libdstore.a` 生成。

---

# 测试

## 1. 单元测试

### 1.1 编译单元测试

```bash
cd dstore
bash build.sh -m debug -tm ut
```

构建完成后，`tmp_build/bin/` 路径下会生成 `unittest` 可执行文件。

### 1.2 运行测试

所有测试目标均在 `tmp_build/` 目录下执行：

```bash
cd ${BUILD_ROOT}/tmp_build
```

| 目标 | 覆盖范围 |
|---|---|
| `make run_dstore_ut_all` | 所有单元测试 |
| `make run_dstore_buffer_unittest` | 缓冲池、页面写入器、脏页队列 |
| `make run_dstore_xact_unittest` | 事务、CSN、Undo（level0）|
| `make run_dstore_index_unittest` | B-tree 索引（level0）|
| `make run_dstore_lock_unittest` | 锁管理器（level0）|
| `make run_dstore_ha_unittest` | WAL、副本、备份恢复、故障恢复 |
| `make run_dstore_framework_unittest` | 内存上下文、公共算法、任务管理器 |
| `make run_dstore_datamanager_unittest` | Heap、表空间、控制文件、VFS、系统表 |

示例：

```bash
make run_dstore_buffer_unittest
```

### 1.3 使用 AddressSanitizer（ASan）运行

```bash
make run_dstore_ut_asan
```

---

## 2. TPCC 测试

### 2.1 编译

```bash
cd dstore
bash build.sh -m release
```

### 2.2 运行 tpcctest

```bash
cd ${BUILD_ROOT}/tmp_build
make run_dstore_tpcctest
```
