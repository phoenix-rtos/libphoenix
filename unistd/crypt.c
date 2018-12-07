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

static void base64_encode(unsigned char *bytes, int len, unsigned char *res);

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
	unsigned int len;					/* message length */
	unsigned int h[5];					/* intermediate hash */
	unsigned char blk[SHA1_BLOCK_SZ];	/* block */
	unsigned int blk_idx;				/* block index */
	unsigned int ext;					/* padding extended to the next block */
} sha1_ctx;


static void sha1_init(sha1_ctx *ctx)
{
	ctx->h[0] = 0x67452301;
	ctx->h[1] = 0xEFCDAB89;
	ctx->h[2] = 0x98BADCFE;
	ctx->h[3] = 0x10325476;
	ctx->h[4] = 0xC3D2E1F0;
	ctx->blk_idx = 0;
	ctx->len = 0;
	ctx->ext = 0;
}


static void sha1_processBlock(sha1_ctx *ctx)
{
	unsigned int W[80] = { 0 };
	unsigned int a, b, c, d, e, tmp;
	int t;

	/* message padding */
	if (ctx->blk_idx != SHA1_BLOCK_SZ) {
		/* extend padding to the next block*/
		if (ctx->blk_idx > 55) {
			ctx->ext = 1;

			ctx->blk[ctx->blk_idx] = 0x80;

			while(++ctx->blk_idx < SHA1_BLOCK_SZ)
				ctx->blk[ctx->blk_idx] = 0x0;

		}
		else {
			ctx->blk[ctx->blk_idx++] = 0x80;

			while(ctx->blk_idx < 60)
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
		W[t] |= (ctx->blk[t * 4 + 3] & 0xff) ;
	}

	/* actual hashing */
	for (t = 16; t < 80; t++)
		W[t] = S(1, W[t - 3] ^ W[t - 8] ^ W[t - 14] ^ W[t - 16]);

	a = ctx->h[0];
	b = ctx->h[1];
	c = ctx->h[2];
	d = ctx->h[3];
	e = ctx->h[4];

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
	ctx->h[0] += a;
	ctx->h[1] += b;
	ctx->h[2] += c;
	ctx->h[3] += d;
	ctx->h[4] += e;
}


static void sha1_process(sha1_ctx *ctx, const char *msg)
{
	size_t len = strlen(msg);

	while(len--) {
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
	base64_encode((unsigned char *)ctx.h, sizeof(ctx.h), &enc_res[2]);

	free(msg);
	return enc_res;
}
#endif /* _SHA1_CRYPT_ */


static const unsigned char base64_table[65] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";


static void base64_encode(unsigned char *bytes, int len, unsigned char *res)
{
	int i, j = 0;
	int output_len = len;
	unsigned char *pbytes;
	unsigned int trp;

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
