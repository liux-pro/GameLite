# nes模拟器调色板从rgb888转为rgb565

# https://zhuanlan.zhihu.com/p/34144965
# https://github.com/jay-kumogata/InfoNES/blob/5c6b238b6f5bde3c09c9d8f841558d0342f19d14/src/sdl/InfoNES_System_SDL.cpp#L81

rgb888 = [
    112, 112, 112,     32,  24, 136,      0,   0, 168,     64,   0, 152,
    136,   0, 112,    168,   0,  16,    160,   0,   0,    120,   8,   0,
    64,  40,   0,      0,  64,   0,      0,  80,   0,      0,  56,  16,
    24,  56,  88,      0,   0,   0,      0,   0,   0,      0,   0,   0,
    184, 184, 184,      0, 112, 232,     32,  56, 232,    128,   0, 240,
    184,   0, 184,    224,   0,  88,    216,  40,   0,    200,  72,   8,
    136, 112,   0,      0, 144,   0,      0, 168,   0,      0, 144,  56,
    0, 128, 136,      0,   0,   0,      0,   0,   0,      0,   0,   0,
    248, 248, 248,     56, 184, 248,     88, 144, 248,     64, 136, 248,
    240, 120, 248,    248, 112, 176,    248, 112,  96,    248, 152,  56,
    240, 184,  56,    128, 208,  16,     72, 216,  72,     88, 248, 152,
    0, 232, 216,      0,   0,   0,      0,   0,   0,      0,   0,   0,
    248, 248, 248,    168, 224, 248,    192, 208, 248,    208, 200, 248,
    248, 192, 248,    248, 192, 216,    248, 184, 176,    248, 216, 168,
    248, 224, 160,    224, 248, 160,    168, 240, 184,    176, 248, 200,
    152, 248, 240,      0,   0,   0,      0,   0,   0,      0,   0,   0,
]


result_list = []

size = len(rgb888)//3

for i in range(size):
    r = rgb888[i*3]
    g = rgb888[i*3+1]
    b = rgb888[i*3+2]
    result = r >> 3 << 11 | g >> 2 << 5 | b >> 3
    result = ((result & 0xff) << 8) | ((result & 0xff00) >> 8)
    result_list.append(str(hex(result)))
print("{" + ",".join(result_list) + "};")

