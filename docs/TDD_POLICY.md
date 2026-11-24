# Suchi Test-Driven Development Policy

## Mandatory development loop

1. **Red:** Add or modify a focused test that fails for the intended reason. Confirm the failure before changing production code.
2. **Green:** Implement the smallest production change that makes the test pass.
3. **Refactor:** Improve structure and remove duplication while preserving green tests.
4. **Regression:** Run the full CTest suite and the installed-consumer build when public headers, CMake exports, or ABI-facing declarations change.

A change is not complete merely because it compiles. Every externally observable behavior and reported defect must have a regression test.

## Test organization

- `test_log_level.cpp`: severity names and invalid enum fallback.
- `test_fields.cpp`: structured field type preservation and value ownership.
- `test_config.cpp`: defaults, supported values, whitespace, comments, unknown keys, malformed lines, missing files, and invalid values.
- `test_logger.cpp`: event contents, source location, contextual fields, threshold filtering, pre/post initialization behavior, empty inputs, and concurrent writers.
- `test_backend_lifecycle.cpp`: registration validation, duplicate registration, unknown/null factories, initialization transitions, flush, shutdown, and reinitialization.
- `test_console_backend.cpp`: output layout and configurable timestamp behavior.

## Test quality requirements

- Prefer in-memory/fake backends over assertions against global console output.
- Test both positive and negative behavior, boundary values, and failure paths.
- Avoid order dependence between test cases. Reset test-owned state explicitly.
- Keep tests deterministic; do not assert wall-clock timing or scheduler ordering.
- Do not log patient data, credentials, or other secrets in tests or fixtures.
- Use concurrency tests for data integrity, and ThreadSanitizer in CI when the supported toolchain permits it.
- Never suppress a failing test simply to obtain a green pipeline. Fix the behavior or correct an invalid test expectation with an explanation.

## Coverage

Coverage is a diagnostic, not a substitute for meaningful tests. Review uncovered branches in configuration parsing, initialization failures, backend dispatch, and shutdown paths. Set a project coverage target only after obtaining a stable baseline and ensuring coverage instrumentation is supported by all CI platforms.
