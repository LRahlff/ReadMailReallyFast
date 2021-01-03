#pragma once

#ifdef __GNUC__

#define ATTR_NONNULL_ALL __attribute__((nonnull))
#define ATTR_NONNULL(...) __attribute__((nonnull(__VA_ARGS__)))

#define ATTR_PRINTF(fmtpos, argpos) __attribute__(( format(printf, (fmtpos), (argpos) ) ))
#define ATTR_STRFTIME(fmtpos) __attribute__(( format(strftime, (fmtpos), 0 ) ))

#define ATTR_PACKED __attribute__((packed))
#define ATTR_UNUSED __attribute__((unused))
#define ATTR_WEAK __attribute__((weak))

#define ATTR_LIKELY(x) __builtin_expect((x), 1)
#define ATTR_UNLIKELY(x) __builtin_expect((x), 0)

#else

#define ATTR_NONNULL_ALL
#define ATTR_NONNULL(...)

#define ATTR_PRINTF(fmtpos, argpos)
#define ATTR_STRFTIME(fmtpos)

#define ATTR_PACKED
#define ATTR_UNUSED
#define ATTR_WEAK

#define ATTR_LIKELY(x) (x)
#define ATTR_UNLIKELY(x) (x)

#endif

#define MARK_UNUSED(x) (void)(x)
