[English](./README.en.md) | **简体中文**
simple_ROBOCOPY_tool
一个围绕 Windows 内置工具 ROBOCOPY 的简单交互式封装程序，使用 C23 编写。

功能特性
交互式提示输入源路径和目标路径，并进行校验（存在性、非法字符、长度，以及目标不能是驱动器根目录）。

磁盘感知的 /MT：线程数同时根据物理 CPU 核心数和源/目标介质类型（NVMe / SATA SSD / HDD / 网络）来选择。

可选的平静打印模式（calm-print），会追加 /NFL /NDL /NJH /NJS /NP 以抑制逐文件输出。

在任意提示处输入 exit 或 Exit 即可退出。

命令模板
该工具构建并运行的命令大致为：

text
ROBOCOPY "<source>" "<destination>" /R:3 /W:1 /E /MT:<N> [calm-print flags]
标志	含义
/R:3 /W:1	重试 3 次，每次重试之间等待 1 秒
/E	复制子目录，包括空目录
/MT:<N>	多线程复制，N 由 Num_MT(cores, src, dst) 决定
/NFL /NDL /NJH /NJS /NP	仅在启用平静打印时使用
如果无法探测物理核心数（cores <= 0），工具会输出裸 /MT，这等价于 ROBOCOPY 的默认值 8 线程。

/MT 如何选择
Num_MT() 取源和目标介质中更保守的值，然后进行钳制：

介质	/MT
NVMe SSD	16
SATA SSD	8
HDD / 未知	4
网络（UNC 或映射驱动器）	8
然后所选值会被以下因素限制：

物理核心数（Num_CPU_Cores()，通过 GetLogicalProcessorInformationEx(RelationProcessorCore, ...) 获取），因此 /MT 永远不会超过真实核心数；

ROBOCOPY 的硬性上限 128。

如果介质探测失败，Num_MT() 会回退到 /MT:8（ROBOCOPY 的默认值）。

为什么使用“物理核心”而不是“逻辑线程”？
/MT 并行化的是传输阶段，而不是扫描/比较阶段。超过物理核心数进行超额订阅，对 I/O 密集型工具几乎没有好处，反而会增加上下文切换开销。在混合架构 CPU 上（例如 Intel Core i7-14650HX：8 个 P 核 + 8 个 E 核 = 16 核心 / 24 线程），Num_CPU_Cores() 返回 16，而不是 24。

使用方法
工具在每次提示时都会打印以下规则：

输入 exit 或 Exit 退出。

等待出现 Complete! 后再关闭窗口。

单个中文字符可能按 2 或 3 字节计入路径长度限制。

目标路径不能是裸驱动器号（例如 D:\）。

附加说明：

所有子目录都会被复制，包括空目录。

如果目标目录不存在，ROBOCOPY 会创建它。

路径长度限制为 DEFAULT_BLOCK_SIZE / 2 - 32 个字符（当前为 544）。

平静打印在启动时通过回答 n 到 Is Calm to Print (Y/y n)>> 来选择。

构建
编译器：WinLibs GCC 16.2.0（MinGW-w64，UCRT）

语言标准：C23（-std=c23 / -std=c2x）

编辑器：Visual Studio Code

最终可执行文件：cpytool.exe

依赖
模块	用途
allocer（allocer/allocer.h、allocer/allocer_base.h）	自定义块分配器
config（config/config.h）	构建时常量（DEFAULT_BUF_SIZE、DEFAULT_BLOCK_SIZE、SOURCE_DEFAULT 等）
help_tool（tools/help_tool/help_tool.h）	路径检查、Num_CPU_Cores、Num_MT、Int_Tran_Str 等
shlwapi（Windows）	路径辅助函数
stdio、stdlib、string、stdint、assert	C 标准库
Windows 版本要求
Windows 8 或更高版本。

程序使用 GetLogicalProcessorInformationEx（Windows 7+）以及受 _WIN32_WINNT >= 0x0601 保护的其他 API。对于 WinLibs / UCRT 构建，Windows 8 是安全基线。

版本
启动时打印的当前版本：0.0.3。

已知问题 / 待办事项
提示仍然写作 Diraction path>>（应为 Destination path>>）。

注意：规则中写着“ignore the NOEXITS target diraction”，但工具实际上会通过 /E 创建缺失的目标目录。应改写以反映真实行为。

许可证
（在此添加你的许可证。）
