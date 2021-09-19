/*
	http://en.wikipedia.org/wiki/SHA-3, http://nvlpubs.nist.gov/nistpubs/FIPS/NIST.FIPS.202.pdf
	Written by Jeff Garzik <jeff@garzik.org> for module of GNU/Linux kernel from https://lwn.net/Articles/518415/
	LICENSE GPL
	Originally from linux-4.11/crypto/sha3_generic.c SHA3_256:9a0a3fecbb5a1791895a854ddaae802308f1e75ad42f6e973fa2a43f38e2216f
*/

#include <stdint.h>
#include <string.h>
#include "sha3_impl.h"

#define KECCAK_ROUNDS 24

#define ROTL64(x, y) (((x) << (y)) | ((x) >> (64 - (y))))

static const uint64_t keccakf_rndc[24] =
{
	0x0000000000000001ULL, 0x0000000000008082ULL, 0x800000000000808aULL, 0x8000000080008000ULL, 0x000000000000808bULL, 0x0000000080000001ULL,
	0x8000000080008081ULL, 0x8000000000008009ULL, 0x000000000000008aULL, 0x0000000000000088ULL, 0x0000000080008009ULL, 0x000000008000000aULL,
	0x000000008000808bULL, 0x800000000000008bULL, 0x8000000000008089ULL, 0x8000000000008003ULL, 0x8000000000008002ULL, 0x8000000000000080ULL,
	0x000000000000800aULL, 0x800000008000000aULL, 0x8000000080008081ULL, 0x8000000000008080ULL, 0x0000000080000001ULL, 0x8000000080008008ULL
};

static const int keccakf_rotc[24] = 
{
	1,  3,  6,  10, 15, 21, 28, 36, 45, 55, 2,  14, 
	27, 41, 56, 8,  25, 43, 62, 18, 39, 61, 20, 44
};

static const int keccakf_piln[24] = 
{
	10, 7,  11, 17, 18, 3, 5,  16, 8,  21, 24, 4, 
	15, 23, 19, 13, 12, 2, 20, 14, 22, 9,  6,  1
};

// update the state with given number of rounds
static void keccakf(uint64_t st[25], int rounds)
{
	int i, j, round;
	uint64_t t, bc[5];

	for (round = 0; round < rounds; round++) {

		// Theta
		for (i = 0; i < 5; i++) {
			bc[i] = st[i] ^ st[i + 5] ^ st[i + 10] ^ st[i + 15] ^ st[i + 20];
		}

		for (i = 0; i < 5; i++) {
			t = bc[(i + 4) % 5] ^ ROTL64(bc[(i + 1) % 5], 1);
			for (j = 0; j < 25; j += 5) {
				st[j + i] ^= t;
			}
		}

		// Rho Pi
		t = st[1];
		for (i = 0; i < 24; i++) {
			j = keccakf_piln[i];
			bc[0] = st[j];
			st[j] = ROTL64(t, keccakf_rotc[i]);
			t = bc[0];
		}

		//  Chi
		for (j = 0; j < 25; j += 5) {
			for (i = 0; i < 5; i++) {
				bc[i] = st[j + i];
			}
			for (i = 0; i < 5; i++) {
				st[j + i] ^= (~bc[(i + 1) % 5]) & bc[(i + 2) % 5];
			}
		}

		//  Iota
		st[0] ^= keccakf_rndc[round];
	}
}

void sha3_init(struct sha3_state *sctx, unsigned int digest_sz)
{
	memset(sctx, 0, sizeof(*sctx));
	sctx->md_len = digest_sz;
	sctx->rsiz = 200 - 2 * digest_sz;
	sctx->rsizw = sctx->rsiz / 8;
}

void sha3_update(struct sha3_state *sctx, const uint8_t *data, unsigned int len)
{
	unsigned int done;
	const uint8_t *src;

	done = 0;
	src = data;

	if ((sctx->partial + len) > (sctx->rsiz - 1)) {
		if (sctx->partial) {
			done = sctx->partial; // done = -sctx->partial;
			memcpy(sctx->buf + sctx->partial, data, done + sctx->rsiz);
			src = sctx->buf;
		}

		do {
			unsigned int i;

			for (i = 0; i < sctx->rsizw; i++) {
				sctx->st[i] ^= ((uint64_t *) src)[i];
            }
			keccakf(sctx->st, KECCAK_ROUNDS);

			done += sctx->rsiz;
			src = data + done;
		} while (done + (sctx->rsiz - 1) < len);

		sctx->partial = 0;
	}
	memcpy(sctx->buf + sctx->partial, src, len - done);
	sctx->partial += (len - done);
}

void sha3_final(struct sha3_state *sctx, uint8_t *out)
{
	unsigned int i, inlen = sctx->partial;

	sctx->buf[inlen++] = 0x06; // sha3 standart
	memset(sctx->buf + inlen, 0, sctx->rsiz - inlen);
	sctx->buf[sctx->rsiz - 1] |= 0x80;

	for (i = 0; i < sctx->rsizw; i++)
		sctx->st[i] ^= ((uint64_t *) sctx->buf)[i];

	keccakf(sctx->st, KECCAK_ROUNDS);

//	for (i = 0; i < sctx->rsizw; i++)
//		sctx->st[i] = cpu_to_le64(sctx->st[i]);

	memcpy(out, sctx->st, sctx->md_len);

	memset(sctx, 0, sizeof(*sctx));
}
