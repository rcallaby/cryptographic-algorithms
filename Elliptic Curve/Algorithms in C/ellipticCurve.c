#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/bn.h>

// Curve parameters for secp256k1
const char* curve_p = "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F";
const char* curve_a = "0000000000000000000000000000000000000000000000000000000000000000";
const char* curve_b = "0000000000000000000000000000000000000000000000000000000000000007";
const char* curve_Gx = "79BE667EF9DCBBAC55A06295CE870B07029BFCDB2DCE28D959F2815B16F81798";
const char* curve_Gy = "483ADA7726A3C4655DA4FBFC0E1108A8FD17B448A68554199C47D08FFB10D4B8";
const char* curve_n = "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEBAAEDCE6AF48A03BBFD25E8CD0364141";

typedef struct {
    BIGNUM *x;
    BIGNUM *y;
} ECPoint;

typedef struct {
    BIGNUM *a;
    BIGNUM *b;
    BIGNUM *p;
    ECPoint *G;
    BIGNUM *n;
} ECCurve;

void initialize_curve(ECCurve *curve) {
    curve->a = BN_new();
    curve->b = BN_new();
    curve->p = BN_new();
    curve->G = malloc(sizeof(ECPoint));
    curve->G->x = BN_new();
    curve->G->y = BN_new();
    curve->n = BN_new();

    BN_hex2bn(&curve->a, curve_a);
    BN_hex2bn(&curve->b, curve_b);
    BN_hex2bn(&curve->p, curve_p);
    BN_hex2bn(&curve->G->x, curve_Gx);
    BN_hex2bn(&curve->G->y, curve_Gy);
    BN_hex2bn(&curve->n, curve_n);
}

void point_addition(const ECCurve *curve, const ECPoint *P, const ECPoint *Q, ECPoint *R) {
    BIGNUM *lambda = BN_new();
    BIGNUM *temp1 = BN_new();
    BIGNUM *temp2 = BN_new();

    // Case P = Q (point doubling)
    if (BN_cmp(P->x, Q->x) == 0 && BN_cmp(P->y, Q->y) == 0) {
        // lambda = (3 * P.x^2 + curve.a) / (2 * P.y)
        BN_mod_sqr(temp1, P->x, curve->p); // temp1 = P.x^2
        BN_mod_mul_ui(temp1, temp1, 3, curve->p); // temp1 = 3 * P.x^2
        BN_mod_add(temp1, temp1, curve->a, curve->p); // temp1 = 3 * P.x^2 + curve.a
        BN_mod_mul_ui(temp2, P->y, 2, curve->p); // temp2 = 2 * P.y
        BN_mod_inverse(temp2, temp2, curve->p); // temp2 = 1 / (2 * P.y)
        BN_mod_mul(lambda, temp1, temp2, curve->p); // lambda = (3 * P.x^2 + curve.a) / (2 * P.y)
    }
    // Case P != Q (point addition)
    else {
        // lambda = (Q.y - P.y) / (Q.x - P.x)
        BN_mod_sub(temp1, Q->y, P->y, curve->p); // temp1 = Q.y - P.y
        BN_mod_sub(temp2, Q->x, P->x, curve->p); // temp2 = Q.x - P.x
        BN_mod_inverse(temp2, temp2, curve->p); // temp2 = 1 / (Q.x - P.x)
        BN_mod_mul(lambda, temp1, temp2, curve->p); // lambda = (Q.y - P.y) / (Q.x - P.x)
    }

    // R.x = (lambda^2 - P.x - Q.x) % p
    BN_mod_sqr(temp1, lambda, curve->p); // temp1 = lambda^2
    BN_mod_sub(temp1, temp1, P->x, curve->p); // temp1 = lambda^2 - P.x
    BN_mod_sub(R->x, temp1, Q->x, curve->p); // R.x = lambda^2 - P.x - Q.x

    // R.y = (lambda * (P.x - R.x) - P.y) % p
    BN_mod_sub(temp1, P->x, R->x, curve->p); // temp1 = P.x - R.x
    BN_mod_mul(temp1, temp1, lambda, curve->p); // temp1 = lambda * (P.x - R.x)
    BN_mod_sub(R->y, temp1, P->y, curve->p); // R.y = lambda * (P.x - R.x) - P.y

    BN_free(lambda);
    BN_free(temp1);
    BN_free(temp2);
}

void point_scalar_multiplication(const ECCurve *curve, const ECPoint *P, const BIGNUM *scalar, ECPoint *result) {
    ECPoint *Q = malloc(sizeof(ECPoint));
    ECPoint *R = malloc(sizeof(ECPoint));
    BN_CTX *ctx = BN_CTX_new();
    BN_CTX_start(ctx);

    BN_copy(Q->x, P->x);
    BN_copy(Q->y, P->y);

    BN_one(result->x);
    BN_one(result->y);

    int i, bit;
    int scalar_bits = BN_num_bits(scalar);

    for (i = 0; i < scalar_bits; i++) {
        bit = BN_is_bit_set(scalar, i);

        if (bit) {
            point_addition(curve, result, Q, R);
            BN_copy(result->x, R->x);
            BN_copy(result->y, R->y);
        }

        point_addition(curve, Q, Q, R);
        BN_copy(Q->x, R->x);
        BN_copy(Q->y, R->y);
    }

    BN_CTX_end(ctx);
    BN_CTX_free(ctx);
    free(Q);
    free(R);
}

void print_bn_hex(const char* name, const BIGNUM *bn) {
    char *hex_str = BN_bn2hex(bn);
    printf("%s: %s\n", name, hex_str);
    OPENSSL_free(hex_str);
}

int main() {
    ECCurve curve;
    initialize_curve(&curve);

    ECPoint P;
    P.x = BN_dup(curve.G->x);
    P.y = BN_dup(curve.G->y);

    BIGNUM *private_key = BN_new();
    BN_rand_range(private_key, curve.n); // Generate a random private key within the range [1, n-1]

    ECPoint public_key;
    public_key.x = BN_new();
    public_key.y = BN_new();

    point_scalar_multiplication(&curve, &P, private_key, &public_key);

    print_bn_hex("Private Key", private_key);
    print_bn_hex("Public Key (x)", public_key.x);
    print_bn_hex("Public Key (y)", public_key.y);

    BN_free(private_key);
    BN_free(public_key.x);
    BN_free(public_key.y);
    BN_free(P.x);
    BN_free(P.y);
    BN_free(curve.a);
    BN_free(curve.b);
    BN_free(curve.p);
    BN_free(curve.n);
    BN_free(curve.G->x);
    BN_free(curve.G->y);
    free(curve.G);

    return 0;
}
