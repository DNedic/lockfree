# Changelog

## 1.0.1

- Added static asserts to verify the template type arguments are trivial types
- Added static asserts to verify the minimum size
- Moved all the `relaxed` atomic loads before the `acquire` loads, potentially increasing performance, there was no need for these to be inside fences.
- Updated the documentation in various places

## 1.0.0

- Initial release
