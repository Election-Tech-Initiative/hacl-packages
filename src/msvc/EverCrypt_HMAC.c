/* MIT License
 *
 * Copyright (c) 2016-2022 INRIA, CMU and Microsoft Corporation
 * Copyright (c) 2022-2023 HACL* Contributors
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */


#include "internal/EverCrypt_HMAC.h"

#include "internal/Hacl_Krmllib.h"
#include "internal/Hacl_Hash_SHA2.h"
#include "internal/Hacl_Hash_SHA1.h"
#include "internal/Hacl_Hash_Blake2.h"
#include "internal/EverCrypt_Hash.h"

bool EverCrypt_HMAC_is_supported_alg(Spec_Hash_Definitions_hash_alg uu___)
{
  switch (uu___)
  {
    case Spec_Hash_Definitions_SHA1:
      {
        return true;
      }
    case Spec_Hash_Definitions_SHA2_256:
      {
        return true;
      }
    case Spec_Hash_Definitions_SHA2_384:
      {
        return true;
      }
    case Spec_Hash_Definitions_SHA2_512:
      {
        return true;
      }
    case Spec_Hash_Definitions_Blake2S:
      {
        return true;
      }
    case Spec_Hash_Definitions_Blake2B:
      {
        return true;
      }
    default:
      {
        return false;
      }
  }
}

void
(*EverCrypt_HMAC_hash_256)(uint8_t *x0, uint32_t x1, uint8_t *x2) =
  EverCrypt_Hash_Incremental_hash_256;

void
EverCrypt_HMAC_compute_sha1(
  uint8_t *dst,
  uint8_t *key,
  uint32_t key_len,
  uint8_t *data,
  uint32_t data_len
)
{
  uint32_t l = (uint32_t)64U;
  KRML_CHECK_SIZE(sizeof (uint8_t), l);
  uint8_t *key_block = (uint8_t *)alloca(l * sizeof (uint8_t));
  memset(key_block, 0U, l * sizeof (uint8_t));
  uint8_t *nkey = key_block;
  uint32_t ite;
  if (key_len <= (uint32_t)64U)
  {
    ite = key_len;
  }
  else
  {
    ite = (uint32_t)20U;
  }
  uint8_t *zeroes = key_block + ite;
  KRML_HOST_IGNORE(zeroes);
  if (key_len <= (uint32_t)64U)
  {
    memcpy(nkey, key, key_len * sizeof (uint8_t));
  }
  else
  {
    Hacl_Hash_SHA1_legacy_hash(key, key_len, nkey);
  }
  KRML_CHECK_SIZE(sizeof (uint8_t), l);
  uint8_t *ipad = (uint8_t *)alloca(l * sizeof (uint8_t));
  memset(ipad, (uint8_t)0x36U, l * sizeof (uint8_t));
  for (uint32_t i = (uint32_t)0U; i < l; i++)
  {
    uint8_t xi = ipad[i];
    uint8_t yi = key_block[i];
    ipad[i] = xi ^ yi;
  }
  KRML_CHECK_SIZE(sizeof (uint8_t), l);
  uint8_t *opad = (uint8_t *)alloca(l * sizeof (uint8_t));
  memset(opad, (uint8_t)0x5cU, l * sizeof (uint8_t));
  for (uint32_t i = (uint32_t)0U; i < l; i++)
  {
    uint8_t xi = opad[i];
    uint8_t yi = key_block[i];
    opad[i] = xi ^ yi;
  }
  uint32_t
  s[5U] =
    {
      (uint32_t)0x67452301U, (uint32_t)0xefcdab89U, (uint32_t)0x98badcfeU, (uint32_t)0x10325476U,
      (uint32_t)0xc3d2e1f0U
    };
  uint8_t *dst1 = ipad;
  if (data_len == (uint32_t)0U)
  {
    Hacl_Hash_SHA1_legacy_update_last(s, (uint64_t)0U, ipad, (uint32_t)64U);
  }
  else
  {
    uint32_t block_len = (uint32_t)64U;
    uint32_t n_blocks0 = data_len / block_len;
    uint32_t rem0 = data_len % block_len;
    K___uint32_t_uint32_t scrut;
    if (n_blocks0 > (uint32_t)0U && rem0 == (uint32_t)0U)
    {
      uint32_t n_blocks_ = n_blocks0 - (uint32_t)1U;
      scrut = ((K___uint32_t_uint32_t){ .fst = n_blocks_, .snd = data_len - n_blocks_ * block_len });
    }
    else
    {
      scrut = ((K___uint32_t_uint32_t){ .fst = n_blocks0, .snd = rem0 });
    }
    uint32_t n_blocks = scrut.fst;
    uint32_t rem_len = scrut.snd;
    uint32_t full_blocks_len = n_blocks * block_len;
    uint8_t *full_blocks = data;
    uint8_t *rem = data + full_blocks_len;
    Hacl_Hash_SHA1_legacy_update_multi(s, ipad, (uint32_t)1U);
    Hacl_Hash_SHA1_legacy_update_multi(s, full_blocks, n_blocks);
    Hacl_Hash_SHA1_legacy_update_last(s,
      (uint64_t)(uint32_t)64U + (uint64_t)full_blocks_len,
      rem,
      rem_len);
  }
  Hacl_Hash_Core_SHA1_legacy_finish(s, dst1);
  uint8_t *hash1 = ipad;
  Hacl_Hash_Core_SHA1_legacy_init(s);
  uint32_t block_len = (uint32_t)64U;
  uint32_t n_blocks0 = (uint32_t)20U / block_len;
  uint32_t rem0 = (uint32_t)20U % block_len;
  K___uint32_t_uint32_t scrut;
  if (n_blocks0 > (uint32_t)0U && rem0 == (uint32_t)0U)
  {
    uint32_t n_blocks_ = n_blocks0 - (uint32_t)1U;
    scrut =
      ((K___uint32_t_uint32_t){ .fst = n_blocks_, .snd = (uint32_t)20U - n_blocks_ * block_len });
  }
  else
  {
    scrut = ((K___uint32_t_uint32_t){ .fst = n_blocks0, .snd = rem0 });
  }
  uint32_t n_blocks = scrut.fst;
  uint32_t rem_len = scrut.snd;
  uint32_t full_blocks_len = n_blocks * block_len;
  uint8_t *full_blocks = hash1;
  uint8_t *rem = hash1 + full_blocks_len;
  Hacl_Hash_SHA1_legacy_update_multi(s, opad, (uint32_t)1U);
  Hacl_Hash_SHA1_legacy_update_multi(s, full_blocks, n_blocks);
  Hacl_Hash_SHA1_legacy_update_last(s,
    (uint64_t)(uint32_t)64U + (uint64_t)full_blocks_len,
    rem,
    rem_len);
  Hacl_Hash_Core_SHA1_legacy_finish(s, dst);
}

void
EverCrypt_HMAC_compute_sha2_256(
  uint8_t *dst,
  uint8_t *key,
  uint32_t key_len,
  uint8_t *data,
  uint32_t data_len
)
{
  uint32_t l = (uint32_t)64U;
  KRML_CHECK_SIZE(sizeof (uint8_t), l);
  uint8_t *key_block = (uint8_t *)alloca(l * sizeof (uint8_t));
  memset(key_block, 0U, l * sizeof (uint8_t));
  uint8_t *nkey = key_block;
  uint32_t ite;
  if (key_len <= (uint32_t)64U)
  {
    ite = key_len;
  }
  else
  {
    ite = (uint32_t)32U;
  }
  uint8_t *zeroes = key_block + ite;
  KRML_HOST_IGNORE(zeroes);
  if (key_len <= (uint32_t)64U)
  {
    memcpy(nkey, key, key_len * sizeof (uint8_t));
  }
  else
  {
    EverCrypt_HMAC_hash_256(key, key_len, nkey);
  }
  KRML_CHECK_SIZE(sizeof (uint8_t), l);
  uint8_t *ipad = (uint8_t *)alloca(l * sizeof (uint8_t));
  memset(ipad, (uint8_t)0x36U, l * sizeof (uint8_t));
  for (uint32_t i = (uint32_t)0U; i < l; i++)
  {
    uint8_t xi = ipad[i];
    uint8_t yi = key_block[i];
    ipad[i] = xi ^ yi;
  }
  KRML_CHECK_SIZE(sizeof (uint8_t), l);
  uint8_t *opad = (uint8_t *)alloca(l * sizeof (uint8_t));
  memset(opad, (uint8_t)0x5cU, l * sizeof (uint8_t));
  for (uint32_t i = (uint32_t)0U; i < l; i++)
  {
    uint8_t xi = opad[i];
    uint8_t yi = key_block[i];
    opad[i] = xi ^ yi;
  }
  uint32_t st[8U] = { 0U };
  KRML_MAYBE_FOR8(i,
    (uint32_t)0U,
    (uint32_t)8U,
    (uint32_t)1U,
    uint32_t *os = st;
    uint32_t x = Hacl_Impl_SHA2_Generic_h256[i];
    os[i] = x;);
  uint32_t *s = st;
  uint8_t *dst1 = ipad;
  if (data_len == (uint32_t)0U)
  {
    Hacl_SHA2_Scalar32_sha256_update_last((uint64_t)0U + (uint64_t)(uint32_t)64U,
      (uint32_t)64U,
      ipad,
      s);
  }
  else
  {
    uint32_t block_len = (uint32_t)64U;
    uint32_t n_blocks0 = data_len / block_len;
    uint32_t rem0 = data_len % block_len;
    K___uint32_t_uint32_t scrut;
    if (n_blocks0 > (uint32_t)0U && rem0 == (uint32_t)0U)
    {
      uint32_t n_blocks_ = n_blocks0 - (uint32_t)1U;
      scrut = ((K___uint32_t_uint32_t){ .fst = n_blocks_, .snd = data_len - n_blocks_ * block_len });
    }
    else
    {
      scrut = ((K___uint32_t_uint32_t){ .fst = n_blocks0, .snd = rem0 });
    }
    uint32_t n_blocks = scrut.fst;
    uint32_t rem_len = scrut.snd;
    uint32_t full_blocks_len = n_blocks * block_len;
    uint8_t *full_blocks = data;
    uint8_t *rem = data + full_blocks_len;
    EverCrypt_Hash_update_multi_256(s, ipad, (uint32_t)1U);
    EverCrypt_Hash_update_multi_256(s, full_blocks, n_blocks);
    Hacl_SHA2_Scalar32_sha256_update_last((uint64_t)(uint32_t)64U
      + (uint64_t)full_blocks_len
      + (uint64_t)rem_len,
      rem_len,
      rem,
      s);
  }
  Hacl_SHA2_Scalar32_sha256_finish(s, dst1);
  uint8_t *hash1 = ipad;
  Hacl_SHA2_Scalar32_sha256_init(s);
  uint32_t block_len = (uint32_t)64U;
  uint32_t n_blocks0 = (uint32_t)32U / block_len;
  uint32_t rem0 = (uint32_t)32U % block_len;
  K___uint32_t_uint32_t scrut;
  if (n_blocks0 > (uint32_t)0U && rem0 == (uint32_t)0U)
  {
    uint32_t n_blocks_ = n_blocks0 - (uint32_t)1U;
    scrut =
      ((K___uint32_t_uint32_t){ .fst = n_blocks_, .snd = (uint32_t)32U - n_blocks_ * block_len });
  }
  else
  {
    scrut = ((K___uint32_t_uint32_t){ .fst = n_blocks0, .snd = rem0 });
  }
  uint32_t n_blocks = scrut.fst;
  uint32_t rem_len = scrut.snd;
  uint32_t full_blocks_len = n_blocks * block_len;
  uint8_t *full_blocks = hash1;
  uint8_t *rem = hash1 + full_blocks_len;
  EverCrypt_Hash_update_multi_256(s, opad, (uint32_t)1U);
  EverCrypt_Hash_update_multi_256(s, full_blocks, n_blocks);
  Hacl_SHA2_Scalar32_sha256_update_last((uint64_t)(uint32_t)64U
    + (uint64_t)full_blocks_len
    + (uint64_t)rem_len,
    rem_len,
    rem,
    s);
  Hacl_SHA2_Scalar32_sha256_finish(s, dst);
}

void
EverCrypt_HMAC_compute_sha2_384(
  uint8_t *dst,
  uint8_t *key,
  uint32_t key_len,
  uint8_t *data,
  uint32_t data_len
)
{
  uint32_t l = (uint32_t)128U;
  KRML_CHECK_SIZE(sizeof (uint8_t), l);
  uint8_t *key_block = (uint8_t *)alloca(l * sizeof (uint8_t));
  memset(key_block, 0U, l * sizeof (uint8_t));
  uint8_t *nkey = key_block;
  uint32_t ite;
  if (key_len <= (uint32_t)128U)
  {
    ite = key_len;
  }
  else
  {
    ite = (uint32_t)48U;
  }
  uint8_t *zeroes = key_block + ite;
  KRML_HOST_IGNORE(zeroes);
  if (key_len <= (uint32_t)128U)
  {
    memcpy(nkey, key, key_len * sizeof (uint8_t));
  }
  else
  {
    Hacl_Streaming_SHA2_hash_384(key, key_len, nkey);
  }
  KRML_CHECK_SIZE(sizeof (uint8_t), l);
  uint8_t *ipad = (uint8_t *)alloca(l * sizeof (uint8_t));
  memset(ipad, (uint8_t)0x36U, l * sizeof (uint8_t));
  for (uint32_t i = (uint32_t)0U; i < l; i++)
  {
    uint8_t xi = ipad[i];
    uint8_t yi = key_block[i];
    ipad[i] = xi ^ yi;
  }
  KRML_CHECK_SIZE(sizeof (uint8_t), l);
  uint8_t *opad = (uint8_t *)alloca(l * sizeof (uint8_t));
  memset(opad, (uint8_t)0x5cU, l * sizeof (uint8_t));
  for (uint32_t i = (uint32_t)0U; i < l; i++)
  {
    uint8_t xi = opad[i];
    uint8_t yi = key_block[i];
    opad[i] = xi ^ yi;
  }
  uint64_t st[8U] = { 0U };
  KRML_MAYBE_FOR8(i,
    (uint32_t)0U,
    (uint32_t)8U,
    (uint32_t)1U,
    uint64_t *os = st;
    uint64_t x = Hacl_Impl_SHA2_Generic_h384[i];
    os[i] = x;);
  uint64_t *s = st;
  uint8_t *dst1 = ipad;
  if (data_len == (uint32_t)0U)
  {
    Hacl_SHA2_Scalar32_sha384_update_last(FStar_UInt128_add(FStar_UInt128_uint64_to_uint128((uint64_t)0U),
        FStar_UInt128_uint64_to_uint128((uint64_t)(uint32_t)128U)),
      (uint32_t)128U,
      ipad,
      s);
  }
  else
  {
    uint32_t block_len = (uint32_t)128U;
    uint32_t n_blocks0 = data_len / block_len;
    uint32_t rem0 = data_len % block_len;
    K___uint32_t_uint32_t scrut;
    if (n_blocks0 > (uint32_t)0U && rem0 == (uint32_t)0U)
    {
      uint32_t n_blocks_ = n_blocks0 - (uint32_t)1U;
      scrut = ((K___uint32_t_uint32_t){ .fst = n_blocks_, .snd = data_len - n_blocks_ * block_len });
    }
    else
    {
      scrut = ((K___uint32_t_uint32_t){ .fst = n_blocks0, .snd = rem0 });
    }
    uint32_t n_blocks = scrut.fst;
    uint32_t rem_len = scrut.snd;
    uint32_t full_blocks_len = n_blocks * block_len;
    uint8_t *full_blocks = data;
    uint8_t *rem = data + full_blocks_len;
    Hacl_SHA2_Scalar32_sha384_update_nblocks((uint32_t)128U, ipad, s);
    Hacl_SHA2_Scalar32_sha384_update_nblocks(n_blocks * (uint32_t)128U, full_blocks, s);
    Hacl_SHA2_Scalar32_sha384_update_last(FStar_UInt128_add(FStar_UInt128_add(FStar_UInt128_uint64_to_uint128((uint64_t)(uint32_t)128U),
          FStar_UInt128_uint64_to_uint128((uint64_t)full_blocks_len)),
        FStar_UInt128_uint64_to_uint128((uint64_t)rem_len)),
      rem_len,
      rem,
      s);
  }
  Hacl_SHA2_Scalar32_sha384_finish(s, dst1);
  uint8_t *hash1 = ipad;
  Hacl_SHA2_Scalar32_sha384_init(s);
  uint32_t block_len = (uint32_t)128U;
  uint32_t n_blocks0 = (uint32_t)48U / block_len;
  uint32_t rem0 = (uint32_t)48U % block_len;
  K___uint32_t_uint32_t scrut;
  if (n_blocks0 > (uint32_t)0U && rem0 == (uint32_t)0U)
  {
    uint32_t n_blocks_ = n_blocks0 - (uint32_t)1U;
    scrut =
      ((K___uint32_t_uint32_t){ .fst = n_blocks_, .snd = (uint32_t)48U - n_blocks_ * block_len });
  }
  else
  {
    scrut = ((K___uint32_t_uint32_t){ .fst = n_blocks0, .snd = rem0 });
  }
  uint32_t n_blocks = scrut.fst;
  uint32_t rem_len = scrut.snd;
  uint32_t full_blocks_len = n_blocks * block_len;
  uint8_t *full_blocks = hash1;
  uint8_t *rem = hash1 + full_blocks_len;
  Hacl_SHA2_Scalar32_sha384_update_nblocks((uint32_t)128U, opad, s);
  Hacl_SHA2_Scalar32_sha384_update_nblocks(n_blocks * (uint32_t)128U, full_blocks, s);
  Hacl_SHA2_Scalar32_sha384_update_last(FStar_UInt128_add(FStar_UInt128_add(FStar_UInt128_uint64_to_uint128((uint64_t)(uint32_t)128U),
        FStar_UInt128_uint64_to_uint128((uint64_t)full_blocks_len)),
      FStar_UInt128_uint64_to_uint128((uint64_t)rem_len)),
    rem_len,
    rem,
    s);
  Hacl_SHA2_Scalar32_sha384_finish(s, dst);
}

void
EverCrypt_HMAC_compute_sha2_512(
  uint8_t *dst,
  uint8_t *key,
  uint32_t key_len,
  uint8_t *data,
  uint32_t data_len
)
{
  uint32_t l = (uint32_t)128U;
  KRML_CHECK_SIZE(sizeof (uint8_t), l);
  uint8_t *key_block = (uint8_t *)alloca(l * sizeof (uint8_t));
  memset(key_block, 0U, l * sizeof (uint8_t));
  uint8_t *nkey = key_block;
  uint32_t ite;
  if (key_len <= (uint32_t)128U)
  {
    ite = key_len;
  }
  else
  {
    ite = (uint32_t)64U;
  }
  uint8_t *zeroes = key_block + ite;
  KRML_HOST_IGNORE(zeroes);
  if (key_len <= (uint32_t)128U)
  {
    memcpy(nkey, key, key_len * sizeof (uint8_t));
  }
  else
  {
    Hacl_Streaming_SHA2_hash_512(key, key_len, nkey);
  }
  KRML_CHECK_SIZE(sizeof (uint8_t), l);
  uint8_t *ipad = (uint8_t *)alloca(l * sizeof (uint8_t));
  memset(ipad, (uint8_t)0x36U, l * sizeof (uint8_t));
  for (uint32_t i = (uint32_t)0U; i < l; i++)
  {
    uint8_t xi = ipad[i];
    uint8_t yi = key_block[i];
    ipad[i] = xi ^ yi;
  }
  KRML_CHECK_SIZE(sizeof (uint8_t), l);
  uint8_t *opad = (uint8_t *)alloca(l * sizeof (uint8_t));
  memset(opad, (uint8_t)0x5cU, l * sizeof (uint8_t));
  for (uint32_t i = (uint32_t)0U; i < l; i++)
  {
    uint8_t xi = opad[i];
    uint8_t yi = key_block[i];
    opad[i] = xi ^ yi;
  }
  uint64_t st[8U] = { 0U };
  KRML_MAYBE_FOR8(i,
    (uint32_t)0U,
    (uint32_t)8U,
    (uint32_t)1U,
    uint64_t *os = st;
    uint64_t x = Hacl_Impl_SHA2_Generic_h512[i];
    os[i] = x;);
  uint64_t *s = st;
  uint8_t *dst1 = ipad;
  if (data_len == (uint32_t)0U)
  {
    Hacl_SHA2_Scalar32_sha512_update_last(FStar_UInt128_add(FStar_UInt128_uint64_to_uint128((uint64_t)0U),
        FStar_UInt128_uint64_to_uint128((uint64_t)(uint32_t)128U)),
      (uint32_t)128U,
      ipad,
      s);
  }
  else
  {
    uint32_t block_len = (uint32_t)128U;
    uint32_t n_blocks0 = data_len / block_len;
    uint32_t rem0 = data_len % block_len;
    K___uint32_t_uint32_t scrut;
    if (n_blocks0 > (uint32_t)0U && rem0 == (uint32_t)0U)
    {
      uint32_t n_blocks_ = n_blocks0 - (uint32_t)1U;
      scrut = ((K___uint32_t_uint32_t){ .fst = n_blocks_, .snd = data_len - n_blocks_ * block_len });
    }
    else
    {
      scrut = ((K___uint32_t_uint32_t){ .fst = n_blocks0, .snd = rem0 });
    }
    uint32_t n_blocks = scrut.fst;
    uint32_t rem_len = scrut.snd;
    uint32_t full_blocks_len = n_blocks * block_len;
    uint8_t *full_blocks = data;
    uint8_t *rem = data + full_blocks_len;
    Hacl_SHA2_Scalar32_sha512_update_nblocks((uint32_t)128U, ipad, s);
    Hacl_SHA2_Scalar32_sha512_update_nblocks(n_blocks * (uint32_t)128U, full_blocks, s);
    Hacl_SHA2_Scalar32_sha512_update_last(FStar_UInt128_add(FStar_UInt128_add(FStar_UInt128_uint64_to_uint128((uint64_t)(uint32_t)128U),
          FStar_UInt128_uint64_to_uint128((uint64_t)full_blocks_len)),
        FStar_UInt128_uint64_to_uint128((uint64_t)rem_len)),
      rem_len,
      rem,
      s);
  }
  Hacl_SHA2_Scalar32_sha512_finish(s, dst1);
  uint8_t *hash1 = ipad;
  Hacl_SHA2_Scalar32_sha512_init(s);
  uint32_t block_len = (uint32_t)128U;
  uint32_t n_blocks0 = (uint32_t)64U / block_len;
  uint32_t rem0 = (uint32_t)64U % block_len;
  K___uint32_t_uint32_t scrut;
  if (n_blocks0 > (uint32_t)0U && rem0 == (uint32_t)0U)
  {
    uint32_t n_blocks_ = n_blocks0 - (uint32_t)1U;
    scrut =
      ((K___uint32_t_uint32_t){ .fst = n_blocks_, .snd = (uint32_t)64U - n_blocks_ * block_len });
  }
  else
  {
    scrut = ((K___uint32_t_uint32_t){ .fst = n_blocks0, .snd = rem0 });
  }
  uint32_t n_blocks = scrut.fst;
  uint32_t rem_len = scrut.snd;
  uint32_t full_blocks_len = n_blocks * block_len;
  uint8_t *full_blocks = hash1;
  uint8_t *rem = hash1 + full_blocks_len;
  Hacl_SHA2_Scalar32_sha512_update_nblocks((uint32_t)128U, opad, s);
  Hacl_SHA2_Scalar32_sha512_update_nblocks(n_blocks * (uint32_t)128U, full_blocks, s);
  Hacl_SHA2_Scalar32_sha512_update_last(FStar_UInt128_add(FStar_UInt128_add(FStar_UInt128_uint64_to_uint128((uint64_t)(uint32_t)128U),
        FStar_UInt128_uint64_to_uint128((uint64_t)full_blocks_len)),
      FStar_UInt128_uint64_to_uint128((uint64_t)rem_len)),
    rem_len,
    rem,
    s);
  Hacl_SHA2_Scalar32_sha512_finish(s, dst);
}

void
EverCrypt_HMAC_compute_blake2s(
  uint8_t *dst,
  uint8_t *key,
  uint32_t key_len,
  uint8_t *data,
  uint32_t data_len
)
{
  uint32_t l = (uint32_t)64U;
  KRML_CHECK_SIZE(sizeof (uint8_t), l);
  uint8_t *key_block = (uint8_t *)alloca(l * sizeof (uint8_t));
  memset(key_block, 0U, l * sizeof (uint8_t));
  uint8_t *nkey = key_block;
  uint32_t ite;
  if (key_len <= (uint32_t)64U)
  {
    ite = key_len;
  }
  else
  {
    ite = (uint32_t)32U;
  }
  uint8_t *zeroes = key_block + ite;
  KRML_HOST_IGNORE(zeroes);
  if (key_len <= (uint32_t)64U)
  {
    memcpy(nkey, key, key_len * sizeof (uint8_t));
  }
  else
  {
    Hacl_Blake2s_32_blake2s((uint32_t)32U, nkey, key_len, key, (uint32_t)0U, NULL);
  }
  KRML_CHECK_SIZE(sizeof (uint8_t), l);
  uint8_t *ipad = (uint8_t *)alloca(l * sizeof (uint8_t));
  memset(ipad, (uint8_t)0x36U, l * sizeof (uint8_t));
  for (uint32_t i = (uint32_t)0U; i < l; i++)
  {
    uint8_t xi = ipad[i];
    uint8_t yi = key_block[i];
    ipad[i] = xi ^ yi;
  }
  KRML_CHECK_SIZE(sizeof (uint8_t), l);
  uint8_t *opad = (uint8_t *)alloca(l * sizeof (uint8_t));
  memset(opad, (uint8_t)0x5cU, l * sizeof (uint8_t));
  for (uint32_t i = (uint32_t)0U; i < l; i++)
  {
    uint8_t xi = opad[i];
    uint8_t yi = key_block[i];
    opad[i] = xi ^ yi;
  }
  uint32_t s[16U] = { 0U };
  Hacl_Blake2s_32_blake2s_init(s, (uint32_t)0U, (uint32_t)32U);
  uint32_t *s0 = s;
  uint8_t *dst1 = ipad;
  if (data_len == (uint32_t)0U)
  {
    uint32_t wv[16U] = { 0U };
    Hacl_Blake2s_32_blake2s_update_last((uint32_t)64U, wv, s0, (uint64_t)0U, (uint32_t)64U, ipad);
  }
  else
  {
    uint32_t block_len = (uint32_t)64U;
    uint32_t n_blocks0 = data_len / block_len;
    uint32_t rem0 = data_len % block_len;
    K___uint32_t_uint32_t scrut;
    if (n_blocks0 > (uint32_t)0U && rem0 == (uint32_t)0U)
    {
      uint32_t n_blocks_ = n_blocks0 - (uint32_t)1U;
      scrut = ((K___uint32_t_uint32_t){ .fst = n_blocks_, .snd = data_len - n_blocks_ * block_len });
    }
    else
    {
      scrut = ((K___uint32_t_uint32_t){ .fst = n_blocks0, .snd = rem0 });
    }
    uint32_t n_blocks = scrut.fst;
    uint32_t rem_len = scrut.snd;
    uint32_t full_blocks_len = n_blocks * block_len;
    uint8_t *full_blocks = data;
    uint8_t *rem = data + full_blocks_len;
    uint32_t wv[16U] = { 0U };
    Hacl_Blake2s_32_blake2s_update_multi((uint32_t)64U, wv, s0, (uint64_t)0U, ipad, (uint32_t)1U);
    uint32_t wv0[16U] = { 0U };
    Hacl_Blake2s_32_blake2s_update_multi(n_blocks * (uint32_t)64U,
      wv0,
      s0,
      (uint64_t)block_len,
      full_blocks,
      n_blocks);
    uint32_t wv1[16U] = { 0U };
    Hacl_Blake2s_32_blake2s_update_last(rem_len,
      wv1,
      s0,
      (uint64_t)(uint32_t)64U + (uint64_t)full_blocks_len,
      rem_len,
      rem);
  }
  Hacl_Blake2s_32_blake2s_finish((uint32_t)32U, dst1, s0);
  uint8_t *hash1 = ipad;
  Hacl_Blake2s_32_blake2s_init(s0, (uint32_t)0U, (uint32_t)32U);
  uint32_t block_len = (uint32_t)64U;
  uint32_t n_blocks0 = (uint32_t)32U / block_len;
  uint32_t rem0 = (uint32_t)32U % block_len;
  K___uint32_t_uint32_t scrut;
  if (n_blocks0 > (uint32_t)0U && rem0 == (uint32_t)0U)
  {
    uint32_t n_blocks_ = n_blocks0 - (uint32_t)1U;
    scrut =
      ((K___uint32_t_uint32_t){ .fst = n_blocks_, .snd = (uint32_t)32U - n_blocks_ * block_len });
  }
  else
  {
    scrut = ((K___uint32_t_uint32_t){ .fst = n_blocks0, .snd = rem0 });
  }
  uint32_t n_blocks = scrut.fst;
  uint32_t rem_len = scrut.snd;
  uint32_t full_blocks_len = n_blocks * block_len;
  uint8_t *full_blocks = hash1;
  uint8_t *rem = hash1 + full_blocks_len;
  uint32_t wv[16U] = { 0U };
  Hacl_Blake2s_32_blake2s_update_multi((uint32_t)64U, wv, s0, (uint64_t)0U, opad, (uint32_t)1U);
  uint32_t wv0[16U] = { 0U };
  Hacl_Blake2s_32_blake2s_update_multi(n_blocks * (uint32_t)64U,
    wv0,
    s0,
    (uint64_t)block_len,
    full_blocks,
    n_blocks);
  uint32_t wv1[16U] = { 0U };
  Hacl_Blake2s_32_blake2s_update_last(rem_len,
    wv1,
    s0,
    (uint64_t)(uint32_t)64U + (uint64_t)full_blocks_len,
    rem_len,
    rem);
  Hacl_Blake2s_32_blake2s_finish((uint32_t)32U, dst, s0);
}

void
EverCrypt_HMAC_compute_blake2b(
  uint8_t *dst,
  uint8_t *key,
  uint32_t key_len,
  uint8_t *data,
  uint32_t data_len
)
{
  uint32_t l = (uint32_t)128U;
  KRML_CHECK_SIZE(sizeof (uint8_t), l);
  uint8_t *key_block = (uint8_t *)alloca(l * sizeof (uint8_t));
  memset(key_block, 0U, l * sizeof (uint8_t));
  uint8_t *nkey = key_block;
  uint32_t ite;
  if (key_len <= (uint32_t)128U)
  {
    ite = key_len;
  }
  else
  {
    ite = (uint32_t)64U;
  }
  uint8_t *zeroes = key_block + ite;
  KRML_HOST_IGNORE(zeroes);
  if (key_len <= (uint32_t)128U)
  {
    memcpy(nkey, key, key_len * sizeof (uint8_t));
  }
  else
  {
    Hacl_Blake2b_32_blake2b((uint32_t)64U, nkey, key_len, key, (uint32_t)0U, NULL);
  }
  KRML_CHECK_SIZE(sizeof (uint8_t), l);
  uint8_t *ipad = (uint8_t *)alloca(l * sizeof (uint8_t));
  memset(ipad, (uint8_t)0x36U, l * sizeof (uint8_t));
  for (uint32_t i = (uint32_t)0U; i < l; i++)
  {
    uint8_t xi = ipad[i];
    uint8_t yi = key_block[i];
    ipad[i] = xi ^ yi;
  }
  KRML_CHECK_SIZE(sizeof (uint8_t), l);
  uint8_t *opad = (uint8_t *)alloca(l * sizeof (uint8_t));
  memset(opad, (uint8_t)0x5cU, l * sizeof (uint8_t));
  for (uint32_t i = (uint32_t)0U; i < l; i++)
  {
    uint8_t xi = opad[i];
    uint8_t yi = key_block[i];
    opad[i] = xi ^ yi;
  }
  uint64_t s[16U] = { 0U };
  Hacl_Blake2b_32_blake2b_init(s, (uint32_t)0U, (uint32_t)64U);
  uint64_t *s0 = s;
  uint8_t *dst1 = ipad;
  if (data_len == (uint32_t)0U)
  {
    uint64_t wv[16U] = { 0U };
    Hacl_Blake2b_32_blake2b_update_last((uint32_t)128U,
      wv,
      s0,
      FStar_UInt128_uint64_to_uint128((uint64_t)0U),
      (uint32_t)128U,
      ipad);
  }
  else
  {
    uint32_t block_len = (uint32_t)128U;
    uint32_t n_blocks0 = data_len / block_len;
    uint32_t rem0 = data_len % block_len;
    K___uint32_t_uint32_t scrut;
    if (n_blocks0 > (uint32_t)0U && rem0 == (uint32_t)0U)
    {
      uint32_t n_blocks_ = n_blocks0 - (uint32_t)1U;
      scrut = ((K___uint32_t_uint32_t){ .fst = n_blocks_, .snd = data_len - n_blocks_ * block_len });
    }
    else
    {
      scrut = ((K___uint32_t_uint32_t){ .fst = n_blocks0, .snd = rem0 });
    }
    uint32_t n_blocks = scrut.fst;
    uint32_t rem_len = scrut.snd;
    uint32_t full_blocks_len = n_blocks * block_len;
    uint8_t *full_blocks = data;
    uint8_t *rem = data + full_blocks_len;
    uint64_t wv[16U] = { 0U };
    Hacl_Blake2b_32_blake2b_update_multi((uint32_t)128U,
      wv,
      s0,
      FStar_UInt128_uint64_to_uint128((uint64_t)0U),
      ipad,
      (uint32_t)1U);
    uint64_t wv0[16U] = { 0U };
    Hacl_Blake2b_32_blake2b_update_multi(n_blocks * (uint32_t)128U,
      wv0,
      s0,
      FStar_UInt128_uint64_to_uint128((uint64_t)block_len),
      full_blocks,
      n_blocks);
    uint64_t wv1[16U] = { 0U };
    Hacl_Blake2b_32_blake2b_update_last(rem_len,
      wv1,
      s0,
      FStar_UInt128_add(FStar_UInt128_uint64_to_uint128((uint64_t)(uint32_t)128U),
        FStar_UInt128_uint64_to_uint128((uint64_t)full_blocks_len)),
      rem_len,
      rem);
  }
  Hacl_Blake2b_32_blake2b_finish((uint32_t)64U, dst1, s0);
  uint8_t *hash1 = ipad;
  Hacl_Blake2b_32_blake2b_init(s0, (uint32_t)0U, (uint32_t)64U);
  uint32_t block_len = (uint32_t)128U;
  uint32_t n_blocks0 = (uint32_t)64U / block_len;
  uint32_t rem0 = (uint32_t)64U % block_len;
  K___uint32_t_uint32_t scrut;
  if (n_blocks0 > (uint32_t)0U && rem0 == (uint32_t)0U)
  {
    uint32_t n_blocks_ = n_blocks0 - (uint32_t)1U;
    scrut =
      ((K___uint32_t_uint32_t){ .fst = n_blocks_, .snd = (uint32_t)64U - n_blocks_ * block_len });
  }
  else
  {
    scrut = ((K___uint32_t_uint32_t){ .fst = n_blocks0, .snd = rem0 });
  }
  uint32_t n_blocks = scrut.fst;
  uint32_t rem_len = scrut.snd;
  uint32_t full_blocks_len = n_blocks * block_len;
  uint8_t *full_blocks = hash1;
  uint8_t *rem = hash1 + full_blocks_len;
  uint64_t wv[16U] = { 0U };
  Hacl_Blake2b_32_blake2b_update_multi((uint32_t)128U,
    wv,
    s0,
    FStar_UInt128_uint64_to_uint128((uint64_t)0U),
    opad,
    (uint32_t)1U);
  uint64_t wv0[16U] = { 0U };
  Hacl_Blake2b_32_blake2b_update_multi(n_blocks * (uint32_t)128U,
    wv0,
    s0,
    FStar_UInt128_uint64_to_uint128((uint64_t)block_len),
    full_blocks,
    n_blocks);
  uint64_t wv1[16U] = { 0U };
  Hacl_Blake2b_32_blake2b_update_last(rem_len,
    wv1,
    s0,
    FStar_UInt128_add(FStar_UInt128_uint64_to_uint128((uint64_t)(uint32_t)128U),
      FStar_UInt128_uint64_to_uint128((uint64_t)full_blocks_len)),
    rem_len,
    rem);
  Hacl_Blake2b_32_blake2b_finish((uint32_t)64U, dst, s0);
}

void
EverCrypt_HMAC_compute(
  Spec_Hash_Definitions_hash_alg a,
  uint8_t *mac,
  uint8_t *key,
  uint32_t keylen,
  uint8_t *data,
  uint32_t datalen
)
{
  switch (a)
  {
    case Spec_Hash_Definitions_SHA1:
      {
        EverCrypt_HMAC_compute_sha1(mac, key, keylen, data, datalen);
        break;
      }
    case Spec_Hash_Definitions_SHA2_256:
      {
        EverCrypt_HMAC_compute_sha2_256(mac, key, keylen, data, datalen);
        break;
      }
    case Spec_Hash_Definitions_SHA2_384:
      {
        EverCrypt_HMAC_compute_sha2_384(mac, key, keylen, data, datalen);
        break;
      }
    case Spec_Hash_Definitions_SHA2_512:
      {
        EverCrypt_HMAC_compute_sha2_512(mac, key, keylen, data, datalen);
        break;
      }
    case Spec_Hash_Definitions_Blake2S:
      {
        EverCrypt_HMAC_compute_blake2s(mac, key, keylen, data, datalen);
        break;
      }
    case Spec_Hash_Definitions_Blake2B:
      {
        EverCrypt_HMAC_compute_blake2b(mac, key, keylen, data, datalen);
        break;
      }
    default:
      {
        KRML_HOST_EPRINTF("KaRaMeL incomplete match at %s:%d\n", __FILE__, __LINE__);
        KRML_HOST_EXIT(253U);
      }
  }
}

