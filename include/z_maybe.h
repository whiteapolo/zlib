#define Z_DEFINE_MAYBE(identifier, type) \
typedef struct {                         \
  bool ok;                               \
  type value;                            \
} identifier

#define Z_MAYBE_NOT(identifier) ((identifier){ .ok = false })
#define Z_MAYBE_YES(identifier, val) ((identifier){ .ok = true, .value = val })
