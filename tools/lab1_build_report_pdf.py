"""Build the DataLab report PDF from docs/report.md.

A compact Markdown subset is supported: ATX headings, fenced code blocks,
bullet lists, simple pipe tables and paragraphs.  The body font is a CJK TTF so
that Chinese text renders; code uses the same face for reliability.
"""
import os
import re
import sys

sys.path.insert(0, r'C:\Users\LHQ\Desktop\_pylibs')

from fpdf import FPDF
from fpdf.enums import WrapMode

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(HERE)
SRC = os.path.join(ROOT, 'docs', 'report.md')
OUT = os.path.join(ROOT, 'docs', '25300190071.pdf')

FONT_CANDIDATES = [
    r'C:\Windows\Fonts\simhei.ttf',
    r'C:\Windows\Fonts\msyh.ttc',
    r'C:\Windows\Fonts\msyhbd.ttc',
]


def pick_font():
    for path in FONT_CANDIDATES:
        if os.path.exists(path):
            return path
    raise SystemExit('no CJK font found')


class Report(FPDF):
    def __init__(self, font):
        super().__init__(orientation='P', unit='mm', format='A4')
        self.font_path = font
        self.set_auto_page_break(auto=True, margin=18)
        self.add_font('cn', '', font)
        self.set_font('cn', size=10.5)
        self.set_margins(18, 16, 18)

    def header(self):
        if self.page_no() == 1:
            return
        self.set_font('cn', size=8.5)
        self.set_text_color(120, 120, 120)
        self.cell(0, 6, 'CS:APP Data Lab  25300190071 李和桥', align='R',
                  new_x='LMARGIN', new_y='NEXT')
        self.set_text_color(0, 0, 0)
        self.ln(1)

    def footer(self):
        self.set_y(-14)
        self.set_font('cn', size=8.5)
        self.set_text_color(120, 120, 120)
        self.cell(0, 6, '- %d -' % self.page_no(), align='C')
        self.set_text_color(0, 0, 0)


def wrap(text, pdf, size, style=None):
    pdf.set_font('cn', size=size)
    pdf.multi_cell(0, size * 0.52, text, align='L',
                   new_x='LMARGIN', new_y='NEXT', wrapmode=WrapMode.CHAR)


def code_block(pdf, lines):
    pdf.set_font('cn', size=8.8)
    pdf.set_fill_color(245, 245, 247)
    for line in lines:
        line = line.replace('\t', '    ')
        pdf.multi_cell(0, 4.2, line if line.strip() else ' ', align='L',
                       fill=True, new_x='LMARGIN', new_y='NEXT',
                       wrapmode=WrapMode.CHAR)
    pdf.ln(1.5)


def table(pdf, rows):
    widths = None
    pdf.set_font('cn', size=9)
    for i, row in enumerate(rows):
        if widths is None:
            n = len(row)
            widths = [180.0 / n] * n
        pdf.set_fill_color(238, 240, 244) if i == 0 else pdf.set_fill_color(255, 255, 255)
        for cell, w in zip(row, widths):
            pdf.cell(w, 6.5, cell, border=1, fill=True, align='L')
        pdf.ln()
    pdf.ln(1.5)


def main():
    font = pick_font()
    text = open(SRC, encoding='utf-8').read()
    lines = text.split('\n')

    pdf = Report(font)
    pdf.add_page()

    in_code = False
    code_lines = []
    table_rows = []

    def flush_table():
        if table_rows:
            table(pdf, table_rows[:])
            table_rows.clear()

    for raw in lines:
        line = raw.rstrip('\n')

        if line.startswith('```'):
            if in_code:
                code_block(pdf, code_lines)
                code_lines = []
                in_code = False
            else:
                flush_table()
                in_code = True
            continue

        if in_code:
            code_lines.append(line)
            continue

        if line.startswith('|') and line.endswith('|'):
            cells = [c.strip() for c in line.strip('|').split('|')]
            if all(re.fullmatch(r':?-{2,}:?', c) for c in cells):
                continue
            cells = [re.sub(r'`([^`]*)`', r'\1', c) for c in cells]
            cells = [re.sub(r'\*\*(.+?)\*\*', r'\1', c) for c in cells]
            table_rows.append(cells)
            continue
        flush_table()

        if not line.strip():
            pdf.ln(1.6)
            continue

        if line.startswith('#'):
            level = len(line) - len(line.lstrip('#'))
            title = line[level:].strip()
            flush_table()
            sizes = {1: 17, 2: 13.5, 3: 11.5, 4: 10.5}
            pdf.ln(2.2)
            pdf.set_font('cn', size=sizes.get(level, 10.5))
            pdf.multi_cell(0, sizes.get(level, 10.5) * 0.55, title, align='L',
                           new_x='LMARGIN', new_y='NEXT',
                           wrapmode=WrapMode.CHAR)
            pdf.ln(1.0)
            continue

        # image: ![alt](relative/path)
        m = re.match(r'^!\[([^\]]*)\]\(([^)]+)\)\s*$', line.strip())
        if m:
            flush_table()
            rel = m.group(2)
            img_path = os.path.join(ROOT, rel.replace('/', os.sep))
            if os.path.exists(img_path):
                from PIL import Image
                with Image.open(img_path) as im:
                    w, h = im.size
                avail = pdf.w - pdf.l_margin - pdf.r_margin
                draw_w = min(avail, 150.0)
                draw_h = draw_w * h / float(w)
                if pdf.get_y() + draw_h > pdf.h - pdf.b_margin:
                    pdf.add_page()
                pdf.image(img_path, x=(pdf.w - draw_w) / 2.0, w=draw_w)
                pdf.ln(2)
            else:
                wrap('[missing image: %s]' % rel, pdf, 10)
            continue

        if line.startswith('- '):
            wrap('  - ' + line[2:], pdf, 10.5)
            continue

        # bold spans -> keep the text, drop the markers
        clean = re.sub(r'\*\*(.+?)\*\*', r'\1', line)
        clean = re.sub(r'`([^`]*)`', r'\1', clean)
        wrap(clean, pdf, 10.5)

    flush_table()
    pdf.output(OUT)
    print('wrote', OUT)


if __name__ == '__main__':
    main()
