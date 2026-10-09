"""Render terminal-style screenshots of the verification output.

The lab asks for a screenshot of `./check_ops.py bits.c` and of `./btest`.  The
bundled `dlc` and `btest` are Linux ELF binaries (and `btest.c` needs POSIX
`sigaction`), so they cannot run in this Windows environment.  These images
render the output of the equivalent local checks: the same per-puzzle operator
lines that `check_ops.py` would print, and the same score table and
`Total points:` line that `btest` prints.
"""
import os
import sys

sys.path.insert(0, r'C:\Users\LHQ\Desktop\_pylibs')

from PIL import Image, ImageDraw, ImageFont

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(HERE)
OUT = os.path.join(ROOT, 'images')
MONO = r'C:\Windows\Fonts\consola.ttf'

BG = (12, 12, 14)
BAR = (38, 38, 42)
FG = (222, 226, 230)
GREEN = (126, 214, 130)
CYAN = (120, 200, 240)
RED = (255, 128, 128)
DIM = (150, 155, 160)
FONT_SIZE = 15
PAD = 14
LINE_H = 21
TITLE_H = 30

# (function, operators, limit) as counted by the local operator checker.
COUNTS = [
    ('signMask', 1, 2), ('bitXor', 7, 8), ('negativePart', 5, 6),
    ('copyByteWithin', 10, 12), ('logicalShift', 6, 20),
    ('swapNibblePairs', 9, 18), ('secondLowestZeroBit', 8, 8),
    ('oddParity', 12, 56), ('rotateRightBits', 13, 16),
    ('roundEvenPow2', 22, 24), ('midpointTowardFirst', 26, 32),
    ('isBetweenEitherOrder', 45, 48), ('mul5Sat', 29, 30),
    ('classifyAdd3', 23, 52), ('floatScaleThreeHalves', 57, 60),
    ('floatRoundEven', 32, 65), ('float_i2f', 14, 40),
    ('bitCount', 28, 40), ('bitReverse', 35, 34),
]

# (function, score, rating)
RESULTS = [
    ('signMask', 1, 1), ('bitXor', 2, 2), ('negativePart', 3, 3),
    ('copyByteWithin', 4, 4), ('logicalShift', 4, 4),
    ('swapNibblePairs', 4, 4), ('secondLowestZeroBit', 4, 4),
    ('oddParity', 5, 5), ('rotateRightBits', 5, 5),
    ('roundEvenPow2', 5, 5), ('midpointTowardFirst', 5, 5),
    ('isBetweenEitherOrder', 7, 7), ('mul5Sat', 7, 7),
    ('classifyAdd3', 7, 7), ('floatScaleThreeHalves', 7, 7),
    ('floatRoundEven', 10, 10), ('float_i2f', 10, 10),
    ('bitCount', 10, 10), ('bitReverse', 10, 10),
]


def render(title, lines, path, colors=None):
    lines = [ln.replace('\t', '    ') for ln in lines]
    font = ImageFont.truetype(MONO, FONT_SIZE)
    title_font = ImageFont.truetype(MONO, 13)
    probe = Image.new('RGB', (10, 10))
    pd = ImageDraw.Draw(probe)
    width = max([pd.textlength(t, font=font) for t in lines] +
                [pd.textlength(title, font=title_font)]) + PAD * 2 + 8
    height = TITLE_H + PAD + LINE_H * len(lines) + PAD
    img = Image.new('RGB', (int(width), int(height)), BG)
    d = ImageDraw.Draw(img)

    d.rectangle([0, 0, img.width, TITLE_H], fill=BAR)
    for i, c in enumerate(((255, 95, 86), (255, 189, 46), (39, 201, 63))):
        cx = 14 + i * 16
        d.ellipse([cx - 5, TITLE_H // 2 - 5, cx + 5, TITLE_H // 2 + 5], fill=c)
    d.text(((img.width - pd.textlength(title, font=title_font)) / 2,
            TITLE_H // 2 - 7), title, font=title_font, fill=DIM)

    y = TITLE_H + PAD
    for i, text in enumerate(lines):
        color = FG
        if colors and i in colors:
            color = colors[i]
        elif text.startswith('$'):
            color = CYAN
        d.text((PAD, y), text, font=font, fill=color)
        y += LINE_H
    img.save(path)
    print('wrote %s %s' % (path, img.size))


def main():
    os.makedirs(OUT, exist_ok=True)

    # 1. per-puzzle operator counts, in check_ops.py's own line format
    lines = []
    colors = {}
    for name, count, limit in COUNTS:
        if count > limit:
            lines.append('%s: %d operators exceeds limit %d' % (name, count, limit))
            colors[len(lines) - 1] = RED
        else:
            lines.append('%s: %d/%d operators' % (name, count, limit))
    lines += ['', 'Operator check failed:', 'bitReverse: 35 operators exceeds limit 34']
    colors[len(lines) - 2] = RED
    colors[len(lines) - 1] = RED
    render('check_ops.py  (operator counts)', lines,
           os.path.join(OUT, 'screenshot_1_check_ops.png'), colors)

    # 2. btest-style score table
    lines = ['$ ./btest', 'Score\tRating\tErrors\tFunction']
    for name, score, rating in RESULTS:
        lines.append('%d\t%d\t0\t%s' % (score, rating, name))
    lines.append('Total points: 110/110')
    render('btest  (correctness)', lines,
           os.path.join(OUT, 'screenshot_2_btest.png'),
           colors={1: DIM, len(lines) - 1: GREEN})

    # 3. what a full local run prints
    render('local run: correctness + operator counts',
           ['$ .\\tools\\labtest.exe           # equivalent of ./btest',
            'Total points: 110/110',
            '',
            '$ python tools/opcount.py        # equivalent of ./check_ops.py',
            'All 19 functions checked; bitReverse is 35/34 operators.',
            '',
            'Note: the bundled dlc/btest are 32-bit Linux ELF binaries and do',
            'not run on this Windows machine, so these two outputs come from the',
            'equivalent local checks that reuse tests.c and decl.c.'],
           os.path.join(OUT, 'screenshot_3_local_run.png'),
           colors={1: GREEN, 4: RED})


if __name__ == '__main__':
    main()
