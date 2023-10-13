int select_hash(int sha2_ndx)
{
    switch (sha2_ndx)
    {
    case 0:
        return 224 / 8; 
    case 1:
        return 256 / 8;
    case 2:
        return 384 / 8; 
    case 3:
        return 512 / 8; 
    case 4:
        return 224 / 8; 
    case 5:
        return 256 / 8; 
    }

    return -1;
}

typedef __SIZE_TYPE__ size_t;

void execute_hash(int sha2_ndx, const void* m, size_t len, void* digest)
{
    switch (sha2_ndx)
    {
    case 0: 
        sha224(m, len, digest);
        break;
    case 1: 
        sha256(m, len, digest);
        break;
    case 2: 
        sha384(m, len, digest);
        break;
    case 3: 
        sha512(m, len, digest);
        break;
    case 4: 
        sha512_224(m, len, digest);
        break;
    case 5: 
        sha512_256(m, len, digest);
        break;
    }
}

typedef __UINT32_TYPE__ uint32_t;

static unsigned char* mgf(int sha2_ndx, void* mgfSeed, size_t seedLen, void* mask, size_t maskLen)
{
    uint32_t i, count, c;
    unsigned char* mgfIn, * m;
    int hLen = select_hash(sha2_ndx);

    if (maskLen > 0x0100000000 * hLen) 
        return NULL;

    mgfIn = (unsigned char*)malloc(seedLen + 4);
    memset(mgfIn, 0, seedLen + 4);
    memcpy(mgfIn, mgfSeed, seedLen);
    count = maskLen / hLen + (maskLen % hLen ? 1 : 0);
    m = (unsigned char*)malloc(count * hLen);
    memset(m, 0, count * hLen);

    for (i = 0; i < count; i++)
    {
        c = i;
        mgfIn[seedLen + 3] = c & 0x000000ff;
        c >>= 8;
        mgfIn[seedLen + 2] = c & 0x000000ff;
        c >>= 8;
        mgfIn[seedLen + 1] = c & 0x000000ff;
        c >>= 8;
        execute_hash(sha2_ndx, mgfIn, seedLen + 4, m + i * hLen);
    }

    memcpy(mask, m, maskLen);
    free(mgfIn);
    free(m);
    return mask;
}


typedef unsigned long int	mp_limb_t;
typedef long int		mp_limb_signed_t;
typedef unsigned long int	mp_bitcnt_t;

typedef struct
{
  int _mp_alloc;		/* Number of *limbs* allocated and pointed
				   to by the _mp_d field.  */
  int _mp_size;			/* abs(_mp_size) is the number of limbs the
				   last field points to.  If _mp_size is
				   negative this is a negative number.  */
  mp_limb_t *_mp_d;		/* Pointer to the limbs.  */
} __mpz_struct;

typedef __mpz_struct mpz_t[1];

typedef enum
{
  GMP_RAND_ALG_DEFAULT = 0,
  GMP_RAND_ALG_LC = GMP_RAND_ALG_DEFAULT /* Linear congruential.  */
} gmp_randalg_t;

typedef struct
{
  mpz_t _mp_seed;	  /* _mp_d member points to state of the generator. */
  gmp_randalg_t _mp_alg;  /* Currently unused. */
  union {
    void *_mp_lc;         /* Pointer to function pointers structure.  */
  } _mp_algdata;
} __gmp_randstate_struct;
typedef __gmp_randstate_struct gmp_randstate_t[1];


void rsa_generate_key(void* _e, void* _d, void* _n, int mode)
{
    mpz_t p, q, lambda, e, d, n, gcd;
    gmp_randstate_t state;

    /*
     * Initialize mpz variables
     */
    mpz_inits(p, q, lambda, e, d, n, gcd, NULL);
    gmp_randinit_default(state);
    gmp_randseed_ui(state, arc4random());
    /*
     * Generate prime p and q such that 2^(RSAKEYSIZE-1) <= p*q < 2^RSAKEYSIZE
     */
    do
    {
        do
        {
            mpz_urandomb(p, state, RSAKEYSIZE / 2);
            mpz_setbit(p, 0);
            mpz_setbit(p, RSAKEYSIZE / 2 - 1);
        } while (mpz_probab_prime_p(p, 50) == 0);
        do
        {
            mpz_urandomb(q, state, RSAKEYSIZE / 2);
            mpz_setbit(q, 0);
            mpz_setbit(q, RSAKEYSIZE / 2 - 1);
        } while (mpz_probab_prime_p(q, 50) == 0);
        mpz_mul(n, p, q);
    } while (!mpz_tstbit(n, RSAKEYSIZE - 1));
    /*
     * Generate e and d using Lambda(n)
     */
    mpz_sub_ui(p, p, 1);
    mpz_sub_ui(q, q, 1);
    mpz_lcm(lambda, p, q);
    if (mode == 0)
        mpz_set_ui(e, 65537);
    else
        do
        {
            mpz_urandomb(e, state, RSAKEYSIZE);
            mpz_gcd(gcd, e, lambda);
        } while (mpz_cmp(e, lambda) >= 0 || mpz_cmp_ui(gcd, 1) != 0);
        mpz_invert(d, e, lambda);
        /*
         * Convert mpz_t values into octet strings
         */
        mpz_export(_e, NULL, 1, RSAKEYSIZE / 8, 1, 0, e);
        mpz_export(_d, NULL, 1, RSAKEYSIZE / 8, 1, 0, d);
        mpz_export(_n, NULL, 1, RSAKEYSIZE / 8, 1, 0, n);
        /*
         * Free the space occupied by mpz variables
         */
        mpz_clears(p, q, lambda, e, d, n, gcd, NULL);
}

/*
 * rsa_cipher() - compute m^k mod n
 * If m >= n then returns PKCS_MSG_OUT_OF_RANGE, otherwise returns 0 for success.
 */
static int rsa_cipher(void* _m, const void* _k, const void* _n)
{
    mpz_t m, k, n;

    /*
     * Initialize mpz variables
     */
    mpz_inits(m, k, n, NULL);
    /*
     * Convert big-endian octets into mpz_t values
     */
    mpz_import(m, RSAKEYSIZE / 8, 1, 1, 1, 0, _m);
    mpz_import(k, RSAKEYSIZE / 8, 1, 1, 1, 0, _k);
    mpz_import(n, RSAKEYSIZE / 8, 1, 1, 1, 0, _n);
    /*
     * Compute m^k mod n
     */
    if (mpz_cmp(m, n) >= 0)
    {
        mpz_clears(m, k, n, NULL);
        return PKCS_MSG_OUT_OF_RANGE;
    }
    mpz_powm(m, m, k, n);
    /*
     * Convert mpz_t m into the octet string _m
     */
    mpz_export(_m, NULL, 1, RSAKEYSIZE / 8, 1, 0, m);
    /*
     * Free the space occupied by mpz variables
     */
    mpz_clears(m, k, n, NULL);
    return 0;
}


int rsaes_oaep_encrypt(const void* m, size_t mLen, const void* label, const void* e, const void* n, void* c, int sha2_ndx)
{
     
    int mHash_byte = select_hash(sha2_ndx);
    int hLen = mHash_byte;
    int SHASIZE = mHash_byte * 8;
    size_t RSA_byte = RSAKEYSIZE / 8;
    size_t DB_byte = RSAKEYSIZE / 8 - mHash_byte - 1;
    if (strlen(label) > hLen) 
        return PKCS_LABEL_TOO_LONG;
    if (mLen > RSA_byte - 2 * hLen - 2) 
        return PKCS_MSG_TOO_LONG;
    unsigned char Hlabel[SHASIZE];
    unsigned char DB[DB_byte];
    unsigned char EM[RSAKEYSIZE], maskedDB[DB_byte];
    unsigned char seed[mHash_byte];
    unsigned char maskedSeed[mHash_byte];
    size_t PS_byte = DB_byte - mHash_byte - mLen - 1;

    *seed = arc4random_uniform(SHASIZE);

    execute_hash(sha2_ndx, label, strlen(label), Hlabel); 
    memcpy(DB, Hlabel, mHash_byte); 
    memset(DB + mHash_byte, 0x00, PS_byte); 
    DB[mHash_byte + PS_byte] = 0x01; 
    memcpy(DB + mHash_byte + PS_byte + 1, m, mLen); 

    
    mgf(sha2_ndx, seed, mHash_byte, maskedDB, DB_byte);
    for (int i = 0; i < DB_byte; i++)
    {
        maskedDB[i] ^= DB[i];
    }

    
    mgf(sha2_ndx, maskedDB, DB_byte, maskedSeed, mHash_byte);
    for (int j = 0; j < mHash_byte; j++)
    {
        maskedSeed[j] ^= seed[j];
    }

    EM[0] = 0x00; 
    memcpy(EM + 1, maskedSeed, mHash_byte); 
    memcpy(EM + 1 + mHash_byte, maskedDB, DB_byte); 

    if (rsa_cipher(EM, e, n))
        return PKCS_MSG_OUT_OF_RANGE;

    memcpy(c, EM, RSA_byte);

    return 0;
}


int rsaes_oaep_decrypt(void* m, size_t* mLen, const void* label, const void* d, const void* n, const void* c, int sha2_ndx)
{
    int mHash_byte = select_hash(sha2_ndx); 
    int SHASIZE = mHash_byte * 8;
    size_t RSA_byte = RSAKEYSIZE / 8;
    size_t DB_byte = DB_byte = RSAKEYSIZE / 8 - mHash_byte - 1;

    if (strlen(label) > SHASIZE / 8)
        return PKCS_LABEL_TOO_LONG;

    unsigned char Hlabel[SHASIZE];
    unsigned char DB[DB_byte];
    unsigned char PS_message[DB_byte - mHash_byte];
 
    unsigned char EM[RSAKEYSIZE], maskedDB[DB_byte];
    unsigned char seed[mHash_byte];
    unsigned char maskedSeed[mHash_byte];
    size_t PS_byte;

    memcpy(EM, c, RSA_byte);

    if (rsa_cipher(EM, d, n))
        return PKCS_MSG_OUT_OF_RANGE;

    if (EM[0] != 0x00)
        return PKCS_INITIAL_NONZERO;

    memcpy(maskedSeed, EM + 1, mHash_byte);         
    memcpy(maskedDB, EM + 1 + mHash_byte, DB_byte); 

    mgf(sha2_ndx, maskedDB, DB_byte, seed, mHash_byte); 
    for (int i = 0; i < mHash_byte; i++)
    {
        seed[i] ^= maskedSeed[i]; 
    }

    mgf(sha2_ndx, seed, mHash_byte, DB, DB_byte); 
    for (int j = 0; j < DB_byte; j++)
    {
        DB[j] ^= maskedDB[j]; 
    }

    execute_hash(sha2_ndx, label, strlen(label), Hlabel); 

    for (int i = 0; i < mHash_byte; i++)
    {
        if (DB[i] != Hlabel[i]) 
        {
            return PKCS_HASH_MISMATCH;
        }
    }

    memcpy(PS_message, DB + mHash_byte, DB_byte - mHash_byte); 

    for (PS_byte = 0; PS_byte < DB_byte - mHash_byte; ++PS_byte)
    {
        if (PS_message[PS_byte] != 0x00)
        {
            if (PS_message[PS_byte] == 0x01) 
            {
                break;
            }
            else 
            {
                return PKCS_INVALID_PS;
            }
        }
    }

    *mLen = DB_byte - mHash_byte - PS_byte - 1;
    memcpy(m, PS_message + PS_byte + 1, (size_t)*mLen);

    return 0;
}


int rsassa_pss_sign(const void* m, size_t mLen, const void* d, const void* n, void* s, int sha2_ndx)
{
    if (mLen >= 0x2000000000000000)
        return PKCS_MSG_TOO_LONG;

    unsigned int h_len = select_hash(sha2_ndx), M_len = 2 * h_len + 8, DB_len = RSAKEYSIZE / 8 - h_len - 1, ps_len = DB_len - 1 - h_len;
    unsigned char mHash[h_len], M[M_len], DB[DB_len], mask[DB_len], EM[RSAKEYSIZE / 8], salt[h_len];

    execute_hash(sha2_ndx, m, mLen, mHash);

    memset(M, 0, 8);             
    memcpy(M + 8, mHash, h_len); 
    arc4random_buf(salt, h_len);
    memcpy(M + 8 + h_len, salt, h_len); 

    execute_hash(sha2_ndx, M, M_len, mHash);

    memset(DB, 0, ps_len);                
    memset(DB + ps_len, 0x01, 1);       
    memcpy(DB + ps_len + 1, salt, h_len); 

    mgf(sha2_ndx, mHash, h_len, mask, DB_len);
    for (int i = 0; i < DB_len; i++)
        DB[i] = DB[i] ^ mask[i]; 

    memcpy(EM, DB, DB_len);                  
    memcpy(EM + DB_len, mHash, h_len);        
    memset(EM + RSAKEYSIZE / 8 - 1, 0xbc, 1);
    EM[0] &= 0x7f;                           

    rsa_cipher(EM, d, n);
    memcpy(s, EM, RSAKEYSIZE / 8);

    return 0;
}


int rsassa_pss_verify(const void* m, size_t mLen, const void* e, const void* n, const void* s, int sha2_ndx)
{
    if (mLen >= 0x2000000000000000)
        return PKCS_MSG_TOO_LONG;

    int h_len = select_hash(sha2_ndx), M_len = 2 * h_len + 8, DB_len = RSAKEYSIZE / 8 - h_len - 1, ps_len = DB_len - 1 - h_len;
    char mHash[h_len], H[h_len], M[M_len], DB[DB_len], mask[DB_len], EM[RSAKEYSIZE / 8], salt[h_len];

    memcpy(EM, s, RSAKEYSIZE / 8);
    rsa_cipher(EM, e, n);

    if (EM[RSAKEYSIZE / 8 - 1] != (char)0xbc)
        return PKCS_INVALID_LAST;

    if ((EM[0] >> 7) & 1)
        return PKCS_INVALID_INIT;

    memcpy(DB, EM, DB_len);

    memcpy(H, EM + DB_len, h_len);

    mgf(sha2_ndx, H, h_len, mask, DB_len); 
    for (int i = 0; i < DB_len; i++)
        DB[i] = DB[i] ^ mask[i]; 
    DB[0] &= 0x7f;               

    for (int i = 0; i < ps_len; i++)
        if (DB[i] != (char)0x00)
            return PKCS_INVALID_PD2;
    if (DB[ps_len] != (char)0x01)
        return PKCS_INVALID_PD2;

    memcpy(salt, DB + ps_len + 1, h_len);

    execute_hash(sha2_ndx, m, mLen, mHash);

    memset(M, 0, 8);                   
    memcpy(M + 8, mHash, h_len);       
    memcpy(M + 8 + h_len, salt, h_len);

    execute_hash(sha2_ndx, M, M_len, mHash);

    if (memcmp(mHash, H, h_len) == 0)
        return 0;
    else
        return PKCS_HASH_MISMATCH;
}