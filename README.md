# PrinciplesOfCompilerDesign-SHU
上海大学计算机工程与科学学院 《编译原理》课程实验

## 写在前面
我们完成了以下实验：
1. 实验一 识别标识符
2. 实验二 词法分析
3. 实验三 语法分析
4. 实验四 语义分析
5. 实验六 代码优化

实验三和实验四是基于LL(1)的预测分析表方法实现的，可以根据给定的LL(1)文法自动求出SELECT集并构造预测分析表（我因此经受了滕的长达两小时的拷打，但最终还是圆满完成了验收）。基于LL(1)的语义分析参考了 https://liuyehcf.github.io/2017/11/07/%E7%BC%96%E8%AF%91%E5%8E%9F%E7%90%86-%E8%AF%AD%E6%B3%95%E5%88%B6%E5%AF%BC%E7%BF%BB%E8%AF%912/

`main`分支下为英文注释，`chinese` 分支下为中文注释，可根据需要选择。

这些功能的实现方法可能并不优雅，欢迎提交 issue 或 pull requests 分享你的想法。

## 运行环境要求
### Linux
* g++-13
* CMake

### Windows
* MinGW-win64 (安装方法参考 https://code.visualstudio.com/docs/cpp/config-mingw)
* Microsoft Visual Studio 17 2022
* CMake

## 项目配置方法
### Linux
```bash
# 编译项目
bash scripts/build-linux.sh Release

# 运行实验 1~4
bash scripts/test-linuxX64-exp0x.sh casexx.pl0
# 运行实验 6
bash scripts/test-linuxX64-exp06.sh casexx.plq
```
运行输出的文本文件保存在 `tests/outputs/` 目录的对应实验下.

### Windows
```bash
# 编译项目
pwsh scripts\build-windows.ps1 Release

# 运行实验 1~4
bash scripts\test-windowsX64-exp0x.ps1 casexx.pl0
# 运行实验 6
bash scripts\test-windowsX64-exp06.ps1 casexx.plq
```
运行输出的文本文件保存在 `tests\outputs\` 目录的对应实验下.