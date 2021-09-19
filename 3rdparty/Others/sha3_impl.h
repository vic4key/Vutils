/*
	http://en.wikipedia.org/wiki/SHA-3
	Written by Jeff Garzik <jeff@garzik.org> for module of GNU/Linux kernel from https://lwn.net/Articles/518415/
	LICENSE GPL
	Originaly from linux-4.11/include/crypto/sha3.h SHA3_256:c96944b92955652521900dfdf798d942cc917f3d4903d98a1cda637980c55d8f
*/

#ifndef __CRYPTO_SHA3_H__
#define __CRYPTO_SHA3_H__

#include <stdint.h>

#define SHA3_224_DIGEST_SIZE	(224 / 8)
#define SHA3_224_BLOCK_SIZE	(200 - 2 * SHA3_224_DIGEST_SIZE)

#define SHA3_256_DIGEST_SIZE	(256 / 8)
#define SHA3_256_BLOCK_SIZE	(200 - 2 * SHA3_256_DIGEST_SIZE)

#define SHA3_384_DIGEST_SIZE	(384 / 8)
#define SHA3_384_BLOCK_SIZE	(200 - 2 * SHA3_384_DIGEST_SIZE)

#define SHA3_512_DIGEST_SIZE	(512 / 8)
#define SHA3_512_BLOCK_SIZE	(200 - 2 * SHA3_512_DIGEST_SIZE)

# ifdef __cplusplus
extern "C" {
# endif

struct sha3_state {
	uint64_t		st[25];
	unsigned int	md_len;
	unsigned int	rsiz;
	unsigned int	rsizw;

	unsigned int	partial;
	uint8_t			buf[SHA3_224_BLOCK_SIZE];
};

void sha3_init  (struct sha3_state *sctx, unsigned int digest_sz);
void sha3_update(struct sha3_state *sctx, const uint8_t *data, unsigned int len);
void sha3_final (struct sha3_state *sctx, uint8_t *out);

# ifdef __cplusplus
}
# endif

#endif /* __CRYPTO_SHA3_H__ */
