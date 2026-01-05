# Contributing Guidelines

Thank you for your interest in improving HashMap-Implementation!

## Getting Started

1. **Fork** the repository
2. **Clone** your fork locally
3. **Create** a feature branch (`git checkout -b feature/amazing-feature`)
4. **Make** your changes
5. **Test** thoroughly (see below)
6. **Commit** with clear messages
7. **Push** to your fork
8. **Open** a Pull Request

## Development Setup

```bash
# Clone and build
git clone https://github.com/YOUR-USERNAME/HashMap-Implementation-.git
cd HashMap-Implementation-
make clean
make hashmap_tests
```

## Testing Requirements

All contributions must pass the test suite:

```bash
# Run all tests (must pass)
make test_hashmap_all

# Run specific test categories
make test_hashmap_core      # Core operations
make test_hashmap_aug       # Augmented features
```

**Test Results:**

- ✅ 53/53 tests must pass
- ✅ No memory leaks (AddressSanitizer)
- ✅ No undefined behavior (UBSan)

## Code Style

- **Indentation:** 2 spaces
- **Naming:** camelCase for variables, PascalCase for types
- **Comments:** Clear, concise, explain the "why" not the "what"
- **Header Comments:** Include complexity analysis (Big-O)
- **No compiler warnings:** Compile with `-Wall -Wextra -Werror`

Example:

```cpp
// Inserts key-value pair at front of chain.
// Time: O(L) where L is chain length; O(1) average case.
void insert(KeyT key, ValT value) {
  // Implementation...
}
```

## Documentation

- Update **README.md** if adding features
- Update **QUICKSTART.md** with usage examples
- Add file headers for new components
- Document API changes clearly

## Commit Messages

Use clear, descriptive commit messages:

```
Good:   "Add iterator reset functionality to begin()"
Bad:    "fix stuff"
Better: "Implement O(B) begin() iterator reset with bucket scanning"
```

## Pull Request Process

1. **Title:** Brief description of changes
2. **Description:**
   - What problem does this solve?
   - How does it work?
   - Any trade-offs or limitations?
3. **Tests:** Confirm all tests pass
4. **Review:** Address feedback from maintainer

## What We're Looking For

### ✅ Welcome

- Bug fixes with test cases
- Performance improvements (with benchmarks)
- Documentation improvements
- Code cleanup and refactoring

### ❌ Not Currently Accepting

- Major architectural changes (discuss in issues first)
- Alternative data structures (out of scope)
- Language-specific features

## Questions?

- Check [README.md](README.md) for overview
- See [QUICKSTART.md](QUICKSTART.md) for usage
- Review [GITHUB_CHECKLIST.md](GITHUB_CHECKLIST.md) for details

---

**Thank you for contributing!** 🎉
