# c0mPiLer
A PL/0 compiler

## 要求
### Linux
* g++-13
* CMake

### Windows
* MinGW-win64 (安装方法参考 https://code.visualstudio.com/docs/cpp/config-mingw)
* Microsoft Visual Studio 17 2022
* CMake

## Linux 配置方法
```bash
# 编译项目
bash scripts/build-linux.sh Release

# 运行实验 1~4
bash scripts/test-linuxX64-exp0x.sh casexx.pl0

# 运行实验 6
bash scripts/test-linuxX64-exp06.sh casexx.plq
```
运行输出的文本文件保存在 `tests/outputs/` 目录的对应实验下.

## Windows 配置方法
```bash
# 编译项目
pwsh scripts\build-windows.ps1 Release

# 运行实验 1~4
bash scripts\test-windowsX64-exp0x.ps1 casexx.pl0

# 运行实验 6
bash scripts\test-windowsX64-exp06.ps1 casexx.plq
```
运行输出的文本文件保存在 `tests\outputs\` 目录的对应实验下.