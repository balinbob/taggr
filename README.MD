# taggr

`taggr` is a C++ command-line tool for editing and managing metadata tags in audio files.  
It supports FLAC, MP3, Ogg, WavPack, and more via the excellent [TagLib](https://taglib.org/) library.

With `taggr`, you can:
- Show, set, or remove metadata tags
- Import or export tags based on filename patterns
- Work with cover art and other binary tags
- Batch-edit files using globs and folders

---

## 📦 Build Instructions (Windows Only)

### Requirements

- C++17 compiler (MSVC or MinGW)
- [CMake](https://cmake.org/)
- [vcpkg](https://github.com/microsoft/vcpkg)
- Git

### Dependencies (via vcpkg)

You’ll need these libraries:
- [`taglib`](https://github.com/taglib/taglib) ≥ **2.0.2** — audio tag handling
- [`CLI11`](https://github.com/CLIUtils/CLI11) — command-line parser
- [`glob.hpp`](https://github.com/p-ranav/glob) — simple C++17 globbing

```sh
# Clone taggr source code
git clone https://github.com/balinbob/taggr.git
cd taggr

# Install dependencies via vcpkg
vcpkg install taglib cli11

# Build with CMake (MSVC)
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE="C:/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake"
cmake --build build --config Release

# Resulting binary: build/taggr.exe
```

> Replace the path to `vcpkg` if installed elsewhere.

---

## 🚀 Usage

```sh
taggr [OPTIONS] files...
```

### POSITIONALS

| Name  | Description             |
|-------|-------------------------|
| `files` | File paths or globs (Required) |

---

### OPTIONS

| Option | Description |
|--------|-------------|
| `-h`, `--help`              | Print help message and exit |
| `--recurse`                 | Recurse into subdirectories |
| `-v`, `--verbose`           | Verbose mode (more output) |
| `-q`, `--quiet`             | Quiet mode (no filenames) |
| `-n`, `--noact`             | No action (dry run only) |
| `-l`, `--list`              | List all available tags |
| `--clear`                   | Clear all tags |
| `-s`, `--show TEXT ...`     | Show specific tag(s)<br>Ex: `--show genre` |
| `-t`, `--tag TEXT ...`      | Set tag(s) as key="value"<br>Ex: `-t title="Come Together"` |
| `-a`, `--add TEXT ...`      | Add value to multi-value tag<br>Ex: `-a genre="rock"` |
| `-r`, `--remove TEXT ...`   | Remove tag or tag value<br>Ex: `--remove genre="pop"` |
| `-b`, `--binary TEXT ...`   | Set binary tag (e.g., cover art)<br>Ex: `--binary frontcover="cover.jpg"` |
| `--fn2tag TEXT`             | Extract tags from filename pattern<br>Ex: `--fn2tag "%l\%n %t.flac"` |
| `--tag2fn TEXT`             | Rename file using tag pattern<br>Ex: `--tag2fn "%l\%n %t.flac"` |

---

## 📄 Pattern-Based Tagging

### Supported Codes

| Code  | Tag Field     |
|-------|---------------|
| `%l`  | Album         |
| `%n`  | Track Number  |
| `%t`  | Title         |
| `%a`  | Artist        |
| `%c`  | Composer      !
| `%D`  | Discnumber    |

---

### Extracting Tags from Filenames

Given a filename:

```
Abbey Road\01 Come Together.flac
```

You can extract tags with:

```sh
taggr "Abbey Road\01 Come Together.flac" --fn2tag "%l\%n %t.flac"
```

**Result:**
- `album` = Abbey Road
- `tracknumber` = 01
- `title` = Come Together

---

### Renaming Files Based on Tags

Given these tags:
- Album: `Abbey Road`
- Track Number: `01`
- Title: `Come Together`

You can rename the file like this:

```sh
taggr song.flac --tag2fn "%l\%n %t.flac"
```

Resulting filename:

```
Abbey Road\01 Come Together.flac
```

---

## 🔍 Example Commands

```sh
# Show all tags in a file
taggr song.flac --list

# Show only title and artist
taggr song.flac --show title artist

# Set tags explicitly
taggr song.flac -t title="Come Together" -t album="Abbey Road"

# Add genres (multi-value)
taggr song.flac -a genre="rock" -a genre="classic rock"

# Remove one genre and the comment tag
taggr song.flac -r genre="pop" -r comment

# Extract tags from a structured filename
taggr "01 Come Together.flac" --fn2tag "%n %t.flac"

# Rename a file using its tags
taggr song.flac --tag2fn "%n %t.flac"
```

---

## 🧩 About CLI11 and glob.hpp

- [`CLI11`](https://github.com/CLIUtils/CLI11): A powerful and easy-to-use C++ command-line parser used to define options like `--tag` or `--fn2tag`.

- [`glob.hpp`](https://github.com/p-ranav/glob): A simple C++17 header-only globbing library that lets you use `*.flac` or `**/*.mp3` patterns to match files on Windows.

These are included with the `taggr` source or installed via `vcpkg`.

---

## 📚 Related Projects

- [TagLib](https://taglib.org/) — audio tag reading/writing engine
- [CLI11](https://github.com/CLIUtils/CLI11) — command-line parser
- [glob.hpp](https://github.com/p-ranav/glob) — file pattern matching

---

## 📝 License

This project is licensed under the **MIT License**.

```
MIT License

Copyright (c) 2025 balinbob

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
...
```

---

## 🙏 Acknowledgments

- Thanks to the [TagLib](https://taglib.org/) community for their stable and high-quality library.
- CLI parsing by [CLI11](https://github.com/CLIUtils/CLI11).
- Globbing powered by [glob.hpp](https://github.com/p-ranav/glob).

---

**Author:** [balinbob](https://github.com/balinbob)
