typedef struct {
    const char *apk_name;
    uint64_t start_offset;
    uint64_t end_offset;
} ApkEntry;
extern ApkEntry apk_table[];
extern const size_t apk_table_size;
const ApkEntry* find_apk(const char *name);
