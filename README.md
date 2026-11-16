**English**|[简体中文](./README.zh-CN.md)
# simple_ROBOCOPY_tool

A simple interactive wrapper around the Windows built-in tool **ROBOCOPY**, written in C23.

## Features

- Interactive prompts for source and destination paths, with validation
  (existence, illegal characters, length, and destination-not-a-drive-root).
- **Disk-aware `/MT`**: the thread count is chosen from both the **physical
  CPU core count** and the **media type** of the source / destination
  (NVMe / SATA SSD / HDD / network).
- Optional **calm-print** mode, which appends `/NFL /NDL /NJH /NJS /NP` to
  suppress per-file output.
- Exit at any prompt by typing `exit` or `Exit`.

## Command Template

The tool builds and runs approximately:

```
ROBOCOPY "<source>" "<destination>" /R:3 /W:1 /E /MT:<N> [calm-print flags]
```

| Flag | Meaning |
|------|---------|
| `/R:3 /W:1` | Retry 3 times, wait 1 second between retries |
| `/E` | Copy subdirectories, including empty ones |
| `/MT:<N>` | Multi-threaded copy, `N` decided by `Num_MT(cores, src, dst)` |
| `/NFL /NDL /NJH /NJS /NP` | Only when **calm-print** is enabled |

If the physical core count cannot be probed (`cores <= 0`), the tool emits a
bare `/MT`, which is equivalent to ROBOCOPY's default of 8 threads.

### How `/MT` is chosen

`Num_MT()` takes the **more conservative** value of the source and destination
media, then clamps the result:

| Media | `/MT` |
|-------|-------|
| NVMe SSD | 16 |
| SATA SSD | 8 |
| HDD / unknown | 4 |
| Network (UNC or mapped drive) | 8 |

The chosen value is then capped by:

- the **physical core count** (`Num_CPU_Cores()`, via
  `GetLogicalProcessorInformationEx(RelationProcessorCore, ...)`), so that
  `/MT` never exceeds the number of real cores;
- ROBOCOPY's hard limit of `128`.

If media probing fails, `Num_MT()` falls back to `/MT:8` (ROBOCOPY's default).

### Why "physical cores", not "logical threads"?

`/MT` parallelises the **transfer** stage, not the scan / compare stage.
Oversubscribing past the number of physical cores provides almost no benefit
for an I/O-bound tool and increases context-switch overhead. On a hybrid CPU
(e.g. Intel Core i7-14650HX: 8 P-cores + 8 E-cores = 16 cores / 24 threads),
`Num_CPU_Cores()` returns **16**, not 24.

## Usage

The tool prints the following rules on every prompt:

1. Enter `exit` or `Exit` to quit.
2. Wait for `Complete!` before closing the window.
3. A single Chinese character may count as 2 or 3 bytes toward the
   path-length limit.
4. The destination path must **not** be a bare drive letter
   (e.g. `D:\`).

Additional notes:

- All subdirectories are copied, **including empty ones**.
- If the destination directory does not exist, ROBOCOPY creates it.
- Path length is limited to `DEFAULT_BLOCK_SIZE / 2 - 32` characters
  (currently **544**).
- Calm-print is selected at startup by answering `n` to
  `Is Calm to Print (Y/y n)>>`.

## Build

- **Compiler**: WinLibs GCC 16.2.0 (MinGW-w64, UCRT)
- **Language standard**: C23 (`-std=c23` / `-std=c2x`)
- **Editor**: Visual Studio Code
- **Final executable**: `cpytool.exe`

### Dependencies

| Module | Purpose |
|--------|---------|
| `allocer` (`allocer/allocer.h`, `allocer/allocer_base.h`) | Custom block allocator |
| `config` (`config/config.h`) | Build-time constants (`DEFAULT_BUF_SIZE`, `DEFAULT_BLOCK_SIZE`, `SOURCE_DEFAULT`, …) |
| `help_tool` (`tools/help_tool/help_tool.h`) | Path checking, `Num_CPU_Cores`, `Num_MT`, `Int_Tran_Str`, … |
| `shlwapi` (Windows) | Path helpers |
| `stdio`, `stdlib`, `string`, `stdint`, `assert` | C standard library |

### Windows version requirement

**Windows 8 or above.**

The program uses `GetLogicalProcessorInformationEx` (Windows 7+) and other
APIs guarded by `_WIN32_WINNT >= 0x0601`. Windows 8 is the safe baseline for
WinLibs / UCRT builds.

## Version

Current version, printed at startup: **0.0.3**.

## Known Issues / TODO

- The prompt still says `Diraction path>>` (should be `Destination path>>`).
- `Note:` in the rules says "ignore the NOEXITS target diraction", but the
  tool actually **creates** a missing destination via `/E`. Reword to
  reflect the real behaviour.

## License

Copyright (C) 2025 Your Name

This program is free software: you can redistribute it and/or modify
it under the terms of the **GNU General Public License** as published by
the Free Software Foundation, either **version 3 of the License**, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but **WITHOUT ANY WARRANTY**; without even the implied warranty of
**MERCHANTABILITY** or **FITNESS FOR A PARTICULAR PURPOSE**.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <https://www.gnu.org/licenses/>.

SPDX-License-Identifier: `GPL-3.0-or-later`

See the [LICENSE](LICENSE) file for the full license text.
