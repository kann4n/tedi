# Tedi

A small terminal text editor written in C.

`tedi` is a lightweight, kilo-style editor: launch it, type, save, quit. It has no
ncurses dependency, keeps the codebase intentionally small, and is easy to read,
build, and modify.

- **Current version:** 0.0.16
- **License:** GNU AGPL v3

---

## Table of contents

- [What it is for](#what-it-is-for)
- [Features](#features)
- [Requirements](#requirements)
- [Installing](#installing)
- [Running](#running)
- [Usage](#usage)
- [Themes](#themes)
- [Supported syntax](#supported-syntax)
- [Adding your own syntax highlighting](#adding-your-own-syntax-highlighting)
- [Pros and cons](#pros-and-cons)
- [Project structure](#project-structure)
- [Development](#development)
- [Roadmap](#roadmap)
- [License](#license)

---

## What it is for

`tedi` is for people who want a tiny, hackable terminal editor rather than a
full IDE. It is a good fit for:

- small plain-text and source-code edits
- learning how terminal editors work
- experimenting with ANSI escape sequences and syntax highlighting
- a minimal `$EDITOR` for quick edits on a Linux/POSIX machine

It is **not** intended to replace Vim, Emacs, Helix, Neovim, or VS Code for
large projects, Unicode-heavy text, or day-to-day development. See
[Pros and cons](#pros-and-cons) for the honest list.

---

## Features

- Pure C99 implementation with plain `termios`; no ncurses or other libraries
- Small codebase (roughly 1.4k lines) and a single ~40 KB binary
- Built-in syntax highlighting for C, C++, headers, and Python, with simple
  compiled-in definitions you can extend
- Incremental search with next/previous match navigation
- Tab expansion and control-character display
- Alternate-screen support with terminal restoration on exit
- 24-bit color status bar
- Simple, human-readable color-scheme files
- Unsaved-changes warning before quitting
- Create new files directly from the command line
- Edit files with horizontal scrolling for long lines

---

## Requirements

- Linux or another Unix-like operating system with a POSIX terminal
- A C99 compiler (`gcc` or `clang`)
- `make`
- A terminal emulator

No external runtime libraries are required.

---

## Installing

### Build from source

```bash
git clone https://github.com/kann4n/tedi.git
cd tedi
make
```

The binary is written to:

```text
build/tedi
```

### Optional system-wide install

There is currently no `make install` target. If you want `tedi` on your
`PATH`, copy the binary manually:

```bash
sudo install -m 755 build/tedi /usr/local/bin/tedi
```

### Clean the build

```bash
make clean
```

---

## Running

Run the editor with no file to start with an empty, unnamed buffer:

```bash
tedi
```

Or open a file:

```bash
tedi notes.txt
tedi src/main.c
```

If the file does not exist, `tedi` starts with an empty buffer and creates the
file when you save it. If you built locally without installing, use
`./build/tedi` instead of `tedi`.

Only the first file argument is opened; multiple file arguments are not
supported yet.

---

## Usage

`tedi` is a modeless editor: keys insert text immediately, and commands use
`Ctrl` combinations.

### Keybindings

| Key                     | Action                                                                                                                                   |
| ----------------------- | ---------------------------------------------------------------------------------------------------------------------------------------- |
| `Ctrl-S`                | Save the current buffer. Prompts for a filename if unnamed.                                                                              |
| `Ctrl-F`                | Incremental search. Type a query, use `Left`/`Right` to jump between matches, `Enter` to accept, `Esc` to cancel and restore the cursor. |
| `Ctrl-Q`                | Quit. If there are unsaved changes, press `!` to discard them; any other key cancels the quit.                                           |
| `Ctrl-T`                | Load the color scheme from `test.colorscheme` in the current directory.                                                                  |
| `Enter`                 | Insert a new line.                                                                                                                       |
| `Backspace` / `Ctrl-H`  | Delete the previous character.                                                                                                           |
| `Delete`                | Delete the character under the cursor.                                                                                                   |
| Arrow keys              | Move the cursor.                                                                                                                         |
| `Page Up` / `Page Down` | Scroll one screen up or down.                                                                                                            |
| `Home` / `End`          | Move to the start or end of the line.                                                                                                    |
| `Ctrl-L` / `Esc`        | Ignored.                                                                                                                                 |

Printable characters are inserted directly. Tabs are expanded for display but
stored as tab characters.

### Saving and quitting

- `Ctrl-S` writes the whole buffer back to disk.
- An unnamed buffer asks for a filename the first time you save.
- `Ctrl-Q` refuses to quit with unsaved changes unless you confirm with `!`.

### Search behavior

Search is case-sensitive and scans line by line. Matches are highlighted while
the search prompt is open. `Enter` leaves the cursor at the current match;
`Esc` restores the cursor to where the search started.

---

## Themes

Press `Ctrl-T` to load `test.colorscheme` from the current working directory.
The file is a simple list of `key=value` pairs. Values are ANSI SGR color
numbers (for example `31` for red, `36` for cyan).

Example:

```text
comment=36
mlcomment=36
keyword1=31
keyword2=33
keyword3=34
string=32
number=35
match=43
default_fg=37
default_bg=40
```

| Key          | Meaning                    |
| ------------ | -------------------------- |
| `comment`    | Single-line comments       |
| `mlcomment`  | Multi-line comments        |
| `keyword1`   | Primary keywords           |
| `keyword2`   | Secondary keywords / types |
| `keyword3`   | Reserved for future use    |
| `string`     | String literals            |
| `number`     | Numeric literals           |
| `match`      | Search matches             |
| `default_fg` | Default foreground         |
| `default_bg` | Default background         |

Colors accept ANSI SGR values: `30`–`37` for foreground and `40`–`47` for
background. `default_fg` and `default_bg` set normal text colors; the other keys
map to syntax classes and search matches.

---

## Supported syntax

| Language | Extensions         | Highlighting                                         |
| -------- | ------------------ | ---------------------------------------------------- |
| C / C++  | `.c`, `.cpp`, `.h` | Comments, block comments, strings, numbers, keywords |
| Python   | `.py`              | Comments, strings, numbers, keywords                 |

Other file types open as plain text. Syntax detection is based on the file
extension.

---

## Adding your own syntax highlighting

Syntax definitions are compiled into `tedi`, which keeps startup fast and
avoids runtime dependencies. To add a language, edit `src/syntax.c`:

1. Define a `NULL`-terminated list of file extensions (including the leading
   dot).
2. Define a `NULL`-terminated keyword list. A trailing `|` marks a secondary
   keyword, such as a type, and uses the `keyword2` color instead of
   `keyword1`.
3. Add an entry to the `HLDB[]` table with the file type, extensions, keywords,
   comment markers, and highlight flags.

For example, to add Rust:

```c
char *Rust_HL_extensions[] = {".rs", NULL};

char *Rust_HL_keywords[] = {
    "as", "break", "const", "continue", "crate", "else", "enum", "extern",
    "false", "fn", "for", "if", "impl", "in", "let", "loop", "match", "mod",
    "move", "mut", "pub", "ref", "return", "self", "static", "struct", "trait",
    "true", "type", "unsafe", "use", "where", "while",
    "bool|", "char|", "f32|", "f64|", "i32|", "i64|", "str|", "String|",
    "u32|", "u64|", "usize|",
    NULL
};

struct editorSyntax HLDB[] = {
    {"c",      C_HL_extensions,      C_HL_keywords,      "//", "/*", "*/", HL_HL_NUMBERS | HL_HL_STRINGS},
    {"python", Python_HL_extensions, Python_HL_keywords, "#",  "",   "",   HL_HL_NUMBERS | HL_HL_STRINGS},
    {"rust",   Rust_HL_extensions,   Rust_HL_keywords,   "//", "/*", "*/", HL_HL_NUMBERS | HL_HL_STRINGS},
};
```

Rebuild with `make` and reopen the file. Detection uses the extension from
`filematch`.

Notes:

- `HL_HL_NUMBERS` and `HL_HL_STRINGS` enable number and string highlighting.
  Leave them out to disable those passes for the language.
- For languages without block comments, use empty strings for the multiline
  start/end markers, as the Python entry does.
- `keyword3` exists in the theme, but no highlighting pass currently assigns
  it.
- Highlighting is line-based. Multiline strings and nested comments are not
  supported; only block-comment state is carried between lines.

Adding a language currently requires a rebuild. A data-driven syntax file is on
the roadmap.

---

## Pros and cons

### Pros

- **Dependency-free.** No ncurses, no runtime libraries, no package manager.
- **Easy to read and hack.** The whole implementation fits in a small number
  of source files.
- **Fast to build and start.** Plain `make`, then run the binary.
- **Good learning project.** Demonstrates raw terminal mode, ANSI output,
  gap-free line storage, and syntax highlighting in a manageable codebase.
- **Useful basics.** Syntax highlighting, incremental search, tab handling,
  horizontal scrolling, and unsaved-change protection.
- **Terminal-friendly.** Uses the alternate screen and restores terminal state
  on normal exit.

### Cons / current limitations

- **Early-stage software.** Version 0.0.16 is not production-ready.
- **No UTF-8/Unicode support.** Cursor movement, deletion, and rendering are
  byte-based, so multibyte characters can be split or misaligned. Avoid using
  it on non-ASCII text.
- **No undo/redo.** Every edit is immediate and permanent until you quit
  without saving.
- **No selection, clipboard integration, or mouse support.**
- **No line wrapping or line numbers.** Long lines scroll horizontally.
- **Limited built-in syntax support.** Only C, C++, headers, and Python are
  highlighted; adding another language requires editing and rebuilding.
- **No general configuration system.** No config file or theme flag; `Ctrl-T`
  always reads `test.colorscheme`, and syntax definitions are compiled in.
- **Terminal resize is not handled robustly.** The screen size is read at
  startup.
- **Idle redraw.** The editor redraws periodically while waiting for input,
  which can keep the CPU busier than expected in long sessions.
- **Search is basic.** Case-sensitive only, with no replace function and no
  persistent match highlighting after the prompt closes.
- **Saving is in-place.** There is no atomic save, backup file, or file-locking
  protection against concurrent edits.
- **Single buffer.** No tabs, split windows, project tree, or multiple open
  files.
- **Limited automated tests.** The project currently has no wired-up test
  target or CI.
- **Linux-focused.** It relies on POSIX `termios` and glibc-friendly headers;
  other platforms may need small changes.

---

## Project structure

```text
tedi/
├── include/          headers (editor state, syntax, ANSI helpers, utilities)
├── src/              implementation
│   ├── main.c        entry point
│   ├── init.c        editor and theme initialization
│   ├── terminal.c    raw mode, window size, key decoding
│   ├── input.c       key handling and prompts
│   ├── operations.c  insert/delete operations
│   ├── row.c         line storage, rendering, tab expansion
│   ├── file.c        open/save
│   ├── find.c        incremental search
│   ├── syntax.c      syntax highlighting and themes
│   ├── output.c      screen drawing
│   ├── abuf.c        append buffer for terminal output
│   └── utils.c       small helpers
├── tests/            small standalone checks
├── playground/       experiments and theme examples
├── docs/             notes, including ANSI escape-sequence reference
├── build/            generated objects and binary (ignored by git)
├── Makefile
├── test.colorscheme  example theme
└── help.txt          quick keybinding reference
```

---

## Development

Build:

```bash
make
```

Clean:

```bash
make clean
```

The current build uses:

```text
-Wall -Wextra -std=c99 -Iinclude
```

The project includes a `.clang-format` and `.clangd` configuration for editor
support. There is no `make test` target yet; `tests/` contains standalone
checks such as truecolor detection.

When changing the code:

1. Keep the existing C style and tab indentation.
2. Build with `make` before committing.
3. Test the affected keybindings in a real terminal, not just a pipe.
4. Update `help.txt` if you add or change a keybinding.

---

## Roadmap

Ideas that would have the biggest impact:

- UTF-8-aware cursor movement and rendering
- Undo/redo history
- Configurable themes and a real config file
- Terminal resize handling (`SIGWINCH`)
- Atomic saves and backup files
- More languages and a data-driven syntax table
- Selection, copy/paste, and mouse support
- Automated tests and CI

---

## License

`tedi` is released under the GNU Affero General Public License v3. See
[LICENSE](LICENSE) for the full text.

The architecture is inspired by the classic kilo terminal editor tutorial.
