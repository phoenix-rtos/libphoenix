/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * unistd
 *
 * Copyright 2018 Phoenix Systems
 * Author: Jan Sikorski, Kamil Amanowicz
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

static void base64_encode(uint8_t *bytes, size_t len, unsigned char *res);

#define _SHA1_CRYPT_

#ifdef _SHA1_CRYPT_

#define S(n, X) \
	(((X) << n) | ((X) >> (32 - (n))))

#define F0(B, C, D) ((B & C) | ((~B) & D))
#define F1(B, C, D) (B ^ C ^ D)
#define F2(B, C, D) ((B & C) | (B & D) | (C & D))
#define F3(B, C, D) (B ^ C ^ D)

#define K0 0x5A827999
#define K1 0x6ED9EBA1
#define K2 0x8F1BBCDC
#define K3 0xCA62C1D6

#define SHA1_BLOCK_SZ 64

/* sha1 encoded hash - 2(salt) + 28 + 1 (zero byte) */
static unsigned char enc_res[31];


typedef struct _sha1_ctx {
	unsigned int len; /* message length */
	union {
		uint32_t h[5];   /* intermediate hash */
		uint8_t res[20]; /* hash result */
	} hash;
	uint8_t blk[SHA1_BLOCK_SZ]; /* block */
	size_t blk_idx;             /* block index */
	unsigned int ext;           /* padding extended to the next block */
} sha1_ctx;


static void sha1_init(sha1_ctx *ctx)
{
	ctx->hash.h[0] = 0x67452301;
	ctx->hash.h[1] = 0xEFCDAB89;
	ctx->hash.h[2] = 0x98BADCFE;
	ctx->hash.h[3] = 0x10325476;
	ctx->hash.h[4] = 0xC3D2E1F0;
	ctx->blk_idx = 0;
	ctx->len = 0;
	ctx->ext = 0;
}


static void sha1_processBlock(sha1_ctx *ctx)
{
	uint32_t W[80] = { 0 };
	uint32_t a, b, c, d, e, tmp;
	size_t t;

	/* message padding */
	if (ctx->blk_idx != SHA1_BLOCK_SZ) {
		/* extend padding to the next block*/
		if (ctx->blk_idx > 55) {
			ctx->ext = 1;

			ctx->blk[ctx->blk_idx] = 0x80;

			while (++ctx->blk_idx < SHA1_BLOCK_SZ)
				ctx->blk[ctx->blk_idx] = 0x0;
		}
		else {
			ctx->blk[ctx->blk_idx++] = 0x80;

			while (ctx->blk_idx < 60)
				ctx->blk[ctx->blk_idx++] = 0x0;

			ctx->blk[ctx->blk_idx++] = (ctx->len >> 24) & 0xff;
			ctx->blk[ctx->blk_idx++] = (ctx->len >> 16) & 0xff;
			ctx->blk[ctx->blk_idx++] = (ctx->len >> 8) & 0xff;
			ctx->blk[ctx->blk_idx++] = (ctx->len) & 0xff;
		}
	}

	/* assume that key length is 32 bit */
	for (t = 0; t < 16; t++) {
		W[t] = (ctx->blk[t * 4] & 0xff) << 24;
		W[t] |= (ctx->blk[t * 4 + 1] & 0xff) << 16;
		W[t] |= (ctx->blk[t * 4 + 2] & 0xff) << 8;
		W[t] |= (ctx->blk[t * 4 + 3] & 0xff);
	}

	/* actual hashing */
	for (t = 16; t < 80; t++)
		W[t] = S(1, W[t - 3] ^ W[t - 8] ^ W[t - 14] ^ W[t - 16]);

	a = ctx->hash.h[0];
	b = ctx->hash.h[1];
	c = ctx->hash.h[2];
	d = ctx->hash.h[3];
	e = ctx->hash.h[4];

	for (t = 0; t < 20; t++) {
		tmp = S(5, a) + F0(b, c, d) + e + W[t] + K0;
		e = d;
		d = c;
		c = S(30, b);
		b = a;
		a = tmp;
	}

	for (t = 20; t < 40; t++) {
		tmp = S(5, a) + F1(b, c, d) + e + W[t] + K1;
		e = d;
		d = c;
		c = S(30, b);
		b = a;
		a = tmp;
	}

	for (t = 40; t < 60; t++) {
		tmp = S(5, a) + F2(b, c, d) + e + W[t] + K2;
		e = d;
		d = c;
		c = S(30, b);
		b = a;
		a = tmp;
	}

	for (t = 60; t < 80; t++) {
		tmp = S(5, a) + F3(b, c, d) + e + W[t] + K3;
		e = d;
		d = c;
		c = S(30, b);
		b = a;
		a = tmp;
	}

	ctx->blk_idx = 0;
	ctx->hash.h[0] += a;
	ctx->hash.h[1] += b;
	ctx->hash.h[2] += c;
	ctx->hash.h[3] += d;
	ctx->hash.h[4] += e;
}


static void sha1_process(sha1_ctx *ctx, const char *msg)
{
	size_t len = strlen(msg);

	while (len--) {
		ctx->blk[ctx->blk_idx++] = *msg++ & 0xff;
		ctx->len += 8;
		if (ctx->blk_idx == SHA1_BLOCK_SZ)
			sha1_processBlock(ctx);
	}

	/* process last block */
	if (ctx->blk_idx || !strlen(msg))
		sha1_processBlock(ctx);

	if (ctx->ext)
		sha1_processBlock(ctx);

	/* Fix byte order */
	for (size_t i = 0; i < sizeof(ctx->hash.h) / sizeof(ctx->hash.h[0]); i++) {
		uint32_t tmp = ctx->hash.h[i];
		ctx->hash.res[i * 4] = (tmp & 0xffu);
		ctx->hash.res[i * 4 + 1] = (tmp & 0xff00u) >> 8;
		ctx->hash.res[i * 4 + 2] = (tmp & 0xff0000u) >> 16;
		ctx->hash.res[i * 4 + 3] = (tmp & 0xff000000u) >> 24;
	}
}


static unsigned char *sha1_crypt(const char *key, const char *salt)
{
	sha1_ctx ctx = { 0 };
	char *msg = NULL;
	size_t len;
	memset(enc_res, 0, 31);

	/* init sha1 context */
	sha1_init(&ctx);

	enc_res[0] = salt[0];
	enc_res[1] = salt[1];

	len = strlen(key) + 3;

	msg = malloc(len);

	if (msg == NULL)
		return NULL;

	memset(msg, 0, len);
	msg[0] = salt[0];
	msg[1] = salt[1];
	memcpy(&msg[2], key, len - 3);

	/* process message */
	sha1_process(&ctx, msg);

	/* encode result */
	base64_encode(ctx.hash.res, sizeof(ctx.hash.res), &enc_res[2]);

	free(msg);
	return enc_res;
}
#endif /* _SHA1_CRYPT_ */


static const unsigned char base64_table[65] =
	"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";


static void base64_encode(uint8_t *bytes, size_t len, unsigned char *res)
{
	size_t i, j = 0;
	size_t output_len = len;
	uint8_t *pbytes;
	uint32_t trp;

	while (output_len % 3) output_len++;

	pbytes = malloc(output_len);
	memset(pbytes, 0, output_len);
	memcpy(pbytes, bytes, len);

	for (i = 0; i < output_len; i += 3) {
		trp = pbytes[i] << 16;
		trp |= pbytes[i + 1] << 8;
		trp |= pbytes[i + 2];

		res[j++] = base64_table[(trp >> 18) & 0x3f];
		res[j++] = base64_table[(trp >> 12) & 0x3f];

		if (i + 1 >= len)
			res[j++] = '=';
		else
			res[j++] = base64_table[(trp >> 6) & 0x3f];

		if (i + 2 >= len)
			res[j++] = '=';
		else
			res[j++] = base64_table[trp & 0x3f];
	}
	free(pbytes);
}

/* sha1 based crypt */
char *crypt(const char *key, const char *salt)
{

	/* check the salt presence and length */
	if (key == NULL || salt == NULL || strlen(salt) < 2)
		return NULL;

#ifdef _SHA1_CRYPT_
	return (char *)sha1_crypt(key, salt);
#endif

	return (char *)salt;
}
