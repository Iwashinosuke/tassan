import re

input_file = "keys.cfg"
output_file = "genkeys.h"

with open(input_file) as f:
    lines = f.readlines()

with open(output_file, "w") as out:
    out.write("// Auto-generated header\n")

    for line in lines:
        if "=" not in line:
            continue

        key, value = [v.strip() for v in line.split("=")]
        items = [v.strip() for v in value.split(",")]

        # 数値か文字列かを判定（文字列なら""を付与）
        def convert(v):
            return v if v.replace('.','',1).isdigit() else f"\'{v}\'"

        c_items = [convert(v) for v in items]

        out.write(f"#ifndef TS_GENKEYS_H\n  #define TS_GENKEYS_H\n\n")
        out.write(f"#define TS_{key}_COUNT {len(items)}\n")
        out.write(f"static const char TS_{key}[] = {{{','.join(c_items)}}};\n\n")
        out.write(f"#endif // TS_GENKEYS_H\n")