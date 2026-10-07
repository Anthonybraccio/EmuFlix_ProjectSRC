# EmuFlix

EmuFlix is a Netflix-style emulator frontend built in **C++20** using **Qt 6**, **SQLite**, and **SDL2**. It provides a modern desktop interface for browsing, organizing, and launching games from multiple emulator systems through one unified application.

Emuflix is a windows only application and not yet supported for MacOS or Linux systems.

The goal of EmuFlix is to simplify emulator use by hiding technical setup behind a clean and controller-friendly experience.

---

## Features

- Scan user-selected ROM folders and build a local game library
- Persist game and settings data using SQLite
- Browse games in a Netflix-style multi-row shelf layout
- Search, sort, and filter the library
- Mark games as favourites
- Track last played and time played
- Launch games through emulator-specific configuration templates
- Manage emulator paths and fullscreen launch arguments
- Controller-first navigation for browsing the interface
- Friendly and actionable error messages for common launch and configuration problems

---

## Included Functionality

### Library Management
- Add and remove ROM folders
- Scan folders recursively for supported ROM files
- Avoid duplicate library entries
- Remove stale entries after rescans
- Preserve settings and library data across restarts

### Browsing
- Netflix-style shelf rows
- Continue Playing row
- Favourites row
- System-specific rows
- Mutually exclusive shelf grouping
- Attached details panel for the selected game

### Controller Support
- Navigate the interface using controller directional input
- Keep focus visible and stable during use
- Maintain focus boundaries in shelf rows
- Continue browsing after dialogs and emulator close events

### Launching
- Launch supported games through configured emulators
- Handle emulator paths with spaces
- Validate launch configuration before starting
- Show friendly error dialogs for missing ROMs, missing emulators, invalid configurations, and failed launches

---

## Technology Used

- **Language:** C++20
- **UI:** Qt 6 Widgets
- **Database:** SQLite
- **Controller Input:** SDL2
- **Build System:** CMake
- **Platform Target:** Windows

---

## Running the Application

This submission is intended to be run from the packaged application folder.

To run EmuFlix:


1. Make sure c++ compiler is installed on your machine.
2. Download and extract the packaged EmuFlix folder - realease-package.zip.
3. Launch `EmuFlix.exe`.
4. Open **Settings** to configure ROM folders and emulator paths if needed.
5. Return to the main library screen and scan for games.

The packaged folder is expected to already include the required runtime files and project resources.

---

## Basic Usage

### Add ROM folders
- Open **Settings**
- Add one or more ROM directories
- Save and return to the library

### Configure emulators
- Open **Settings**
- Set the emulator path for each supported system
- Enable fullscreen if desired
- Save and return to the library

### Scan and browse
- Run a scan so ROMs appear in the library
- Browse using mouse/keyboard or controller
- Use search, sort, and favourites filtering as needed

### Launch a game
- Select a game from the library
- Launch it through the configured emulator

---

## Testing

EmuFlix can be tested by having by using the "roms" folder for working and the "testRoms" for non working roms to see how ui handles mass ammounts of games.

---

## Documentation

Doxygen documentation is included separately for the source code.

To view the generated documentation, open:

```text
docs/html/index.html
```

---

## Notes on ROMs and BIOS Files

EmuFlix is a frontend application only. It does **not** include commercial ROMs, BIOS files, or firmware as part of the final distributable application.

Users are responsible for supplying:

- their own legally obtained ROM files
- any required BIOS or firmware files
- any emulator files needed beyond what is distributed with the project

Commercial ROMs, proprietary BIOS files, and firmware should **not** be redistributed with this project.

---

## Project Goals

This project was developed around the following design goals:

- modern Netflix-style browsing experience
- controller-first interaction
- practical emulator configuration
- stable local persistence
- clean and friendly error handling
- a maintainable architecture aligned with the course UML design

---

## Disclaimer

EmuFlix is an academic software project intended for educational purposes.

The project does not endorse or permit unauthorized distribution of copyrighted game files, BIOS files, or proprietary firmware. All third-party content must be legally obtained and used in accordance with applicable laws and licenses.

---

## Authors

**Group 47**

- Anthony Braccio
- Muhammad Shabeeh Haider
- Syed Hussain Zaidi
