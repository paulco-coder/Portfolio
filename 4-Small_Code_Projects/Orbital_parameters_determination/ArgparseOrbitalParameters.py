import math as m
import argparse

# Defining the important functions of our program

def norm(vector):
    return (vector[0]**2 + vector[1]**2 + vector[2]**2)**0.5

def cross_product(v1, v2):
    return [v1[1]*v2[2] - v1[2]*v2[1], v1[2]*v2[0] - v1[0]*v2[2], v1[0]*v2[1] - v1[1]*v2[0]]

def dot_product(v1, v2):
    return v1[0]*v2[0] + v1[1]*v2[1] + v1[2]*v2[2]

def parse_arguments():
    parser = argparse.ArgumentParser(description="Calculate orbital parameters.")
    parser.add_argument("--canonical", action="store_true", help="Use canonical units")
    parser.add_argument("--position", type=float, nargs=3, required=True, help="Position vector components separated by spaces")
    parser.add_argument("--velocity", type=float, nargs=3, required=True, help="Velocity vector components separated by spaces")
    return parser.parse_args()

# Main function
def main():
    args = parse_arguments()

    if args.canonical:
        GM = 1
        distance_unit = "DU"
        print("Canonical units are being used.")
    else:
        GM = 398600.442
        distance_unit = "km"

    r = args.position
    v = args.velocity

    h = cross_product(r, v)
    E = (norm(v)**2 / 2) - (GM / norm(r))
    a = -GM / (2 * E)
    evector = [cross_product(v, h)[0] / GM - r[0] / norm(r), cross_product(v, h)[1] / GM - r[1] / norm(r), cross_product(v, h)[2] / GM - r[2] / norm(r)]
    e = norm(evector)
    i = m.acos(h[2] / norm(h))

    n = cross_product([0, 0, 1], h)
    if n[1] >= 0:
        Ω = m.acos(-n[0] / norm(n))
    else:
        Ω = m.pi + m.acos(-n[0] / norm(n))

    if evector[2] >= 0:
        ω = m.acos(dot_product(n, evector) / (norm(n) * e))
    else:
        ω = m.pi + m.acos(dot_product(n, evector) / (norm(n) * e))

    if dot_product(r, v) >= 0:
        θ = m.acos(dot_product(r, evector) / (norm(r) * e))
    else:
        θ = m.pi + m.acos(dot_product(r, evector) / (norm(r) * e))

    print(f'a= {a:.10f} {distance_unit}')
    print(f'e= {e:.10f}')
    print(f'i= {i:.10f} radians = {m.degrees(i):.12} degrees')
    print(f'Ω= {Ω:.10f} radians = {m.degrees(Ω):.12} degrees')
    print(f'ω= {ω:.10f} radians = {m.degrees(ω):.12} degrees')
    print(f'θ= {θ:.10f} radians = {m.degrees(θ):.12} degrees')

if __name__ == "__main__":
    main()
