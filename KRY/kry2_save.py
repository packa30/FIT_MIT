#!/usr/bin/env python3

# autor: Peter Suhaj (xsuhaj02)
# predmet: KRY 2019L
# zadanie: 2. projekt

import sys


# parametry ecc
a = int("-0x3", 16)
b = int("0x5ac635d8aa3a93e7b3ebbd55769886bc651d06b0cc53b0f63bce3c3e27d2604b", 16)
Fp = int("0xffffffff00000001000000000000000000000000ffffffffffffffffffffffff", 16)

# zakladny bod
x_p = int("0x6b17d1f2e12c4247f8bce6e563a440f277037d812deb33a0f4a13945d898c296", 16)
y_p = int("0x4fe342e2fe1a7f9b8ee7eb4a7c0f9e162bce33576b315ececbb6406837bf51f5", 16)


# ziskanie inverzneho prvku pre nasobenie
def get_inverse_mul(x):
    if (x >= Fp):
        x = x % Fp

    y = pow(x, Fp - 2, Fp)
    return y


# ziskanie inverzneho prvku pre scitanie
def get_inverse_add(x):
    if (x >= Fp):
        x = x % Fp
    return Fp - x


# overenie validity bodu
def check_points(x, y):
    if (y ** 2) % Fp != ((x ** 3) + a * x + b) % Fp:
        print("Neplatny bod!")
        exit(1)


# scitanie bodov
def add_points(x_q, y_q):
    l = ((y_q + get_inverse_add(y_p)) * get_inverse_mul(x_q + get_inverse_add(x_p))) % Fp
    x = (pow(l, 2, Fp) + get_inverse_add(x_p) + get_inverse_add(x_q)) % Fp
    y = (l*(x_p + get_inverse_add(x)) + get_inverse_add(y_p)) % Fp
    return x, y


# zdvojenie bodu
def double_points():
    l = ((pow(x_p, 2, Fp) + pow(x_p, 2, Fp) + pow(x_p, 2, Fp) + a) * get_inverse_mul(y_p + y_p)) % Fp
    x = (pow(l, 2, Fp) + get_inverse_add(x_p + x_p)) % Fp
    y = (l * (x_p + get_inverse_add(x)) + get_inverse_add(y_p)) % Fp
    return x, y


def main(argv):
    global x_p
    global y_p

    # nacitanie vstupu
    pub_key = argv[1][1:len(argv[1])-1]
    result = [x.strip() for x in pub_key.split(',')]
    X = int(result[0], 16)
    Y = int(result[1], 16)

    # kontrola platnosti
    check_points(X, Y)

    # inicializacie privatneho kluca
    n = 1

    # ak vstup sa rovna zakladnemu bodu tak priv. kl. je 1
    if (x_p == X and y_p == Y):
        print(n)
        exit(0)


    # prvy krok - double points -> dva rovnake body
    x_q, y_q = double_points()
    n += 1

    # scitujeme dalej az kym sa bod nerovna verejnemu klucu
    while X != x_q and Y != y_q:
        x_q, y_q = add_points(x_q, y_q)
        n += 1

    print(n)


if __name__ == "__main__":
    main(sys.argv)

