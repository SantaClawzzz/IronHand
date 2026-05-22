"""
Convert tarkov.jpg to an RGB565 C header for use as an LVGL image on the
ST7735R display (160x80 landscape after 90° CW rotation).

Usage:
    python convert_image.py

Output:
    src/display/tarkov_img_data.h
"""

from PIL import Image
import os

TARGET_W = 160
TARGET_H = 80
OUTPUT_PATH = os.path.join(os.path.dirname(__file__), "src", "display", "tarkov_img_data.h")


def rgb888_to_rgb565_le(r, g, b):
    r5 = (r >> 3) & 0x1F
    g6 = (g >> 2) & 0x3F
    b5 = (b >> 3) & 0x1F
    val = (r5 << 11) | (g6 << 5) | b5
    lo = val & 0xFF
    hi = (val >> 8) & 0xFF
    return lo, hi


def main():
    src = os.path.join(os.path.dirname(__file__), "tarkov.jpg")
    img = Image.open(src).convert("RGB")
    img = img.resize((TARGET_W, TARGET_H), Image.LANCZOS)

    os.makedirs(os.path.dirname(OUTPUT_PATH), exist_ok=True)

    pixels = list(img.getdata())
    data = []
    for r, g, b in pixels:
        lo, hi = rgb888_to_rgb565_le(r, g, b)
        data.append(lo)
        data.append(hi)

    with open(OUTPUT_PATH, "w") as f:
        f.write("#pragma once\n")
        f.write("#include <stdint.h>\n\n")
        f.write(f"#define TARKOV_IMG_W {TARGET_W}\n")
        f.write(f"#define TARKOV_IMG_H {TARGET_H}\n\n")
        f.write(f"static const uint8_t tarkov_img_data[{len(data)}] = {{\n")
        for i, byte in enumerate(data):
            if i % 16 == 0:
                f.write("    ")
            f.write(f"0x{byte:02X}")
            if i < len(data) - 1:
                f.write(", ")
            if (i + 1) % 16 == 0:
                f.write("\n")
        if len(data) % 16 != 0:
            f.write("\n")
        f.write("};\n")

    print(f"Written {len(data)} bytes ({TARGET_W}x{TARGET_H} RGB565) -> {OUTPUT_PATH}")


if __name__ == "__main__":
    main()
