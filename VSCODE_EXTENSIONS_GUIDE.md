# 🔌 VS Code Extensions Guide

This guide explains the recommended extensions for the DAG RPG System project.

## 🚀 Quick Install

Open VS Code Command Palette (`Ctrl+Shift+P`) and run:
```
Extensions: Show Recommended Extensions
```

Click "Install All" to install all recommended extensions at once.

## 📦 Extension Categories

### 🎯 Essential C++ Development

#### 1. **C/C++ (ms-vscode.cpptools)** ⭐ REQUIRED
- **Publisher**: Microsoft
- **What it does**: Core C++ language support
- **Features**:
  - IntelliSense (code completion)
  - Debugging
  - Code navigation
  - Syntax highlighting
- **Why you need it**: Absolutely essential for C++ development

#### 2. **C/C++ Extension Pack (ms-vscode.cpptools-extension-pack)** ⭐ REQUIRED
- **Publisher**: Microsoft
- **What it does**: Bundle of C++ tools
- **Includes**:
  - C/C++ Tools
  - CMake Tools
  - CMake Language Support
- **Why you need it**: One-click install of core tools

#### 3. **CMake Tools (ms-vscode.cmake-tools)** ⭐ REQUIRED
- **Publisher**: Microsoft
- **What it does**: CMake integration
- **Features**:
  - Configure, build, test from VS Code
  - Kit selection
  - Target selection
  - CTest integration
- **Why you need it**: Makes CMake development seamless

#### 4. **CMake (twxs.cmake)** ⭐ REQUIRED
- **Publisher**: twxs
- **What it does**: CMake language support
- **Features**:
  - Syntax highlighting for CMakeLists.txt
  - Code completion
  - Command documentation
- **Why you need it**: Better CMake editing experience

### 🧪 Testing & Quality

#### 5. **Test Explorer UI (hbenl.vscode-test-explorer)** ⭐ RECOMMENDED
- **Publisher**: Holger Benl
- **What it does**: Visual test runner
- **Features**:
  - Run tests from sidebar
  - View test results
  - Debug individual tests
  - Test output
- **Why you need it**: Much better than command-line testing

#### 6. **C++ TestMate (matepek.vscode-catch2-test-adapter)** ⭐ RECOMMENDED
- **Publisher**: Mate Pek
- **What it does**: Google Test adapter for Test Explorer
- **Features**:
  - Auto-discover Google Test tests
  - Run/debug individual tests
  - Test output in UI
- **Why you need it**: Integrates Google Test with Test Explorer

#### 7. **Coverage Gutters (ryanluker.vscode-coverage-gutters)** ⭐ RECOMMENDED
- **Publisher**: Ryan Luker
- **What it does**: Display code coverage in editor
- **Features**:
  - Green/red lines showing coverage
  - Coverage percentage
  - Watch mode
  - Multiple coverage formats
- **Why you need it**: Visual feedback on test coverage

### 🔍 Code Quality & Analysis

#### 8. **clangd (llvm-vs-code-extensions.vscode-clangd)** 🎓 OPTIONAL
- **Publisher**: LLVM
- **What it does**: Advanced C++ language server
- **Features**:
  - Faster IntelliSense
  - Better diagnostics
  - Code completion
  - Refactoring
- **Why you might want it**: Better than default C++ extension for large projects
- **Note**: Conflicts with C/C++ extension, choose one

#### 9. **Clang-Tidy (notskm.clang-tidy)** 🎓 OPTIONAL
- **Publisher**: notskm
- **What it does**: Static analysis
- **Features**:
  - Find bugs
  - Code style issues
  - Performance problems
  - Modernization suggestions
- **Why you need it**: Catch errors before runtime

#### 10. **Doxygen Documentation Generator (cschlosser.doxdocgen)** 🎓 RECOMMENDED
- **Publisher**: Christoph Schlosser
- **What it does**: Generate documentation comments
- **Features**:
  - Auto-generate Doxygen comments
  - Function parameter detection
  - Return value documentation
- **Why you need it**: Makes documentation easier

### 📝 Git & Version Control

#### 11. **GitLens (eamodio.gitlens)** ⭐ HIGHLY RECOMMENDED
- **Publisher**: GitKraken
- **What it does**: Supercharge Git in VS Code
- **Features**:
  - Blame annotations
  - Commit history
  - File history
  - Branch comparisons
  - Rich hovers
- **Why you need it**: Essential for Git workflows

#### 12. **Git History (donjayamanne.githistory)** 🎓 RECOMMENDED
- **Publisher**: Don Jayamanne
- **What it does**: View git log and file history
- **Features**:
  - Visual git log
  - File history
  - Compare branches
  - Search history
- **Why you need it**: Better understanding of changes

#### 13. **Git Graph (mhutchie.git-graph)** 🎓 RECOMMENDED
- **Publisher**: mhutchie
- **What it does**: Visual git graph
- **Features**:
  - Branch visualization
  - Interactive graph
  - Commit details
  - Branch operations
- **Why you need it**: See your git tree visually

### 💡 Productivity

#### 14. **Todo Tree (gruntfuggly.todo-tree)** 🎓 RECOMMENDED
- **Publisher**: Gruntfuggly
- **What it does**: Track TODO comments
- **Features**:
  - Find all TODOs
  - Tree view of tasks
  - Highlights in editor
  - Custom tags
- **Why you need it**: Never forget TODOs

#### 15. **TODO Highlight (wayou.vscode-todo-highlight)** 🎓 OPTIONAL
- **Publisher**: Wayou Liu
- **What it does**: Highlight TODO/FIXME comments
- **Features**:
  - Color-coded comments
  - Customizable keywords
  - List all TODOs
- **Why you might want it**: Visual TODO reminders

#### 16. **Better Comments (aaron-bond.better-comments)** 🎓 RECOMMENDED
- **Publisher**: Aaron Bond
- **What it does**: Colorful comment categories
- **Features**:
  ```cpp
  // ! Important warning
  // ? Question or consideration
  // TODO: Task to complete
  // * Highlighted comment
  ```
- **Why you need it**: Better comment organization

#### 17. **Indent Rainbow (oderwat.indent-rainbow)** 🎓 OPTIONAL
- **Publisher**: oderwat
- **What it does**: Colorize indentation
- **Features**:
  - Different colors per indent level
  - Easy to spot indent errors
- **Why you might want it**: Visual guide for nested code

#### 18. **Code Spell Checker (streetsidesoftware.code-spell-checker)** ⭐ RECOMMENDED
- **Publisher**: Street Side Software
- **What it does**: Spell checking in code
- **Features**:
  - Catch typos in comments
  - Variable name checking
  - Custom dictionaries
  - Multiple languages
- **Why you need it**: Professional documentation

### 📚 Documentation

#### 19. **Markdown All in One (yzhang.markdown-all-in-one)** ⭐ RECOMMENDED
- **Publisher**: Yu Zhang
- **What it does**: Complete Markdown support
- **Features**:
  - Preview
  - Shortcuts
  - Table of contents
  - Math support
- **Why you need it**: Better README editing

#### 20. **Markdown Lint (davidanson.vscode-markdownlint)** 🎓 OPTIONAL
- **Publisher**: David Anson
- **What it does**: Markdown style checker
- **Features**:
  - Enforce style rules
  - Fix common issues
  - Consistent formatting
- **Why you might want it**: Professional documentation

#### 21. **Markdown Preview Mermaid (bierner.markdown-mermaid)** 🎓 OPTIONAL
- **Publisher**: Matt Bierner
- **What it does**: Mermaid diagram support
- **Features**:
  - Flowcharts in Markdown
  - Sequence diagrams
  - Class diagrams
- **Why you might want it**: Architecture diagrams

### 🎨 UI & Themes

#### 22. **Material Icon Theme (pkief.material-icon-theme)** 🎓 RECOMMENDED
- **Publisher**: Philipp Kief
- **What it does**: Better file icons
- **Features**:
  - Recognizable file types
  - Folder icons
  - Custom icon packs
- **Why you need it**: Better visual organization

#### 23. **GitHub Theme (github.github-vscode-theme)** 🎓 OPTIONAL
- **Publisher**: GitHub
- **What it does**: GitHub-like theme
- **Features**:
  - Light and dark variants
  - Consistent with GitHub
- **Why you might want it**: Familiar look

### 🌐 Web Development (for Visualization)

#### 24. **ESLint (dbaeumer.vscode-eslint)** 🎓 OPTIONAL
- **Publisher**: Microsoft
- **What it does**: JavaScript linting
- **Features**:
  - Find JS errors
  - Code style
  - Auto-fix
- **Why you might want it**: For editing index.html JavaScript

#### 25. **Prettier (esbenp.prettier-vscode)** 🎓 OPTIONAL
- **Publisher**: Prettier
- **What it does**: Code formatter for web
- **Features**:
  - Format HTML, CSS, JS
  - Auto-format on save
- **Why you might want it**: For editing index.html

### 🤖 AI Assistance (If Available)

#### 26. **GitHub Copilot (github.copilot)** 💎 PREMIUM
- **Publisher**: GitHub
- **What it does**: AI code completion
- **Features**:
  - Complete functions
  - Write tests
  - Generate comments
- **Why you might want it**: Faster development
- **Note**: Requires subscription

#### 27. **GitHub Copilot Chat (github.copilot-chat)** 💎 PREMIUM
- **Publisher**: GitHub
- **What it does**: AI chat assistant
- **Features**:
  - Ask coding questions
  - Explain code
  - Find bugs
- **Why you might want it**: AI pair programmer
- **Note**: Requires Copilot subscription

## 📊 Priority Installation Order

### Phase 1: Essential (Install First)
1. C/C++ Extension Pack
2. CMake Tools
3. GitLens

### Phase 2: Testing (Install Second)
4. Test Explorer UI
5. C++ TestMate
6. Coverage Gutters

### Phase 3: Quality (Install Third)
7. Code Spell Checker
8. Better Comments
9. Doxygen Documentation Generator

### Phase 4: Productivity (Install as Needed)
10. Todo Tree
11. Git Graph
12. Markdown All in One

## ⚙️ Configuration Tips

### Coverage Gutters Setup
Add to `.vscode/settings.json`:
```json
{
    "coverage-gutters.coverageFileNames": [
        "lcov.info",
        "coverage.xml"
    ],
    "coverage-gutters.showLineCoverage": true
}
```

### Todo Tree Setup
Customize tags in settings:
```json
{
    "todo-tree.general.tags": [
        "TODO",
        "FIXME",
        "HACK",
        "NOTE",
        "BUG"
    ]
}
```

## 🚫 Conflicts to Avoid

- **clangd vs C/C++**: Don't use both, they conflict
- **Multiple formatters**: Choose one formatter per language
- **Too many Git extensions**: Can slow down VS Code

## 📈 Performance Tips

- Don't install extensions you don't use
- Disable extensions for specific workspaces
- Use workspace recommendations
- Reload window after installing many extensions

## 🔄 Keeping Extensions Updated

- VS Code auto-updates extensions by default
- Manually check: Extensions view → Check for Updates
- Review changelog before updating

## ✅ Verification

After installing, verify:
- [ ] IntelliSense works (code completion)
- [ ] CMake configures successfully
- [ ] Tests appear in Test Explorer
- [ ] Coverage shows in gutters
- [ ] Git features work

---

**Legend:**
- ⭐ **REQUIRED**: Must install for basic functionality
- 🎓 **RECOMMENDED**: Highly beneficial for study project
- 💎 **PREMIUM**: Requires paid subscription
- 🎓 **OPTIONAL**: Nice to have

Enjoy your enhanced development environment! 🚀
