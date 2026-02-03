# Contributing to DAG RPG Attribute System

Thank you for your interest in contributing to this academic study project! This document provides guidelines and information for contributors.

## 🎯 Project Goals

This is an **academic research project** focused on:
- Understanding game engine architecture patterns
- Exploring performance optimization techniques
- Studying data structures and algorithms
- Developing clean, maintainable C++ code

## 🤝 How to Contribute

### Types of Contributions

We welcome:
- **Bug reports**: Found an issue? Let us know!
- **Feature suggestions**: Ideas for new mechanics or optimizations
- **Code contributions**: Bug fixes, new features, tests
- **Documentation**: Improvements to docs, examples, tutorials
- **Performance analysis**: Benchmarks, profiling results
- **Academic discussion**: Theoretical improvements, alternative approaches

### Getting Started

1. **Fork** the repository
2. **Clone** your fork locally
3. **Create** a feature branch
4. **Make** your changes
5. **Test** your changes
6. **Commit** with clear messages
7. **Push** to your fork
8. **Create** a Pull Request

## 📋 Development Guidelines

### Code Style

We follow Google C++ Style Guide with some modifications:

```cpp
// Use 4 spaces for indentation
class StatNode {
public:
    // Public members first
    void DoSomething();

private:
    // Private members with m_ prefix
    int m_value;
    std::string m_name;
};

// Use auto when type is obvious
auto node = std::make_shared<StatNode>("id", "label", 10.0);

// Meaningful names
bool isNodeDirty = node->IsDirty();  // Good
bool flag = node->IsDirty();         // Bad
```

### Formatting

The project uses `.clang-format` for automatic formatting:

```bash
# Format all files
clang-format -i src/*.cpp include/*.h

# Or use the VS Code task (Ctrl+Shift+P → Run Task → Format All Files)
```

### Testing Requirements

All new features must include tests:

```cpp
TEST_F(StatNodeTest, NewFeature_BehavesCorrectly) {
    // Arrange
    auto node = CreateTestNode();

    // Act
    auto result = node->NewFeature();

    // Assert
    EXPECT_EQ(result, expectedValue);
}
```

**Requirements:**
- Write tests BEFORE implementing features (TDD approach)
- Aim for 100% code coverage
- Test edge cases and error conditions
- Use descriptive test names

### Running Tests

```bash
# Build with tests
cmake -S . -B build -DENABLE_TESTING=ON

# Run all tests
ctest --test-dir build --output-on-failure

# Run specific test suite
./build/bin/Debug/dag_rpg_tests --gtest_filter=StatNodeTest.*

# Check coverage
cmake --build build --target coverage
```

### Commit Messages

Follow conventional commit format:

```
<type>(<scope>): <subject>

<body>

<footer>
```

**Types:**
- `feat`: New feature
- `fix`: Bug fix
- `docs`: Documentation changes
- `test`: Adding or updating tests
- `refactor`: Code refactoring
- `perf`: Performance improvements
- `style`: Formatting changes

**Examples:**
```
feat(StatNode): Add conditional dependency support

Implemented AddConditionalParent() method to support
dynamic dependencies based on game state.

Closes #42
```

```
fix(Modifier): Correct More modifier calculation order

More modifiers were being applied before Increased,
causing incorrect results. Fixed application order.

Fixes #58
```

### Branch Naming

Use descriptive branch names:
- `feature/conditional-dependencies`
- `fix/dirty-flag-propagation`
- `docs/architecture-diagrams`
- `test/coverage-improvements`

## 🔍 Pull Request Process

1. **Update Documentation**: If you change behavior, update docs
2. **Add Tests**: Ensure all new code is tested
3. **Run Tests**: Verify all tests pass locally
4. **Check Coverage**: Aim to maintain or improve coverage
5. **Lint**: Ensure code follows style guidelines
6. **Describe Changes**: Write clear PR description

### PR Template

```markdown
## Description
Brief description of changes

## Motivation
Why is this change needed?

## Type of Change
- [ ] Bug fix
- [ ] New feature
- [ ] Documentation update
- [ ] Test improvement
- [ ] Refactoring

## Testing
- [ ] All existing tests pass
- [ ] Added new tests for changes
- [ ] Manually tested in running application

## Checklist
- [ ] Code follows project style guidelines
- [ ] Documentation updated
- [ ] Tests added/updated
- [ ] No new warnings introduced
```

## 🧪 Testing Guidelines

### Test Structure

```cpp
// Use test fixtures for common setup
class ComponentTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Common initialization
        graph = std::make_shared<GraphManager>();
    }

    void TearDown() override {
        // Cleanup
    }

    std::shared_ptr<GraphManager> graph;
};
```

### Test Categories

1. **Unit Tests**: Test individual components
2. **Integration Tests**: Test component interactions
3. **Performance Tests**: Benchmark critical paths
4. **Edge Case Tests**: Boundary conditions, error handling

### Coverage Goals

- **Line Coverage**: 100% target
- **Branch Coverage**: >90%
- **Function Coverage**: 100%

## 📚 Documentation Standards

### Code Documentation

Use clear, descriptive comments:

```cpp
/**
 * Calculates the value of this stat node.
 *
 * If the node is dirty, recalculates by pulling values from
 * parent nodes. Otherwise, returns cached value for O(1) performance.
 *
 * @return The current calculated value
 * @throws std::runtime_error if calculation function is not set
 */
double GetValue() const;
```

### Documentation Files

- **README.md**: Project overview and quick start
- **ARCHITECTURE.md**: System design and patterns
- **CONTRIBUTING.md**: This file
- **API.md**: API reference (planned)

## 🐛 Bug Reports

### Good Bug Report Template

```markdown
**Description**
Clear description of the bug

**To Reproduce**
1. Create node X
2. Set value to Y
3. Call method Z
4. See error

**Expected Behavior**
What should happen

**Actual Behavior**
What actually happens

**Environment**
- OS: Windows 11
- Compiler: MSVC 2022
- CMake version: 3.27
- Build type: Debug

**Additional Context**
Stack traces, screenshots, etc.
```

## 💡 Feature Requests

### Feature Request Template

```markdown
**Feature Description**
Clear description of proposed feature

**Use Case**
Why is this feature needed?

**Proposed Solution**
How might this be implemented?

**Alternatives Considered**
Other ways to solve the problem

**Additional Context**
Examples, mockups, etc.
```

## 🎓 Academic Contributions

This is a study project! Academic contributions are highly valued:

- **Performance Analysis**: Benchmark results, profiling
- **Algorithm Improvements**: Better data structures, optimizations
- **Theoretical Analysis**: Complexity proofs, formal verification
- **Comparative Studies**: vs ECS, vs traditional OOP, etc.
- **Research Papers**: Academic papers referencing this project

## 📞 Questions & Discussion

- **Issues**: For bugs and features
- **Discussions**: For questions and ideas
- **Email**: [Your Email] for academic collaboration

## 📜 Code of Conduct

### Our Pledge

We are committed to providing a welcoming and inspiring community for all.

### Standards

- Be respectful and inclusive
- Accept constructive criticism
- Focus on what is best for the community
- Show empathy towards others

### Unacceptable Behavior

- Harassment or discriminatory language
- Personal or political attacks
- Trolling or insulting comments
- Publishing private information

## 📄 License

By contributing, you agree that your contributions will be licensed under the MIT License.

## 🙏 Recognition

Contributors will be acknowledged in:
- CONTRIBUTORS.md file
- Release notes
- Project documentation

Thank you for contributing to this academic project!
