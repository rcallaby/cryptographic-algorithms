#include <iostream>
#include <cmath>
#include <vector>

// Define the parameters of the elliptic curve
const int a = 1;  // coefficient a
const int b = 6;  // coefficient b
const int p = 13; // prime modulus
const int Gx = 2; // x-coordinate of the base point
const int Gy = 7; // y-coordinate of the base point

// Structure to represent a point on the elliptic curve
struct Point {
    int x, y;
};

// Check if a number is a quadratic residue modulo p
bool isQuadraticResidue(int n) {
    return (std::pow(n, (p - 1) / 2) == 1);
}

// Modulo inverse of a number
int modInverse(int a, int m) {
    int m0 = m;
    int y = 0, x = 1;

    if (m == 1)
        return 0;

    while (a > 1) {
        int q = a / m;
        int t = m;

        m = a % m;
        a = t;
        t = y;

        y = x - q * y;
        x = t;
    }

    if (x < 0)
        x += m0;

    return x;
}

// Elliptic curve addition
Point ellipticCurveAddition(Point p1, Point p2) {
    Point result;

    int m;
    if (p1.x == p2.x) {
        // Point doubling
        m = ((3 * std::pow(p1.x, 2) + a) * modInverse(2 * p1.y, p)) % p;
    } else {
        // Point addition
        m = ((p2.y - p1.y) * modInverse(p2.x - p1.x, p)) % p;
    }

    result.x = (std::pow(m, 2) - p1.x - p2.x + 2 * p) % p;
    result.y = ((m * (p1.x - result.x)) - p1.y + 2 * p) % p;

    return result;
}

// Elliptic curve multiplication
Point ellipticCurveMultiplication(Point p, int k) {
    Point result = {0, 0};

    for (int i = 0; i < k; i++) {
        result = ellipticCurveAddition(result, p);
    }

    return result;
}

// Print the coordinates of a point
void printPoint(Point p) {
    std::cout << "(" << p.x << ", " << p.y << ")" << std::endl;
}

int main() {
    // Generate the base point
    Point basePoint = {Gx, Gy};

    // Generate a random scalar
    int scalar = 5;

    // Perform scalar multiplication
    Point publicKey = ellipticCurveMultiplication(basePoint, scalar);

    // Print the public key
    std::cout << "Public key: ";
    printPoint(publicKey);

    return 0;
}
