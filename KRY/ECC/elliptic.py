# Author: Peter Grofcik, xgrofc00
# Date: May 2021

import sys

Fp = 0xffffffff00000001000000000000000000000000ffffffffffffffffffffffff
a = -0x3
b = 0x5ac635d8aa3a93e7b3ebbd55769886bc651d06b0cc53b0f63bce3c3e27d2604b

xp = 0x6b17d1f2e12c4247f8bce6e563a440f277037d812deb33a0f4a13945d898c296
yp = 0x4fe342e2fe1a7f9b8ee7eb4a7c0f9e162bce33576b315ececbb6406837bf51f5


# compute lambda / Inverted value of divider used for point doubling and adding / dividend & divider expresion required
def lamb(dividend, divider):
    return (dividend * pow(divider, Fp-2, Fp)) % Fp


# compute Rx value for doubling/adding / s = lambda value / p = 2*Px or Px+Qx depending on doubling/adding
def Rx(s, p):
    return (pow(s, 2) - p) % Fp


# compute Ry value for doubling/adding / s = lambda value / p = computed Rx
def Ry(s, p):
    return (s * (xp - p) - yp) % Fp


# Point doubling / dividend = 3*Px^2+a / divider = 2*Py / lam = dividend/divider (*using Inverted point)/ p = 2*Px
def pointDoubling():
    dividend = 3 * pow(xp, 2) + a
    divider = 2 * yp
    lam = lamb(dividend, divider)
    x = Rx(lam, 2 * xp)
    return x, Ry(lam, x)


# Point doubling / dividend = Py-Qy / divider = Px-Qx / lam = dividend/divider (*using Inverted point)/ p = Px+Qx /
# pk = (Px,Py) - computed x/y from last cycle
def pointAdding(pk):
    dividend = pk[1]-yp
    divider = pk[0]-xp
    lam = lamb(dividend, divider)
    x = Rx(lam, pk[0] + xp)
    return x, Ry(lam, x)


def main(public):
    # parse obtained input (0x..., 0x...) -> [0x..., 0x...]
    vk = [int(pub.strip(), 16) for pub in public[1:len(public)-1].split(',')]
    counter = 1

    if vk[0] == xp and vk[1] == yp:
        print(counter)
    else:
        # 2P = P + P = R / double points
        pk = pointDoubling()
        counter += 1

        # Point adding 3P = R + P, 4P, 5P... -> until point is the same as vk(public key)
        while pk[0] != vk[0] and pk[1] != vk[0]:
            pk = pointAdding(pk)
            counter += 1
        print(counter)


if __name__ == "__main__":
    main(sys.argv[1])
