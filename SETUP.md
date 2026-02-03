# 🚀 Project Setup Guide

This guide will help you set up the DAG RPG Attribute System for development.

## 📋 Prerequisites

### Required Software

1. **C++ Compiler** (C++20 compatible)
   - **Windows**: Visual Studio 2019+ (MSVC)
   - **Linux**: GCC 10+ or Clang 12+
   - **macOS**: Xcode 13+ or Clang 12+

2. **CMake** 3.16 or higher
   - Download: https://cmake.org/download/
   - Verify: `cmake --version`

3. **Git**
   - Download: https://git-scm.com/
   - Verify: `git --version`

4. **VS Code** (recommended)
   - Download: https://code.visualstudio.com/

### Optional Tools

- **OpenCppCoverage** (Windows, for code coverage)
- **lcov** (Linux, for code coverage)
- **Clang-Format** (for code formatting)
- **Clang-Tidy** (for static analysis)

## 🔧 Step-by-Step Setup

### 1. Clone the Repository

```bash
git clone <your-repo-url>
cd DAG_PROJECT_TRYOUT_1
```

### 2. Install VS Code Extensions

Open VS Code and install recommended extensions:

1. Press `Ctrl+Shift+P` (or `Cmd+Shift+P` on macOS)
2. Type "Extensions: Show Recommended Extensions"
3. Click "Install All"

**Essential Extensions:**
- C/C++ (Microsoft)
- CMake Tools
- CMake
- GitLens
- Test Explorer UI
- Coverage Gutters

### 3. Configure CMake

#### Using VS Code (Recommended)

1. Open the project folder in VS Code
2. Press `Ctrl+Shift+P` and select "CMake: Configure"
3. Select your compiler kit
4. CMake will automatically configure

#### Using Command Line

```bash
# Debug build with tests and coverage
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DENABLE_TESTING=ON -DENABLE_COVERAGE=ON

# Release build
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
```

### 4. Build the Project

#### Using VS Code

Press `Ctrl+Shift+B` and select "CMake: Build"

#### Using Command Line

```bash
# Build all targets
cmake --build build --config Debug

# Build specific target
cmake --build build --target dag_rpg_system --config Debug
```

### 5. Run Tests

#### Using VS Code

1. Open Test Explorer (flask icon in sidebar)
2. Click "Run All Tests" button
3. View results in Test Explorer

#### Using Command Line

```bash
# Run all tests
ctest --test-dir build --output-on-failure

# Run with verbose output
ctest --test-dir build --verbose

# Run specific test
./build/bin/Debug/dag_rpg_tests --gtest_filter=StatNodeTest.*
```

### 6. Generate Code Coverage

#### Linux (GCC/Clang)

```bash
# Build with coverage enabled
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DENABLE_COVERAGE=ON
cmake --build build

# Generate coverage report
cmake --build build --target coverage

# View HTML report
xdg-open build/coverage/index.html
```

#### Windows (MSVC)

Use OpenCppCoverage:

```powershell
# Install OpenCppCoverage
# https://github.com/OpenCppCoverage/OpenCppCoverage/releases

# Run tests with coverage
OpenCppCoverage.exe --sources "src" --sources "include" -- build\bin\Debug\dag_rpg_tests.exe

# Generate HTML report
OpenCppCoverage.exe --sources "src" --sources "include" --export_type html:coverage -- build\bin\Debug\dag_rpg_tests.exe
```

## 🎮 Running the Application

### Main Application

```bash
# Linux/macOS
./build/bin/dag_rpg_system

# Windows
.\build\bin\Debug\dag_rpg_system.exe
```

### With Visualization

1. Run the main application
2. Open `index.html` in your browser
3. The visualization will connect via WebSocket

## 🛠️ VS Code Tasks

Access tasks via `Ctrl+Shift+P` → "Tasks: Run Task"

Available tasks:
- **CMake: Configure** - Configure CMake project
- **CMake: Build** (Default: `Ctrl+Shift+B`) - Build the project
- **CMake: Rebuild** - Clean and rebuild
- **CTest: Run All Tests** - Run unit tests
- **Coverage: Generate Report** - Generate coverage
- **Run Main Application** - Build and run main app
- **Format All Files** - Format C++ files

## 🐛 Debugging

### Debug Main Application

1. Open any source file
2. Press `F5` or go to Run and Debug view
3. Select "Debug Main Application"
4. Set breakpoints by clicking left of line numbers

### Debug Tests

1. Press `F5` or go to Run and Debug view
2. Select "Debug Tests"
3. All tests will run under debugger

### Debug Specific Test

1. Edit `.vscode/launch.json`
2. Use "Debug Current Test File" configuration
3. Open a test file and press `F5`

## 📊 Code Quality Tools

### Formatting

```bash
# Format single file
clang-format -i src/main.cpp

# Format all files
clang-format -i src/*.cpp include/*.h

# Or use VS Code: Ctrl+Shift+I
```

### Static Analysis

```bash
# Run clang-tidy
clang-tidy src/main.cpp -- -Iinclude

# With CMake integration
cmake -DCMAKE_CXX_CLANG_TIDY="clang-tidy;-checks=*" ..
```

## 🔍 Troubleshooting

### CMake Configuration Fails

**Problem**: CMake can't find compiler

**Solution**:
```bash
# Specify compiler explicitly
cmake -S . -B build -DCMAKE_CXX_COMPILER=g++ -DCMAKE_C_COMPILER=gcc
```

### Tests Won't Build

**Problem**: Google Test not found

**Solution**: CMake will automatically download Google Test via FetchContent. Ensure you have internet connection on first configure.

### IntelliSense Not Working

**Problem**: Red squiggles everywhere

**Solution**:
1. Ensure CMake is configured: `Ctrl+Shift+P` → "CMake: Configure"
2. Check `.vscode/settings.json` has correct paths
3. Reload window: `Ctrl+Shift+P` → "Developer: Reload Window"

### Coverage Not Generating

**Problem**: No coverage files created

**Solution**:
- **Linux**: Install `lcov` and `genhtml`: `sudo apt install lcov`
- **Windows**: Use OpenCppCoverage or Visual Studio Enterprise

## 📦 Build Configurations

### Debug Build (Development)

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DENABLE_TESTING=ON
```

**Characteristics:**
- No optimization
- Debug symbols included
- Assertions enabled
- Slower but debuggable

### Release Build (Production)

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
```

**Characteristics:**
- Full optimization (-O3)
- No debug symbols
- Assertions disabled
- Fast execution

### RelWithDebInfo (Profiling)

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=RelWithDebInfo
```

**Characteristics:**
- Optimized but debuggable
- Debug symbols included
- Good for profiling

## 🌐 Project URLs

- **Repository**: <your-repo-url>
- **Issues**: <your-repo-url>/issues
- **Documentation**: See README.md, ARCHITECTURE.md, CONTRIBUTING.md

## 📞 Getting Help

If you encounter issues:

1. Check this guide first
2. Search existing GitHub issues
3. Create a new issue with:
   - Your OS and compiler version
   - CMake version
   - Error messages
   - Steps to reproduce

## ✅ Verification Checklist

After setup, verify everything works:

- [ ] CMake configures without errors
- [ ] Project builds successfully
- [ ] All tests pass
- [ ] Main application runs
- [ ] Coverage can be generated
- [ ] VS Code IntelliSense works
- [ ] Debugging works

## 🎉 You're Ready!

Your development environment is now set up! Check out:

- [README.md](README.md) - Project overview
- [ARCHITECTURE.md](ARCHITECTURE.md) - System design
- [CONTRIBUTING.md](CONTRIBUTING.md) - Contribution guidelines

Happy coding! 🚀
