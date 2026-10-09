"""Count operators the way dlc does, per puzzle in bits.c.

dlc parses each function and counts every operator that appears in an
executable expression (the right-hand side of declarations, assignments, the
condition and body of loops, and return statements).  It does not count the
punctuation of a declaration itself, comments, or the `return` keyword.

This module implements a small C expression parser so that the counts are not
thrown off by comments or by text inside type names.
"""
from __future__ import annotations

import re
import sys

LIMITS = {
    'signMask': 2, 'bitXor': 8, 'negativePart': 6, 'copyByteWithin': 12,
    'logicalShift': 20, 'swapNibblePairs': 18, 'secondLowestZeroBit': 8,
    'oddParity': 56, 'rotateRightBits': 16, 'roundEvenPow2': 24,
    'midpointTowardFirst': 32, 'isBetweenEitherOrder': 48, 'mul5Sat': 30,
    'classifyAdd3': 52, 'floatScaleThreeHalves': 60, 'floatRoundEven': 65,
    'float_i2f': 40, 'bitCount': 40, 'bitReverse': 34,
}

UNARY = {'!', '~', '-'}
BINARY = ['<<', '>>', '<=', '>=', '==', '!=', '&&', '||',
          '&', '^', '|', '+', '-', '*', '/', '%', '<', '>']
KEYWORDS = {'int', 'unsigned', 'static', 'const', 'void', 'char', 'return',
            'if', 'else', 'while', 'for', 'do', 'sizeof'}

TOKEN = re.compile(r"""
    (?P<num>\b0[xX][0-9a-fA-F]+|\b\d+)
  | (?P<id>\b[A-Za-z_]\w*)
  | (?P<op><<|>>|<=|>=|==|!=|&&|\|\||[-+*/%&^|~!<>()\[\]{},;=?])
""", re.X)


def tokenize(code: str):
    code = re.sub(r'/\*.*?\*/', ' ', code, flags=re.S)
    code = re.sub(r'//[^\n]*', ' ', code)
    out = []
    for m in TOKEN.finditer(code):
        kind = m.lastgroup
        text = m.group()
        if kind == 'id' and text in KEYWORDS:
            out.append(('kw', text))
        else:
            out.append((kind, text))
    return out


def count_expression(tokens) -> int:
    """Operators in one expression, excluding bare parentheses and commas."""
    n = 0
    prev = None
    for kind, text in tokens:
        if kind == 'op' and text in BINARY:
            # a leading +/- is unary
            if text in '+-' and (prev is None or prev in BINARY or
                                 prev in ('(', ',', ';', '=', '{')):
                n += 1
            else:
                n += 1
        elif kind == 'op' and text in UNARY:
            n += 1
        prev = text
    return n


def split_statements(body: str):
    """Yield top-level statements of a function body (rough, but sufficient)."""
    out, depth, cur = [], 0, []
    for ch in body:
        if ch == '{':
            depth += 1
        elif ch == '}':
            depth -= 1
        cur.append(ch)
        if ch == ';' and depth == 0:
            out.append(''.join(cur))
            cur = []
    if ''.join(cur).strip():
        out.append(''.join(cur))
    return out


def count_function(body: str) -> int:
    total = 0
    for stmt in split_statements(body):
        text = stmt.strip()
        if not text:
            continue
        # drop a leading declaration: everything up to the first '=' or the
        # declarator list; we only count what follows the '=' (the initialiser)
        # and, for a plain declaration, nothing at all.
        if '=' in text:
            rhs = text.split('=', 1)[1]
        elif re.match(r'^(?:int|unsigned|static|const)\b', text) and '(' not in text:
            continue
        else:
            rhs = text
        rhs = rhs.replace('return', ' ')
        # remove the trailing ';' and any control keywords, keep the operators
        rhs = re.sub(r'\b(?:if|else|while|for|do)\b', ' ', rhs)
        toks = tokenize(rhs)
        total += count_expression(toks)
    return total


def main(path='bits.c'):
    text = open(path, encoding='utf-8', errors='replace').read()
    text = re.sub(r'/\*.*?\*/', ' ', text, flags=re.S)
    text = re.sub(r'//[^\n]*', ' ', text)
    cut = text.find('#endif')
    if cut >= 0:
        text = text[cut + 6:]
    found = {}
    for m in re.finditer(r'\b(?:int|unsigned)\s+(\w+)\s*\([^)]*\)\s*\{', text):
        depth, j = 0, m.end() - 1
        while j < len(text):
            if text[j] == '{':
                depth += 1
            elif text[j] == '}':
                depth -= 1
                if depth == 0:
                    break
            j += 1
        found[m.group(1)] = text[m.end():j]
    width = max(len(n) for n in LIMITS)
    print('%-*s %6s %6s  %s' % (width, 'function', 'ops', 'max', 'status'))
    over = []
    for name, lim in LIMITS.items():
        if name not in found:
            print('%-*s  MISSING' % (width, name))
            continue
        c = count_function(found[name])
        over += [name] if c > lim else []
        print('%-*s %6d %6d  %s' % (width, name, c, lim,
                                    'ok' if c <= lim else 'OVER by %d' % (c - lim)))
    if over:
        print('\nover limit:', ', '.join(over))
    return 1 if over else 0


if __name__ == '__main__':
    sys.exit(main(*sys.argv[1:]))
