# Unit Test Coverage Matrix

| Component | Behaviors exercised | Follow-up gaps |
|---|---|---|
| LogLevel | All level labels; invalid enum fallback | Compile-time assertions for enum ordering |
| LogField | bool, signed/unsigned integer, double, string alternatives; moved string | Long double policy; invalid/duplicate keys policy |
| LoggingConfig | Defaults; all supported keys; case-insensitive levels; aliases; comments; whitespace; backend options; missing file; malformed line; unknown key; invalid level/boolean | Duplicate-key semantics; embedded `=`; read I/O error; path edge cases |
| Logger | Record contents; source metadata; context composition; severity thresholds; Off; inactive runtime; empty values; multithreaded event count | Concurrent shutdown/reinitialize stress; allocation failure behavior |
| Backend lifecycle | registration validation; duplicate; missing factory; null factory result; initialized state; flush; shutdown; replacement | Factory throws; backend write/flush exception behavior; reentrant backend contract |
| Console backend | Severity/logger/message/field output; timestamp disabled | Timestamp enabled format; thread ID; source location; escaping/control characters; locale |
| Packaging | Existing CI builds and tests; installed consumer compile/run | Add package consumer to local validation matrix and header self-containment tests |

The listed tests assert current behavior. Follow-up gaps are intentionally not claimed as covered. Tests should be added before implementing those behaviors, following the project's TDD policy.
