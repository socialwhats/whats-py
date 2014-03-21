
#include <Python.h>
#include <stddef.h>

#ifdef MS_WIN32
#include <malloc.h>   /* for alloca() */
typedef __int8 int8_t;
typedef __int16 int16_t;
typedef __int32 int32_t;
typedef __int64 int64_t;
typedef unsigned __int8 uint8_t;
typedef unsigned __int16 uint16_t;
typedef unsigned __int32 uint32_t;
typedef unsigned __int64 uint64_t;
typedef unsigned char _Bool;
#endif

#if PY_MAJOR_VERSION < 3
# undef PyCapsule_CheckExact
# undef PyCapsule_GetPointer
# define PyCapsule_CheckExact(capsule) (PyCObject_Check(capsule))
# define PyCapsule_GetPointer(capsule, name) \
    (PyCObject_AsVoidPtr(capsule))
#endif

#if PY_MAJOR_VERSION >= 3
# define PyInt_FromLong PyLong_FromLong
#endif

#define _cffi_from_c_double PyFloat_FromDouble
#define _cffi_from_c_float PyFloat_FromDouble
#define _cffi_from_c_long PyInt_FromLong
#define _cffi_from_c_ulong PyLong_FromUnsignedLong
#define _cffi_from_c_longlong PyLong_FromLongLong
#define _cffi_from_c_ulonglong PyLong_FromUnsignedLongLong

#define _cffi_to_c_double PyFloat_AsDouble
#define _cffi_to_c_float PyFloat_AsDouble

#define _cffi_from_c_SIGNED(x, type)                                     \
    (sizeof(type) <= sizeof(long) ? PyInt_FromLong(x) :                  \
                                    PyLong_FromLongLong(x))
#define _cffi_from_c_UNSIGNED(x, type)                                   \
    (sizeof(type) < sizeof(long) ? PyInt_FromLong(x) :                   \
     sizeof(type) == sizeof(long) ? PyLong_FromUnsignedLong(x) :         \
                                    PyLong_FromUnsignedLongLong(x))

#define _cffi_to_c_SIGNED(o, type)                                       \
    (sizeof(type) == 1 ? _cffi_to_c_i8(o) :                              \
     sizeof(type) == 2 ? _cffi_to_c_i16(o) :                             \
     sizeof(type) == 4 ? _cffi_to_c_i32(o) :                             \
     sizeof(type) == 8 ? _cffi_to_c_i64(o) :                             \
     (Py_FatalError("unsupported size for type " #type), 0))
#define _cffi_to_c_UNSIGNED(o, type)                                     \
    (sizeof(type) == 1 ? _cffi_to_c_u8(o) :                              \
     sizeof(type) == 2 ? _cffi_to_c_u16(o) :                             \
     sizeof(type) == 4 ? _cffi_to_c_u32(o) :                             \
     sizeof(type) == 8 ? _cffi_to_c_u64(o) :                             \
     (Py_FatalError("unsupported size for type " #type), 0))

#define _cffi_to_c_i8                                                    \
                 ((int(*)(PyObject *))_cffi_exports[1])
#define _cffi_to_c_u8                                                    \
                 ((int(*)(PyObject *))_cffi_exports[2])
#define _cffi_to_c_i16                                                   \
                 ((int(*)(PyObject *))_cffi_exports[3])
#define _cffi_to_c_u16                                                   \
                 ((int(*)(PyObject *))_cffi_exports[4])
#define _cffi_to_c_i32                                                   \
                 ((int(*)(PyObject *))_cffi_exports[5])
#define _cffi_to_c_u32                                                   \
                 ((unsigned int(*)(PyObject *))_cffi_exports[6])
#define _cffi_to_c_i64                                                   \
                 ((long long(*)(PyObject *))_cffi_exports[7])
#define _cffi_to_c_u64                                                   \
                 ((unsigned long long(*)(PyObject *))_cffi_exports[8])
#define _cffi_to_c_char                                                  \
                 ((int(*)(PyObject *))_cffi_exports[9])
#define _cffi_from_c_pointer                                             \
    ((PyObject *(*)(char *, CTypeDescrObject *))_cffi_exports[10])
#define _cffi_to_c_pointer                                               \
    ((char *(*)(PyObject *, CTypeDescrObject *))_cffi_exports[11])
#define _cffi_get_struct_layout                                          \
    ((PyObject *(*)(Py_ssize_t[]))_cffi_exports[12])
#define _cffi_restore_errno                                              \
    ((void(*)(void))_cffi_exports[13])
#define _cffi_save_errno                                                 \
    ((void(*)(void))_cffi_exports[14])
#define _cffi_from_c_char                                                \
    ((PyObject *(*)(char))_cffi_exports[15])
#define _cffi_from_c_deref                                               \
    ((PyObject *(*)(char *, CTypeDescrObject *))_cffi_exports[16])
#define _cffi_to_c                                                       \
    ((int(*)(char *, CTypeDescrObject *, PyObject *))_cffi_exports[17])
#define _cffi_from_c_struct                                              \
    ((PyObject *(*)(char *, CTypeDescrObject *))_cffi_exports[18])
#define _cffi_to_c_wchar_t                                               \
    ((wchar_t(*)(PyObject *))_cffi_exports[19])
#define _cffi_from_c_wchar_t                                             \
    ((PyObject *(*)(wchar_t))_cffi_exports[20])
#define _cffi_to_c_long_double                                           \
    ((long double(*)(PyObject *))_cffi_exports[21])
#define _cffi_to_c__Bool                                                 \
    ((_Bool(*)(PyObject *))_cffi_exports[22])
#define _cffi_prepare_pointer_call_argument                              \
    ((Py_ssize_t(*)(CTypeDescrObject *, PyObject *, char **))_cffi_exports[23])
#define _cffi_convert_array_from_object                                  \
    ((int(*)(char *, CTypeDescrObject *, PyObject *))_cffi_exports[24])
#define _CFFI_NUM_EXPORTS 25

typedef struct _ctypedescr CTypeDescrObject;

static void *_cffi_exports[_CFFI_NUM_EXPORTS];
static PyObject *_cffi_types, *_cffi_VerificationError;

static int _cffi_setup_custom(PyObject *lib);   /* forward */

static PyObject *_cffi_setup(PyObject *self, PyObject *args)
{
    PyObject *library;
    int was_alive = (_cffi_types != NULL);
    if (!PyArg_ParseTuple(args, "OOO", &_cffi_types, &_cffi_VerificationError,
                                       &library))
        return NULL;
    Py_INCREF(_cffi_types);
    Py_INCREF(_cffi_VerificationError);
    if (_cffi_setup_custom(library) < 0)
        return NULL;
    return PyBool_FromLong(was_alive);
}

static void _cffi_init(void)
{
    PyObject *module = PyImport_ImportModule("_cffi_backend");
    PyObject *c_api_object;

    if (module == NULL)
        return;

    c_api_object = PyObject_GetAttrString(module, "_C_API");
    if (c_api_object == NULL)
        return;
    if (!PyCapsule_CheckExact(c_api_object)) {
        PyErr_SetNone(PyExc_ImportError);
        return;
    }
    memcpy(_cffi_exports, PyCapsule_GetPointer(c_api_object, "cffi"),
           _CFFI_NUM_EXPORTS * sizeof(void *));
}

#define _cffi_type(num) ((CTypeDescrObject *)PyList_GET_ITEM(_cffi_types, num))

/**********/



#ifdef __APPLE__
#include <AvailabilityMacros.h>
#define __ORIG_DEPRECATED_IN_MAC_OS_X_VERSION_10_7_AND_LATER     DEPRECATED_IN_MAC_OS_X_VERSION_10_7_AND_LATER
#undef DEPRECATED_IN_MAC_OS_X_VERSION_10_7_AND_LATER
#define DEPRECATED_IN_MAC_OS_X_VERSION_10_7_AND_LATER
#endif


#include <openssl/aes.h>


#include <openssl/asn1.h>


#include <openssl/bn.h>


#include <openssl/bio.h>


#include <openssl/conf.h>


#include <openssl/crypto.h>


#include <openssl/dh.h>


#include <openssl/dsa.h>


#ifndef OPENSSL_NO_EC
#include <openssl/ec.h>
#endif

#include <openssl/obj_mac.h>


#include <openssl/engine.h>


#include <openssl/err.h>


#include <openssl/evp.h>


#include <openssl/hmac.h>



#include <openssl/objects.h>


#include <openssl/opensslv.h>


#ifdef _WIN32
#include <Wincrypt.h>
#else
#include <fcntl.h>
#include <unistd.h>
#endif


#include <openssl/pem.h>


#include <openssl/pkcs7.h>


#include <openssl/pkcs12.h>


#include <openssl/rand.h>


#include <openssl/rsa.h>


#include <openssl/ssl.h>


#include <openssl/ssl.h>

/*
 * This is part of a work-around for the difficulty cffi has in dealing with
 * `STACK_OF(foo)` as the name of a type.  We invent a new, simpler name that
 * will be an alias for this type and use the alias throughout.  This works
 * together with another opaque typedef for the same name in the TYPES section.
 * Note that the result is an opaque type.
 */
typedef STACK_OF(X509) Cryptography_STACK_OF_X509;
typedef STACK_OF(X509_REVOKED) Cryptography_STACK_OF_X509_REVOKED;


#include <openssl/x509.h>

/*
 * See the comment above Cryptography_STACK_OF_X509 in x509.py
 */
typedef STACK_OF(X509_NAME) Cryptography_STACK_OF_X509_NAME;


#include <openssl/x509v3.h>


#ifdef __APPLE__
#undef DEPRECATED_IN_MAC_OS_X_VERSION_10_7_AND_LATER
#define DEPRECATED_IN_MAC_OS_X_VERSION_10_7_AND_LATER     __ORIG_DEPRECATED_IN_MAC_OS_X_VERSION_10_7_AND_LATER
#endif


int AES_set_encrypt_key(const unsigned char *, const int, AES_KEY *);
int AES_set_decrypt_key(const unsigned char *, const int, AES_KEY *);
int AES_wrap_key(AES_KEY *, const unsigned char *, unsigned char *,
                 const unsigned char *, unsigned int);
int AES_unwrap_key(AES_KEY *, const unsigned char *, unsigned char *,
                   const unsigned char *, unsigned int);


ASN1_OBJECT *ASN1_OBJECT_new(void);
void ASN1_OBJECT_free(ASN1_OBJECT *);

/*  ASN1 OBJECT IDENTIFIER */
ASN1_OBJECT *d2i_ASN1_OBJECT(ASN1_OBJECT **, const unsigned char **, long);
int i2d_ASN1_OBJECT(ASN1_OBJECT *, unsigned char **);

/*  ASN1 STRING */
ASN1_STRING *ASN1_STRING_new(void);
ASN1_STRING *ASN1_STRING_type_new(int);
void ASN1_STRING_free(ASN1_STRING *);
unsigned char *ASN1_STRING_data(ASN1_STRING *);
int ASN1_STRING_set(ASN1_STRING *, const void *, int);
int ASN1_STRING_type(ASN1_STRING *);
int ASN1_STRING_to_UTF8(unsigned char **, ASN1_STRING *);

/*  ASN1 OCTET STRING */
ASN1_OCTET_STRING *ASN1_OCTET_STRING_new(void);
void ASN1_OCTET_STRING_free(ASN1_OCTET_STRING *);
int ASN1_OCTET_STRING_set(ASN1_OCTET_STRING *, const unsigned char *, int);

/*  ASN1 INTEGER */
ASN1_INTEGER *ASN1_INTEGER_new(void);
void ASN1_INTEGER_free(ASN1_INTEGER *);
int ASN1_INTEGER_set(ASN1_INTEGER *, long);
int i2a_ASN1_INTEGER(BIO *, ASN1_INTEGER *);

/*  ASN1 TIME */
ASN1_TIME *ASN1_TIME_new(void);
ASN1_GENERALIZEDTIME *ASN1_TIME_to_generalizedtime(ASN1_TIME *,
                                                   ASN1_GENERALIZEDTIME **);

/*  ASN1 UTCTIME */
int ASN1_UTCTIME_cmp_time_t(const ASN1_UTCTIME *, time_t);

/*  ASN1 GENERALIZEDTIME */
int ASN1_GENERALIZEDTIME_set_string(ASN1_GENERALIZEDTIME *, const char *);
void ASN1_GENERALIZEDTIME_free(ASN1_GENERALIZEDTIME *);
int ASN1_GENERALIZEDTIME_check(ASN1_GENERALIZEDTIME *);

/*  ASN1 ENUMERATED */
ASN1_ENUMERATED *ASN1_ENUMERATED_new(void);
void ASN1_ENUMERATED_free(ASN1_ENUMERATED *);
int ASN1_ENUMERATED_set(ASN1_ENUMERATED *, long);

ASN1_VALUE *ASN1_item_d2i(ASN1_VALUE **, const unsigned char **, long,
                          const ASN1_ITEM *);


BIGNUM *BN_new(void);
void BN_free(BIGNUM *);

BIGNUM *BN_copy(BIGNUM *, const BIGNUM *);
BIGNUM *BN_dup(const BIGNUM *);

int BN_set_word(BIGNUM *, BN_ULONG);
BN_ULONG BN_get_word(const BIGNUM *);

const BIGNUM *BN_value_one(void);

char *BN_bn2hex(const BIGNUM *);
int BN_hex2bn(BIGNUM **, const char *);
int BN_dec2bn(BIGNUM **, const char *);

int BN_bn2bin(const BIGNUM *, unsigned char *);
BIGNUM *BN_bin2bn(const unsigned char *, int, BIGNUM *);

int BN_num_bits(const BIGNUM *);

int BN_add(BIGNUM *, const BIGNUM *, const BIGNUM *);
int BN_sub(BIGNUM *, const BIGNUM *, const BIGNUM *);
int BN_mul(BIGNUM *, const BIGNUM *, const BIGNUM *, BN_CTX *);
int BN_sqr(BIGNUM *, const BIGNUM *, BN_CTX *);
int BN_div(BIGNUM *, BIGNUM *, const BIGNUM *, const BIGNUM *, BN_CTX *);
int BN_nnmod(BIGNUM *, const BIGNUM *, const BIGNUM *, BN_CTX *);
int BN_mod_add(BIGNUM *, const BIGNUM *, const BIGNUM *, const BIGNUM *,
               BN_CTX *);
int BN_mod_sub(BIGNUM *, const BIGNUM *, const BIGNUM *, const BIGNUM *,
               BN_CTX *);
int BN_mod_mul(BIGNUM *, const BIGNUM *, const BIGNUM *, const BIGNUM *,
               BN_CTX *);
int BN_mod_sqr(BIGNUM *, const BIGNUM *, const BIGNUM *, BN_CTX *);
int BN_exp(BIGNUM *, const BIGNUM *, const BIGNUM *, BN_CTX *);
int BN_mod_exp(BIGNUM *, const BIGNUM *, const BIGNUM *, const BIGNUM *,
               BN_CTX *);
int BN_gcd(BIGNUM *, const BIGNUM *, const BIGNUM *, BN_CTX *);
BIGNUM *BN_mod_inverse(BIGNUM *, const BIGNUM *, const BIGNUM *, BN_CTX *);


BIO* BIO_new(BIO_METHOD *);
int BIO_set(BIO *, BIO_METHOD *);
int BIO_free(BIO *);
void BIO_vfree(BIO *);
void BIO_free_all(BIO *);
BIO *BIO_push(BIO *, BIO *);
BIO *BIO_pop(BIO *);
BIO *BIO_next(BIO *);
BIO *BIO_find_type(BIO *, int);
int BIO_method_type(const BIO *);
BIO_METHOD *BIO_s_mem(void);
BIO *BIO_new_mem_buf(void *, int);
BIO_METHOD *BIO_s_file(void);
BIO *BIO_new_file(const char *, const char *);
BIO *BIO_new_fp(FILE *, int);
BIO_METHOD *BIO_s_fd(void);
BIO *BIO_new_fd(int, int);
BIO_METHOD *BIO_s_socket(void);
BIO *BIO_new_socket(int, int);
BIO_METHOD *BIO_s_null(void);
long BIO_ctrl(BIO *, int, long, void *);
long BIO_callback_ctrl(
    BIO *,
    int,
    void (*)(struct bio_st *, int, const char *, int, long, long)
);
char* BIO_ptr_ctrl(BIO *bp, int cmd, long larg);
long BIO_int_ctrl(BIO *bp, int cmd, long larg, int iarg);
size_t BIO_ctrl_pending(BIO *b);
size_t BIO_ctrl_wpending(BIO *b);
int BIO_read(BIO *, void *, int);
int BIO_gets(BIO *, char *, int);
int BIO_write(BIO *, const void *, int);
int BIO_puts(BIO *, const char *);
BIO_METHOD *BIO_f_null(void);
BIO_METHOD *BIO_f_buffer(void);




unsigned long SSLeay(void);
const char *SSLeay_version(int);

void CRYPTO_free(void *);
int CRYPTO_mem_ctrl(int);
int CRYPTO_is_mem_check_on(void);
void CRYPTO_mem_leaks(struct bio_st *);
void CRYPTO_cleanup_all_ex_data(void);
int CRYPTO_num_locks(void);
void CRYPTO_set_locking_callback(void(*)(int, int, const char *, int));
void CRYPTO_set_id_callback(unsigned long (*)(void));
unsigned long (*CRYPTO_get_id_callback(void))(void);
void (*CRYPTO_get_locking_callback(void))(int, int, const char *, int);
void CRYPTO_lock(int, int, const char *, int);

void OPENSSL_free(void *);


DH *DH_new(void);
void DH_free(DH *);


DSA *DSA_generate_parameters(int, unsigned char *, int, int *, unsigned long *,
                             void (*)(int, int, void *), void *);
int DSA_generate_key(DSA *);
void DSA_free(DSA *);




ENGINE *ENGINE_get_first(void);
ENGINE *ENGINE_get_last(void);
ENGINE *ENGINE_get_next(ENGINE *);
ENGINE *ENGINE_get_prev(ENGINE *);
int ENGINE_add(ENGINE *);
int ENGINE_remove(ENGINE *);
ENGINE *ENGINE_by_id(const char *);
int ENGINE_init(ENGINE *);
int ENGINE_finish(ENGINE *);
void ENGINE_load_openssl(void);
void ENGINE_load_dynamic(void);
void ENGINE_load_cryptodev(void);
void ENGINE_load_builtin_engines(void);
void ENGINE_cleanup(void);
ENGINE *ENGINE_get_default_RSA(void);
ENGINE *ENGINE_get_default_DSA(void);
ENGINE *ENGINE_get_default_ECDH(void);
ENGINE *ENGINE_get_default_ECDSA(void);
ENGINE *ENGINE_get_default_DH(void);
ENGINE *ENGINE_get_default_RAND(void);
ENGINE *ENGINE_get_cipher_engine(int);
ENGINE *ENGINE_get_digest_engine(int);
int ENGINE_set_default_RSA(ENGINE *);
int ENGINE_set_default_DSA(ENGINE *);
int ENGINE_set_default_ECDH(ENGINE *);
int ENGINE_set_default_ECDSA(ENGINE *);
int ENGINE_set_default_DH(ENGINE *);
int ENGINE_set_default_RAND(ENGINE *);
int ENGINE_set_default_ciphers(ENGINE *);
int ENGINE_set_default_digests(ENGINE *);
int ENGINE_set_default_string(ENGINE *, const char *);
int ENGINE_set_default(ENGINE *, unsigned int);
unsigned int ENGINE_get_table_flags(void);
void ENGINE_set_table_flags(unsigned int);
int ENGINE_register_RSA(ENGINE *);
void ENGINE_unregister_RSA(ENGINE *);
void ENGINE_register_all_RSA(void);
int ENGINE_register_DSA(ENGINE *);
void ENGINE_unregister_DSA(ENGINE *);
void ENGINE_register_all_DSA(void);
int ENGINE_register_ECDH(ENGINE *);
void ENGINE_unregister_ECDH(ENGINE *);
void ENGINE_register_all_ECDH(void);
int ENGINE_register_ECDSA(ENGINE *);
void ENGINE_unregister_ECDSA(ENGINE *);
void ENGINE_register_all_ECDSA(void);
int ENGINE_register_DH(ENGINE *);
void ENGINE_unregister_DH(ENGINE *);
void ENGINE_register_all_DH(void);
int ENGINE_register_RAND(ENGINE *);
void ENGINE_unregister_RAND(ENGINE *);
void ENGINE_register_all_RAND(void);
int ENGINE_register_STORE(ENGINE *);
void ENGINE_unregister_STORE(ENGINE *);
void ENGINE_register_all_STORE(void);
int ENGINE_register_ciphers(ENGINE *);
void ENGINE_unregister_ciphers(ENGINE *);
void ENGINE_register_all_ciphers(void);
int ENGINE_register_digests(ENGINE *);
void ENGINE_unregister_digests(ENGINE *);
void ENGINE_register_all_digests(void);
int ENGINE_register_complete(ENGINE *);
int ENGINE_register_all_complete(void);
int ENGINE_ctrl(ENGINE *, int, long, void *, void (*)(void));
int ENGINE_cmd_is_executable(ENGINE *, int);
int ENGINE_ctrl_cmd(ENGINE *, const char *, long, void *, void (*)(void), int);
int ENGINE_ctrl_cmd_string(ENGINE *, const char *, const char *, int);

ENGINE *ENGINE_new(void);
int ENGINE_free(ENGINE *);
int ENGINE_up_ref(ENGINE *);
int ENGINE_set_id(ENGINE *, const char *);
int ENGINE_set_name(ENGINE *, const char *);
int ENGINE_set_RSA(ENGINE *, const RSA_METHOD *);
int ENGINE_set_DSA(ENGINE *, const DSA_METHOD *);
int ENGINE_set_ECDH(ENGINE *, const ECDH_METHOD *);
int ENGINE_set_ECDSA(ENGINE *, const ECDSA_METHOD *);
int ENGINE_set_DH(ENGINE *, const DH_METHOD *);
int ENGINE_set_RAND(ENGINE *, const RAND_METHOD *);
int ENGINE_set_STORE(ENGINE *, const STORE_METHOD *);
int ENGINE_set_destroy_function(ENGINE *, ENGINE_GEN_INT_FUNC_PTR);
int ENGINE_set_init_function(ENGINE *, ENGINE_GEN_INT_FUNC_PTR);
int ENGINE_set_finish_function(ENGINE *, ENGINE_GEN_INT_FUNC_PTR);
int ENGINE_set_ctrl_function(ENGINE *, ENGINE_CTRL_FUNC_PTR);
int ENGINE_set_load_privkey_function(ENGINE *, ENGINE_LOAD_KEY_PTR);
int ENGINE_set_load_pubkey_function(ENGINE *, ENGINE_LOAD_KEY_PTR);
int ENGINE_set_ciphers(ENGINE *, ENGINE_CIPHERS_PTR);
int ENGINE_set_digests(ENGINE *, ENGINE_DIGESTS_PTR);
int ENGINE_set_flags(ENGINE *, int);
int ENGINE_set_cmd_defns(ENGINE *, const ENGINE_CMD_DEFN *);
const char *ENGINE_get_id(const ENGINE *);
const char *ENGINE_get_name(const ENGINE *);
const RSA_METHOD *ENGINE_get_RSA(const ENGINE *);
const DSA_METHOD *ENGINE_get_DSA(const ENGINE *);
const ECDH_METHOD *ENGINE_get_ECDH(const ENGINE *);
const ECDSA_METHOD *ENGINE_get_ECDSA(const ENGINE *);
const DH_METHOD *ENGINE_get_DH(const ENGINE *);
const RAND_METHOD *ENGINE_get_RAND(const ENGINE *);
const STORE_METHOD *ENGINE_get_STORE(const ENGINE *);

const EVP_CIPHER *ENGINE_get_cipher(ENGINE *, int);
const EVP_MD *ENGINE_get_digest(ENGINE *, int);
int ENGINE_get_flags(const ENGINE *);
const ENGINE_CMD_DEFN *ENGINE_get_cmd_defns(const ENGINE *);
EVP_PKEY *ENGINE_load_private_key(ENGINE *, const char *, UI_METHOD *, void *);
EVP_PKEY *ENGINE_load_public_key(ENGINE *, const char *, UI_METHOD *, void *);
void ENGINE_add_conf_module(void);


void ERR_load_crypto_strings(void);
void ERR_load_SSL_strings(void);
void ERR_free_strings(void);
char* ERR_error_string(unsigned long, char *);
void ERR_error_string_n(unsigned long, char *, size_t);
const char* ERR_lib_error_string(unsigned long);
const char* ERR_func_error_string(unsigned long);
const char* ERR_reason_error_string(unsigned long);
void ERR_print_errors(BIO *);
void ERR_print_errors_fp(FILE *);
unsigned long ERR_get_error(void);
unsigned long ERR_peek_error(void);
unsigned long ERR_peek_last_error(void);
unsigned long ERR_get_error_line(const char **, int *);
unsigned long ERR_peek_error_line(const char **, int *);
unsigned long ERR_peek_last_error_line(const char **, int *);
unsigned long ERR_get_error_line_data(const char **, int *,
                                      const char **, int *);
unsigned long ERR_peek_error_line_data(const char **,
                                       int *, const char **, int *);
unsigned long ERR_peek_last_error_line_data(const char **,
                                            int *, const char **, int *);
void ERR_put_error(int, int, int, const char *, int);
void ERR_add_error_data(int, ...);
int ERR_get_next_error_library(void);


const EVP_CIPHER *EVP_get_cipherbyname(const char *);
int EVP_EncryptInit_ex(EVP_CIPHER_CTX *, const EVP_CIPHER *, ENGINE *,
                       const unsigned char *, const unsigned char *);
int EVP_CIPHER_CTX_set_padding(EVP_CIPHER_CTX *, int);
int EVP_EncryptUpdate(EVP_CIPHER_CTX *, unsigned char *, int *,
                      const unsigned char *, int);
int EVP_EncryptFinal_ex(EVP_CIPHER_CTX *, unsigned char *, int *);
int EVP_DecryptInit_ex(EVP_CIPHER_CTX *, const EVP_CIPHER *, ENGINE *,
                       const unsigned char *, const unsigned char *);
int EVP_DecryptUpdate(EVP_CIPHER_CTX *, unsigned char *, int *,
                      const unsigned char *, int);
int EVP_DecryptFinal_ex(EVP_CIPHER_CTX *, unsigned char *, int *);
int EVP_CipherInit_ex(EVP_CIPHER_CTX *, const EVP_CIPHER *, ENGINE *,
                      const unsigned char *, const unsigned char *, int);
int EVP_CipherUpdate(EVP_CIPHER_CTX *, unsigned char *, int *,
                     const unsigned char *, int);
int EVP_CipherFinal_ex(EVP_CIPHER_CTX *, unsigned char *, int *);
int EVP_CIPHER_CTX_cleanup(EVP_CIPHER_CTX *);
const EVP_CIPHER *EVP_CIPHER_CTX_cipher(const EVP_CIPHER_CTX *);
int EVP_CIPHER_block_size(const EVP_CIPHER *);
void EVP_CIPHER_CTX_init(EVP_CIPHER_CTX *);
EVP_CIPHER_CTX *EVP_CIPHER_CTX_new(void);
void EVP_CIPHER_CTX_free(EVP_CIPHER_CTX *);
int EVP_CIPHER_CTX_set_key_length(EVP_CIPHER_CTX *, int);

EVP_MD_CTX *EVP_MD_CTX_create(void);
int EVP_MD_CTX_copy_ex(EVP_MD_CTX *, const EVP_MD_CTX *);
int EVP_DigestInit_ex(EVP_MD_CTX *, const EVP_MD *, ENGINE *);
int EVP_DigestUpdate(EVP_MD_CTX *, const void *, size_t);
int EVP_DigestFinal_ex(EVP_MD_CTX *, unsigned char *, unsigned int *);
int EVP_MD_CTX_cleanup(EVP_MD_CTX *);
void EVP_MD_CTX_destroy(EVP_MD_CTX *);
const EVP_MD *EVP_get_digestbyname(const char *);
const EVP_MD *EVP_MD_CTX_md(const EVP_MD_CTX *);
int EVP_MD_size(const EVP_MD *);

EVP_PKEY *EVP_PKEY_new(void);
void EVP_PKEY_free(EVP_PKEY *);
int EVP_PKEY_type(int);
int EVP_PKEY_bits(EVP_PKEY *);
int EVP_PKEY_size(EVP_PKEY *);
RSA *EVP_PKEY_get1_RSA(EVP_PKEY *);

int EVP_SignInit(EVP_MD_CTX *, const EVP_MD *);
int EVP_SignUpdate(EVP_MD_CTX *, const void *, size_t);
int EVP_SignFinal(EVP_MD_CTX *, unsigned char *, unsigned int *, EVP_PKEY *);

int EVP_VerifyInit(EVP_MD_CTX *, const EVP_MD *);
int EVP_VerifyUpdate(EVP_MD_CTX *, const void *, size_t);
int EVP_VerifyFinal(EVP_MD_CTX *, const unsigned char *, unsigned int,
                    EVP_PKEY *);

const EVP_MD *EVP_md5(void);

int PKCS5_PBKDF2_HMAC_SHA1(const char *, int, const unsigned char *, int, int,
                           int, unsigned char *);

int EVP_PKEY_set1_RSA(EVP_PKEY *, struct rsa_st *);
int EVP_PKEY_set1_DSA(EVP_PKEY *, struct dsa_st *);


void HMAC_CTX_init(HMAC_CTX *);
void HMAC_CTX_cleanup(HMAC_CTX *);

int Cryptography_HMAC_Init_ex(HMAC_CTX *, const void *, int, const EVP_MD *,
                              ENGINE *);
int Cryptography_HMAC_Update(HMAC_CTX *, const unsigned char *, size_t);
int Cryptography_HMAC_Final(HMAC_CTX *, unsigned char *, unsigned int *);
int Cryptography_HMAC_CTX_copy(HMAC_CTX *, HMAC_CTX *);




ASN1_OBJECT *OBJ_nid2obj(int);
const char *OBJ_nid2ln(int);
const char *OBJ_nid2sn(int);
int OBJ_obj2nid(const ASN1_OBJECT *);
int OBJ_ln2nid(const char *);
int OBJ_sn2nid(const char *);
int OBJ_txt2nid(const char *);
ASN1_OBJECT *OBJ_txt2obj(const char *, int);
int OBJ_obj2txt(char *, int, const ASN1_OBJECT *, int);
int OBJ_cmp(const ASN1_OBJECT *, const ASN1_OBJECT *);
ASN1_OBJECT *OBJ_dup(const ASN1_OBJECT *);
int OBJ_create(const char *, const char *, const char *);
void OBJ_cleanup(void);




int Cryptography_add_osrandom_engine(void);


X509 *PEM_read_bio_X509(BIO *, X509 **, pem_password_cb *, void *);
int PEM_write_bio_X509(BIO *, X509 *);

int PEM_write_bio_PrivateKey(BIO *, EVP_PKEY *, const EVP_CIPHER *,
                             unsigned char *, int, pem_password_cb *, void *);

EVP_PKEY *PEM_read_bio_PrivateKey(BIO *, EVP_PKEY **, pem_password_cb *,
                                 void *);

int PEM_write_bio_PKCS8PrivateKey(BIO *, EVP_PKEY *, const EVP_CIPHER *,
                                  char *, int, pem_password_cb *, void *);

int i2d_PKCS8PrivateKey_bio(BIO *, EVP_PKEY *, const EVP_CIPHER *,
                            char *, int, pem_password_cb *, void *);

EVP_PKEY *d2i_PKCS8PrivateKey_bio(BIO *, EVP_PKEY **, pem_password_cb *,
                                  void *);

int PEM_write_bio_X509_REQ(BIO *, X509_REQ *);

X509_REQ *PEM_read_bio_X509_REQ(BIO *, X509_REQ **, pem_password_cb *, void *);

X509_CRL *PEM_read_bio_X509_CRL(BIO *, X509_CRL **, pem_password_cb *, void *);

int PEM_write_bio_X509_CRL(BIO *, X509_CRL *);

PKCS7 *PEM_read_bio_PKCS7(BIO *, PKCS7 **, pem_password_cb *, void *);
DH *PEM_read_bio_DHparams(BIO *, DH **, pem_password_cb *, void *);

DSA *PEM_read_bio_DSAPrivateKey(BIO *, DSA **, pem_password_cb *, void *);

RSA *PEM_read_bio_RSAPrivateKey(BIO *, RSA **, pem_password_cb *, void *);

int PEM_write_bio_DSAPrivateKey(BIO *, DSA *, const EVP_CIPHER *,
                                unsigned char *, int,
                                pem_password_cb *, void *);

int PEM_write_bio_RSAPrivateKey(BIO *, RSA *, const EVP_CIPHER *,
                                unsigned char *, int,
                                pem_password_cb *, void *);

DSA *PEM_read_bio_DSA_PUBKEY(BIO *, DSA **, pem_password_cb *, void *);

RSA *PEM_read_bio_RSAPublicKey(BIO *, RSA **, pem_password_cb *, void *);

int PEM_write_bio_DSA_PUBKEY(BIO *, DSA *);

int PEM_write_bio_RSAPublicKey(BIO *, const RSA *);

EVP_PKEY *PEM_read_bio_PUBKEY(BIO *, EVP_PKEY **, pem_password_cb *, void *);
int PEM_write_bio_PUBKEY(BIO *, EVP_PKEY *);


void PKCS7_free(PKCS7 *);


void PKCS12_free(PKCS12 *);

PKCS12 *d2i_PKCS12_bio(BIO *, PKCS12 **);
int i2d_PKCS12_bio(BIO *, PKCS12 *);


void ERR_load_RAND_strings(void);
void RAND_seed(const void *, int);
void RAND_add(const void *, int, double);
int RAND_status(void);
int RAND_egd(const char *);
int RAND_egd_bytes(const char *, int);
int RAND_query_egd_bytes(const char *, unsigned char *, int);
const char *RAND_file_name(char *, size_t);
int RAND_load_file(const char *, long);
int RAND_write_file(const char *);
void RAND_cleanup(void);
int RAND_bytes(unsigned char *, int);
int RAND_pseudo_bytes(unsigned char *, int);


RSA *RSA_new(void);
void RSA_free(RSA *);
int RSA_size(const RSA *);
int RSA_generate_key_ex(RSA *, int, BIGNUM *, BN_GENCB *);
int RSA_check_key(const RSA *);
RSA *RSAPublicKey_dup(RSA *);
int RSA_blinding_on(RSA *, BN_CTX *);
void RSA_blinding_off(RSA *);
int RSA_public_encrypt(int, const unsigned char *, unsigned char *,
                       RSA *, int);
int RSA_private_encrypt(int, const unsigned char *, unsigned char *,
                        RSA *, int);
int RSA_public_decrypt(int, const unsigned char *, unsigned char *,
                       RSA *, int);
int RSA_private_decrypt(int, const unsigned char *, unsigned char *,
                        RSA *, int);
int RSA_print(BIO *, const RSA *, int);
int RSA_verify_PKCS1_PSS(RSA *, const unsigned char *, const EVP_MD *,
                         const unsigned char *, int);
int RSA_padding_add_PKCS1_PSS(RSA *, unsigned char *, const unsigned char *,
                              const EVP_MD *, int);
int RSA_padding_add_PKCS1_OAEP(unsigned char *, int, const unsigned char *,
                               int, const unsigned char *, int);
int RSA_padding_check_PKCS1_OAEP(unsigned char *, int, const unsigned char *,
                                 int, int, const unsigned char *, int);


void SSL_load_error_strings(void);
int SSL_library_init(void);

/*  SSL */
SSL_CTX *SSL_set_SSL_CTX(SSL *, SSL_CTX *);
SSL_SESSION *SSL_get1_session(SSL *);
int SSL_set_session(SSL *, SSL_SESSION *);
int SSL_get_verify_mode(const SSL *);
void SSL_set_verify_depth(SSL *, int);
int SSL_get_verify_depth(const SSL *);
int (*SSL_get_verify_callback(const SSL *))(int, X509_STORE_CTX *);
void SSL_set_info_callback(SSL *ssl, void (*)(const SSL *, int, int));
void (*SSL_get_info_callback(const SSL *))(const SSL *, int, int);
SSL *SSL_new(SSL_CTX *);
void SSL_free(SSL *);
int SSL_set_fd(SSL *, int);
void SSL_set_bio(SSL *, BIO *, BIO *);
void SSL_set_connect_state(SSL *);
void SSL_set_accept_state(SSL *);
void SSL_set_shutdown(SSL *, int);
int SSL_get_shutdown(const SSL *);
int SSL_pending(const SSL *);
int SSL_write(SSL *, const void *, int);
int SSL_read(SSL *, void *, int);
X509 *SSL_get_peer_certificate(const SSL *);

Cryptography_STACK_OF_X509 *SSL_get_peer_cert_chain(const SSL *);
Cryptography_STACK_OF_X509_NAME *SSL_get_client_CA_list(const SSL *);

int SSL_get_error(const SSL *, int);
int SSL_do_handshake(SSL *);
int SSL_shutdown(SSL *);
const char *SSL_get_cipher_list(const SSL *, int);

/*  context */
void SSL_CTX_free(SSL_CTX *);
long SSL_CTX_set_timeout(SSL_CTX *, long);
int SSL_CTX_set_default_verify_paths(SSL_CTX *);
void SSL_CTX_set_verify(SSL_CTX *, int, int (*)(int, X509_STORE_CTX *));
void SSL_CTX_set_verify_depth(SSL_CTX *, int);
int (*SSL_CTX_get_verify_callback(const SSL_CTX *))(int, X509_STORE_CTX *);
void SSL_CTX_set_info_callback(SSL_CTX *, void (*)(const SSL *, int, int));
void (*SSL_CTX_get_info_callback(SSL_CTX *))(const SSL *, int, int);
int SSL_CTX_get_verify_mode(const SSL_CTX *);
int SSL_CTX_get_verify_depth(const SSL_CTX *);
int SSL_CTX_set_cipher_list(SSL_CTX *, const char *);
int SSL_CTX_load_verify_locations(SSL_CTX *, const char *, const char *);
void SSL_CTX_set_default_passwd_cb(SSL_CTX *, pem_password_cb *);
void SSL_CTX_set_default_passwd_cb_userdata(SSL_CTX *, void *);
int SSL_CTX_use_certificate(SSL_CTX *, X509 *);
int SSL_CTX_use_certificate_file(SSL_CTX *, const char *, int);
int SSL_CTX_use_certificate_chain_file(SSL_CTX *, const char *);
int SSL_CTX_use_PrivateKey(SSL_CTX *, EVP_PKEY *);
int SSL_CTX_use_PrivateKey_file(SSL_CTX *, const char *, int);
void SSL_CTX_set_cert_store(SSL_CTX *, X509_STORE *);
X509_STORE *SSL_CTX_get_cert_store(const SSL_CTX *);
int SSL_CTX_add_client_CA(SSL_CTX *, X509 *);

void SSL_CTX_set_client_CA_list(SSL_CTX *, Cryptography_STACK_OF_X509_NAME *);


/*  X509_STORE_CTX */
int X509_STORE_CTX_get_error(X509_STORE_CTX *);
void X509_STORE_CTX_set_error(X509_STORE_CTX *, int);
int X509_STORE_CTX_get_error_depth(X509_STORE_CTX *);
X509 *X509_STORE_CTX_get_current_cert(X509_STORE_CTX *);

/*  SSL_SESSION */
void SSL_SESSION_free(SSL_SESSION *);

/* Information about actually used cipher */
const char *SSL_CIPHER_get_name(const SSL_CIPHER *);
int SSL_CIPHER_get_bits(const SSL_CIPHER *, int *);
char *SSL_CIPHER_get_version(const SSL_CIPHER *);


X509 *X509_new(void);
void X509_free(X509 *);
X509 *X509_dup(X509 *);

int X509_print_ex(BIO *, X509 *, unsigned long, unsigned long);

int X509_set_version(X509 *, long);

EVP_PKEY *X509_get_pubkey(X509 *);
int X509_set_pubkey(X509 *, EVP_PKEY *);

unsigned char *X509_alias_get0(X509 *, int *);
int X509_sign(X509 *, EVP_PKEY *, const EVP_MD *);

int X509_digest(const X509 *, const EVP_MD *, unsigned char *, unsigned int *);

ASN1_TIME *X509_gmtime_adj(ASN1_TIME *, long);

unsigned long X509_subject_name_hash(X509 *);

X509_NAME *X509_get_subject_name(X509 *);
int X509_set_subject_name(X509 *, X509_NAME *);

X509_NAME *X509_get_issuer_name(X509 *);
int X509_set_issuer_name(X509 *, X509_NAME *);

int X509_get_ext_count(X509 *);
int X509_add_ext(X509 *, X509_EXTENSION *, int);
X509_EXTENSION *X509_EXTENSION_dup(X509_EXTENSION *);
X509_EXTENSION *X509_get_ext(X509 *, int);
int X509_EXTENSION_get_critical(X509_EXTENSION *);
ASN1_OBJECT *X509_EXTENSION_get_object(X509_EXTENSION *);
void X509_EXTENSION_free(X509_EXTENSION *);

int X509_REQ_set_version(X509_REQ *, long);
X509_REQ *X509_REQ_new(void);
void X509_REQ_free(X509_REQ *);
int X509_REQ_set_pubkey(X509_REQ *, EVP_PKEY *);
int X509_REQ_sign(X509_REQ *, EVP_PKEY *, const EVP_MD *);
int X509_REQ_verify(X509_REQ *, EVP_PKEY *);
EVP_PKEY *X509_REQ_get_pubkey(X509_REQ *);
int X509_REQ_add_extensions(X509_REQ *, X509_EXTENSIONS *);
X509_EXTENSIONS *X509_REQ_get_extensions(X509_REQ *);
int X509_REQ_print_ex(BIO *, X509_REQ *, unsigned long, unsigned long);

int X509V3_EXT_print(BIO *, X509_EXTENSION *, unsigned long, int);
ASN1_OCTET_STRING *X509_EXTENSION_get_data(X509_EXTENSION *);

X509_REVOKED *X509_REVOKED_new(void);
void X509_REVOKED_free(X509_REVOKED *);

int X509_REVOKED_set_serialNumber(X509_REVOKED *, ASN1_INTEGER *);

int X509_REVOKED_add1_ext_i2d(X509_REVOKED *, int, void *, int, unsigned long);

X509_CRL *d2i_X509_CRL_bio(BIO *, X509_CRL **);
X509_CRL *X509_CRL_new(void);
void X509_CRL_free(X509_CRL *);
int X509_CRL_add0_revoked(X509_CRL *, X509_REVOKED *);
int i2d_X509_CRL_bio(BIO *, X509_CRL *);
int X509_CRL_print(BIO *, X509_CRL *);
int X509_CRL_set_issuer_name(X509_CRL *, X509_NAME *);
int X509_CRL_sign(X509_CRL *, EVP_PKEY *, const EVP_MD *);

int NETSCAPE_SPKI_verify(NETSCAPE_SPKI *, EVP_PKEY *);
int NETSCAPE_SPKI_sign(NETSCAPE_SPKI *, EVP_PKEY *, const EVP_MD *);
char *NETSCAPE_SPKI_b64_encode(NETSCAPE_SPKI *);
EVP_PKEY *NETSCAPE_SPKI_get_pubkey(NETSCAPE_SPKI *);
int NETSCAPE_SPKI_set_pubkey(NETSCAPE_SPKI *, EVP_PKEY *);
NETSCAPE_SPKI *NETSCAPE_SPKI_new(void);
void NETSCAPE_SPKI_free(NETSCAPE_SPKI *);

/*  ASN1 serialization */
int i2d_X509_bio(BIO *, X509 *);
X509 *d2i_X509_bio(BIO *, X509 **);

int i2d_X509_REQ_bio(BIO *, X509_REQ *);
X509_REQ *d2i_X509_REQ_bio(BIO *, X509_REQ **);

int i2d_PrivateKey_bio(BIO *, EVP_PKEY *);
EVP_PKEY *d2i_PrivateKey_bio(BIO *, EVP_PKEY **);

ASN1_INTEGER *X509_get_serialNumber(X509 *);
int X509_set_serialNumber(X509 *, ASN1_INTEGER *);

/*  X509_STORE */
X509_STORE *X509_STORE_new(void);
void X509_STORE_free(X509_STORE *);
int X509_STORE_add_cert(X509_STORE *, X509 *);
int X509_verify_cert(X509_STORE_CTX *);

const char *X509_verify_cert_error_string(long);

const char *X509_get_default_cert_area(void);
const char *X509_get_default_cert_dir(void);
const char *X509_get_default_cert_file(void);
const char *X509_get_default_cert_dir_env(void);
const char *X509_get_default_cert_file_env(void);
const char *X509_get_default_private_dir(void);


int X509_NAME_entry_count(X509_NAME *);
X509_NAME_ENTRY *X509_NAME_get_entry(X509_NAME *, int);
ASN1_OBJECT *X509_NAME_ENTRY_get_object(X509_NAME_ENTRY *);
ASN1_STRING *X509_NAME_ENTRY_get_data(X509_NAME_ENTRY *);
unsigned long X509_NAME_hash(X509_NAME *);

int i2d_X509_NAME(X509_NAME *, unsigned char **);
int X509_NAME_add_entry_by_NID(X509_NAME *, int, int, unsigned char *,
                               int, int, int);
X509_NAME_ENTRY *X509_NAME_delete_entry(X509_NAME *, int);
void X509_NAME_ENTRY_free(X509_NAME_ENTRY *);
int X509_NAME_get_index_by_NID(X509_NAME *, int, int);
int X509_NAME_cmp(const X509_NAME *, const X509_NAME *);
char *X509_NAME_oneline(X509_NAME *, char *, int);
X509_NAME *X509_NAME_dup(X509_NAME *);
void X509_NAME_free(X509_NAME *);


void X509V3_set_ctx(X509V3_CTX *, X509 *, X509 *, X509_REQ *, X509_CRL *, int);
X509_EXTENSION *X509V3_EXT_nconf(CONF *, X509V3_CTX *, char *, char *);
int GENERAL_NAME_print(BIO *, GENERAL_NAME *);


















#ifdef OPENSSL_NO_EC
static const long Cryptography_HAS_EC = 0;
typedef void EC_KEY;
EC_KEY* (*EC_KEY_new_by_curve_name)(int) = NULL;
void (*EC_KEY_free)(EC_KEY *) = NULL;
#else
static const long Cryptography_HAS_EC = 1;
#endif




#if OPENSSL_VERSION_NUMBER >= 0x10000000L
static const long Cryptography_HAS_REMOVE_THREAD_STATE = 1;
#else
static const long Cryptography_HAS_REMOVE_THREAD_STATE = 0;
typedef uint32_t CRYPTO_THREADID;
void (*ERR_remove_thread_state)(const CRYPTO_THREADID *);
#endif


#ifdef EVP_CTRL_GCM_SET_TAG
const long Cryptography_HAS_GCM = 1;
#else
const long Cryptography_HAS_GCM = 0;
const long EVP_CTRL_GCM_GET_TAG = -1;
const long EVP_CTRL_GCM_SET_TAG = -1;
const long EVP_CTRL_GCM_SET_IVLEN = -1;
#endif
#if OPENSSL_VERSION_NUMBER >= 0x10000000
const long Cryptography_HAS_PBKDF2_HMAC = 1;
const long Cryptography_HAS_PKEY_CTX = 1;
#else
const long Cryptography_HAS_PBKDF2_HMAC = 0;
int (*PKCS5_PBKDF2_HMAC)(const char *, int, const unsigned char *, int, int,
                         const EVP_MD *, int, unsigned char *) = NULL;
const long Cryptography_HAS_PKEY_CTX = 0;
typedef void EVP_PKEY_CTX;
int (*EVP_PKEY_CTX_set_signature_md)(EVP_PKEY_CTX *, const EVP_MD *) = NULL;
int (*EVP_PKEY_sign_init)(EVP_PKEY_CTX *) = NULL;
int (*EVP_PKEY_sign)(EVP_PKEY_CTX *, unsigned char *, size_t *,
                     const unsigned char *, size_t) = NULL;
int (*EVP_PKEY_verify_init)(EVP_PKEY_CTX *) = NULL;
int (*EVP_PKEY_verify)(EVP_PKEY_CTX *, const unsigned char *, size_t,
                       const unsigned char *, size_t) = NULL;
EVP_PKEY_CTX *(*EVP_PKEY_CTX_new)(EVP_PKEY *, ENGINE *) = NULL;
EVP_PKEY_CTX *(*EVP_PKEY_CTX_new_id)(int, ENGINE *) = NULL;
EVP_PKEY_CTX *(*EVP_PKEY_CTX_dup)(EVP_PKEY_CTX *) = NULL;
void (*EVP_PKEY_CTX_free)(EVP_PKEY_CTX *) = NULL;
#endif


int Cryptography_HMAC_Init_ex(HMAC_CTX *ctx, const void *key, int key_len,
                              const EVP_MD *md, ENGINE *impl) {
#if OPENSSL_VERSION_NUMBER >= 0x010000000
    return HMAC_Init_ex(ctx, key, key_len, md, impl);
#else
    HMAC_Init_ex(ctx, key, key_len, md, impl);
    return 1;
#endif
}

int Cryptography_HMAC_Update(HMAC_CTX *ctx, const unsigned char *data,
                             size_t data_len) {
#if OPENSSL_VERSION_NUMBER >= 0x010000000
    return HMAC_Update(ctx, data, data_len);
#else
    HMAC_Update(ctx, data, data_len);
    return 1;
#endif
}

int Cryptography_HMAC_Final(HMAC_CTX *ctx, unsigned char *digest,
    unsigned int *digest_len) {
#if OPENSSL_VERSION_NUMBER >= 0x010000000
    return HMAC_Final(ctx, digest, digest_len);
#else
    HMAC_Final(ctx, digest, digest_len);
    return 1;
#endif
}

int Cryptography_HMAC_CTX_copy(HMAC_CTX *dst_ctx, HMAC_CTX *src_ctx) {
#if OPENSSL_VERSION_NUMBER >= 0x010000000
    return HMAC_CTX_copy(dst_ctx, src_ctx);
#else
    HMAC_CTX_init(dst_ctx);
    if (!EVP_MD_CTX_copy_ex(&dst_ctx->i_ctx, &src_ctx->i_ctx)) {
        goto err;
    }
    if (!EVP_MD_CTX_copy_ex(&dst_ctx->o_ctx, &src_ctx->o_ctx)) {
        goto err;
    }
    if (!EVP_MD_CTX_copy_ex(&dst_ctx->md_ctx, &src_ctx->md_ctx)) {
        goto err;
    }
    memcpy(dst_ctx->key, src_ctx->key, HMAC_MAX_MD_CBLOCK);
    dst_ctx->key_length = src_ctx->key_length;
    dst_ctx->md = src_ctx->md;
    return 1;

    err:
        return 0;
#endif
}








static const char *Cryptography_osrandom_engine_id = "osrandom";
static const char *Cryptography_osrandom_engine_name = "osrandom_engine";

#if defined(_WIN32)

static HCRYPTPROV hCryptProv = 0;

static int osrandom_init(ENGINE *e) {
    if (hCryptProv > 0) {
        return 1;
    }
    if (CryptAcquireContext(&hCryptProv, NULL, NULL,
                            PROV_RSA_FULL, CRYPT_VERIFYCONTEXT)) {
        return 1;
    } else {
        return 0;
    }
}

static int osrandom_rand_bytes(unsigned char *buffer, int size) {
    if (hCryptProv == 0) {
        return 0;
    }

    if (!CryptGenRandom(hCryptProv, (DWORD)size, buffer)) {
        ERR_put_error(
            ERR_LIB_RAND, 0, ERR_R_RAND_LIB, "osrandom_engine.py", 0
        );
        return 0;
    }
    return 1;
}

static int osrandom_finish(ENGINE *e) {
    if (CryptReleaseContext(hCryptProv, 0)) {
        hCryptProv = 0;
        return 1;
    } else {
        return 0;
    }
}

static int osrandom_rand_status(void) {
    if (hCryptProv == 0) {
        return 0;
    } else {
        return 1;
    }
}

#else

static int urandom_fd = -1;

static int osrandom_finish(ENGINE *e);

static int osrandom_init(ENGINE *e) {
    if (urandom_fd > -1) {
        return 1;
    }
    urandom_fd = open("/dev/urandom", O_RDONLY);
    if (urandom_fd > -1) {
        int flags = fcntl(urandom_fd, F_GETFD);
        if (flags == -1) {
            osrandom_finish(e);
            return 0;
        } else if (fcntl(urandom_fd, F_SETFD, flags | FD_CLOEXEC) == -1) {
            osrandom_finish(e);
            return 0;
        }
        return 1;
    } else {
        return 0;
    }
}

static int osrandom_rand_bytes(unsigned char *buffer, int size) {
    ssize_t n;
    while (size > 0) {
        do {
            n = read(urandom_fd, buffer, (size_t)size);
        } while (n < 0 && errno == EINTR);
        if (n <= 0) {
            ERR_put_error(
                ERR_LIB_RAND, 0, ERR_R_RAND_LIB, "osrandom_engine.py", 0
            );
            return 0;
        }
        buffer += n;
        size -= n;
    }
    return 1;
}

static int osrandom_finish(ENGINE *e) {
    int n;
    do {
        n = close(urandom_fd);
    } while (n < 0 && errno == EINTR);
    urandom_fd = -1;
    if (n < 0) {
        return 0;
    } else {
        return 1;
    }
}

static int osrandom_rand_status(void) {
    if (urandom_fd == -1) {
        return 0;
    } else {
        return 1;
    }
}

#endif

/* This replicates the behavior of the OpenSSL FIPS RNG, which returns a
   -1 in the event that there is an error when calling RAND_pseudo_bytes. */
static int osrandom_pseudo_rand_bytes(unsigned char *buffer, int size) {
    int res = osrandom_rand_bytes(buffer, size);
    if (res == 0) {
        return -1;
    } else {
        return res;
    }
}

static RAND_METHOD osrandom_rand = {
    NULL,
    osrandom_rand_bytes,
    NULL,
    NULL,
    osrandom_pseudo_rand_bytes,
    osrandom_rand_status,
};

/* Returns 1 if successfully added, 2 if engine has previously been added,
   and 0 for error. */
int Cryptography_add_osrandom_engine(void) {
    ENGINE *e;
    e = ENGINE_by_id(Cryptography_osrandom_engine_id);
    if (e != NULL) {
        ENGINE_free(e);
        return 2;
    } else {
        ERR_clear_error();
    }

    e = ENGINE_new();
    if (e == NULL) {
        return 0;
    }
    if(!ENGINE_set_id(e, Cryptography_osrandom_engine_id) ||
            !ENGINE_set_name(e, Cryptography_osrandom_engine_name) ||
            !ENGINE_set_RAND(e, &osrandom_rand) ||
            !ENGINE_set_init_function(e, osrandom_init) ||
            !ENGINE_set_finish_function(e, osrandom_finish)) {
        ENGINE_free(e);
        return 0;
    }
    if (!ENGINE_add(e)) {
        ENGINE_free(e);
        return 0;
    }
    if (!ENGINE_free(e)) {
        return 0;
    }

    return 1;
}










#if OPENSSL_VERSION_NUMBER >= 0x10000000
static const long Cryptography_HAS_PSS_PADDING = 1;
#else
// see evp.py for the definition of Cryptography_HAS_PKEY_CTX
static const long Cryptography_HAS_PSS_PADDING = 0;
int (*EVP_PKEY_CTX_set_rsa_padding)(EVP_PKEY_CTX *, int) = NULL;
int (*EVP_PKEY_CTX_set_rsa_pss_saltlen)(EVP_PKEY_CTX *, int) = NULL;
static const long RSA_PKCS1_PSS_PADDING = 0;
#endif


#ifdef OPENSSL_NO_SSL2
static const long Cryptography_HAS_SSL2 = 0;
SSL_METHOD* (*SSLv2_method)(void) = NULL;
SSL_METHOD* (*SSLv2_client_method)(void) = NULL;
SSL_METHOD* (*SSLv2_server_method)(void) = NULL;
#else
static const long Cryptography_HAS_SSL2 = 1;
#endif

#ifdef SSL_CTRL_SET_TLSEXT_HOSTNAME
static const long Cryptography_HAS_TLSEXT_HOSTNAME = 1;
#else
static const long Cryptography_HAS_TLSEXT_HOSTNAME = 0;
void (*SSL_set_tlsext_host_name)(SSL *, char *) = NULL;
const char* (*SSL_get_servername)(const SSL *, const int) = NULL;
void (*SSL_CTX_set_tlsext_servername_callback)(
    SSL_CTX *,
    int (*)(const SSL *, int *, void *)) = NULL;
#endif

#ifdef SSL_MODE_RELEASE_BUFFERS
static const long Cryptography_HAS_RELEASE_BUFFERS = 1;
#else
static const long Cryptography_HAS_RELEASE_BUFFERS = 0;
const long SSL_MODE_RELEASE_BUFFERS = 0;
#endif

#ifdef SSL_OP_NO_COMPRESSION
static const long Cryptography_HAS_OP_NO_COMPRESSION = 1;
#else
static const long Cryptography_HAS_OP_NO_COMPRESSION = 0;
const long SSL_OP_NO_COMPRESSION = 0;
#endif

#ifdef SSL_OP_NO_TLSv1_1
static const long Cryptography_HAS_TLSv1_1 = 1;
#else
static const long Cryptography_HAS_TLSv1_1 = 0;
static const long SSL_OP_NO_TLSv1_1 = 0;
SSL_METHOD* (*TLSv1_1_method)(void) = NULL;
SSL_METHOD* (*TLSv1_1_client_method)(void) = NULL;
SSL_METHOD* (*TLSv1_1_server_method)(void) = NULL;
#endif

#ifdef SSL_OP_NO_TLSv1_2
static const long Cryptography_HAS_TLSv1_2 = 1;
#else
static const long Cryptography_HAS_TLSv1_2 = 0;
static const long SSL_OP_NO_TLSv1_2 = 0;
SSL_METHOD* (*TLSv1_2_method)(void) = NULL;
SSL_METHOD* (*TLSv1_2_client_method)(void) = NULL;
SSL_METHOD* (*TLSv1_2_server_method)(void) = NULL;
#endif

#ifdef SSL_OP_MSIE_SSLV2_RSA_PADDING
static const long Cryptography_HAS_SSL_OP_MSIE_SSLV2_RSA_PADDING = 1;
#else
static const long Cryptography_HAS_SSL_OP_MSIE_SSLV2_RSA_PADDING = 0;
const long SSL_OP_MSIE_SSLV2_RSA_PADDING = 0;
#endif

#ifdef OPENSSL_NO_EC
long (*SSL_CTX_set_tmp_ecdh)(SSL_CTX *, EC_KEY *) = NULL;
#endif








static void _cffi_check__ASN1_TIME(ASN1_TIME *p)
{
  /* only to generate compile-time warnings or errors */
}
static PyObject *
_cffi_layout__ASN1_TIME(PyObject *self, PyObject *noarg)
{
  struct _cffi_aligncheck { char x; ASN1_TIME y; };
  static Py_ssize_t nums[] = {
    sizeof(ASN1_TIME),
    offsetof(struct _cffi_aligncheck, y),
    -1
  };
  return _cffi_get_struct_layout(nums);
  /* the next line is not executed, but compiled */
  _cffi_check__ASN1_TIME(0);
}

static void _cffi_check__EDIPARTYNAME(EDIPARTYNAME *p)
{
  /* only to generate compile-time warnings or errors */
}
static PyObject *
_cffi_layout__EDIPARTYNAME(PyObject *self, PyObject *noarg)
{
  struct _cffi_aligncheck { char x; EDIPARTYNAME y; };
  static Py_ssize_t nums[] = {
    sizeof(EDIPARTYNAME),
    offsetof(struct _cffi_aligncheck, y),
    -1
  };
  return _cffi_get_struct_layout(nums);
  /* the next line is not executed, but compiled */
  _cffi_check__EDIPARTYNAME(0);
}

static void _cffi_check__EVP_CIPHER_CTX(EVP_CIPHER_CTX *p)
{
  /* only to generate compile-time warnings or errors */
  { EVP_CIPHER const * *tmp = &p->cipher; (void)tmp; }
  { ENGINE * *tmp = &p->engine; (void)tmp; }
  (void)((p->encrypt) << 1);
}
static PyObject *
_cffi_layout__EVP_CIPHER_CTX(PyObject *self, PyObject *noarg)
{
  struct _cffi_aligncheck { char x; EVP_CIPHER_CTX y; };
  static Py_ssize_t nums[] = {
    sizeof(EVP_CIPHER_CTX),
    offsetof(struct _cffi_aligncheck, y),
    offsetof(EVP_CIPHER_CTX, cipher),
    sizeof(((EVP_CIPHER_CTX *)0)->cipher),
    offsetof(EVP_CIPHER_CTX, engine),
    sizeof(((EVP_CIPHER_CTX *)0)->engine),
    offsetof(EVP_CIPHER_CTX, encrypt),
    sizeof(((EVP_CIPHER_CTX *)0)->encrypt),
    -1
  };
  return _cffi_get_struct_layout(nums);
  /* the next line is not executed, but compiled */
  _cffi_check__EVP_CIPHER_CTX(0);
}

static void _cffi_check__GENERAL_NAME(GENERAL_NAME *p)
{
  /* only to generate compile-time warnings or errors */
  (void)((p->type) << 1);
  /* cannot generate 'union $1' in field 'd': unknown type name */
}
static PyObject *
_cffi_layout__GENERAL_NAME(PyObject *self, PyObject *noarg)
{
  struct _cffi_aligncheck { char x; GENERAL_NAME y; };
  static Py_ssize_t nums[] = {
    sizeof(GENERAL_NAME),
    offsetof(struct _cffi_aligncheck, y),
    offsetof(GENERAL_NAME, type),
    sizeof(((GENERAL_NAME *)0)->type),
    offsetof(GENERAL_NAME, d),
    sizeof(((GENERAL_NAME *)0)->d),
    -1
  };
  return _cffi_get_struct_layout(nums);
  /* the next line is not executed, but compiled */
  _cffi_check__GENERAL_NAME(0);
}

static void _cffi_check__HMAC_CTX(HMAC_CTX *p)
{
  /* only to generate compile-time warnings or errors */
}
static PyObject *
_cffi_layout__HMAC_CTX(PyObject *self, PyObject *noarg)
{
  struct _cffi_aligncheck { char x; HMAC_CTX y; };
  static Py_ssize_t nums[] = {
    sizeof(HMAC_CTX),
    offsetof(struct _cffi_aligncheck, y),
    -1
  };
  return _cffi_get_struct_layout(nums);
  /* the next line is not executed, but compiled */
  _cffi_check__HMAC_CTX(0);
}

static void _cffi_check__OTHERNAME(OTHERNAME *p)
{
  /* only to generate compile-time warnings or errors */
}
static PyObject *
_cffi_layout__OTHERNAME(PyObject *self, PyObject *noarg)
{
  struct _cffi_aligncheck { char x; OTHERNAME y; };
  static Py_ssize_t nums[] = {
    sizeof(OTHERNAME),
    offsetof(struct _cffi_aligncheck, y),
    -1
  };
  return _cffi_get_struct_layout(nums);
  /* the next line is not executed, but compiled */
  _cffi_check__OTHERNAME(0);
}

static void _cffi_check__PKCS7(PKCS7 *p)
{
  /* only to generate compile-time warnings or errors */
  { ASN1_OBJECT * *tmp = &p->type; (void)tmp; }
}
static PyObject *
_cffi_layout__PKCS7(PyObject *self, PyObject *noarg)
{
  struct _cffi_aligncheck { char x; PKCS7 y; };
  static Py_ssize_t nums[] = {
    sizeof(PKCS7),
    offsetof(struct _cffi_aligncheck, y),
    offsetof(PKCS7, type),
    sizeof(((PKCS7 *)0)->type),
    -1
  };
  return _cffi_get_struct_layout(nums);
  /* the next line is not executed, but compiled */
  _cffi_check__PKCS7(0);
}

static void _cffi_check__SSL(SSL *p)
{
  /* only to generate compile-time warnings or errors */
  { SSL3_STATE * *tmp = &p->s3; (void)tmp; }
  { SSL_SESSION * *tmp = &p->session; (void)tmp; }
}
static PyObject *
_cffi_layout__SSL(PyObject *self, PyObject *noarg)
{
  struct _cffi_aligncheck { char x; SSL y; };
  static Py_ssize_t nums[] = {
    sizeof(SSL),
    offsetof(struct _cffi_aligncheck, y),
    offsetof(SSL, s3),
    sizeof(((SSL *)0)->s3),
    offsetof(SSL, session),
    sizeof(((SSL *)0)->session),
    -1
  };
  return _cffi_get_struct_layout(nums);
  /* the next line is not executed, but compiled */
  _cffi_check__SSL(0);
}

static void _cffi_check__SSL3_STATE(SSL3_STATE *p)
{
  /* only to generate compile-time warnings or errors */
  { unsigned char(*tmp)[/*...*/] = &p->server_random; (void)tmp; }
  { unsigned char(*tmp)[/*...*/] = &p->client_random; (void)tmp; }
}
static PyObject *
_cffi_layout__SSL3_STATE(PyObject *self, PyObject *noarg)
{
  struct _cffi_aligncheck { char x; SSL3_STATE y; };
  static Py_ssize_t nums[] = {
    sizeof(SSL3_STATE),
    offsetof(struct _cffi_aligncheck, y),
    offsetof(SSL3_STATE, server_random),
    sizeof(((SSL3_STATE *)0)->server_random),
    offsetof(SSL3_STATE, client_random),
    sizeof(((SSL3_STATE *)0)->client_random),
    -1
  };
  return _cffi_get_struct_layout(nums);
  /* the next line is not executed, but compiled */
  _cffi_check__SSL3_STATE(0);
}

static void _cffi_check__SSL_SESSION(SSL_SESSION *p)
{
  /* only to generate compile-time warnings or errors */
  (void)((p->master_key_length) << 1);
  { unsigned char(*tmp)[/*...*/] = &p->master_key; (void)tmp; }
}
static PyObject *
_cffi_layout__SSL_SESSION(PyObject *self, PyObject *noarg)
{
  struct _cffi_aligncheck { char x; SSL_SESSION y; };
  static Py_ssize_t nums[] = {
    sizeof(SSL_SESSION),
    offsetof(struct _cffi_aligncheck, y),
    offsetof(SSL_SESSION, master_key_length),
    sizeof(((SSL_SESSION *)0)->master_key_length),
    offsetof(SSL_SESSION, master_key),
    sizeof(((SSL_SESSION *)0)->master_key),
    -1
  };
  return _cffi_get_struct_layout(nums);
  /* the next line is not executed, but compiled */
  _cffi_check__SSL_SESSION(0);
}

static void _cffi_check__X509(X509 *p)
{
  /* only to generate compile-time warnings or errors */
  { X509_CINF * *tmp = &p->cert_info; (void)tmp; }
}
static PyObject *
_cffi_layout__X509(PyObject *self, PyObject *noarg)
{
  struct _cffi_aligncheck { char x; X509 y; };
  static Py_ssize_t nums[] = {
    sizeof(X509),
    offsetof(struct _cffi_aligncheck, y),
    offsetof(X509, cert_info),
    sizeof(((X509 *)0)->cert_info),
    -1
  };
  return _cffi_get_struct_layout(nums);
  /* the next line is not executed, but compiled */
  _cffi_check__X509(0);
}

static void _cffi_check__X509V3_CTX(X509V3_CTX *p)
{
  /* only to generate compile-time warnings or errors */
  { X509 * *tmp = &p->issuer_cert; (void)tmp; }
  { X509 * *tmp = &p->subject_cert; (void)tmp; }
}
static PyObject *
_cffi_layout__X509V3_CTX(PyObject *self, PyObject *noarg)
{
  struct _cffi_aligncheck { char x; X509V3_CTX y; };
  static Py_ssize_t nums[] = {
    sizeof(X509V3_CTX),
    offsetof(struct _cffi_aligncheck, y),
    offsetof(X509V3_CTX, issuer_cert),
    sizeof(((X509V3_CTX *)0)->issuer_cert),
    offsetof(X509V3_CTX, subject_cert),
    sizeof(((X509V3_CTX *)0)->subject_cert),
    -1
  };
  return _cffi_get_struct_layout(nums);
  /* the next line is not executed, but compiled */
  _cffi_check__X509V3_CTX(0);
}

static void _cffi_check__X509V3_EXT_METHOD(X509V3_EXT_METHOD *p)
{
  /* only to generate compile-time warnings or errors */
  { ASN1_ITEM_EXP * *tmp = &p->it; (void)tmp; }
  { void *(* *tmp)(void *, unsigned char const * *, long) = &p->d2i; (void)tmp; }
}
static PyObject *
_cffi_layout__X509V3_EXT_METHOD(PyObject *self, PyObject *noarg)
{
  struct _cffi_aligncheck { char x; X509V3_EXT_METHOD y; };
  static Py_ssize_t nums[] = {
    sizeof(X509V3_EXT_METHOD),
    offsetof(struct _cffi_aligncheck, y),
    offsetof(X509V3_EXT_METHOD, it),
    sizeof(((X509V3_EXT_METHOD *)0)->it),
    offsetof(X509V3_EXT_METHOD, d2i),
    sizeof(((X509V3_EXT_METHOD *)0)->d2i),
    -1
  };
  return _cffi_get_struct_layout(nums);
  /* the next line is not executed, but compiled */
  _cffi_check__X509V3_EXT_METHOD(0);
}

static void _cffi_check__X509_ALGOR(X509_ALGOR *p)
{
  /* only to generate compile-time warnings or errors */
  { ASN1_OBJECT * *tmp = &p->algorithm; (void)tmp; }
}
static PyObject *
_cffi_layout__X509_ALGOR(PyObject *self, PyObject *noarg)
{
  struct _cffi_aligncheck { char x; X509_ALGOR y; };
  static Py_ssize_t nums[] = {
    sizeof(X509_ALGOR),
    offsetof(struct _cffi_aligncheck, y),
    offsetof(X509_ALGOR, algorithm),
    sizeof(((X509_ALGOR *)0)->algorithm),
    -1
  };
  return _cffi_get_struct_layout(nums);
  /* the next line is not executed, but compiled */
  _cffi_check__X509_ALGOR(0);
}

static void _cffi_check__X509_CINF(X509_CINF *p)
{
  /* only to generate compile-time warnings or errors */
  { X509_ALGOR * *tmp = &p->signature; (void)tmp; }
}
static PyObject *
_cffi_layout__X509_CINF(PyObject *self, PyObject *noarg)
{
  struct _cffi_aligncheck { char x; X509_CINF y; };
  static Py_ssize_t nums[] = {
    sizeof(X509_CINF),
    offsetof(struct _cffi_aligncheck, y),
    offsetof(X509_CINF, signature),
    sizeof(((X509_CINF *)0)->signature),
    -1
  };
  return _cffi_get_struct_layout(nums);
  /* the next line is not executed, but compiled */
  _cffi_check__X509_CINF(0);
}

static void _cffi_check__X509_CRL(X509_CRL *p)
{
  /* only to generate compile-time warnings or errors */
  { X509_CRL_INFO * *tmp = &p->crl; (void)tmp; }
}
static PyObject *
_cffi_layout__X509_CRL(PyObject *self, PyObject *noarg)
{
  struct _cffi_aligncheck { char x; X509_CRL y; };
  static Py_ssize_t nums[] = {
    sizeof(X509_CRL),
    offsetof(struct _cffi_aligncheck, y),
    offsetof(X509_CRL, crl),
    sizeof(((X509_CRL *)0)->crl),
    -1
  };
  return _cffi_get_struct_layout(nums);
  /* the next line is not executed, but compiled */
  _cffi_check__X509_CRL(0);
}

static void _cffi_check__X509_CRL_INFO(X509_CRL_INFO *p)
{
  /* only to generate compile-time warnings or errors */
  { Cryptography_STACK_OF_X509_REVOKED * *tmp = &p->revoked; (void)tmp; }
}
static PyObject *
_cffi_layout__X509_CRL_INFO(PyObject *self, PyObject *noarg)
{
  struct _cffi_aligncheck { char x; X509_CRL_INFO y; };
  static Py_ssize_t nums[] = {
    sizeof(X509_CRL_INFO),
    offsetof(struct _cffi_aligncheck, y),
    offsetof(X509_CRL_INFO, revoked),
    sizeof(((X509_CRL_INFO *)0)->revoked),
    -1
  };
  return _cffi_get_struct_layout(nums);
  /* the next line is not executed, but compiled */
  _cffi_check__X509_CRL_INFO(0);
}

static void _cffi_check__X509_EXTENSION(X509_EXTENSION *p)
{
  /* only to generate compile-time warnings or errors */
  { ASN1_OBJECT * *tmp = &p->object; (void)tmp; }
  (void)((p->critical) << 1);
  { ASN1_OCTET_STRING * *tmp = &p->value; (void)tmp; }
}
static PyObject *
_cffi_layout__X509_EXTENSION(PyObject *self, PyObject *noarg)
{
  struct _cffi_aligncheck { char x; X509_EXTENSION y; };
  static Py_ssize_t nums[] = {
    sizeof(X509_EXTENSION),
    offsetof(struct _cffi_aligncheck, y),
    offsetof(X509_EXTENSION, object),
    sizeof(((X509_EXTENSION *)0)->object),
    offsetof(X509_EXTENSION, critical),
    sizeof(((X509_EXTENSION *)0)->critical),
    offsetof(X509_EXTENSION, value),
    sizeof(((X509_EXTENSION *)0)->value),
    -1
  };
  return _cffi_get_struct_layout(nums);
  /* the next line is not executed, but compiled */
  _cffi_check__X509_EXTENSION(0);
}

static void _cffi_check__X509_REVOKED(X509_REVOKED *p)
{
  /* only to generate compile-time warnings or errors */
  { ASN1_INTEGER * *tmp = &p->serialNumber; (void)tmp; }
  { ASN1_TIME * *tmp = &p->revocationDate; (void)tmp; }
  { X509_EXTENSIONS * *tmp = &p->extensions; (void)tmp; }
  (void)((p->sequence) << 1);
}
static PyObject *
_cffi_layout__X509_REVOKED(PyObject *self, PyObject *noarg)
{
  struct _cffi_aligncheck { char x; X509_REVOKED y; };
  static Py_ssize_t nums[] = {
    sizeof(X509_REVOKED),
    offsetof(struct _cffi_aligncheck, y),
    offsetof(X509_REVOKED, serialNumber),
    sizeof(((X509_REVOKED *)0)->serialNumber),
    offsetof(X509_REVOKED, revocationDate),
    sizeof(((X509_REVOKED *)0)->revocationDate),
    offsetof(X509_REVOKED, extensions),
    sizeof(((X509_REVOKED *)0)->extensions),
    offsetof(X509_REVOKED, sequence),
    sizeof(((X509_REVOKED *)0)->sequence),
    -1
  };
  return _cffi_get_struct_layout(nums);
  /* the next line is not executed, but compiled */
  _cffi_check__X509_REVOKED(0);
}

static int _cffi_const_ASN1_F_ASN1_ENUMERATED_TO_BN(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (ASN1_F_ASN1_ENUMERATED_TO_BN) && (ASN1_F_ASN1_ENUMERATED_TO_BN) <= LONG_MAX)
    o = PyInt_FromLong((long)(ASN1_F_ASN1_ENUMERATED_TO_BN));
  else if ((ASN1_F_ASN1_ENUMERATED_TO_BN) <= 0)
    o = PyLong_FromLongLong((long long)(ASN1_F_ASN1_ENUMERATED_TO_BN));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(ASN1_F_ASN1_ENUMERATED_TO_BN));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "ASN1_F_ASN1_ENUMERATED_TO_BN", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return 0;
}

static int _cffi_const_ASN1_F_ASN1_EX_C2I(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (ASN1_F_ASN1_EX_C2I) && (ASN1_F_ASN1_EX_C2I) <= LONG_MAX)
    o = PyInt_FromLong((long)(ASN1_F_ASN1_EX_C2I));
  else if ((ASN1_F_ASN1_EX_C2I) <= 0)
    o = PyLong_FromLongLong((long long)(ASN1_F_ASN1_EX_C2I));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(ASN1_F_ASN1_EX_C2I));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "ASN1_F_ASN1_EX_C2I", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_ASN1_F_ASN1_ENUMERATED_TO_BN(lib);
}

static int _cffi_const_ASN1_F_ASN1_FIND_END(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (ASN1_F_ASN1_FIND_END) && (ASN1_F_ASN1_FIND_END) <= LONG_MAX)
    o = PyInt_FromLong((long)(ASN1_F_ASN1_FIND_END));
  else if ((ASN1_F_ASN1_FIND_END) <= 0)
    o = PyLong_FromLongLong((long long)(ASN1_F_ASN1_FIND_END));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(ASN1_F_ASN1_FIND_END));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "ASN1_F_ASN1_FIND_END", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_ASN1_F_ASN1_EX_C2I(lib);
}

static int _cffi_const_ASN1_F_ASN1_GENERALIZEDTIME_SET(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (ASN1_F_ASN1_GENERALIZEDTIME_SET) && (ASN1_F_ASN1_GENERALIZEDTIME_SET) <= LONG_MAX)
    o = PyInt_FromLong((long)(ASN1_F_ASN1_GENERALIZEDTIME_SET));
  else if ((ASN1_F_ASN1_GENERALIZEDTIME_SET) <= 0)
    o = PyLong_FromLongLong((long long)(ASN1_F_ASN1_GENERALIZEDTIME_SET));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(ASN1_F_ASN1_GENERALIZEDTIME_SET));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "ASN1_F_ASN1_GENERALIZEDTIME_SET", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_ASN1_F_ASN1_FIND_END(lib);
}

static int _cffi_const_ASN1_F_ASN1_GENERATE_V3(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (ASN1_F_ASN1_GENERATE_V3) && (ASN1_F_ASN1_GENERATE_V3) <= LONG_MAX)
    o = PyInt_FromLong((long)(ASN1_F_ASN1_GENERATE_V3));
  else if ((ASN1_F_ASN1_GENERATE_V3) <= 0)
    o = PyLong_FromLongLong((long long)(ASN1_F_ASN1_GENERATE_V3));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(ASN1_F_ASN1_GENERATE_V3));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "ASN1_F_ASN1_GENERATE_V3", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_ASN1_F_ASN1_GENERALIZEDTIME_SET(lib);
}

static int _cffi_const_ASN1_F_ASN1_GET_OBJECT(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (ASN1_F_ASN1_GET_OBJECT) && (ASN1_F_ASN1_GET_OBJECT) <= LONG_MAX)
    o = PyInt_FromLong((long)(ASN1_F_ASN1_GET_OBJECT));
  else if ((ASN1_F_ASN1_GET_OBJECT) <= 0)
    o = PyLong_FromLongLong((long long)(ASN1_F_ASN1_GET_OBJECT));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(ASN1_F_ASN1_GET_OBJECT));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "ASN1_F_ASN1_GET_OBJECT", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_ASN1_F_ASN1_GENERATE_V3(lib);
}

static int _cffi_const_ASN1_F_ASN1_ITEM_I2D_FP(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (ASN1_F_ASN1_ITEM_I2D_FP) && (ASN1_F_ASN1_ITEM_I2D_FP) <= LONG_MAX)
    o = PyInt_FromLong((long)(ASN1_F_ASN1_ITEM_I2D_FP));
  else if ((ASN1_F_ASN1_ITEM_I2D_FP) <= 0)
    o = PyLong_FromLongLong((long long)(ASN1_F_ASN1_ITEM_I2D_FP));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(ASN1_F_ASN1_ITEM_I2D_FP));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "ASN1_F_ASN1_ITEM_I2D_FP", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_ASN1_F_ASN1_GET_OBJECT(lib);
}

static int _cffi_const_ASN1_F_ASN1_ITEM_PACK(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (ASN1_F_ASN1_ITEM_PACK) && (ASN1_F_ASN1_ITEM_PACK) <= LONG_MAX)
    o = PyInt_FromLong((long)(ASN1_F_ASN1_ITEM_PACK));
  else if ((ASN1_F_ASN1_ITEM_PACK) <= 0)
    o = PyLong_FromLongLong((long long)(ASN1_F_ASN1_ITEM_PACK));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(ASN1_F_ASN1_ITEM_PACK));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "ASN1_F_ASN1_ITEM_PACK", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_ASN1_F_ASN1_ITEM_I2D_FP(lib);
}

static int _cffi_const_ASN1_F_ASN1_ITEM_SIGN(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (ASN1_F_ASN1_ITEM_SIGN) && (ASN1_F_ASN1_ITEM_SIGN) <= LONG_MAX)
    o = PyInt_FromLong((long)(ASN1_F_ASN1_ITEM_SIGN));
  else if ((ASN1_F_ASN1_ITEM_SIGN) <= 0)
    o = PyLong_FromLongLong((long long)(ASN1_F_ASN1_ITEM_SIGN));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(ASN1_F_ASN1_ITEM_SIGN));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "ASN1_F_ASN1_ITEM_SIGN", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_ASN1_F_ASN1_ITEM_PACK(lib);
}

static int _cffi_const_ASN1_F_ASN1_ITEM_UNPACK(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (ASN1_F_ASN1_ITEM_UNPACK) && (ASN1_F_ASN1_ITEM_UNPACK) <= LONG_MAX)
    o = PyInt_FromLong((long)(ASN1_F_ASN1_ITEM_UNPACK));
  else if ((ASN1_F_ASN1_ITEM_UNPACK) <= 0)
    o = PyLong_FromLongLong((long long)(ASN1_F_ASN1_ITEM_UNPACK));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(ASN1_F_ASN1_ITEM_UNPACK));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "ASN1_F_ASN1_ITEM_UNPACK", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_ASN1_F_ASN1_ITEM_SIGN(lib);
}

static int _cffi_const_ASN1_F_ASN1_ITEM_VERIFY(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (ASN1_F_ASN1_ITEM_VERIFY) && (ASN1_F_ASN1_ITEM_VERIFY) <= LONG_MAX)
    o = PyInt_FromLong((long)(ASN1_F_ASN1_ITEM_VERIFY));
  else if ((ASN1_F_ASN1_ITEM_VERIFY) <= 0)
    o = PyLong_FromLongLong((long long)(ASN1_F_ASN1_ITEM_VERIFY));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(ASN1_F_ASN1_ITEM_VERIFY));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "ASN1_F_ASN1_ITEM_VERIFY", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_ASN1_F_ASN1_ITEM_UNPACK(lib);
}

static int _cffi_const_ASN1_F_ASN1_MBSTRING_NCOPY(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (ASN1_F_ASN1_MBSTRING_NCOPY) && (ASN1_F_ASN1_MBSTRING_NCOPY) <= LONG_MAX)
    o = PyInt_FromLong((long)(ASN1_F_ASN1_MBSTRING_NCOPY));
  else if ((ASN1_F_ASN1_MBSTRING_NCOPY) <= 0)
    o = PyLong_FromLongLong((long long)(ASN1_F_ASN1_MBSTRING_NCOPY));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(ASN1_F_ASN1_MBSTRING_NCOPY));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "ASN1_F_ASN1_MBSTRING_NCOPY", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_ASN1_F_ASN1_ITEM_VERIFY(lib);
}

static int _cffi_const_ASN1_F_ASN1_TEMPLATE_EX_D2I(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (ASN1_F_ASN1_TEMPLATE_EX_D2I) && (ASN1_F_ASN1_TEMPLATE_EX_D2I) <= LONG_MAX)
    o = PyInt_FromLong((long)(ASN1_F_ASN1_TEMPLATE_EX_D2I));
  else if ((ASN1_F_ASN1_TEMPLATE_EX_D2I) <= 0)
    o = PyLong_FromLongLong((long long)(ASN1_F_ASN1_TEMPLATE_EX_D2I));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(ASN1_F_ASN1_TEMPLATE_EX_D2I));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "ASN1_F_ASN1_TEMPLATE_EX_D2I", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_ASN1_F_ASN1_MBSTRING_NCOPY(lib);
}

static int _cffi_const_ASN1_F_ASN1_TEMPLATE_NEW(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (ASN1_F_ASN1_TEMPLATE_NEW) && (ASN1_F_ASN1_TEMPLATE_NEW) <= LONG_MAX)
    o = PyInt_FromLong((long)(ASN1_F_ASN1_TEMPLATE_NEW));
  else if ((ASN1_F_ASN1_TEMPLATE_NEW) <= 0)
    o = PyLong_FromLongLong((long long)(ASN1_F_ASN1_TEMPLATE_NEW));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(ASN1_F_ASN1_TEMPLATE_NEW));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "ASN1_F_ASN1_TEMPLATE_NEW", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_ASN1_F_ASN1_TEMPLATE_EX_D2I(lib);
}

static int _cffi_const_ASN1_F_ASN1_TEMPLATE_NOEXP_D2I(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (ASN1_F_ASN1_TEMPLATE_NOEXP_D2I) && (ASN1_F_ASN1_TEMPLATE_NOEXP_D2I) <= LONG_MAX)
    o = PyInt_FromLong((long)(ASN1_F_ASN1_TEMPLATE_NOEXP_D2I));
  else if ((ASN1_F_ASN1_TEMPLATE_NOEXP_D2I) <= 0)
    o = PyLong_FromLongLong((long long)(ASN1_F_ASN1_TEMPLATE_NOEXP_D2I));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(ASN1_F_ASN1_TEMPLATE_NOEXP_D2I));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "ASN1_F_ASN1_TEMPLATE_NOEXP_D2I", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_ASN1_F_ASN1_TEMPLATE_NEW(lib);
}

static int _cffi_const_ASN1_F_ASN1_TIME_SET(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (ASN1_F_ASN1_TIME_SET) && (ASN1_F_ASN1_TIME_SET) <= LONG_MAX)
    o = PyInt_FromLong((long)(ASN1_F_ASN1_TIME_SET));
  else if ((ASN1_F_ASN1_TIME_SET) <= 0)
    o = PyLong_FromLongLong((long long)(ASN1_F_ASN1_TIME_SET));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(ASN1_F_ASN1_TIME_SET));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "ASN1_F_ASN1_TIME_SET", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_ASN1_F_ASN1_TEMPLATE_NOEXP_D2I(lib);
}

static int _cffi_const_ASN1_F_ASN1_TYPE_GET_INT_OCTETSTRING(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (ASN1_F_ASN1_TYPE_GET_INT_OCTETSTRING) && (ASN1_F_ASN1_TYPE_GET_INT_OCTETSTRING) <= LONG_MAX)
    o = PyInt_FromLong((long)(ASN1_F_ASN1_TYPE_GET_INT_OCTETSTRING));
  else if ((ASN1_F_ASN1_TYPE_GET_INT_OCTETSTRING) <= 0)
    o = PyLong_FromLongLong((long long)(ASN1_F_ASN1_TYPE_GET_INT_OCTETSTRING));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(ASN1_F_ASN1_TYPE_GET_INT_OCTETSTRING));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "ASN1_F_ASN1_TYPE_GET_INT_OCTETSTRING", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_ASN1_F_ASN1_TIME_SET(lib);
}

static int _cffi_const_ASN1_F_ASN1_TYPE_GET_OCTETSTRING(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (ASN1_F_ASN1_TYPE_GET_OCTETSTRING) && (ASN1_F_ASN1_TYPE_GET_OCTETSTRING) <= LONG_MAX)
    o = PyInt_FromLong((long)(ASN1_F_ASN1_TYPE_GET_OCTETSTRING));
  else if ((ASN1_F_ASN1_TYPE_GET_OCTETSTRING) <= 0)
    o = PyLong_FromLongLong((long long)(ASN1_F_ASN1_TYPE_GET_OCTETSTRING));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(ASN1_F_ASN1_TYPE_GET_OCTETSTRING));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "ASN1_F_ASN1_TYPE_GET_OCTETSTRING", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_ASN1_F_ASN1_TYPE_GET_INT_OCTETSTRING(lib);
}

static int _cffi_const_ASN1_F_ASN1_UNPACK_STRING(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (ASN1_F_ASN1_UNPACK_STRING) && (ASN1_F_ASN1_UNPACK_STRING) <= LONG_MAX)
    o = PyInt_FromLong((long)(ASN1_F_ASN1_UNPACK_STRING));
  else if ((ASN1_F_ASN1_UNPACK_STRING) <= 0)
    o = PyLong_FromLongLong((long long)(ASN1_F_ASN1_UNPACK_STRING));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(ASN1_F_ASN1_UNPACK_STRING));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "ASN1_F_ASN1_UNPACK_STRING", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_ASN1_F_ASN1_TYPE_GET_OCTETSTRING(lib);
}

static int _cffi_const_ASN1_F_ASN1_UTCTIME_SET(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (ASN1_F_ASN1_UTCTIME_SET) && (ASN1_F_ASN1_UTCTIME_SET) <= LONG_MAX)
    o = PyInt_FromLong((long)(ASN1_F_ASN1_UTCTIME_SET));
  else if ((ASN1_F_ASN1_UTCTIME_SET) <= 0)
    o = PyLong_FromLongLong((long long)(ASN1_F_ASN1_UTCTIME_SET));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(ASN1_F_ASN1_UTCTIME_SET));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "ASN1_F_ASN1_UTCTIME_SET", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_ASN1_F_ASN1_UNPACK_STRING(lib);
}

static int _cffi_const_ASN1_F_ASN1_VERIFY(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (ASN1_F_ASN1_VERIFY) && (ASN1_F_ASN1_VERIFY) <= LONG_MAX)
    o = PyInt_FromLong((long)(ASN1_F_ASN1_VERIFY));
  else if ((ASN1_F_ASN1_VERIFY) <= 0)
    o = PyLong_FromLongLong((long long)(ASN1_F_ASN1_VERIFY));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(ASN1_F_ASN1_VERIFY));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "ASN1_F_ASN1_VERIFY", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_ASN1_F_ASN1_UTCTIME_SET(lib);
}

static int _cffi_const_ASN1_F_B64_READ_ASN1(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (ASN1_F_B64_READ_ASN1) && (ASN1_F_B64_READ_ASN1) <= LONG_MAX)
    o = PyInt_FromLong((long)(ASN1_F_B64_READ_ASN1));
  else if ((ASN1_F_B64_READ_ASN1) <= 0)
    o = PyLong_FromLongLong((long long)(ASN1_F_B64_READ_ASN1));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(ASN1_F_B64_READ_ASN1));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "ASN1_F_B64_READ_ASN1", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_ASN1_F_ASN1_VERIFY(lib);
}

static int _cffi_const_ASN1_F_B64_WRITE_ASN1(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (ASN1_F_B64_WRITE_ASN1) && (ASN1_F_B64_WRITE_ASN1) <= LONG_MAX)
    o = PyInt_FromLong((long)(ASN1_F_B64_WRITE_ASN1));
  else if ((ASN1_F_B64_WRITE_ASN1) <= 0)
    o = PyLong_FromLongLong((long long)(ASN1_F_B64_WRITE_ASN1));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(ASN1_F_B64_WRITE_ASN1));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "ASN1_F_B64_WRITE_ASN1", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_ASN1_F_B64_READ_ASN1(lib);
}

static int _cffi_const_ASN1_F_BITSTR_CB(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (ASN1_F_BITSTR_CB) && (ASN1_F_BITSTR_CB) <= LONG_MAX)
    o = PyInt_FromLong((long)(ASN1_F_BITSTR_CB));
  else if ((ASN1_F_BITSTR_CB) <= 0)
    o = PyLong_FromLongLong((long long)(ASN1_F_BITSTR_CB));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(ASN1_F_BITSTR_CB));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "ASN1_F_BITSTR_CB", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_ASN1_F_B64_WRITE_ASN1(lib);
}

static int _cffi_const_ASN1_F_BN_TO_ASN1_ENUMERATED(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (ASN1_F_BN_TO_ASN1_ENUMERATED) && (ASN1_F_BN_TO_ASN1_ENUMERATED) <= LONG_MAX)
    o = PyInt_FromLong((long)(ASN1_F_BN_TO_ASN1_ENUMERATED));
  else if ((ASN1_F_BN_TO_ASN1_ENUMERATED) <= 0)
    o = PyLong_FromLongLong((long long)(ASN1_F_BN_TO_ASN1_ENUMERATED));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(ASN1_F_BN_TO_ASN1_ENUMERATED));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "ASN1_F_BN_TO_ASN1_ENUMERATED", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_ASN1_F_BITSTR_CB(lib);
}

static int _cffi_const_ASN1_F_BN_TO_ASN1_INTEGER(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (ASN1_F_BN_TO_ASN1_INTEGER) && (ASN1_F_BN_TO_ASN1_INTEGER) <= LONG_MAX)
    o = PyInt_FromLong((long)(ASN1_F_BN_TO_ASN1_INTEGER));
  else if ((ASN1_F_BN_TO_ASN1_INTEGER) <= 0)
    o = PyLong_FromLongLong((long long)(ASN1_F_BN_TO_ASN1_INTEGER));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(ASN1_F_BN_TO_ASN1_INTEGER));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "ASN1_F_BN_TO_ASN1_INTEGER", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_ASN1_F_BN_TO_ASN1_ENUMERATED(lib);
}

static int _cffi_const_ASN1_F_D2I_ASN1_TYPE_BYTES(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (ASN1_F_D2I_ASN1_TYPE_BYTES) && (ASN1_F_D2I_ASN1_TYPE_BYTES) <= LONG_MAX)
    o = PyInt_FromLong((long)(ASN1_F_D2I_ASN1_TYPE_BYTES));
  else if ((ASN1_F_D2I_ASN1_TYPE_BYTES) <= 0)
    o = PyLong_FromLongLong((long long)(ASN1_F_D2I_ASN1_TYPE_BYTES));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(ASN1_F_D2I_ASN1_TYPE_BYTES));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "ASN1_F_D2I_ASN1_TYPE_BYTES", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_ASN1_F_BN_TO_ASN1_INTEGER(lib);
}

static int _cffi_const_ASN1_F_D2I_ASN1_UINTEGER(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (ASN1_F_D2I_ASN1_UINTEGER) && (ASN1_F_D2I_ASN1_UINTEGER) <= LONG_MAX)
    o = PyInt_FromLong((long)(ASN1_F_D2I_ASN1_UINTEGER));
  else if ((ASN1_F_D2I_ASN1_UINTEGER) <= 0)
    o = PyLong_FromLongLong((long long)(ASN1_F_D2I_ASN1_UINTEGER));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(ASN1_F_D2I_ASN1_UINTEGER));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "ASN1_F_D2I_ASN1_UINTEGER", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_ASN1_F_D2I_ASN1_TYPE_BYTES(lib);
}

static int _cffi_const_ASN1_F_D2I_ASN1_UTCTIME(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (ASN1_F_D2I_ASN1_UTCTIME) && (ASN1_F_D2I_ASN1_UTCTIME) <= LONG_MAX)
    o = PyInt_FromLong((long)(ASN1_F_D2I_ASN1_UTCTIME));
  else if ((ASN1_F_D2I_ASN1_UTCTIME) <= 0)
    o = PyLong_FromLongLong((long long)(ASN1_F_D2I_ASN1_UTCTIME));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(ASN1_F_D2I_ASN1_UTCTIME));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "ASN1_F_D2I_ASN1_UTCTIME", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_ASN1_F_D2I_ASN1_UINTEGER(lib);
}

static int _cffi_const_ASN1_F_D2I_NETSCAPE_RSA(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (ASN1_F_D2I_NETSCAPE_RSA) && (ASN1_F_D2I_NETSCAPE_RSA) <= LONG_MAX)
    o = PyInt_FromLong((long)(ASN1_F_D2I_NETSCAPE_RSA));
  else if ((ASN1_F_D2I_NETSCAPE_RSA) <= 0)
    o = PyLong_FromLongLong((long long)(ASN1_F_D2I_NETSCAPE_RSA));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(ASN1_F_D2I_NETSCAPE_RSA));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "ASN1_F_D2I_NETSCAPE_RSA", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_ASN1_F_D2I_ASN1_UTCTIME(lib);
}

static int _cffi_const_ASN1_F_D2I_NETSCAPE_RSA_2(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (ASN1_F_D2I_NETSCAPE_RSA_2) && (ASN1_F_D2I_NETSCAPE_RSA_2) <= LONG_MAX)
    o = PyInt_FromLong((long)(ASN1_F_D2I_NETSCAPE_RSA_2));
  else if ((ASN1_F_D2I_NETSCAPE_RSA_2) <= 0)
    o = PyLong_FromLongLong((long long)(ASN1_F_D2I_NETSCAPE_RSA_2));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(ASN1_F_D2I_NETSCAPE_RSA_2));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "ASN1_F_D2I_NETSCAPE_RSA_2", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_ASN1_F_D2I_NETSCAPE_RSA(lib);
}

static int _cffi_const_ASN1_F_D2I_PRIVATEKEY(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (ASN1_F_D2I_PRIVATEKEY) && (ASN1_F_D2I_PRIVATEKEY) <= LONG_MAX)
    o = PyInt_FromLong((long)(ASN1_F_D2I_PRIVATEKEY));
  else if ((ASN1_F_D2I_PRIVATEKEY) <= 0)
    o = PyLong_FromLongLong((long long)(ASN1_F_D2I_PRIVATEKEY));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(ASN1_F_D2I_PRIVATEKEY));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "ASN1_F_D2I_PRIVATEKEY", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_ASN1_F_D2I_NETSCAPE_RSA_2(lib);
}

static int _cffi_const_ASN1_F_D2I_X509(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (ASN1_F_D2I_X509) && (ASN1_F_D2I_X509) <= LONG_MAX)
    o = PyInt_FromLong((long)(ASN1_F_D2I_X509));
  else if ((ASN1_F_D2I_X509) <= 0)
    o = PyLong_FromLongLong((long long)(ASN1_F_D2I_X509));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(ASN1_F_D2I_X509));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "ASN1_F_D2I_X509", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_ASN1_F_D2I_PRIVATEKEY(lib);
}

static int _cffi_const_ASN1_F_D2I_X509_CINF(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (ASN1_F_D2I_X509_CINF) && (ASN1_F_D2I_X509_CINF) <= LONG_MAX)
    o = PyInt_FromLong((long)(ASN1_F_D2I_X509_CINF));
  else if ((ASN1_F_D2I_X509_CINF) <= 0)
    o = PyLong_FromLongLong((long long)(ASN1_F_D2I_X509_CINF));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(ASN1_F_D2I_X509_CINF));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "ASN1_F_D2I_X509_CINF", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_ASN1_F_D2I_X509(lib);
}

static int _cffi_const_ASN1_F_D2I_X509_PKEY(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (ASN1_F_D2I_X509_PKEY) && (ASN1_F_D2I_X509_PKEY) <= LONG_MAX)
    o = PyInt_FromLong((long)(ASN1_F_D2I_X509_PKEY));
  else if ((ASN1_F_D2I_X509_PKEY) <= 0)
    o = PyLong_FromLongLong((long long)(ASN1_F_D2I_X509_PKEY));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(ASN1_F_D2I_X509_PKEY));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "ASN1_F_D2I_X509_PKEY", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_ASN1_F_D2I_X509_CINF(lib);
}

static int _cffi_const_ASN1_F_I2D_ASN1_SET(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (ASN1_F_I2D_ASN1_SET) && (ASN1_F_I2D_ASN1_SET) <= LONG_MAX)
    o = PyInt_FromLong((long)(ASN1_F_I2D_ASN1_SET));
  else if ((ASN1_F_I2D_ASN1_SET) <= 0)
    o = PyLong_FromLongLong((long long)(ASN1_F_I2D_ASN1_SET));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(ASN1_F_I2D_ASN1_SET));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "ASN1_F_I2D_ASN1_SET", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_ASN1_F_D2I_X509_PKEY(lib);
}

static int _cffi_const_ASN1_F_I2D_ASN1_TIME(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (ASN1_F_I2D_ASN1_TIME) && (ASN1_F_I2D_ASN1_TIME) <= LONG_MAX)
    o = PyInt_FromLong((long)(ASN1_F_I2D_ASN1_TIME));
  else if ((ASN1_F_I2D_ASN1_TIME) <= 0)
    o = PyLong_FromLongLong((long long)(ASN1_F_I2D_ASN1_TIME));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(ASN1_F_I2D_ASN1_TIME));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "ASN1_F_I2D_ASN1_TIME", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_ASN1_F_I2D_ASN1_SET(lib);
}

static int _cffi_const_ASN1_F_I2D_DSA_PUBKEY(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (ASN1_F_I2D_DSA_PUBKEY) && (ASN1_F_I2D_DSA_PUBKEY) <= LONG_MAX)
    o = PyInt_FromLong((long)(ASN1_F_I2D_DSA_PUBKEY));
  else if ((ASN1_F_I2D_DSA_PUBKEY) <= 0)
    o = PyLong_FromLongLong((long long)(ASN1_F_I2D_DSA_PUBKEY));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(ASN1_F_I2D_DSA_PUBKEY));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "ASN1_F_I2D_DSA_PUBKEY", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_ASN1_F_I2D_ASN1_TIME(lib);
}

static int _cffi_const_ASN1_F_LONG_C2I(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (ASN1_F_LONG_C2I) && (ASN1_F_LONG_C2I) <= LONG_MAX)
    o = PyInt_FromLong((long)(ASN1_F_LONG_C2I));
  else if ((ASN1_F_LONG_C2I) <= 0)
    o = PyLong_FromLongLong((long long)(ASN1_F_LONG_C2I));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(ASN1_F_LONG_C2I));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "ASN1_F_LONG_C2I", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_ASN1_F_I2D_DSA_PUBKEY(lib);
}

static int _cffi_const_ASN1_F_OID_MODULE_INIT(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (ASN1_F_OID_MODULE_INIT) && (ASN1_F_OID_MODULE_INIT) <= LONG_MAX)
    o = PyInt_FromLong((long)(ASN1_F_OID_MODULE_INIT));
  else if ((ASN1_F_OID_MODULE_INIT) <= 0)
    o = PyLong_FromLongLong((long long)(ASN1_F_OID_MODULE_INIT));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(ASN1_F_OID_MODULE_INIT));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "ASN1_F_OID_MODULE_INIT", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_ASN1_F_LONG_C2I(lib);
}

static int _cffi_const_ASN1_F_PARSE_TAGGING(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (ASN1_F_PARSE_TAGGING) && (ASN1_F_PARSE_TAGGING) <= LONG_MAX)
    o = PyInt_FromLong((long)(ASN1_F_PARSE_TAGGING));
  else if ((ASN1_F_PARSE_TAGGING) <= 0)
    o = PyLong_FromLongLong((long long)(ASN1_F_PARSE_TAGGING));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(ASN1_F_PARSE_TAGGING));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "ASN1_F_PARSE_TAGGING", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_ASN1_F_OID_MODULE_INIT(lib);
}

static int _cffi_const_ASN1_F_PKCS5_PBE_SET(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (ASN1_F_PKCS5_PBE_SET) && (ASN1_F_PKCS5_PBE_SET) <= LONG_MAX)
    o = PyInt_FromLong((long)(ASN1_F_PKCS5_PBE_SET));
  else if ((ASN1_F_PKCS5_PBE_SET) <= 0)
    o = PyLong_FromLongLong((long long)(ASN1_F_PKCS5_PBE_SET));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(ASN1_F_PKCS5_PBE_SET));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "ASN1_F_PKCS5_PBE_SET", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_ASN1_F_PARSE_TAGGING(lib);
}

static int _cffi_const_ASN1_F_SMIME_READ_ASN1(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (ASN1_F_SMIME_READ_ASN1) && (ASN1_F_SMIME_READ_ASN1) <= LONG_MAX)
    o = PyInt_FromLong((long)(ASN1_F_SMIME_READ_ASN1));
  else if ((ASN1_F_SMIME_READ_ASN1) <= 0)
    o = PyLong_FromLongLong((long long)(ASN1_F_SMIME_READ_ASN1));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(ASN1_F_SMIME_READ_ASN1));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "ASN1_F_SMIME_READ_ASN1", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_ASN1_F_PKCS5_PBE_SET(lib);
}

static int _cffi_const_ASN1_F_SMIME_TEXT(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (ASN1_F_SMIME_TEXT) && (ASN1_F_SMIME_TEXT) <= LONG_MAX)
    o = PyInt_FromLong((long)(ASN1_F_SMIME_TEXT));
  else if ((ASN1_F_SMIME_TEXT) <= 0)
    o = PyLong_FromLongLong((long long)(ASN1_F_SMIME_TEXT));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(ASN1_F_SMIME_TEXT));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "ASN1_F_SMIME_TEXT", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_ASN1_F_SMIME_READ_ASN1(lib);
}

static int _cffi_const_ASN1_F_X509_CINF_NEW(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (ASN1_F_X509_CINF_NEW) && (ASN1_F_X509_CINF_NEW) <= LONG_MAX)
    o = PyInt_FromLong((long)(ASN1_F_X509_CINF_NEW));
  else if ((ASN1_F_X509_CINF_NEW) <= 0)
    o = PyLong_FromLongLong((long long)(ASN1_F_X509_CINF_NEW));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(ASN1_F_X509_CINF_NEW));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "ASN1_F_X509_CINF_NEW", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_ASN1_F_SMIME_TEXT(lib);
}

static int _cffi_const_ASN1_R_BOOLEAN_IS_WRONG_LENGTH(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (ASN1_R_BOOLEAN_IS_WRONG_LENGTH) && (ASN1_R_BOOLEAN_IS_WRONG_LENGTH) <= LONG_MAX)
    o = PyInt_FromLong((long)(ASN1_R_BOOLEAN_IS_WRONG_LENGTH));
  else if ((ASN1_R_BOOLEAN_IS_WRONG_LENGTH) <= 0)
    o = PyLong_FromLongLong((long long)(ASN1_R_BOOLEAN_IS_WRONG_LENGTH));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(ASN1_R_BOOLEAN_IS_WRONG_LENGTH));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "ASN1_R_BOOLEAN_IS_WRONG_LENGTH", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_ASN1_F_X509_CINF_NEW(lib);
}

static int _cffi_const_ASN1_R_BUFFER_TOO_SMALL(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (ASN1_R_BUFFER_TOO_SMALL) && (ASN1_R_BUFFER_TOO_SMALL) <= LONG_MAX)
    o = PyInt_FromLong((long)(ASN1_R_BUFFER_TOO_SMALL));
  else if ((ASN1_R_BUFFER_TOO_SMALL) <= 0)
    o = PyLong_FromLongLong((long long)(ASN1_R_BUFFER_TOO_SMALL));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(ASN1_R_BUFFER_TOO_SMALL));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "ASN1_R_BUFFER_TOO_SMALL", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_ASN1_R_BOOLEAN_IS_WRONG_LENGTH(lib);
}

static int _cffi_const_ASN1_R_CIPHER_HAS_NO_OBJECT_IDENTIFIER(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (ASN1_R_CIPHER_HAS_NO_OBJECT_IDENTIFIER) && (ASN1_R_CIPHER_HAS_NO_OBJECT_IDENTIFIER) <= LONG_MAX)
    o = PyInt_FromLong((long)(ASN1_R_CIPHER_HAS_NO_OBJECT_IDENTIFIER));
  else if ((ASN1_R_CIPHER_HAS_NO_OBJECT_IDENTIFIER) <= 0)
    o = PyLong_FromLongLong((long long)(ASN1_R_CIPHER_HAS_NO_OBJECT_IDENTIFIER));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(ASN1_R_CIPHER_HAS_NO_OBJECT_IDENTIFIER));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "ASN1_R_CIPHER_HAS_NO_OBJECT_IDENTIFIER", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_ASN1_R_BUFFER_TOO_SMALL(lib);
}

static int _cffi_const_ASN1_R_DATA_IS_WRONG(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (ASN1_R_DATA_IS_WRONG) && (ASN1_R_DATA_IS_WRONG) <= LONG_MAX)
    o = PyInt_FromLong((long)(ASN1_R_DATA_IS_WRONG));
  else if ((ASN1_R_DATA_IS_WRONG) <= 0)
    o = PyLong_FromLongLong((long long)(ASN1_R_DATA_IS_WRONG));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(ASN1_R_DATA_IS_WRONG));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "ASN1_R_DATA_IS_WRONG", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_ASN1_R_CIPHER_HAS_NO_OBJECT_IDENTIFIER(lib);
}

static int _cffi_const_ASN1_R_DECODE_ERROR(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (ASN1_R_DECODE_ERROR) && (ASN1_R_DECODE_ERROR) <= LONG_MAX)
    o = PyInt_FromLong((long)(ASN1_R_DECODE_ERROR));
  else if ((ASN1_R_DECODE_ERROR) <= 0)
    o = PyLong_FromLongLong((long long)(ASN1_R_DECODE_ERROR));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(ASN1_R_DECODE_ERROR));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "ASN1_R_DECODE_ERROR", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_ASN1_R_DATA_IS_WRONG(lib);
}

static int _cffi_const_ASN1_R_DECODING_ERROR(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (ASN1_R_DECODING_ERROR) && (ASN1_R_DECODING_ERROR) <= LONG_MAX)
    o = PyInt_FromLong((long)(ASN1_R_DECODING_ERROR));
  else if ((ASN1_R_DECODING_ERROR) <= 0)
    o = PyLong_FromLongLong((long long)(ASN1_R_DECODING_ERROR));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(ASN1_R_DECODING_ERROR));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "ASN1_R_DECODING_ERROR", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_ASN1_R_DECODE_ERROR(lib);
}

static int _cffi_const_ASN1_R_DEPTH_EXCEEDED(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (ASN1_R_DEPTH_EXCEEDED) && (ASN1_R_DEPTH_EXCEEDED) <= LONG_MAX)
    o = PyInt_FromLong((long)(ASN1_R_DEPTH_EXCEEDED));
  else if ((ASN1_R_DEPTH_EXCEEDED) <= 0)
    o = PyLong_FromLongLong((long long)(ASN1_R_DEPTH_EXCEEDED));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(ASN1_R_DEPTH_EXCEEDED));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "ASN1_R_DEPTH_EXCEEDED", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_ASN1_R_DECODING_ERROR(lib);
}

static int _cffi_const_ASN1_R_ENCODE_ERROR(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (ASN1_R_ENCODE_ERROR) && (ASN1_R_ENCODE_ERROR) <= LONG_MAX)
    o = PyInt_FromLong((long)(ASN1_R_ENCODE_ERROR));
  else if ((ASN1_R_ENCODE_ERROR) <= 0)
    o = PyLong_FromLongLong((long long)(ASN1_R_ENCODE_ERROR));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(ASN1_R_ENCODE_ERROR));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "ASN1_R_ENCODE_ERROR", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_ASN1_R_DEPTH_EXCEEDED(lib);
}

static int _cffi_const_ASN1_R_ERROR_GETTING_TIME(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (ASN1_R_ERROR_GETTING_TIME) && (ASN1_R_ERROR_GETTING_TIME) <= LONG_MAX)
    o = PyInt_FromLong((long)(ASN1_R_ERROR_GETTING_TIME));
  else if ((ASN1_R_ERROR_GETTING_TIME) <= 0)
    o = PyLong_FromLongLong((long long)(ASN1_R_ERROR_GETTING_TIME));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(ASN1_R_ERROR_GETTING_TIME));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "ASN1_R_ERROR_GETTING_TIME", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_ASN1_R_ENCODE_ERROR(lib);
}

static int _cffi_const_ASN1_R_ERROR_LOADING_SECTION(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (ASN1_R_ERROR_LOADING_SECTION) && (ASN1_R_ERROR_LOADING_SECTION) <= LONG_MAX)
    o = PyInt_FromLong((long)(ASN1_R_ERROR_LOADING_SECTION));
  else if ((ASN1_R_ERROR_LOADING_SECTION) <= 0)
    o = PyLong_FromLongLong((long long)(ASN1_R_ERROR_LOADING_SECTION));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(ASN1_R_ERROR_LOADING_SECTION));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "ASN1_R_ERROR_LOADING_SECTION", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_ASN1_R_ERROR_GETTING_TIME(lib);
}

static int _cffi_const_ASN1_R_MSTRING_WRONG_TAG(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (ASN1_R_MSTRING_WRONG_TAG) && (ASN1_R_MSTRING_WRONG_TAG) <= LONG_MAX)
    o = PyInt_FromLong((long)(ASN1_R_MSTRING_WRONG_TAG));
  else if ((ASN1_R_MSTRING_WRONG_TAG) <= 0)
    o = PyLong_FromLongLong((long long)(ASN1_R_MSTRING_WRONG_TAG));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(ASN1_R_MSTRING_WRONG_TAG));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "ASN1_R_MSTRING_WRONG_TAG", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_ASN1_R_ERROR_LOADING_SECTION(lib);
}

static int _cffi_const_ASN1_R_NESTED_ASN1_STRING(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (ASN1_R_NESTED_ASN1_STRING) && (ASN1_R_NESTED_ASN1_STRING) <= LONG_MAX)
    o = PyInt_FromLong((long)(ASN1_R_NESTED_ASN1_STRING));
  else if ((ASN1_R_NESTED_ASN1_STRING) <= 0)
    o = PyLong_FromLongLong((long long)(ASN1_R_NESTED_ASN1_STRING));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(ASN1_R_NESTED_ASN1_STRING));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "ASN1_R_NESTED_ASN1_STRING", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_ASN1_R_MSTRING_WRONG_TAG(lib);
}

static int _cffi_const_ASN1_R_NO_CONTENT_TYPE(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (ASN1_R_NO_CONTENT_TYPE) && (ASN1_R_NO_CONTENT_TYPE) <= LONG_MAX)
    o = PyInt_FromLong((long)(ASN1_R_NO_CONTENT_TYPE));
  else if ((ASN1_R_NO_CONTENT_TYPE) <= 0)
    o = PyLong_FromLongLong((long long)(ASN1_R_NO_CONTENT_TYPE));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(ASN1_R_NO_CONTENT_TYPE));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "ASN1_R_NO_CONTENT_TYPE", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_ASN1_R_NESTED_ASN1_STRING(lib);
}

static int _cffi_const_ASN1_R_NO_MATCHING_CHOICE_TYPE(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (ASN1_R_NO_MATCHING_CHOICE_TYPE) && (ASN1_R_NO_MATCHING_CHOICE_TYPE) <= LONG_MAX)
    o = PyInt_FromLong((long)(ASN1_R_NO_MATCHING_CHOICE_TYPE));
  else if ((ASN1_R_NO_MATCHING_CHOICE_TYPE) <= 0)
    o = PyLong_FromLongLong((long long)(ASN1_R_NO_MATCHING_CHOICE_TYPE));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(ASN1_R_NO_MATCHING_CHOICE_TYPE));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "ASN1_R_NO_MATCHING_CHOICE_TYPE", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_ASN1_R_NO_CONTENT_TYPE(lib);
}

static int _cffi_const_ASN1_R_NO_MULTIPART_BODY_FAILURE(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (ASN1_R_NO_MULTIPART_BODY_FAILURE) && (ASN1_R_NO_MULTIPART_BODY_FAILURE) <= LONG_MAX)
    o = PyInt_FromLong((long)(ASN1_R_NO_MULTIPART_BODY_FAILURE));
  else if ((ASN1_R_NO_MULTIPART_BODY_FAILURE) <= 0)
    o = PyLong_FromLongLong((long long)(ASN1_R_NO_MULTIPART_BODY_FAILURE));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(ASN1_R_NO_MULTIPART_BODY_FAILURE));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "ASN1_R_NO_MULTIPART_BODY_FAILURE", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_ASN1_R_NO_MATCHING_CHOICE_TYPE(lib);
}

static int _cffi_const_ASN1_R_NO_MULTIPART_BOUNDARY(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (ASN1_R_NO_MULTIPART_BOUNDARY) && (ASN1_R_NO_MULTIPART_BOUNDARY) <= LONG_MAX)
    o = PyInt_FromLong((long)(ASN1_R_NO_MULTIPART_BOUNDARY));
  else if ((ASN1_R_NO_MULTIPART_BOUNDARY) <= 0)
    o = PyLong_FromLongLong((long long)(ASN1_R_NO_MULTIPART_BOUNDARY));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(ASN1_R_NO_MULTIPART_BOUNDARY));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "ASN1_R_NO_MULTIPART_BOUNDARY", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_ASN1_R_NO_MULTIPART_BODY_FAILURE(lib);
}

static int _cffi_const_ASN1_R_UNKNOWN_MESSAGE_DIGEST_ALGORITHM(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (ASN1_R_UNKNOWN_MESSAGE_DIGEST_ALGORITHM) && (ASN1_R_UNKNOWN_MESSAGE_DIGEST_ALGORITHM) <= LONG_MAX)
    o = PyInt_FromLong((long)(ASN1_R_UNKNOWN_MESSAGE_DIGEST_ALGORITHM));
  else if ((ASN1_R_UNKNOWN_MESSAGE_DIGEST_ALGORITHM) <= 0)
    o = PyLong_FromLongLong((long long)(ASN1_R_UNKNOWN_MESSAGE_DIGEST_ALGORITHM));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(ASN1_R_UNKNOWN_MESSAGE_DIGEST_ALGORITHM));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "ASN1_R_UNKNOWN_MESSAGE_DIGEST_ALGORITHM", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_ASN1_R_NO_MULTIPART_BOUNDARY(lib);
}

static int _cffi_const_ASN1_R_UNKNOWN_OBJECT_TYPE(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (ASN1_R_UNKNOWN_OBJECT_TYPE) && (ASN1_R_UNKNOWN_OBJECT_TYPE) <= LONG_MAX)
    o = PyInt_FromLong((long)(ASN1_R_UNKNOWN_OBJECT_TYPE));
  else if ((ASN1_R_UNKNOWN_OBJECT_TYPE) <= 0)
    o = PyLong_FromLongLong((long long)(ASN1_R_UNKNOWN_OBJECT_TYPE));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(ASN1_R_UNKNOWN_OBJECT_TYPE));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "ASN1_R_UNKNOWN_OBJECT_TYPE", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_ASN1_R_UNKNOWN_MESSAGE_DIGEST_ALGORITHM(lib);
}

static int _cffi_const_ASN1_R_UNKNOWN_PUBLIC_KEY_TYPE(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (ASN1_R_UNKNOWN_PUBLIC_KEY_TYPE) && (ASN1_R_UNKNOWN_PUBLIC_KEY_TYPE) <= LONG_MAX)
    o = PyInt_FromLong((long)(ASN1_R_UNKNOWN_PUBLIC_KEY_TYPE));
  else if ((ASN1_R_UNKNOWN_PUBLIC_KEY_TYPE) <= 0)
    o = PyLong_FromLongLong((long long)(ASN1_R_UNKNOWN_PUBLIC_KEY_TYPE));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(ASN1_R_UNKNOWN_PUBLIC_KEY_TYPE));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "ASN1_R_UNKNOWN_PUBLIC_KEY_TYPE", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_ASN1_R_UNKNOWN_OBJECT_TYPE(lib);
}

static int _cffi_const_ASN1_R_UNKNOWN_TAG(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (ASN1_R_UNKNOWN_TAG) && (ASN1_R_UNKNOWN_TAG) <= LONG_MAX)
    o = PyInt_FromLong((long)(ASN1_R_UNKNOWN_TAG));
  else if ((ASN1_R_UNKNOWN_TAG) <= 0)
    o = PyLong_FromLongLong((long long)(ASN1_R_UNKNOWN_TAG));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(ASN1_R_UNKNOWN_TAG));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "ASN1_R_UNKNOWN_TAG", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_ASN1_R_UNKNOWN_PUBLIC_KEY_TYPE(lib);
}

static int _cffi_const_ASN1_R_UNKOWN_FORMAT(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (ASN1_R_UNKOWN_FORMAT) && (ASN1_R_UNKOWN_FORMAT) <= LONG_MAX)
    o = PyInt_FromLong((long)(ASN1_R_UNKOWN_FORMAT));
  else if ((ASN1_R_UNKOWN_FORMAT) <= 0)
    o = PyLong_FromLongLong((long long)(ASN1_R_UNKOWN_FORMAT));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(ASN1_R_UNKOWN_FORMAT));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "ASN1_R_UNKOWN_FORMAT", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_ASN1_R_UNKNOWN_TAG(lib);
}

static int _cffi_const_ASN1_R_UNSUPPORTED_ANY_DEFINED_BY_TYPE(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (ASN1_R_UNSUPPORTED_ANY_DEFINED_BY_TYPE) && (ASN1_R_UNSUPPORTED_ANY_DEFINED_BY_TYPE) <= LONG_MAX)
    o = PyInt_FromLong((long)(ASN1_R_UNSUPPORTED_ANY_DEFINED_BY_TYPE));
  else if ((ASN1_R_UNSUPPORTED_ANY_DEFINED_BY_TYPE) <= 0)
    o = PyLong_FromLongLong((long long)(ASN1_R_UNSUPPORTED_ANY_DEFINED_BY_TYPE));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(ASN1_R_UNSUPPORTED_ANY_DEFINED_BY_TYPE));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "ASN1_R_UNSUPPORTED_ANY_DEFINED_BY_TYPE", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_ASN1_R_UNKOWN_FORMAT(lib);
}

static int _cffi_const_ASN1_R_UNSUPPORTED_ENCRYPTION_ALGORITHM(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (ASN1_R_UNSUPPORTED_ENCRYPTION_ALGORITHM) && (ASN1_R_UNSUPPORTED_ENCRYPTION_ALGORITHM) <= LONG_MAX)
    o = PyInt_FromLong((long)(ASN1_R_UNSUPPORTED_ENCRYPTION_ALGORITHM));
  else if ((ASN1_R_UNSUPPORTED_ENCRYPTION_ALGORITHM) <= 0)
    o = PyLong_FromLongLong((long long)(ASN1_R_UNSUPPORTED_ENCRYPTION_ALGORITHM));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(ASN1_R_UNSUPPORTED_ENCRYPTION_ALGORITHM));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "ASN1_R_UNSUPPORTED_ENCRYPTION_ALGORITHM", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_ASN1_R_UNSUPPORTED_ANY_DEFINED_BY_TYPE(lib);
}

static int _cffi_const_ASN1_R_UNSUPPORTED_PUBLIC_KEY_TYPE(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (ASN1_R_UNSUPPORTED_PUBLIC_KEY_TYPE) && (ASN1_R_UNSUPPORTED_PUBLIC_KEY_TYPE) <= LONG_MAX)
    o = PyInt_FromLong((long)(ASN1_R_UNSUPPORTED_PUBLIC_KEY_TYPE));
  else if ((ASN1_R_UNSUPPORTED_PUBLIC_KEY_TYPE) <= 0)
    o = PyLong_FromLongLong((long long)(ASN1_R_UNSUPPORTED_PUBLIC_KEY_TYPE));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(ASN1_R_UNSUPPORTED_PUBLIC_KEY_TYPE));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "ASN1_R_UNSUPPORTED_PUBLIC_KEY_TYPE", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_ASN1_R_UNSUPPORTED_ENCRYPTION_ALGORITHM(lib);
}

static int _cffi_const_ASN1_R_UNSUPPORTED_TYPE(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (ASN1_R_UNSUPPORTED_TYPE) && (ASN1_R_UNSUPPORTED_TYPE) <= LONG_MAX)
    o = PyInt_FromLong((long)(ASN1_R_UNSUPPORTED_TYPE));
  else if ((ASN1_R_UNSUPPORTED_TYPE) <= 0)
    o = PyLong_FromLongLong((long long)(ASN1_R_UNSUPPORTED_TYPE));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(ASN1_R_UNSUPPORTED_TYPE));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "ASN1_R_UNSUPPORTED_TYPE", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_ASN1_R_UNSUPPORTED_PUBLIC_KEY_TYPE(lib);
}

static int _cffi_const_ASN1_R_WRONG_TAG(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (ASN1_R_WRONG_TAG) && (ASN1_R_WRONG_TAG) <= LONG_MAX)
    o = PyInt_FromLong((long)(ASN1_R_WRONG_TAG));
  else if ((ASN1_R_WRONG_TAG) <= 0)
    o = PyLong_FromLongLong((long long)(ASN1_R_WRONG_TAG));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(ASN1_R_WRONG_TAG));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "ASN1_R_WRONG_TAG", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_ASN1_R_UNSUPPORTED_TYPE(lib);
}

static int _cffi_const_ASN1_R_WRONG_TYPE(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (ASN1_R_WRONG_TYPE) && (ASN1_R_WRONG_TYPE) <= LONG_MAX)
    o = PyInt_FromLong((long)(ASN1_R_WRONG_TYPE));
  else if ((ASN1_R_WRONG_TYPE) <= 0)
    o = PyLong_FromLongLong((long long)(ASN1_R_WRONG_TYPE));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(ASN1_R_WRONG_TYPE));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "ASN1_R_WRONG_TYPE", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_ASN1_R_WRONG_TAG(lib);
}

static int _cffi_const_BIO_CLOSE(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (BIO_CLOSE) && (BIO_CLOSE) <= LONG_MAX)
    o = PyInt_FromLong((long)(BIO_CLOSE));
  else if ((BIO_CLOSE) <= 0)
    o = PyLong_FromLongLong((long long)(BIO_CLOSE));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(BIO_CLOSE));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "BIO_CLOSE", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_ASN1_R_WRONG_TYPE(lib);
}

static int _cffi_const_BIO_CTRL_DUP(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (BIO_CTRL_DUP) && (BIO_CTRL_DUP) <= LONG_MAX)
    o = PyInt_FromLong((long)(BIO_CTRL_DUP));
  else if ((BIO_CTRL_DUP) <= 0)
    o = PyLong_FromLongLong((long long)(BIO_CTRL_DUP));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(BIO_CTRL_DUP));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "BIO_CTRL_DUP", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_BIO_CLOSE(lib);
}

static int _cffi_const_BIO_CTRL_EOF(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (BIO_CTRL_EOF) && (BIO_CTRL_EOF) <= LONG_MAX)
    o = PyInt_FromLong((long)(BIO_CTRL_EOF));
  else if ((BIO_CTRL_EOF) <= 0)
    o = PyLong_FromLongLong((long long)(BIO_CTRL_EOF));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(BIO_CTRL_EOF));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "BIO_CTRL_EOF", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_BIO_CTRL_DUP(lib);
}

static int _cffi_const_BIO_CTRL_FLUSH(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (BIO_CTRL_FLUSH) && (BIO_CTRL_FLUSH) <= LONG_MAX)
    o = PyInt_FromLong((long)(BIO_CTRL_FLUSH));
  else if ((BIO_CTRL_FLUSH) <= 0)
    o = PyLong_FromLongLong((long long)(BIO_CTRL_FLUSH));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(BIO_CTRL_FLUSH));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "BIO_CTRL_FLUSH", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_BIO_CTRL_EOF(lib);
}

static int _cffi_const_BIO_CTRL_GET(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (BIO_CTRL_GET) && (BIO_CTRL_GET) <= LONG_MAX)
    o = PyInt_FromLong((long)(BIO_CTRL_GET));
  else if ((BIO_CTRL_GET) <= 0)
    o = PyLong_FromLongLong((long long)(BIO_CTRL_GET));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(BIO_CTRL_GET));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "BIO_CTRL_GET", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_BIO_CTRL_FLUSH(lib);
}

static int _cffi_const_BIO_CTRL_GET_CLOSE(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (BIO_CTRL_GET_CLOSE) && (BIO_CTRL_GET_CLOSE) <= LONG_MAX)
    o = PyInt_FromLong((long)(BIO_CTRL_GET_CLOSE));
  else if ((BIO_CTRL_GET_CLOSE) <= 0)
    o = PyLong_FromLongLong((long long)(BIO_CTRL_GET_CLOSE));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(BIO_CTRL_GET_CLOSE));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "BIO_CTRL_GET_CLOSE", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_BIO_CTRL_GET(lib);
}

static int _cffi_const_BIO_CTRL_INFO(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (BIO_CTRL_INFO) && (BIO_CTRL_INFO) <= LONG_MAX)
    o = PyInt_FromLong((long)(BIO_CTRL_INFO));
  else if ((BIO_CTRL_INFO) <= 0)
    o = PyLong_FromLongLong((long long)(BIO_CTRL_INFO));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(BIO_CTRL_INFO));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "BIO_CTRL_INFO", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_BIO_CTRL_GET_CLOSE(lib);
}

static int _cffi_const_BIO_CTRL_PENDING(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (BIO_CTRL_PENDING) && (BIO_CTRL_PENDING) <= LONG_MAX)
    o = PyInt_FromLong((long)(BIO_CTRL_PENDING));
  else if ((BIO_CTRL_PENDING) <= 0)
    o = PyLong_FromLongLong((long long)(BIO_CTRL_PENDING));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(BIO_CTRL_PENDING));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "BIO_CTRL_PENDING", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_BIO_CTRL_INFO(lib);
}

static int _cffi_const_BIO_CTRL_RESET(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (BIO_CTRL_RESET) && (BIO_CTRL_RESET) <= LONG_MAX)
    o = PyInt_FromLong((long)(BIO_CTRL_RESET));
  else if ((BIO_CTRL_RESET) <= 0)
    o = PyLong_FromLongLong((long long)(BIO_CTRL_RESET));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(BIO_CTRL_RESET));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "BIO_CTRL_RESET", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_BIO_CTRL_PENDING(lib);
}

static int _cffi_const_BIO_CTRL_SET(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (BIO_CTRL_SET) && (BIO_CTRL_SET) <= LONG_MAX)
    o = PyInt_FromLong((long)(BIO_CTRL_SET));
  else if ((BIO_CTRL_SET) <= 0)
    o = PyLong_FromLongLong((long long)(BIO_CTRL_SET));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(BIO_CTRL_SET));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "BIO_CTRL_SET", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_BIO_CTRL_RESET(lib);
}

static int _cffi_const_BIO_CTRL_SET_CLOSE(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (BIO_CTRL_SET_CLOSE) && (BIO_CTRL_SET_CLOSE) <= LONG_MAX)
    o = PyInt_FromLong((long)(BIO_CTRL_SET_CLOSE));
  else if ((BIO_CTRL_SET_CLOSE) <= 0)
    o = PyLong_FromLongLong((long long)(BIO_CTRL_SET_CLOSE));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(BIO_CTRL_SET_CLOSE));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "BIO_CTRL_SET_CLOSE", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_BIO_CTRL_SET(lib);
}

static int _cffi_const_BIO_CTRL_WPENDING(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (BIO_CTRL_WPENDING) && (BIO_CTRL_WPENDING) <= LONG_MAX)
    o = PyInt_FromLong((long)(BIO_CTRL_WPENDING));
  else if ((BIO_CTRL_WPENDING) <= 0)
    o = PyLong_FromLongLong((long long)(BIO_CTRL_WPENDING));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(BIO_CTRL_WPENDING));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "BIO_CTRL_WPENDING", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_BIO_CTRL_SET_CLOSE(lib);
}

static int _cffi_const_BIO_C_FILE_SEEK(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (BIO_C_FILE_SEEK) && (BIO_C_FILE_SEEK) <= LONG_MAX)
    o = PyInt_FromLong((long)(BIO_C_FILE_SEEK));
  else if ((BIO_C_FILE_SEEK) <= 0)
    o = PyLong_FromLongLong((long long)(BIO_C_FILE_SEEK));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(BIO_C_FILE_SEEK));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "BIO_C_FILE_SEEK", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_BIO_CTRL_WPENDING(lib);
}

static int _cffi_const_BIO_C_FILE_TELL(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (BIO_C_FILE_TELL) && (BIO_C_FILE_TELL) <= LONG_MAX)
    o = PyInt_FromLong((long)(BIO_C_FILE_TELL));
  else if ((BIO_C_FILE_TELL) <= 0)
    o = PyLong_FromLongLong((long long)(BIO_C_FILE_TELL));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(BIO_C_FILE_TELL));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "BIO_C_FILE_TELL", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_BIO_C_FILE_SEEK(lib);
}

static int _cffi_const_BIO_FLAGS_IO_SPECIAL(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (BIO_FLAGS_IO_SPECIAL) && (BIO_FLAGS_IO_SPECIAL) <= LONG_MAX)
    o = PyInt_FromLong((long)(BIO_FLAGS_IO_SPECIAL));
  else if ((BIO_FLAGS_IO_SPECIAL) <= 0)
    o = PyLong_FromLongLong((long long)(BIO_FLAGS_IO_SPECIAL));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(BIO_FLAGS_IO_SPECIAL));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "BIO_FLAGS_IO_SPECIAL", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_BIO_C_FILE_TELL(lib);
}

static int _cffi_const_BIO_FLAGS_READ(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (BIO_FLAGS_READ) && (BIO_FLAGS_READ) <= LONG_MAX)
    o = PyInt_FromLong((long)(BIO_FLAGS_READ));
  else if ((BIO_FLAGS_READ) <= 0)
    o = PyLong_FromLongLong((long long)(BIO_FLAGS_READ));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(BIO_FLAGS_READ));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "BIO_FLAGS_READ", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_BIO_FLAGS_IO_SPECIAL(lib);
}

static int _cffi_const_BIO_FLAGS_RWS(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (BIO_FLAGS_RWS) && (BIO_FLAGS_RWS) <= LONG_MAX)
    o = PyInt_FromLong((long)(BIO_FLAGS_RWS));
  else if ((BIO_FLAGS_RWS) <= 0)
    o = PyLong_FromLongLong((long long)(BIO_FLAGS_RWS));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(BIO_FLAGS_RWS));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "BIO_FLAGS_RWS", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_BIO_FLAGS_READ(lib);
}

static int _cffi_const_BIO_FLAGS_SHOULD_RETRY(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (BIO_FLAGS_SHOULD_RETRY) && (BIO_FLAGS_SHOULD_RETRY) <= LONG_MAX)
    o = PyInt_FromLong((long)(BIO_FLAGS_SHOULD_RETRY));
  else if ((BIO_FLAGS_SHOULD_RETRY) <= 0)
    o = PyLong_FromLongLong((long long)(BIO_FLAGS_SHOULD_RETRY));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(BIO_FLAGS_SHOULD_RETRY));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "BIO_FLAGS_SHOULD_RETRY", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_BIO_FLAGS_RWS(lib);
}

static int _cffi_const_BIO_FLAGS_WRITE(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (BIO_FLAGS_WRITE) && (BIO_FLAGS_WRITE) <= LONG_MAX)
    o = PyInt_FromLong((long)(BIO_FLAGS_WRITE));
  else if ((BIO_FLAGS_WRITE) <= 0)
    o = PyLong_FromLongLong((long long)(BIO_FLAGS_WRITE));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(BIO_FLAGS_WRITE));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "BIO_FLAGS_WRITE", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_BIO_FLAGS_SHOULD_RETRY(lib);
}

static int _cffi_const_BIO_NOCLOSE(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (BIO_NOCLOSE) && (BIO_NOCLOSE) <= LONG_MAX)
    o = PyInt_FromLong((long)(BIO_NOCLOSE));
  else if ((BIO_NOCLOSE) <= 0)
    o = PyLong_FromLongLong((long long)(BIO_NOCLOSE));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(BIO_NOCLOSE));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "BIO_NOCLOSE", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_BIO_FLAGS_WRITE(lib);
}

static int _cffi_const_BIO_TYPE_ACCEPT(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (BIO_TYPE_ACCEPT) && (BIO_TYPE_ACCEPT) <= LONG_MAX)
    o = PyInt_FromLong((long)(BIO_TYPE_ACCEPT));
  else if ((BIO_TYPE_ACCEPT) <= 0)
    o = PyLong_FromLongLong((long long)(BIO_TYPE_ACCEPT));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(BIO_TYPE_ACCEPT));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "BIO_TYPE_ACCEPT", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_BIO_NOCLOSE(lib);
}

static int _cffi_const_BIO_TYPE_BASE64(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (BIO_TYPE_BASE64) && (BIO_TYPE_BASE64) <= LONG_MAX)
    o = PyInt_FromLong((long)(BIO_TYPE_BASE64));
  else if ((BIO_TYPE_BASE64) <= 0)
    o = PyLong_FromLongLong((long long)(BIO_TYPE_BASE64));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(BIO_TYPE_BASE64));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "BIO_TYPE_BASE64", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_BIO_TYPE_ACCEPT(lib);
}

static int _cffi_const_BIO_TYPE_BER(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (BIO_TYPE_BER) && (BIO_TYPE_BER) <= LONG_MAX)
    o = PyInt_FromLong((long)(BIO_TYPE_BER));
  else if ((BIO_TYPE_BER) <= 0)
    o = PyLong_FromLongLong((long long)(BIO_TYPE_BER));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(BIO_TYPE_BER));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "BIO_TYPE_BER", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_BIO_TYPE_BASE64(lib);
}

static int _cffi_const_BIO_TYPE_BIO(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (BIO_TYPE_BIO) && (BIO_TYPE_BIO) <= LONG_MAX)
    o = PyInt_FromLong((long)(BIO_TYPE_BIO));
  else if ((BIO_TYPE_BIO) <= 0)
    o = PyLong_FromLongLong((long long)(BIO_TYPE_BIO));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(BIO_TYPE_BIO));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "BIO_TYPE_BIO", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_BIO_TYPE_BER(lib);
}

static int _cffi_const_BIO_TYPE_BUFFER(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (BIO_TYPE_BUFFER) && (BIO_TYPE_BUFFER) <= LONG_MAX)
    o = PyInt_FromLong((long)(BIO_TYPE_BUFFER));
  else if ((BIO_TYPE_BUFFER) <= 0)
    o = PyLong_FromLongLong((long long)(BIO_TYPE_BUFFER));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(BIO_TYPE_BUFFER));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "BIO_TYPE_BUFFER", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_BIO_TYPE_BIO(lib);
}

static int _cffi_const_BIO_TYPE_CIPHER(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (BIO_TYPE_CIPHER) && (BIO_TYPE_CIPHER) <= LONG_MAX)
    o = PyInt_FromLong((long)(BIO_TYPE_CIPHER));
  else if ((BIO_TYPE_CIPHER) <= 0)
    o = PyLong_FromLongLong((long long)(BIO_TYPE_CIPHER));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(BIO_TYPE_CIPHER));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "BIO_TYPE_CIPHER", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_BIO_TYPE_BUFFER(lib);
}

static int _cffi_const_BIO_TYPE_CONNECT(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (BIO_TYPE_CONNECT) && (BIO_TYPE_CONNECT) <= LONG_MAX)
    o = PyInt_FromLong((long)(BIO_TYPE_CONNECT));
  else if ((BIO_TYPE_CONNECT) <= 0)
    o = PyLong_FromLongLong((long long)(BIO_TYPE_CONNECT));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(BIO_TYPE_CONNECT));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "BIO_TYPE_CONNECT", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_BIO_TYPE_CIPHER(lib);
}

static int _cffi_const_BIO_TYPE_DESCRIPTOR(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (BIO_TYPE_DESCRIPTOR) && (BIO_TYPE_DESCRIPTOR) <= LONG_MAX)
    o = PyInt_FromLong((long)(BIO_TYPE_DESCRIPTOR));
  else if ((BIO_TYPE_DESCRIPTOR) <= 0)
    o = PyLong_FromLongLong((long long)(BIO_TYPE_DESCRIPTOR));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(BIO_TYPE_DESCRIPTOR));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "BIO_TYPE_DESCRIPTOR", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_BIO_TYPE_CONNECT(lib);
}

static int _cffi_const_BIO_TYPE_FD(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (BIO_TYPE_FD) && (BIO_TYPE_FD) <= LONG_MAX)
    o = PyInt_FromLong((long)(BIO_TYPE_FD));
  else if ((BIO_TYPE_FD) <= 0)
    o = PyLong_FromLongLong((long long)(BIO_TYPE_FD));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(BIO_TYPE_FD));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "BIO_TYPE_FD", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_BIO_TYPE_DESCRIPTOR(lib);
}

static int _cffi_const_BIO_TYPE_FILE(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (BIO_TYPE_FILE) && (BIO_TYPE_FILE) <= LONG_MAX)
    o = PyInt_FromLong((long)(BIO_TYPE_FILE));
  else if ((BIO_TYPE_FILE) <= 0)
    o = PyLong_FromLongLong((long long)(BIO_TYPE_FILE));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(BIO_TYPE_FILE));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "BIO_TYPE_FILE", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_BIO_TYPE_FD(lib);
}

static int _cffi_const_BIO_TYPE_FILTER(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (BIO_TYPE_FILTER) && (BIO_TYPE_FILTER) <= LONG_MAX)
    o = PyInt_FromLong((long)(BIO_TYPE_FILTER));
  else if ((BIO_TYPE_FILTER) <= 0)
    o = PyLong_FromLongLong((long long)(BIO_TYPE_FILTER));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(BIO_TYPE_FILTER));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "BIO_TYPE_FILTER", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_BIO_TYPE_FILE(lib);
}

static int _cffi_const_BIO_TYPE_MD(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (BIO_TYPE_MD) && (BIO_TYPE_MD) <= LONG_MAX)
    o = PyInt_FromLong((long)(BIO_TYPE_MD));
  else if ((BIO_TYPE_MD) <= 0)
    o = PyLong_FromLongLong((long long)(BIO_TYPE_MD));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(BIO_TYPE_MD));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "BIO_TYPE_MD", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_BIO_TYPE_FILTER(lib);
}

static int _cffi_const_BIO_TYPE_MEM(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (BIO_TYPE_MEM) && (BIO_TYPE_MEM) <= LONG_MAX)
    o = PyInt_FromLong((long)(BIO_TYPE_MEM));
  else if ((BIO_TYPE_MEM) <= 0)
    o = PyLong_FromLongLong((long long)(BIO_TYPE_MEM));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(BIO_TYPE_MEM));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "BIO_TYPE_MEM", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_BIO_TYPE_MD(lib);
}

static int _cffi_const_BIO_TYPE_NBIO_TEST(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (BIO_TYPE_NBIO_TEST) && (BIO_TYPE_NBIO_TEST) <= LONG_MAX)
    o = PyInt_FromLong((long)(BIO_TYPE_NBIO_TEST));
  else if ((BIO_TYPE_NBIO_TEST) <= 0)
    o = PyLong_FromLongLong((long long)(BIO_TYPE_NBIO_TEST));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(BIO_TYPE_NBIO_TEST));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "BIO_TYPE_NBIO_TEST", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_BIO_TYPE_MEM(lib);
}

static int _cffi_const_BIO_TYPE_NONE(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (BIO_TYPE_NONE) && (BIO_TYPE_NONE) <= LONG_MAX)
    o = PyInt_FromLong((long)(BIO_TYPE_NONE));
  else if ((BIO_TYPE_NONE) <= 0)
    o = PyLong_FromLongLong((long long)(BIO_TYPE_NONE));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(BIO_TYPE_NONE));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "BIO_TYPE_NONE", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_BIO_TYPE_NBIO_TEST(lib);
}

static int _cffi_const_BIO_TYPE_NULL(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (BIO_TYPE_NULL) && (BIO_TYPE_NULL) <= LONG_MAX)
    o = PyInt_FromLong((long)(BIO_TYPE_NULL));
  else if ((BIO_TYPE_NULL) <= 0)
    o = PyLong_FromLongLong((long long)(BIO_TYPE_NULL));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(BIO_TYPE_NULL));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "BIO_TYPE_NULL", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_BIO_TYPE_NONE(lib);
}

static int _cffi_const_BIO_TYPE_NULL_FILTER(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (BIO_TYPE_NULL_FILTER) && (BIO_TYPE_NULL_FILTER) <= LONG_MAX)
    o = PyInt_FromLong((long)(BIO_TYPE_NULL_FILTER));
  else if ((BIO_TYPE_NULL_FILTER) <= 0)
    o = PyLong_FromLongLong((long long)(BIO_TYPE_NULL_FILTER));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(BIO_TYPE_NULL_FILTER));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "BIO_TYPE_NULL_FILTER", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_BIO_TYPE_NULL(lib);
}

static int _cffi_const_BIO_TYPE_PROXY_CLIENT(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (BIO_TYPE_PROXY_CLIENT) && (BIO_TYPE_PROXY_CLIENT) <= LONG_MAX)
    o = PyInt_FromLong((long)(BIO_TYPE_PROXY_CLIENT));
  else if ((BIO_TYPE_PROXY_CLIENT) <= 0)
    o = PyLong_FromLongLong((long long)(BIO_TYPE_PROXY_CLIENT));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(BIO_TYPE_PROXY_CLIENT));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "BIO_TYPE_PROXY_CLIENT", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_BIO_TYPE_NULL_FILTER(lib);
}

static int _cffi_const_BIO_TYPE_PROXY_SERVER(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (BIO_TYPE_PROXY_SERVER) && (BIO_TYPE_PROXY_SERVER) <= LONG_MAX)
    o = PyInt_FromLong((long)(BIO_TYPE_PROXY_SERVER));
  else if ((BIO_TYPE_PROXY_SERVER) <= 0)
    o = PyLong_FromLongLong((long long)(BIO_TYPE_PROXY_SERVER));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(BIO_TYPE_PROXY_SERVER));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "BIO_TYPE_PROXY_SERVER", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_BIO_TYPE_PROXY_CLIENT(lib);
}

static int _cffi_const_BIO_TYPE_SOCKET(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (BIO_TYPE_SOCKET) && (BIO_TYPE_SOCKET) <= LONG_MAX)
    o = PyInt_FromLong((long)(BIO_TYPE_SOCKET));
  else if ((BIO_TYPE_SOCKET) <= 0)
    o = PyLong_FromLongLong((long long)(BIO_TYPE_SOCKET));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(BIO_TYPE_SOCKET));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "BIO_TYPE_SOCKET", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_BIO_TYPE_PROXY_SERVER(lib);
}

static int _cffi_const_BIO_TYPE_SOURCE_SINK(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (BIO_TYPE_SOURCE_SINK) && (BIO_TYPE_SOURCE_SINK) <= LONG_MAX)
    o = PyInt_FromLong((long)(BIO_TYPE_SOURCE_SINK));
  else if ((BIO_TYPE_SOURCE_SINK) <= 0)
    o = PyLong_FromLongLong((long long)(BIO_TYPE_SOURCE_SINK));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(BIO_TYPE_SOURCE_SINK));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "BIO_TYPE_SOURCE_SINK", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_BIO_TYPE_SOCKET(lib);
}

static int _cffi_const_BIO_TYPE_SSL(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (BIO_TYPE_SSL) && (BIO_TYPE_SSL) <= LONG_MAX)
    o = PyInt_FromLong((long)(BIO_TYPE_SSL));
  else if ((BIO_TYPE_SSL) <= 0)
    o = PyLong_FromLongLong((long long)(BIO_TYPE_SSL));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(BIO_TYPE_SSL));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "BIO_TYPE_SSL", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_BIO_TYPE_SOURCE_SINK(lib);
}

static int _cffi_const_CRYPTO_LOCK(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (CRYPTO_LOCK) && (CRYPTO_LOCK) <= LONG_MAX)
    o = PyInt_FromLong((long)(CRYPTO_LOCK));
  else if ((CRYPTO_LOCK) <= 0)
    o = PyLong_FromLongLong((long long)(CRYPTO_LOCK));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(CRYPTO_LOCK));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "CRYPTO_LOCK", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_BIO_TYPE_SSL(lib);
}

static int _cffi_const_CRYPTO_LOCK_SSL(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (CRYPTO_LOCK_SSL) && (CRYPTO_LOCK_SSL) <= LONG_MAX)
    o = PyInt_FromLong((long)(CRYPTO_LOCK_SSL));
  else if ((CRYPTO_LOCK_SSL) <= 0)
    o = PyLong_FromLongLong((long long)(CRYPTO_LOCK_SSL));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(CRYPTO_LOCK_SSL));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "CRYPTO_LOCK_SSL", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_CRYPTO_LOCK(lib);
}

static int _cffi_const_CRYPTO_MEM_CHECK_DISABLE(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (CRYPTO_MEM_CHECK_DISABLE) && (CRYPTO_MEM_CHECK_DISABLE) <= LONG_MAX)
    o = PyInt_FromLong((long)(CRYPTO_MEM_CHECK_DISABLE));
  else if ((CRYPTO_MEM_CHECK_DISABLE) <= 0)
    o = PyLong_FromLongLong((long long)(CRYPTO_MEM_CHECK_DISABLE));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(CRYPTO_MEM_CHECK_DISABLE));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "CRYPTO_MEM_CHECK_DISABLE", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_CRYPTO_LOCK_SSL(lib);
}

static int _cffi_const_CRYPTO_MEM_CHECK_ENABLE(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (CRYPTO_MEM_CHECK_ENABLE) && (CRYPTO_MEM_CHECK_ENABLE) <= LONG_MAX)
    o = PyInt_FromLong((long)(CRYPTO_MEM_CHECK_ENABLE));
  else if ((CRYPTO_MEM_CHECK_ENABLE) <= 0)
    o = PyLong_FromLongLong((long long)(CRYPTO_MEM_CHECK_ENABLE));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(CRYPTO_MEM_CHECK_ENABLE));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "CRYPTO_MEM_CHECK_ENABLE", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_CRYPTO_MEM_CHECK_DISABLE(lib);
}

static int _cffi_const_CRYPTO_MEM_CHECK_OFF(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (CRYPTO_MEM_CHECK_OFF) && (CRYPTO_MEM_CHECK_OFF) <= LONG_MAX)
    o = PyInt_FromLong((long)(CRYPTO_MEM_CHECK_OFF));
  else if ((CRYPTO_MEM_CHECK_OFF) <= 0)
    o = PyLong_FromLongLong((long long)(CRYPTO_MEM_CHECK_OFF));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(CRYPTO_MEM_CHECK_OFF));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "CRYPTO_MEM_CHECK_OFF", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_CRYPTO_MEM_CHECK_ENABLE(lib);
}

static int _cffi_const_CRYPTO_MEM_CHECK_ON(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (CRYPTO_MEM_CHECK_ON) && (CRYPTO_MEM_CHECK_ON) <= LONG_MAX)
    o = PyInt_FromLong((long)(CRYPTO_MEM_CHECK_ON));
  else if ((CRYPTO_MEM_CHECK_ON) <= 0)
    o = PyLong_FromLongLong((long long)(CRYPTO_MEM_CHECK_ON));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(CRYPTO_MEM_CHECK_ON));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "CRYPTO_MEM_CHECK_ON", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_CRYPTO_MEM_CHECK_OFF(lib);
}

static int _cffi_const_CRYPTO_READ(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (CRYPTO_READ) && (CRYPTO_READ) <= LONG_MAX)
    o = PyInt_FromLong((long)(CRYPTO_READ));
  else if ((CRYPTO_READ) <= 0)
    o = PyLong_FromLongLong((long long)(CRYPTO_READ));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(CRYPTO_READ));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "CRYPTO_READ", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_CRYPTO_MEM_CHECK_ON(lib);
}

static int _cffi_const_CRYPTO_UNLOCK(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (CRYPTO_UNLOCK) && (CRYPTO_UNLOCK) <= LONG_MAX)
    o = PyInt_FromLong((long)(CRYPTO_UNLOCK));
  else if ((CRYPTO_UNLOCK) <= 0)
    o = PyLong_FromLongLong((long long)(CRYPTO_UNLOCK));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(CRYPTO_UNLOCK));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "CRYPTO_UNLOCK", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_CRYPTO_READ(lib);
}

static int _cffi_const_CRYPTO_WRITE(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (CRYPTO_WRITE) && (CRYPTO_WRITE) <= LONG_MAX)
    o = PyInt_FromLong((long)(CRYPTO_WRITE));
  else if ((CRYPTO_WRITE) <= 0)
    o = PyLong_FromLongLong((long long)(CRYPTO_WRITE));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(CRYPTO_WRITE));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "CRYPTO_WRITE", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_CRYPTO_UNLOCK(lib);
}

static int _cffi_const_Cryptography_HAS_EC(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (Cryptography_HAS_EC) && (Cryptography_HAS_EC) <= LONG_MAX)
    o = PyInt_FromLong((long)(Cryptography_HAS_EC));
  else if ((Cryptography_HAS_EC) <= 0)
    o = PyLong_FromLongLong((long long)(Cryptography_HAS_EC));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(Cryptography_HAS_EC));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "Cryptography_HAS_EC", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_CRYPTO_WRITE(lib);
}

static int _cffi_const_Cryptography_HAS_GCM(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (Cryptography_HAS_GCM) && (Cryptography_HAS_GCM) <= LONG_MAX)
    o = PyInt_FromLong((long)(Cryptography_HAS_GCM));
  else if ((Cryptography_HAS_GCM) <= 0)
    o = PyLong_FromLongLong((long long)(Cryptography_HAS_GCM));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(Cryptography_HAS_GCM));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "Cryptography_HAS_GCM", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_Cryptography_HAS_EC(lib);
}

static int _cffi_const_Cryptography_HAS_OP_NO_COMPRESSION(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (Cryptography_HAS_OP_NO_COMPRESSION) && (Cryptography_HAS_OP_NO_COMPRESSION) <= LONG_MAX)
    o = PyInt_FromLong((long)(Cryptography_HAS_OP_NO_COMPRESSION));
  else if ((Cryptography_HAS_OP_NO_COMPRESSION) <= 0)
    o = PyLong_FromLongLong((long long)(Cryptography_HAS_OP_NO_COMPRESSION));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(Cryptography_HAS_OP_NO_COMPRESSION));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "Cryptography_HAS_OP_NO_COMPRESSION", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_Cryptography_HAS_GCM(lib);
}

static int _cffi_const_Cryptography_HAS_PBKDF2_HMAC(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (Cryptography_HAS_PBKDF2_HMAC) && (Cryptography_HAS_PBKDF2_HMAC) <= LONG_MAX)
    o = PyInt_FromLong((long)(Cryptography_HAS_PBKDF2_HMAC));
  else if ((Cryptography_HAS_PBKDF2_HMAC) <= 0)
    o = PyLong_FromLongLong((long long)(Cryptography_HAS_PBKDF2_HMAC));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(Cryptography_HAS_PBKDF2_HMAC));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "Cryptography_HAS_PBKDF2_HMAC", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_Cryptography_HAS_OP_NO_COMPRESSION(lib);
}

static int _cffi_const_Cryptography_HAS_PKEY_CTX(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (Cryptography_HAS_PKEY_CTX) && (Cryptography_HAS_PKEY_CTX) <= LONG_MAX)
    o = PyInt_FromLong((long)(Cryptography_HAS_PKEY_CTX));
  else if ((Cryptography_HAS_PKEY_CTX) <= 0)
    o = PyLong_FromLongLong((long long)(Cryptography_HAS_PKEY_CTX));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(Cryptography_HAS_PKEY_CTX));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "Cryptography_HAS_PKEY_CTX", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_Cryptography_HAS_PBKDF2_HMAC(lib);
}

static int _cffi_const_Cryptography_HAS_PSS_PADDING(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (Cryptography_HAS_PSS_PADDING) && (Cryptography_HAS_PSS_PADDING) <= LONG_MAX)
    o = PyInt_FromLong((long)(Cryptography_HAS_PSS_PADDING));
  else if ((Cryptography_HAS_PSS_PADDING) <= 0)
    o = PyLong_FromLongLong((long long)(Cryptography_HAS_PSS_PADDING));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(Cryptography_HAS_PSS_PADDING));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "Cryptography_HAS_PSS_PADDING", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_Cryptography_HAS_PKEY_CTX(lib);
}

static int _cffi_const_Cryptography_HAS_RELEASE_BUFFERS(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (Cryptography_HAS_RELEASE_BUFFERS) && (Cryptography_HAS_RELEASE_BUFFERS) <= LONG_MAX)
    o = PyInt_FromLong((long)(Cryptography_HAS_RELEASE_BUFFERS));
  else if ((Cryptography_HAS_RELEASE_BUFFERS) <= 0)
    o = PyLong_FromLongLong((long long)(Cryptography_HAS_RELEASE_BUFFERS));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(Cryptography_HAS_RELEASE_BUFFERS));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "Cryptography_HAS_RELEASE_BUFFERS", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_Cryptography_HAS_PSS_PADDING(lib);
}

static int _cffi_const_Cryptography_HAS_REMOVE_THREAD_STATE(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (Cryptography_HAS_REMOVE_THREAD_STATE) && (Cryptography_HAS_REMOVE_THREAD_STATE) <= LONG_MAX)
    o = PyInt_FromLong((long)(Cryptography_HAS_REMOVE_THREAD_STATE));
  else if ((Cryptography_HAS_REMOVE_THREAD_STATE) <= 0)
    o = PyLong_FromLongLong((long long)(Cryptography_HAS_REMOVE_THREAD_STATE));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(Cryptography_HAS_REMOVE_THREAD_STATE));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "Cryptography_HAS_REMOVE_THREAD_STATE", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_Cryptography_HAS_RELEASE_BUFFERS(lib);
}

static int _cffi_const_Cryptography_HAS_SSL2(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (Cryptography_HAS_SSL2) && (Cryptography_HAS_SSL2) <= LONG_MAX)
    o = PyInt_FromLong((long)(Cryptography_HAS_SSL2));
  else if ((Cryptography_HAS_SSL2) <= 0)
    o = PyLong_FromLongLong((long long)(Cryptography_HAS_SSL2));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(Cryptography_HAS_SSL2));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "Cryptography_HAS_SSL2", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_Cryptography_HAS_REMOVE_THREAD_STATE(lib);
}

static int _cffi_const_Cryptography_HAS_SSL_OP_MSIE_SSLV2_RSA_PADDING(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (Cryptography_HAS_SSL_OP_MSIE_SSLV2_RSA_PADDING) && (Cryptography_HAS_SSL_OP_MSIE_SSLV2_RSA_PADDING) <= LONG_MAX)
    o = PyInt_FromLong((long)(Cryptography_HAS_SSL_OP_MSIE_SSLV2_RSA_PADDING));
  else if ((Cryptography_HAS_SSL_OP_MSIE_SSLV2_RSA_PADDING) <= 0)
    o = PyLong_FromLongLong((long long)(Cryptography_HAS_SSL_OP_MSIE_SSLV2_RSA_PADDING));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(Cryptography_HAS_SSL_OP_MSIE_SSLV2_RSA_PADDING));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "Cryptography_HAS_SSL_OP_MSIE_SSLV2_RSA_PADDING", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_Cryptography_HAS_SSL2(lib);
}

static int _cffi_const_Cryptography_HAS_TLSEXT_HOSTNAME(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (Cryptography_HAS_TLSEXT_HOSTNAME) && (Cryptography_HAS_TLSEXT_HOSTNAME) <= LONG_MAX)
    o = PyInt_FromLong((long)(Cryptography_HAS_TLSEXT_HOSTNAME));
  else if ((Cryptography_HAS_TLSEXT_HOSTNAME) <= 0)
    o = PyLong_FromLongLong((long long)(Cryptography_HAS_TLSEXT_HOSTNAME));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(Cryptography_HAS_TLSEXT_HOSTNAME));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "Cryptography_HAS_TLSEXT_HOSTNAME", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_Cryptography_HAS_SSL_OP_MSIE_SSLV2_RSA_PADDING(lib);
}

static int _cffi_const_Cryptography_HAS_TLSv1_1(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (Cryptography_HAS_TLSv1_1) && (Cryptography_HAS_TLSv1_1) <= LONG_MAX)
    o = PyInt_FromLong((long)(Cryptography_HAS_TLSv1_1));
  else if ((Cryptography_HAS_TLSv1_1) <= 0)
    o = PyLong_FromLongLong((long long)(Cryptography_HAS_TLSv1_1));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(Cryptography_HAS_TLSv1_1));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "Cryptography_HAS_TLSv1_1", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_Cryptography_HAS_TLSEXT_HOSTNAME(lib);
}

static int _cffi_const_Cryptography_HAS_TLSv1_2(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (Cryptography_HAS_TLSv1_2) && (Cryptography_HAS_TLSv1_2) <= LONG_MAX)
    o = PyInt_FromLong((long)(Cryptography_HAS_TLSv1_2));
  else if ((Cryptography_HAS_TLSv1_2) <= 0)
    o = PyLong_FromLongLong((long long)(Cryptography_HAS_TLSv1_2));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(Cryptography_HAS_TLSv1_2));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "Cryptography_HAS_TLSv1_2", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_Cryptography_HAS_TLSv1_1(lib);
}

static int _cffi_const_Cryptography_osrandom_engine_id(PyObject *lib)
{
  PyObject *o;
  int res;
  char const * i;
  i = (Cryptography_osrandom_engine_id);
  o = _cffi_from_c_pointer((char *)i, _cffi_type(0));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "Cryptography_osrandom_engine_id", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_Cryptography_HAS_TLSv1_2(lib);
}

static int _cffi_const_Cryptography_osrandom_engine_name(PyObject *lib)
{
  PyObject *o;
  int res;
  char const * i;
  i = (Cryptography_osrandom_engine_name);
  o = _cffi_from_c_pointer((char *)i, _cffi_type(0));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "Cryptography_osrandom_engine_name", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_Cryptography_osrandom_engine_id(lib);
}

static int _cffi_const_ENGINE_METHOD_ALL(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (ENGINE_METHOD_ALL) && (ENGINE_METHOD_ALL) <= LONG_MAX)
    o = PyInt_FromLong((long)(ENGINE_METHOD_ALL));
  else if ((ENGINE_METHOD_ALL) <= 0)
    o = PyLong_FromLongLong((long long)(ENGINE_METHOD_ALL));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(ENGINE_METHOD_ALL));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "ENGINE_METHOD_ALL", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_Cryptography_osrandom_engine_name(lib);
}

static int _cffi_const_ENGINE_METHOD_CIPHERS(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (ENGINE_METHOD_CIPHERS) && (ENGINE_METHOD_CIPHERS) <= LONG_MAX)
    o = PyInt_FromLong((long)(ENGINE_METHOD_CIPHERS));
  else if ((ENGINE_METHOD_CIPHERS) <= 0)
    o = PyLong_FromLongLong((long long)(ENGINE_METHOD_CIPHERS));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(ENGINE_METHOD_CIPHERS));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "ENGINE_METHOD_CIPHERS", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_ENGINE_METHOD_ALL(lib);
}

static int _cffi_const_ENGINE_METHOD_DIGESTS(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (ENGINE_METHOD_DIGESTS) && (ENGINE_METHOD_DIGESTS) <= LONG_MAX)
    o = PyInt_FromLong((long)(ENGINE_METHOD_DIGESTS));
  else if ((ENGINE_METHOD_DIGESTS) <= 0)
    o = PyLong_FromLongLong((long long)(ENGINE_METHOD_DIGESTS));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(ENGINE_METHOD_DIGESTS));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "ENGINE_METHOD_DIGESTS", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_ENGINE_METHOD_CIPHERS(lib);
}

static int _cffi_const_ENGINE_METHOD_DSA(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (ENGINE_METHOD_DSA) && (ENGINE_METHOD_DSA) <= LONG_MAX)
    o = PyInt_FromLong((long)(ENGINE_METHOD_DSA));
  else if ((ENGINE_METHOD_DSA) <= 0)
    o = PyLong_FromLongLong((long long)(ENGINE_METHOD_DSA));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(ENGINE_METHOD_DSA));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "ENGINE_METHOD_DSA", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_ENGINE_METHOD_DIGESTS(lib);
}

static int _cffi_const_ENGINE_METHOD_ECDH(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (ENGINE_METHOD_ECDH) && (ENGINE_METHOD_ECDH) <= LONG_MAX)
    o = PyInt_FromLong((long)(ENGINE_METHOD_ECDH));
  else if ((ENGINE_METHOD_ECDH) <= 0)
    o = PyLong_FromLongLong((long long)(ENGINE_METHOD_ECDH));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(ENGINE_METHOD_ECDH));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "ENGINE_METHOD_ECDH", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_ENGINE_METHOD_DSA(lib);
}

static int _cffi_const_ENGINE_METHOD_ECDSA(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (ENGINE_METHOD_ECDSA) && (ENGINE_METHOD_ECDSA) <= LONG_MAX)
    o = PyInt_FromLong((long)(ENGINE_METHOD_ECDSA));
  else if ((ENGINE_METHOD_ECDSA) <= 0)
    o = PyLong_FromLongLong((long long)(ENGINE_METHOD_ECDSA));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(ENGINE_METHOD_ECDSA));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "ENGINE_METHOD_ECDSA", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_ENGINE_METHOD_ECDH(lib);
}

static int _cffi_const_ENGINE_METHOD_NONE(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (ENGINE_METHOD_NONE) && (ENGINE_METHOD_NONE) <= LONG_MAX)
    o = PyInt_FromLong((long)(ENGINE_METHOD_NONE));
  else if ((ENGINE_METHOD_NONE) <= 0)
    o = PyLong_FromLongLong((long long)(ENGINE_METHOD_NONE));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(ENGINE_METHOD_NONE));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "ENGINE_METHOD_NONE", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_ENGINE_METHOD_ECDSA(lib);
}

static int _cffi_const_ENGINE_METHOD_RAND(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (ENGINE_METHOD_RAND) && (ENGINE_METHOD_RAND) <= LONG_MAX)
    o = PyInt_FromLong((long)(ENGINE_METHOD_RAND));
  else if ((ENGINE_METHOD_RAND) <= 0)
    o = PyLong_FromLongLong((long long)(ENGINE_METHOD_RAND));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(ENGINE_METHOD_RAND));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "ENGINE_METHOD_RAND", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_ENGINE_METHOD_NONE(lib);
}

static int _cffi_const_ENGINE_METHOD_RSA(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (ENGINE_METHOD_RSA) && (ENGINE_METHOD_RSA) <= LONG_MAX)
    o = PyInt_FromLong((long)(ENGINE_METHOD_RSA));
  else if ((ENGINE_METHOD_RSA) <= 0)
    o = PyLong_FromLongLong((long long)(ENGINE_METHOD_RSA));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(ENGINE_METHOD_RSA));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "ENGINE_METHOD_RSA", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_ENGINE_METHOD_RAND(lib);
}

static int _cffi_const_ENGINE_METHOD_STORE(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (ENGINE_METHOD_STORE) && (ENGINE_METHOD_STORE) <= LONG_MAX)
    o = PyInt_FromLong((long)(ENGINE_METHOD_STORE));
  else if ((ENGINE_METHOD_STORE) <= 0)
    o = PyLong_FromLongLong((long long)(ENGINE_METHOD_STORE));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(ENGINE_METHOD_STORE));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "ENGINE_METHOD_STORE", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_ENGINE_METHOD_RSA(lib);
}

static int _cffi_const_ERR_LIB_ASN1(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (ERR_LIB_ASN1) && (ERR_LIB_ASN1) <= LONG_MAX)
    o = PyInt_FromLong((long)(ERR_LIB_ASN1));
  else if ((ERR_LIB_ASN1) <= 0)
    o = PyLong_FromLongLong((long long)(ERR_LIB_ASN1));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(ERR_LIB_ASN1));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "ERR_LIB_ASN1", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_ENGINE_METHOD_STORE(lib);
}

static int _cffi_const_ERR_LIB_EVP(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (ERR_LIB_EVP) && (ERR_LIB_EVP) <= LONG_MAX)
    o = PyInt_FromLong((long)(ERR_LIB_EVP));
  else if ((ERR_LIB_EVP) <= 0)
    o = PyLong_FromLongLong((long long)(ERR_LIB_EVP));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(ERR_LIB_EVP));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "ERR_LIB_EVP", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_ERR_LIB_ASN1(lib);
}

static int _cffi_const_ERR_LIB_PEM(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (ERR_LIB_PEM) && (ERR_LIB_PEM) <= LONG_MAX)
    o = PyInt_FromLong((long)(ERR_LIB_PEM));
  else if ((ERR_LIB_PEM) <= 0)
    o = PyLong_FromLongLong((long long)(ERR_LIB_PEM));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(ERR_LIB_PEM));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "ERR_LIB_PEM", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_ERR_LIB_EVP(lib);
}

static int _cffi_const_EVP_CTRL_GCM_GET_TAG(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (EVP_CTRL_GCM_GET_TAG) && (EVP_CTRL_GCM_GET_TAG) <= LONG_MAX)
    o = PyInt_FromLong((long)(EVP_CTRL_GCM_GET_TAG));
  else if ((EVP_CTRL_GCM_GET_TAG) <= 0)
    o = PyLong_FromLongLong((long long)(EVP_CTRL_GCM_GET_TAG));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(EVP_CTRL_GCM_GET_TAG));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "EVP_CTRL_GCM_GET_TAG", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_ERR_LIB_PEM(lib);
}

static int _cffi_const_EVP_CTRL_GCM_SET_IVLEN(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (EVP_CTRL_GCM_SET_IVLEN) && (EVP_CTRL_GCM_SET_IVLEN) <= LONG_MAX)
    o = PyInt_FromLong((long)(EVP_CTRL_GCM_SET_IVLEN));
  else if ((EVP_CTRL_GCM_SET_IVLEN) <= 0)
    o = PyLong_FromLongLong((long long)(EVP_CTRL_GCM_SET_IVLEN));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(EVP_CTRL_GCM_SET_IVLEN));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "EVP_CTRL_GCM_SET_IVLEN", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_EVP_CTRL_GCM_GET_TAG(lib);
}

static int _cffi_const_EVP_CTRL_GCM_SET_TAG(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (EVP_CTRL_GCM_SET_TAG) && (EVP_CTRL_GCM_SET_TAG) <= LONG_MAX)
    o = PyInt_FromLong((long)(EVP_CTRL_GCM_SET_TAG));
  else if ((EVP_CTRL_GCM_SET_TAG) <= 0)
    o = PyLong_FromLongLong((long long)(EVP_CTRL_GCM_SET_TAG));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(EVP_CTRL_GCM_SET_TAG));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "EVP_CTRL_GCM_SET_TAG", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_EVP_CTRL_GCM_SET_IVLEN(lib);
}

static int _cffi_const_EVP_F_AES_INIT_KEY(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (EVP_F_AES_INIT_KEY) && (EVP_F_AES_INIT_KEY) <= LONG_MAX)
    o = PyInt_FromLong((long)(EVP_F_AES_INIT_KEY));
  else if ((EVP_F_AES_INIT_KEY) <= 0)
    o = PyLong_FromLongLong((long long)(EVP_F_AES_INIT_KEY));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(EVP_F_AES_INIT_KEY));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "EVP_F_AES_INIT_KEY", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_EVP_CTRL_GCM_SET_TAG(lib);
}

static int _cffi_const_EVP_F_CAMELLIA_INIT_KEY(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (EVP_F_CAMELLIA_INIT_KEY) && (EVP_F_CAMELLIA_INIT_KEY) <= LONG_MAX)
    o = PyInt_FromLong((long)(EVP_F_CAMELLIA_INIT_KEY));
  else if ((EVP_F_CAMELLIA_INIT_KEY) <= 0)
    o = PyLong_FromLongLong((long long)(EVP_F_CAMELLIA_INIT_KEY));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(EVP_F_CAMELLIA_INIT_KEY));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "EVP_F_CAMELLIA_INIT_KEY", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_EVP_F_AES_INIT_KEY(lib);
}

static int _cffi_const_EVP_F_D2I_PKEY(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (EVP_F_D2I_PKEY) && (EVP_F_D2I_PKEY) <= LONG_MAX)
    o = PyInt_FromLong((long)(EVP_F_D2I_PKEY));
  else if ((EVP_F_D2I_PKEY) <= 0)
    o = PyLong_FromLongLong((long long)(EVP_F_D2I_PKEY));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(EVP_F_D2I_PKEY));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "EVP_F_D2I_PKEY", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_EVP_F_CAMELLIA_INIT_KEY(lib);
}

static int _cffi_const_EVP_F_DSAPKEY2PKCS8(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (EVP_F_DSAPKEY2PKCS8) && (EVP_F_DSAPKEY2PKCS8) <= LONG_MAX)
    o = PyInt_FromLong((long)(EVP_F_DSAPKEY2PKCS8));
  else if ((EVP_F_DSAPKEY2PKCS8) <= 0)
    o = PyLong_FromLongLong((long long)(EVP_F_DSAPKEY2PKCS8));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(EVP_F_DSAPKEY2PKCS8));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "EVP_F_DSAPKEY2PKCS8", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_EVP_F_D2I_PKEY(lib);
}

static int _cffi_const_EVP_F_DSA_PKEY2PKCS8(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (EVP_F_DSA_PKEY2PKCS8) && (EVP_F_DSA_PKEY2PKCS8) <= LONG_MAX)
    o = PyInt_FromLong((long)(EVP_F_DSA_PKEY2PKCS8));
  else if ((EVP_F_DSA_PKEY2PKCS8) <= 0)
    o = PyLong_FromLongLong((long long)(EVP_F_DSA_PKEY2PKCS8));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(EVP_F_DSA_PKEY2PKCS8));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "EVP_F_DSA_PKEY2PKCS8", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_EVP_F_DSAPKEY2PKCS8(lib);
}

static int _cffi_const_EVP_F_ECDSA_PKEY2PKCS8(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (EVP_F_ECDSA_PKEY2PKCS8) && (EVP_F_ECDSA_PKEY2PKCS8) <= LONG_MAX)
    o = PyInt_FromLong((long)(EVP_F_ECDSA_PKEY2PKCS8));
  else if ((EVP_F_ECDSA_PKEY2PKCS8) <= 0)
    o = PyLong_FromLongLong((long long)(EVP_F_ECDSA_PKEY2PKCS8));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(EVP_F_ECDSA_PKEY2PKCS8));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "EVP_F_ECDSA_PKEY2PKCS8", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_EVP_F_DSA_PKEY2PKCS8(lib);
}

static int _cffi_const_EVP_F_ECKEY_PKEY2PKCS8(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (EVP_F_ECKEY_PKEY2PKCS8) && (EVP_F_ECKEY_PKEY2PKCS8) <= LONG_MAX)
    o = PyInt_FromLong((long)(EVP_F_ECKEY_PKEY2PKCS8));
  else if ((EVP_F_ECKEY_PKEY2PKCS8) <= 0)
    o = PyLong_FromLongLong((long long)(EVP_F_ECKEY_PKEY2PKCS8));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(EVP_F_ECKEY_PKEY2PKCS8));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "EVP_F_ECKEY_PKEY2PKCS8", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_EVP_F_ECDSA_PKEY2PKCS8(lib);
}

static int _cffi_const_EVP_F_EVP_CIPHERINIT_EX(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (EVP_F_EVP_CIPHERINIT_EX) && (EVP_F_EVP_CIPHERINIT_EX) <= LONG_MAX)
    o = PyInt_FromLong((long)(EVP_F_EVP_CIPHERINIT_EX));
  else if ((EVP_F_EVP_CIPHERINIT_EX) <= 0)
    o = PyLong_FromLongLong((long long)(EVP_F_EVP_CIPHERINIT_EX));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(EVP_F_EVP_CIPHERINIT_EX));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "EVP_F_EVP_CIPHERINIT_EX", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_EVP_F_ECKEY_PKEY2PKCS8(lib);
}

static int _cffi_const_EVP_F_EVP_CIPHER_CTX_CTRL(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (EVP_F_EVP_CIPHER_CTX_CTRL) && (EVP_F_EVP_CIPHER_CTX_CTRL) <= LONG_MAX)
    o = PyInt_FromLong((long)(EVP_F_EVP_CIPHER_CTX_CTRL));
  else if ((EVP_F_EVP_CIPHER_CTX_CTRL) <= 0)
    o = PyLong_FromLongLong((long long)(EVP_F_EVP_CIPHER_CTX_CTRL));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(EVP_F_EVP_CIPHER_CTX_CTRL));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "EVP_F_EVP_CIPHER_CTX_CTRL", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_EVP_F_EVP_CIPHERINIT_EX(lib);
}

static int _cffi_const_EVP_F_EVP_CIPHER_CTX_SET_KEY_LENGTH(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (EVP_F_EVP_CIPHER_CTX_SET_KEY_LENGTH) && (EVP_F_EVP_CIPHER_CTX_SET_KEY_LENGTH) <= LONG_MAX)
    o = PyInt_FromLong((long)(EVP_F_EVP_CIPHER_CTX_SET_KEY_LENGTH));
  else if ((EVP_F_EVP_CIPHER_CTX_SET_KEY_LENGTH) <= 0)
    o = PyLong_FromLongLong((long long)(EVP_F_EVP_CIPHER_CTX_SET_KEY_LENGTH));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(EVP_F_EVP_CIPHER_CTX_SET_KEY_LENGTH));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "EVP_F_EVP_CIPHER_CTX_SET_KEY_LENGTH", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_EVP_F_EVP_CIPHER_CTX_CTRL(lib);
}

static int _cffi_const_EVP_F_EVP_DECRYPTFINAL_EX(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (EVP_F_EVP_DECRYPTFINAL_EX) && (EVP_F_EVP_DECRYPTFINAL_EX) <= LONG_MAX)
    o = PyInt_FromLong((long)(EVP_F_EVP_DECRYPTFINAL_EX));
  else if ((EVP_F_EVP_DECRYPTFINAL_EX) <= 0)
    o = PyLong_FromLongLong((long long)(EVP_F_EVP_DECRYPTFINAL_EX));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(EVP_F_EVP_DECRYPTFINAL_EX));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "EVP_F_EVP_DECRYPTFINAL_EX", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_EVP_F_EVP_CIPHER_CTX_SET_KEY_LENGTH(lib);
}

static int _cffi_const_EVP_F_EVP_DIGESTINIT_EX(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (EVP_F_EVP_DIGESTINIT_EX) && (EVP_F_EVP_DIGESTINIT_EX) <= LONG_MAX)
    o = PyInt_FromLong((long)(EVP_F_EVP_DIGESTINIT_EX));
  else if ((EVP_F_EVP_DIGESTINIT_EX) <= 0)
    o = PyLong_FromLongLong((long long)(EVP_F_EVP_DIGESTINIT_EX));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(EVP_F_EVP_DIGESTINIT_EX));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "EVP_F_EVP_DIGESTINIT_EX", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_EVP_F_EVP_DECRYPTFINAL_EX(lib);
}

static int _cffi_const_EVP_F_EVP_ENCRYPTFINAL_EX(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (EVP_F_EVP_ENCRYPTFINAL_EX) && (EVP_F_EVP_ENCRYPTFINAL_EX) <= LONG_MAX)
    o = PyInt_FromLong((long)(EVP_F_EVP_ENCRYPTFINAL_EX));
  else if ((EVP_F_EVP_ENCRYPTFINAL_EX) <= 0)
    o = PyLong_FromLongLong((long long)(EVP_F_EVP_ENCRYPTFINAL_EX));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(EVP_F_EVP_ENCRYPTFINAL_EX));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "EVP_F_EVP_ENCRYPTFINAL_EX", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_EVP_F_EVP_DIGESTINIT_EX(lib);
}

static int _cffi_const_EVP_F_EVP_MD_CTX_COPY_EX(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (EVP_F_EVP_MD_CTX_COPY_EX) && (EVP_F_EVP_MD_CTX_COPY_EX) <= LONG_MAX)
    o = PyInt_FromLong((long)(EVP_F_EVP_MD_CTX_COPY_EX));
  else if ((EVP_F_EVP_MD_CTX_COPY_EX) <= 0)
    o = PyLong_FromLongLong((long long)(EVP_F_EVP_MD_CTX_COPY_EX));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(EVP_F_EVP_MD_CTX_COPY_EX));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "EVP_F_EVP_MD_CTX_COPY_EX", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_EVP_F_EVP_ENCRYPTFINAL_EX(lib);
}

static int _cffi_const_EVP_F_EVP_OPENINIT(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (EVP_F_EVP_OPENINIT) && (EVP_F_EVP_OPENINIT) <= LONG_MAX)
    o = PyInt_FromLong((long)(EVP_F_EVP_OPENINIT));
  else if ((EVP_F_EVP_OPENINIT) <= 0)
    o = PyLong_FromLongLong((long long)(EVP_F_EVP_OPENINIT));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(EVP_F_EVP_OPENINIT));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "EVP_F_EVP_OPENINIT", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_EVP_F_EVP_MD_CTX_COPY_EX(lib);
}

static int _cffi_const_EVP_F_EVP_PBE_ALG_ADD(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (EVP_F_EVP_PBE_ALG_ADD) && (EVP_F_EVP_PBE_ALG_ADD) <= LONG_MAX)
    o = PyInt_FromLong((long)(EVP_F_EVP_PBE_ALG_ADD));
  else if ((EVP_F_EVP_PBE_ALG_ADD) <= 0)
    o = PyLong_FromLongLong((long long)(EVP_F_EVP_PBE_ALG_ADD));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(EVP_F_EVP_PBE_ALG_ADD));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "EVP_F_EVP_PBE_ALG_ADD", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_EVP_F_EVP_OPENINIT(lib);
}

static int _cffi_const_EVP_F_EVP_PBE_CIPHERINIT(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (EVP_F_EVP_PBE_CIPHERINIT) && (EVP_F_EVP_PBE_CIPHERINIT) <= LONG_MAX)
    o = PyInt_FromLong((long)(EVP_F_EVP_PBE_CIPHERINIT));
  else if ((EVP_F_EVP_PBE_CIPHERINIT) <= 0)
    o = PyLong_FromLongLong((long long)(EVP_F_EVP_PBE_CIPHERINIT));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(EVP_F_EVP_PBE_CIPHERINIT));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "EVP_F_EVP_PBE_CIPHERINIT", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_EVP_F_EVP_PBE_ALG_ADD(lib);
}

static int _cffi_const_EVP_F_EVP_PKCS82PKEY(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (EVP_F_EVP_PKCS82PKEY) && (EVP_F_EVP_PKCS82PKEY) <= LONG_MAX)
    o = PyInt_FromLong((long)(EVP_F_EVP_PKCS82PKEY));
  else if ((EVP_F_EVP_PKCS82PKEY) <= 0)
    o = PyLong_FromLongLong((long long)(EVP_F_EVP_PKCS82PKEY));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(EVP_F_EVP_PKCS82PKEY));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "EVP_F_EVP_PKCS82PKEY", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_EVP_F_EVP_PBE_CIPHERINIT(lib);
}

static int _cffi_const_EVP_F_EVP_PKEY2PKCS8_BROKEN(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (EVP_F_EVP_PKEY2PKCS8_BROKEN) && (EVP_F_EVP_PKEY2PKCS8_BROKEN) <= LONG_MAX)
    o = PyInt_FromLong((long)(EVP_F_EVP_PKEY2PKCS8_BROKEN));
  else if ((EVP_F_EVP_PKEY2PKCS8_BROKEN) <= 0)
    o = PyLong_FromLongLong((long long)(EVP_F_EVP_PKEY2PKCS8_BROKEN));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(EVP_F_EVP_PKEY2PKCS8_BROKEN));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "EVP_F_EVP_PKEY2PKCS8_BROKEN", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_EVP_F_EVP_PKCS82PKEY(lib);
}

static int _cffi_const_EVP_F_EVP_PKEY_COPY_PARAMETERS(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (EVP_F_EVP_PKEY_COPY_PARAMETERS) && (EVP_F_EVP_PKEY_COPY_PARAMETERS) <= LONG_MAX)
    o = PyInt_FromLong((long)(EVP_F_EVP_PKEY_COPY_PARAMETERS));
  else if ((EVP_F_EVP_PKEY_COPY_PARAMETERS) <= 0)
    o = PyLong_FromLongLong((long long)(EVP_F_EVP_PKEY_COPY_PARAMETERS));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(EVP_F_EVP_PKEY_COPY_PARAMETERS));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "EVP_F_EVP_PKEY_COPY_PARAMETERS", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_EVP_F_EVP_PKEY2PKCS8_BROKEN(lib);
}

static int _cffi_const_EVP_F_EVP_PKEY_DECRYPT(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (EVP_F_EVP_PKEY_DECRYPT) && (EVP_F_EVP_PKEY_DECRYPT) <= LONG_MAX)
    o = PyInt_FromLong((long)(EVP_F_EVP_PKEY_DECRYPT));
  else if ((EVP_F_EVP_PKEY_DECRYPT) <= 0)
    o = PyLong_FromLongLong((long long)(EVP_F_EVP_PKEY_DECRYPT));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(EVP_F_EVP_PKEY_DECRYPT));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "EVP_F_EVP_PKEY_DECRYPT", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_EVP_F_EVP_PKEY_COPY_PARAMETERS(lib);
}

static int _cffi_const_EVP_F_EVP_PKEY_ENCRYPT(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (EVP_F_EVP_PKEY_ENCRYPT) && (EVP_F_EVP_PKEY_ENCRYPT) <= LONG_MAX)
    o = PyInt_FromLong((long)(EVP_F_EVP_PKEY_ENCRYPT));
  else if ((EVP_F_EVP_PKEY_ENCRYPT) <= 0)
    o = PyLong_FromLongLong((long long)(EVP_F_EVP_PKEY_ENCRYPT));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(EVP_F_EVP_PKEY_ENCRYPT));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "EVP_F_EVP_PKEY_ENCRYPT", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_EVP_F_EVP_PKEY_DECRYPT(lib);
}

static int _cffi_const_EVP_F_EVP_PKEY_GET1_DH(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (EVP_F_EVP_PKEY_GET1_DH) && (EVP_F_EVP_PKEY_GET1_DH) <= LONG_MAX)
    o = PyInt_FromLong((long)(EVP_F_EVP_PKEY_GET1_DH));
  else if ((EVP_F_EVP_PKEY_GET1_DH) <= 0)
    o = PyLong_FromLongLong((long long)(EVP_F_EVP_PKEY_GET1_DH));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(EVP_F_EVP_PKEY_GET1_DH));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "EVP_F_EVP_PKEY_GET1_DH", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_EVP_F_EVP_PKEY_ENCRYPT(lib);
}

static int _cffi_const_EVP_F_EVP_PKEY_GET1_DSA(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (EVP_F_EVP_PKEY_GET1_DSA) && (EVP_F_EVP_PKEY_GET1_DSA) <= LONG_MAX)
    o = PyInt_FromLong((long)(EVP_F_EVP_PKEY_GET1_DSA));
  else if ((EVP_F_EVP_PKEY_GET1_DSA) <= 0)
    o = PyLong_FromLongLong((long long)(EVP_F_EVP_PKEY_GET1_DSA));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(EVP_F_EVP_PKEY_GET1_DSA));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "EVP_F_EVP_PKEY_GET1_DSA", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_EVP_F_EVP_PKEY_GET1_DH(lib);
}

static int _cffi_const_EVP_F_EVP_PKEY_GET1_ECDSA(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (EVP_F_EVP_PKEY_GET1_ECDSA) && (EVP_F_EVP_PKEY_GET1_ECDSA) <= LONG_MAX)
    o = PyInt_FromLong((long)(EVP_F_EVP_PKEY_GET1_ECDSA));
  else if ((EVP_F_EVP_PKEY_GET1_ECDSA) <= 0)
    o = PyLong_FromLongLong((long long)(EVP_F_EVP_PKEY_GET1_ECDSA));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(EVP_F_EVP_PKEY_GET1_ECDSA));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "EVP_F_EVP_PKEY_GET1_ECDSA", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_EVP_F_EVP_PKEY_GET1_DSA(lib);
}

static int _cffi_const_EVP_F_EVP_PKEY_GET1_EC_KEY(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (EVP_F_EVP_PKEY_GET1_EC_KEY) && (EVP_F_EVP_PKEY_GET1_EC_KEY) <= LONG_MAX)
    o = PyInt_FromLong((long)(EVP_F_EVP_PKEY_GET1_EC_KEY));
  else if ((EVP_F_EVP_PKEY_GET1_EC_KEY) <= 0)
    o = PyLong_FromLongLong((long long)(EVP_F_EVP_PKEY_GET1_EC_KEY));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(EVP_F_EVP_PKEY_GET1_EC_KEY));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "EVP_F_EVP_PKEY_GET1_EC_KEY", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_EVP_F_EVP_PKEY_GET1_ECDSA(lib);
}

static int _cffi_const_EVP_F_EVP_PKEY_GET1_RSA(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (EVP_F_EVP_PKEY_GET1_RSA) && (EVP_F_EVP_PKEY_GET1_RSA) <= LONG_MAX)
    o = PyInt_FromLong((long)(EVP_F_EVP_PKEY_GET1_RSA));
  else if ((EVP_F_EVP_PKEY_GET1_RSA) <= 0)
    o = PyLong_FromLongLong((long long)(EVP_F_EVP_PKEY_GET1_RSA));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(EVP_F_EVP_PKEY_GET1_RSA));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "EVP_F_EVP_PKEY_GET1_RSA", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_EVP_F_EVP_PKEY_GET1_EC_KEY(lib);
}

static int _cffi_const_EVP_F_EVP_PKEY_NEW(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (EVP_F_EVP_PKEY_NEW) && (EVP_F_EVP_PKEY_NEW) <= LONG_MAX)
    o = PyInt_FromLong((long)(EVP_F_EVP_PKEY_NEW));
  else if ((EVP_F_EVP_PKEY_NEW) <= 0)
    o = PyLong_FromLongLong((long long)(EVP_F_EVP_PKEY_NEW));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(EVP_F_EVP_PKEY_NEW));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "EVP_F_EVP_PKEY_NEW", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_EVP_F_EVP_PKEY_GET1_RSA(lib);
}

static int _cffi_const_EVP_F_EVP_RIJNDAEL(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (EVP_F_EVP_RIJNDAEL) && (EVP_F_EVP_RIJNDAEL) <= LONG_MAX)
    o = PyInt_FromLong((long)(EVP_F_EVP_RIJNDAEL));
  else if ((EVP_F_EVP_RIJNDAEL) <= 0)
    o = PyLong_FromLongLong((long long)(EVP_F_EVP_RIJNDAEL));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(EVP_F_EVP_RIJNDAEL));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "EVP_F_EVP_RIJNDAEL", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_EVP_F_EVP_PKEY_NEW(lib);
}

static int _cffi_const_EVP_F_EVP_SIGNFINAL(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (EVP_F_EVP_SIGNFINAL) && (EVP_F_EVP_SIGNFINAL) <= LONG_MAX)
    o = PyInt_FromLong((long)(EVP_F_EVP_SIGNFINAL));
  else if ((EVP_F_EVP_SIGNFINAL) <= 0)
    o = PyLong_FromLongLong((long long)(EVP_F_EVP_SIGNFINAL));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(EVP_F_EVP_SIGNFINAL));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "EVP_F_EVP_SIGNFINAL", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_EVP_F_EVP_RIJNDAEL(lib);
}

static int _cffi_const_EVP_F_EVP_VERIFYFINAL(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (EVP_F_EVP_VERIFYFINAL) && (EVP_F_EVP_VERIFYFINAL) <= LONG_MAX)
    o = PyInt_FromLong((long)(EVP_F_EVP_VERIFYFINAL));
  else if ((EVP_F_EVP_VERIFYFINAL) <= 0)
    o = PyLong_FromLongLong((long long)(EVP_F_EVP_VERIFYFINAL));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(EVP_F_EVP_VERIFYFINAL));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "EVP_F_EVP_VERIFYFINAL", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_EVP_F_EVP_SIGNFINAL(lib);
}

static int _cffi_const_EVP_F_PKCS5_PBE_KEYIVGEN(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (EVP_F_PKCS5_PBE_KEYIVGEN) && (EVP_F_PKCS5_PBE_KEYIVGEN) <= LONG_MAX)
    o = PyInt_FromLong((long)(EVP_F_PKCS5_PBE_KEYIVGEN));
  else if ((EVP_F_PKCS5_PBE_KEYIVGEN) <= 0)
    o = PyLong_FromLongLong((long long)(EVP_F_PKCS5_PBE_KEYIVGEN));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(EVP_F_PKCS5_PBE_KEYIVGEN));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "EVP_F_PKCS5_PBE_KEYIVGEN", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_EVP_F_EVP_VERIFYFINAL(lib);
}

static int _cffi_const_EVP_F_PKCS5_V2_PBE_KEYIVGEN(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (EVP_F_PKCS5_V2_PBE_KEYIVGEN) && (EVP_F_PKCS5_V2_PBE_KEYIVGEN) <= LONG_MAX)
    o = PyInt_FromLong((long)(EVP_F_PKCS5_V2_PBE_KEYIVGEN));
  else if ((EVP_F_PKCS5_V2_PBE_KEYIVGEN) <= 0)
    o = PyLong_FromLongLong((long long)(EVP_F_PKCS5_V2_PBE_KEYIVGEN));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(EVP_F_PKCS5_V2_PBE_KEYIVGEN));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "EVP_F_PKCS5_V2_PBE_KEYIVGEN", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_EVP_F_PKCS5_PBE_KEYIVGEN(lib);
}

static int _cffi_const_EVP_F_PKCS8_SET_BROKEN(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (EVP_F_PKCS8_SET_BROKEN) && (EVP_F_PKCS8_SET_BROKEN) <= LONG_MAX)
    o = PyInt_FromLong((long)(EVP_F_PKCS8_SET_BROKEN));
  else if ((EVP_F_PKCS8_SET_BROKEN) <= 0)
    o = PyLong_FromLongLong((long long)(EVP_F_PKCS8_SET_BROKEN));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(EVP_F_PKCS8_SET_BROKEN));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "EVP_F_PKCS8_SET_BROKEN", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_EVP_F_PKCS5_V2_PBE_KEYIVGEN(lib);
}

static int _cffi_const_EVP_F_RC2_MAGIC_TO_METH(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (EVP_F_RC2_MAGIC_TO_METH) && (EVP_F_RC2_MAGIC_TO_METH) <= LONG_MAX)
    o = PyInt_FromLong((long)(EVP_F_RC2_MAGIC_TO_METH));
  else if ((EVP_F_RC2_MAGIC_TO_METH) <= 0)
    o = PyLong_FromLongLong((long long)(EVP_F_RC2_MAGIC_TO_METH));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(EVP_F_RC2_MAGIC_TO_METH));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "EVP_F_RC2_MAGIC_TO_METH", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_EVP_F_PKCS8_SET_BROKEN(lib);
}

static int _cffi_const_EVP_F_RC5_CTRL(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (EVP_F_RC5_CTRL) && (EVP_F_RC5_CTRL) <= LONG_MAX)
    o = PyInt_FromLong((long)(EVP_F_RC5_CTRL));
  else if ((EVP_F_RC5_CTRL) <= 0)
    o = PyLong_FromLongLong((long long)(EVP_F_RC5_CTRL));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(EVP_F_RC5_CTRL));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "EVP_F_RC5_CTRL", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_EVP_F_RC2_MAGIC_TO_METH(lib);
}

static int _cffi_const_EVP_MAX_MD_SIZE(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (EVP_MAX_MD_SIZE) && (EVP_MAX_MD_SIZE) <= LONG_MAX)
    o = PyInt_FromLong((long)(EVP_MAX_MD_SIZE));
  else if ((EVP_MAX_MD_SIZE) <= 0)
    o = PyLong_FromLongLong((long long)(EVP_MAX_MD_SIZE));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(EVP_MAX_MD_SIZE));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "EVP_MAX_MD_SIZE", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_EVP_F_RC5_CTRL(lib);
}

static int _cffi_const_EVP_PKEY_DSA(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (EVP_PKEY_DSA) && (EVP_PKEY_DSA) <= LONG_MAX)
    o = PyInt_FromLong((long)(EVP_PKEY_DSA));
  else if ((EVP_PKEY_DSA) <= 0)
    o = PyLong_FromLongLong((long long)(EVP_PKEY_DSA));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(EVP_PKEY_DSA));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "EVP_PKEY_DSA", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_EVP_MAX_MD_SIZE(lib);
}

static int _cffi_const_EVP_PKEY_RSA(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (EVP_PKEY_RSA) && (EVP_PKEY_RSA) <= LONG_MAX)
    o = PyInt_FromLong((long)(EVP_PKEY_RSA));
  else if ((EVP_PKEY_RSA) <= 0)
    o = PyLong_FromLongLong((long long)(EVP_PKEY_RSA));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(EVP_PKEY_RSA));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "EVP_PKEY_RSA", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_EVP_PKEY_DSA(lib);
}

static int _cffi_const_EVP_R_AES_KEY_SETUP_FAILED(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (EVP_R_AES_KEY_SETUP_FAILED) && (EVP_R_AES_KEY_SETUP_FAILED) <= LONG_MAX)
    o = PyInt_FromLong((long)(EVP_R_AES_KEY_SETUP_FAILED));
  else if ((EVP_R_AES_KEY_SETUP_FAILED) <= 0)
    o = PyLong_FromLongLong((long long)(EVP_R_AES_KEY_SETUP_FAILED));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(EVP_R_AES_KEY_SETUP_FAILED));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "EVP_R_AES_KEY_SETUP_FAILED", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_EVP_PKEY_RSA(lib);
}

static int _cffi_const_EVP_R_ASN1_LIB(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (EVP_R_ASN1_LIB) && (EVP_R_ASN1_LIB) <= LONG_MAX)
    o = PyInt_FromLong((long)(EVP_R_ASN1_LIB));
  else if ((EVP_R_ASN1_LIB) <= 0)
    o = PyLong_FromLongLong((long long)(EVP_R_ASN1_LIB));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(EVP_R_ASN1_LIB));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "EVP_R_ASN1_LIB", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_EVP_R_AES_KEY_SETUP_FAILED(lib);
}

static int _cffi_const_EVP_R_BAD_BLOCK_LENGTH(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (EVP_R_BAD_BLOCK_LENGTH) && (EVP_R_BAD_BLOCK_LENGTH) <= LONG_MAX)
    o = PyInt_FromLong((long)(EVP_R_BAD_BLOCK_LENGTH));
  else if ((EVP_R_BAD_BLOCK_LENGTH) <= 0)
    o = PyLong_FromLongLong((long long)(EVP_R_BAD_BLOCK_LENGTH));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(EVP_R_BAD_BLOCK_LENGTH));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "EVP_R_BAD_BLOCK_LENGTH", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_EVP_R_ASN1_LIB(lib);
}

static int _cffi_const_EVP_R_BAD_KEY_LENGTH(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (EVP_R_BAD_KEY_LENGTH) && (EVP_R_BAD_KEY_LENGTH) <= LONG_MAX)
    o = PyInt_FromLong((long)(EVP_R_BAD_KEY_LENGTH));
  else if ((EVP_R_BAD_KEY_LENGTH) <= 0)
    o = PyLong_FromLongLong((long long)(EVP_R_BAD_KEY_LENGTH));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(EVP_R_BAD_KEY_LENGTH));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "EVP_R_BAD_KEY_LENGTH", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_EVP_R_BAD_BLOCK_LENGTH(lib);
}

static int _cffi_const_EVP_R_BN_DECODE_ERROR(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (EVP_R_BN_DECODE_ERROR) && (EVP_R_BN_DECODE_ERROR) <= LONG_MAX)
    o = PyInt_FromLong((long)(EVP_R_BN_DECODE_ERROR));
  else if ((EVP_R_BN_DECODE_ERROR) <= 0)
    o = PyLong_FromLongLong((long long)(EVP_R_BN_DECODE_ERROR));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(EVP_R_BN_DECODE_ERROR));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "EVP_R_BN_DECODE_ERROR", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_EVP_R_BAD_KEY_LENGTH(lib);
}

static int _cffi_const_EVP_R_BN_PUBKEY_ERROR(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (EVP_R_BN_PUBKEY_ERROR) && (EVP_R_BN_PUBKEY_ERROR) <= LONG_MAX)
    o = PyInt_FromLong((long)(EVP_R_BN_PUBKEY_ERROR));
  else if ((EVP_R_BN_PUBKEY_ERROR) <= 0)
    o = PyLong_FromLongLong((long long)(EVP_R_BN_PUBKEY_ERROR));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(EVP_R_BN_PUBKEY_ERROR));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "EVP_R_BN_PUBKEY_ERROR", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_EVP_R_BN_DECODE_ERROR(lib);
}

static int _cffi_const_EVP_R_CAMELLIA_KEY_SETUP_FAILED(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (EVP_R_CAMELLIA_KEY_SETUP_FAILED) && (EVP_R_CAMELLIA_KEY_SETUP_FAILED) <= LONG_MAX)
    o = PyInt_FromLong((long)(EVP_R_CAMELLIA_KEY_SETUP_FAILED));
  else if ((EVP_R_CAMELLIA_KEY_SETUP_FAILED) <= 0)
    o = PyLong_FromLongLong((long long)(EVP_R_CAMELLIA_KEY_SETUP_FAILED));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(EVP_R_CAMELLIA_KEY_SETUP_FAILED));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "EVP_R_CAMELLIA_KEY_SETUP_FAILED", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_EVP_R_BN_PUBKEY_ERROR(lib);
}

static int _cffi_const_EVP_R_CIPHER_PARAMETER_ERROR(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (EVP_R_CIPHER_PARAMETER_ERROR) && (EVP_R_CIPHER_PARAMETER_ERROR) <= LONG_MAX)
    o = PyInt_FromLong((long)(EVP_R_CIPHER_PARAMETER_ERROR));
  else if ((EVP_R_CIPHER_PARAMETER_ERROR) <= 0)
    o = PyLong_FromLongLong((long long)(EVP_R_CIPHER_PARAMETER_ERROR));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(EVP_R_CIPHER_PARAMETER_ERROR));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "EVP_R_CIPHER_PARAMETER_ERROR", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_EVP_R_CAMELLIA_KEY_SETUP_FAILED(lib);
}

static int _cffi_const_EVP_R_CTRL_NOT_IMPLEMENTED(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (EVP_R_CTRL_NOT_IMPLEMENTED) && (EVP_R_CTRL_NOT_IMPLEMENTED) <= LONG_MAX)
    o = PyInt_FromLong((long)(EVP_R_CTRL_NOT_IMPLEMENTED));
  else if ((EVP_R_CTRL_NOT_IMPLEMENTED) <= 0)
    o = PyLong_FromLongLong((long long)(EVP_R_CTRL_NOT_IMPLEMENTED));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(EVP_R_CTRL_NOT_IMPLEMENTED));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "EVP_R_CTRL_NOT_IMPLEMENTED", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_EVP_R_CIPHER_PARAMETER_ERROR(lib);
}

static int _cffi_const_EVP_R_CTRL_OPERATION_NOT_IMPLEMENTED(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (EVP_R_CTRL_OPERATION_NOT_IMPLEMENTED) && (EVP_R_CTRL_OPERATION_NOT_IMPLEMENTED) <= LONG_MAX)
    o = PyInt_FromLong((long)(EVP_R_CTRL_OPERATION_NOT_IMPLEMENTED));
  else if ((EVP_R_CTRL_OPERATION_NOT_IMPLEMENTED) <= 0)
    o = PyLong_FromLongLong((long long)(EVP_R_CTRL_OPERATION_NOT_IMPLEMENTED));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(EVP_R_CTRL_OPERATION_NOT_IMPLEMENTED));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "EVP_R_CTRL_OPERATION_NOT_IMPLEMENTED", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_EVP_R_CTRL_NOT_IMPLEMENTED(lib);
}

static int _cffi_const_EVP_R_DATA_NOT_MULTIPLE_OF_BLOCK_LENGTH(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (EVP_R_DATA_NOT_MULTIPLE_OF_BLOCK_LENGTH) && (EVP_R_DATA_NOT_MULTIPLE_OF_BLOCK_LENGTH) <= LONG_MAX)
    o = PyInt_FromLong((long)(EVP_R_DATA_NOT_MULTIPLE_OF_BLOCK_LENGTH));
  else if ((EVP_R_DATA_NOT_MULTIPLE_OF_BLOCK_LENGTH) <= 0)
    o = PyLong_FromLongLong((long long)(EVP_R_DATA_NOT_MULTIPLE_OF_BLOCK_LENGTH));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(EVP_R_DATA_NOT_MULTIPLE_OF_BLOCK_LENGTH));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "EVP_R_DATA_NOT_MULTIPLE_OF_BLOCK_LENGTH", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_EVP_R_CTRL_OPERATION_NOT_IMPLEMENTED(lib);
}

static int _cffi_const_EVP_R_DECODE_ERROR(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (EVP_R_DECODE_ERROR) && (EVP_R_DECODE_ERROR) <= LONG_MAX)
    o = PyInt_FromLong((long)(EVP_R_DECODE_ERROR));
  else if ((EVP_R_DECODE_ERROR) <= 0)
    o = PyLong_FromLongLong((long long)(EVP_R_DECODE_ERROR));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(EVP_R_DECODE_ERROR));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "EVP_R_DECODE_ERROR", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_EVP_R_DATA_NOT_MULTIPLE_OF_BLOCK_LENGTH(lib);
}

static int _cffi_const_EVP_R_DIFFERENT_KEY_TYPES(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (EVP_R_DIFFERENT_KEY_TYPES) && (EVP_R_DIFFERENT_KEY_TYPES) <= LONG_MAX)
    o = PyInt_FromLong((long)(EVP_R_DIFFERENT_KEY_TYPES));
  else if ((EVP_R_DIFFERENT_KEY_TYPES) <= 0)
    o = PyLong_FromLongLong((long long)(EVP_R_DIFFERENT_KEY_TYPES));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(EVP_R_DIFFERENT_KEY_TYPES));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "EVP_R_DIFFERENT_KEY_TYPES", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_EVP_R_DECODE_ERROR(lib);
}

static int _cffi_const_EVP_R_ENCODE_ERROR(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (EVP_R_ENCODE_ERROR) && (EVP_R_ENCODE_ERROR) <= LONG_MAX)
    o = PyInt_FromLong((long)(EVP_R_ENCODE_ERROR));
  else if ((EVP_R_ENCODE_ERROR) <= 0)
    o = PyLong_FromLongLong((long long)(EVP_R_ENCODE_ERROR));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(EVP_R_ENCODE_ERROR));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "EVP_R_ENCODE_ERROR", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_EVP_R_DIFFERENT_KEY_TYPES(lib);
}

static int _cffi_const_EVP_R_INITIALIZATION_ERROR(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (EVP_R_INITIALIZATION_ERROR) && (EVP_R_INITIALIZATION_ERROR) <= LONG_MAX)
    o = PyInt_FromLong((long)(EVP_R_INITIALIZATION_ERROR));
  else if ((EVP_R_INITIALIZATION_ERROR) <= 0)
    o = PyLong_FromLongLong((long long)(EVP_R_INITIALIZATION_ERROR));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(EVP_R_INITIALIZATION_ERROR));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "EVP_R_INITIALIZATION_ERROR", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_EVP_R_ENCODE_ERROR(lib);
}

static int _cffi_const_EVP_R_INPUT_NOT_INITIALIZED(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (EVP_R_INPUT_NOT_INITIALIZED) && (EVP_R_INPUT_NOT_INITIALIZED) <= LONG_MAX)
    o = PyInt_FromLong((long)(EVP_R_INPUT_NOT_INITIALIZED));
  else if ((EVP_R_INPUT_NOT_INITIALIZED) <= 0)
    o = PyLong_FromLongLong((long long)(EVP_R_INPUT_NOT_INITIALIZED));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(EVP_R_INPUT_NOT_INITIALIZED));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "EVP_R_INPUT_NOT_INITIALIZED", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_EVP_R_INITIALIZATION_ERROR(lib);
}

static int _cffi_const_EVP_R_INVALID_KEY_LENGTH(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (EVP_R_INVALID_KEY_LENGTH) && (EVP_R_INVALID_KEY_LENGTH) <= LONG_MAX)
    o = PyInt_FromLong((long)(EVP_R_INVALID_KEY_LENGTH));
  else if ((EVP_R_INVALID_KEY_LENGTH) <= 0)
    o = PyLong_FromLongLong((long long)(EVP_R_INVALID_KEY_LENGTH));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(EVP_R_INVALID_KEY_LENGTH));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "EVP_R_INVALID_KEY_LENGTH", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_EVP_R_INPUT_NOT_INITIALIZED(lib);
}

static int _cffi_const_EVP_R_IV_TOO_LARGE(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (EVP_R_IV_TOO_LARGE) && (EVP_R_IV_TOO_LARGE) <= LONG_MAX)
    o = PyInt_FromLong((long)(EVP_R_IV_TOO_LARGE));
  else if ((EVP_R_IV_TOO_LARGE) <= 0)
    o = PyLong_FromLongLong((long long)(EVP_R_IV_TOO_LARGE));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(EVP_R_IV_TOO_LARGE));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "EVP_R_IV_TOO_LARGE", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_EVP_R_INVALID_KEY_LENGTH(lib);
}

static int _cffi_const_EVP_R_KEYGEN_FAILURE(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (EVP_R_KEYGEN_FAILURE) && (EVP_R_KEYGEN_FAILURE) <= LONG_MAX)
    o = PyInt_FromLong((long)(EVP_R_KEYGEN_FAILURE));
  else if ((EVP_R_KEYGEN_FAILURE) <= 0)
    o = PyLong_FromLongLong((long long)(EVP_R_KEYGEN_FAILURE));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(EVP_R_KEYGEN_FAILURE));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "EVP_R_KEYGEN_FAILURE", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_EVP_R_IV_TOO_LARGE(lib);
}

static int _cffi_const_EVP_R_MISSING_PARAMETERS(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (EVP_R_MISSING_PARAMETERS) && (EVP_R_MISSING_PARAMETERS) <= LONG_MAX)
    o = PyInt_FromLong((long)(EVP_R_MISSING_PARAMETERS));
  else if ((EVP_R_MISSING_PARAMETERS) <= 0)
    o = PyLong_FromLongLong((long long)(EVP_R_MISSING_PARAMETERS));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(EVP_R_MISSING_PARAMETERS));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "EVP_R_MISSING_PARAMETERS", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_EVP_R_KEYGEN_FAILURE(lib);
}

static int _cffi_const_EVP_R_NO_CIPHER_SET(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (EVP_R_NO_CIPHER_SET) && (EVP_R_NO_CIPHER_SET) <= LONG_MAX)
    o = PyInt_FromLong((long)(EVP_R_NO_CIPHER_SET));
  else if ((EVP_R_NO_CIPHER_SET) <= 0)
    o = PyLong_FromLongLong((long long)(EVP_R_NO_CIPHER_SET));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(EVP_R_NO_CIPHER_SET));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "EVP_R_NO_CIPHER_SET", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_EVP_R_MISSING_PARAMETERS(lib);
}

static int _cffi_const_EVP_R_NO_DIGEST_SET(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (EVP_R_NO_DIGEST_SET) && (EVP_R_NO_DIGEST_SET) <= LONG_MAX)
    o = PyInt_FromLong((long)(EVP_R_NO_DIGEST_SET));
  else if ((EVP_R_NO_DIGEST_SET) <= 0)
    o = PyLong_FromLongLong((long long)(EVP_R_NO_DIGEST_SET));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(EVP_R_NO_DIGEST_SET));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "EVP_R_NO_DIGEST_SET", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_EVP_R_NO_CIPHER_SET(lib);
}

static int _cffi_const_EVP_R_NO_DSA_PARAMETERS(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (EVP_R_NO_DSA_PARAMETERS) && (EVP_R_NO_DSA_PARAMETERS) <= LONG_MAX)
    o = PyInt_FromLong((long)(EVP_R_NO_DSA_PARAMETERS));
  else if ((EVP_R_NO_DSA_PARAMETERS) <= 0)
    o = PyLong_FromLongLong((long long)(EVP_R_NO_DSA_PARAMETERS));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(EVP_R_NO_DSA_PARAMETERS));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "EVP_R_NO_DSA_PARAMETERS", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_EVP_R_NO_DIGEST_SET(lib);
}

static int _cffi_const_EVP_R_NO_SIGN_FUNCTION_CONFIGURED(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (EVP_R_NO_SIGN_FUNCTION_CONFIGURED) && (EVP_R_NO_SIGN_FUNCTION_CONFIGURED) <= LONG_MAX)
    o = PyInt_FromLong((long)(EVP_R_NO_SIGN_FUNCTION_CONFIGURED));
  else if ((EVP_R_NO_SIGN_FUNCTION_CONFIGURED) <= 0)
    o = PyLong_FromLongLong((long long)(EVP_R_NO_SIGN_FUNCTION_CONFIGURED));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(EVP_R_NO_SIGN_FUNCTION_CONFIGURED));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "EVP_R_NO_SIGN_FUNCTION_CONFIGURED", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_EVP_R_NO_DSA_PARAMETERS(lib);
}

static int _cffi_const_EVP_R_NO_VERIFY_FUNCTION_CONFIGURED(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (EVP_R_NO_VERIFY_FUNCTION_CONFIGURED) && (EVP_R_NO_VERIFY_FUNCTION_CONFIGURED) <= LONG_MAX)
    o = PyInt_FromLong((long)(EVP_R_NO_VERIFY_FUNCTION_CONFIGURED));
  else if ((EVP_R_NO_VERIFY_FUNCTION_CONFIGURED) <= 0)
    o = PyLong_FromLongLong((long long)(EVP_R_NO_VERIFY_FUNCTION_CONFIGURED));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(EVP_R_NO_VERIFY_FUNCTION_CONFIGURED));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "EVP_R_NO_VERIFY_FUNCTION_CONFIGURED", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_EVP_R_NO_SIGN_FUNCTION_CONFIGURED(lib);
}

static int _cffi_const_EVP_R_PKCS8_UNKNOWN_BROKEN_TYPE(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (EVP_R_PKCS8_UNKNOWN_BROKEN_TYPE) && (EVP_R_PKCS8_UNKNOWN_BROKEN_TYPE) <= LONG_MAX)
    o = PyInt_FromLong((long)(EVP_R_PKCS8_UNKNOWN_BROKEN_TYPE));
  else if ((EVP_R_PKCS8_UNKNOWN_BROKEN_TYPE) <= 0)
    o = PyLong_FromLongLong((long long)(EVP_R_PKCS8_UNKNOWN_BROKEN_TYPE));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(EVP_R_PKCS8_UNKNOWN_BROKEN_TYPE));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "EVP_R_PKCS8_UNKNOWN_BROKEN_TYPE", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_EVP_R_NO_VERIFY_FUNCTION_CONFIGURED(lib);
}

static int _cffi_const_EVP_R_PUBLIC_KEY_NOT_RSA(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (EVP_R_PUBLIC_KEY_NOT_RSA) && (EVP_R_PUBLIC_KEY_NOT_RSA) <= LONG_MAX)
    o = PyInt_FromLong((long)(EVP_R_PUBLIC_KEY_NOT_RSA));
  else if ((EVP_R_PUBLIC_KEY_NOT_RSA) <= 0)
    o = PyLong_FromLongLong((long long)(EVP_R_PUBLIC_KEY_NOT_RSA));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(EVP_R_PUBLIC_KEY_NOT_RSA));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "EVP_R_PUBLIC_KEY_NOT_RSA", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_EVP_R_PKCS8_UNKNOWN_BROKEN_TYPE(lib);
}

static int _cffi_const_EVP_R_UNKNOWN_PBE_ALGORITHM(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (EVP_R_UNKNOWN_PBE_ALGORITHM) && (EVP_R_UNKNOWN_PBE_ALGORITHM) <= LONG_MAX)
    o = PyInt_FromLong((long)(EVP_R_UNKNOWN_PBE_ALGORITHM));
  else if ((EVP_R_UNKNOWN_PBE_ALGORITHM) <= 0)
    o = PyLong_FromLongLong((long long)(EVP_R_UNKNOWN_PBE_ALGORITHM));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(EVP_R_UNKNOWN_PBE_ALGORITHM));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "EVP_R_UNKNOWN_PBE_ALGORITHM", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_EVP_R_PUBLIC_KEY_NOT_RSA(lib);
}

static int _cffi_const_EVP_R_UNSUPORTED_NUMBER_OF_ROUNDS(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (EVP_R_UNSUPORTED_NUMBER_OF_ROUNDS) && (EVP_R_UNSUPORTED_NUMBER_OF_ROUNDS) <= LONG_MAX)
    o = PyInt_FromLong((long)(EVP_R_UNSUPORTED_NUMBER_OF_ROUNDS));
  else if ((EVP_R_UNSUPORTED_NUMBER_OF_ROUNDS) <= 0)
    o = PyLong_FromLongLong((long long)(EVP_R_UNSUPORTED_NUMBER_OF_ROUNDS));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(EVP_R_UNSUPORTED_NUMBER_OF_ROUNDS));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "EVP_R_UNSUPORTED_NUMBER_OF_ROUNDS", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_EVP_R_UNKNOWN_PBE_ALGORITHM(lib);
}

static int _cffi_const_EVP_R_UNSUPPORTED_CIPHER(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (EVP_R_UNSUPPORTED_CIPHER) && (EVP_R_UNSUPPORTED_CIPHER) <= LONG_MAX)
    o = PyInt_FromLong((long)(EVP_R_UNSUPPORTED_CIPHER));
  else if ((EVP_R_UNSUPPORTED_CIPHER) <= 0)
    o = PyLong_FromLongLong((long long)(EVP_R_UNSUPPORTED_CIPHER));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(EVP_R_UNSUPPORTED_CIPHER));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "EVP_R_UNSUPPORTED_CIPHER", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_EVP_R_UNSUPORTED_NUMBER_OF_ROUNDS(lib);
}

static int _cffi_const_EVP_R_UNSUPPORTED_KEYLENGTH(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (EVP_R_UNSUPPORTED_KEYLENGTH) && (EVP_R_UNSUPPORTED_KEYLENGTH) <= LONG_MAX)
    o = PyInt_FromLong((long)(EVP_R_UNSUPPORTED_KEYLENGTH));
  else if ((EVP_R_UNSUPPORTED_KEYLENGTH) <= 0)
    o = PyLong_FromLongLong((long long)(EVP_R_UNSUPPORTED_KEYLENGTH));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(EVP_R_UNSUPPORTED_KEYLENGTH));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "EVP_R_UNSUPPORTED_KEYLENGTH", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_EVP_R_UNSUPPORTED_CIPHER(lib);
}

static int _cffi_const_EVP_R_UNSUPPORTED_KEY_DERIVATION_FUNCTION(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (EVP_R_UNSUPPORTED_KEY_DERIVATION_FUNCTION) && (EVP_R_UNSUPPORTED_KEY_DERIVATION_FUNCTION) <= LONG_MAX)
    o = PyInt_FromLong((long)(EVP_R_UNSUPPORTED_KEY_DERIVATION_FUNCTION));
  else if ((EVP_R_UNSUPPORTED_KEY_DERIVATION_FUNCTION) <= 0)
    o = PyLong_FromLongLong((long long)(EVP_R_UNSUPPORTED_KEY_DERIVATION_FUNCTION));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(EVP_R_UNSUPPORTED_KEY_DERIVATION_FUNCTION));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "EVP_R_UNSUPPORTED_KEY_DERIVATION_FUNCTION", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_EVP_R_UNSUPPORTED_KEYLENGTH(lib);
}

static int _cffi_const_EVP_R_UNSUPPORTED_SALT_TYPE(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (EVP_R_UNSUPPORTED_SALT_TYPE) && (EVP_R_UNSUPPORTED_SALT_TYPE) <= LONG_MAX)
    o = PyInt_FromLong((long)(EVP_R_UNSUPPORTED_SALT_TYPE));
  else if ((EVP_R_UNSUPPORTED_SALT_TYPE) <= 0)
    o = PyLong_FromLongLong((long long)(EVP_R_UNSUPPORTED_SALT_TYPE));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(EVP_R_UNSUPPORTED_SALT_TYPE));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "EVP_R_UNSUPPORTED_SALT_TYPE", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_EVP_R_UNSUPPORTED_KEY_DERIVATION_FUNCTION(lib);
}

static int _cffi_const_EVP_R_WRONG_FINAL_BLOCK_LENGTH(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (EVP_R_WRONG_FINAL_BLOCK_LENGTH) && (EVP_R_WRONG_FINAL_BLOCK_LENGTH) <= LONG_MAX)
    o = PyInt_FromLong((long)(EVP_R_WRONG_FINAL_BLOCK_LENGTH));
  else if ((EVP_R_WRONG_FINAL_BLOCK_LENGTH) <= 0)
    o = PyLong_FromLongLong((long long)(EVP_R_WRONG_FINAL_BLOCK_LENGTH));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(EVP_R_WRONG_FINAL_BLOCK_LENGTH));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "EVP_R_WRONG_FINAL_BLOCK_LENGTH", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_EVP_R_UNSUPPORTED_SALT_TYPE(lib);
}

static int _cffi_const_EVP_R_WRONG_PUBLIC_KEY_TYPE(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (EVP_R_WRONG_PUBLIC_KEY_TYPE) && (EVP_R_WRONG_PUBLIC_KEY_TYPE) <= LONG_MAX)
    o = PyInt_FromLong((long)(EVP_R_WRONG_PUBLIC_KEY_TYPE));
  else if ((EVP_R_WRONG_PUBLIC_KEY_TYPE) <= 0)
    o = PyLong_FromLongLong((long long)(EVP_R_WRONG_PUBLIC_KEY_TYPE));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(EVP_R_WRONG_PUBLIC_KEY_TYPE));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "EVP_R_WRONG_PUBLIC_KEY_TYPE", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_EVP_R_WRONG_FINAL_BLOCK_LENGTH(lib);
}

static int _cffi_const_GEN_DIRNAME(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (GEN_DIRNAME) && (GEN_DIRNAME) <= LONG_MAX)
    o = PyInt_FromLong((long)(GEN_DIRNAME));
  else if ((GEN_DIRNAME) <= 0)
    o = PyLong_FromLongLong((long long)(GEN_DIRNAME));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(GEN_DIRNAME));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "GEN_DIRNAME", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_EVP_R_WRONG_PUBLIC_KEY_TYPE(lib);
}

static int _cffi_const_GEN_DNS(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (GEN_DNS) && (GEN_DNS) <= LONG_MAX)
    o = PyInt_FromLong((long)(GEN_DNS));
  else if ((GEN_DNS) <= 0)
    o = PyLong_FromLongLong((long long)(GEN_DNS));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(GEN_DNS));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "GEN_DNS", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_GEN_DIRNAME(lib);
}

static int _cffi_const_GEN_EDIPARTY(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (GEN_EDIPARTY) && (GEN_EDIPARTY) <= LONG_MAX)
    o = PyInt_FromLong((long)(GEN_EDIPARTY));
  else if ((GEN_EDIPARTY) <= 0)
    o = PyLong_FromLongLong((long long)(GEN_EDIPARTY));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(GEN_EDIPARTY));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "GEN_EDIPARTY", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_GEN_DNS(lib);
}

static int _cffi_const_GEN_EMAIL(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (GEN_EMAIL) && (GEN_EMAIL) <= LONG_MAX)
    o = PyInt_FromLong((long)(GEN_EMAIL));
  else if ((GEN_EMAIL) <= 0)
    o = PyLong_FromLongLong((long long)(GEN_EMAIL));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(GEN_EMAIL));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "GEN_EMAIL", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_GEN_EDIPARTY(lib);
}

static int _cffi_const_GEN_IPADD(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (GEN_IPADD) && (GEN_IPADD) <= LONG_MAX)
    o = PyInt_FromLong((long)(GEN_IPADD));
  else if ((GEN_IPADD) <= 0)
    o = PyLong_FromLongLong((long long)(GEN_IPADD));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(GEN_IPADD));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "GEN_IPADD", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_GEN_EMAIL(lib);
}

static int _cffi_const_GEN_OTHERNAME(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (GEN_OTHERNAME) && (GEN_OTHERNAME) <= LONG_MAX)
    o = PyInt_FromLong((long)(GEN_OTHERNAME));
  else if ((GEN_OTHERNAME) <= 0)
    o = PyLong_FromLongLong((long long)(GEN_OTHERNAME));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(GEN_OTHERNAME));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "GEN_OTHERNAME", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_GEN_IPADD(lib);
}

static int _cffi_const_GEN_RID(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (GEN_RID) && (GEN_RID) <= LONG_MAX)
    o = PyInt_FromLong((long)(GEN_RID));
  else if ((GEN_RID) <= 0)
    o = PyLong_FromLongLong((long long)(GEN_RID));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(GEN_RID));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "GEN_RID", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_GEN_OTHERNAME(lib);
}

static int _cffi_const_GEN_URI(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (GEN_URI) && (GEN_URI) <= LONG_MAX)
    o = PyInt_FromLong((long)(GEN_URI));
  else if ((GEN_URI) <= 0)
    o = PyLong_FromLongLong((long long)(GEN_URI));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(GEN_URI));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "GEN_URI", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_GEN_RID(lib);
}

static int _cffi_const_GEN_X400(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (GEN_X400) && (GEN_X400) <= LONG_MAX)
    o = PyInt_FromLong((long)(GEN_X400));
  else if ((GEN_X400) <= 0)
    o = PyLong_FromLongLong((long long)(GEN_X400));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(GEN_X400));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "GEN_X400", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_GEN_URI(lib);
}

static int _cffi_const_MBSTRING_UTF8(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (MBSTRING_UTF8) && (MBSTRING_UTF8) <= LONG_MAX)
    o = PyInt_FromLong((long)(MBSTRING_UTF8));
  else if ((MBSTRING_UTF8) <= 0)
    o = PyLong_FromLongLong((long long)(MBSTRING_UTF8));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(MBSTRING_UTF8));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "MBSTRING_UTF8", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_GEN_X400(lib);
}

static int _cffi_const_NID_X9_62_prime192v1(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (NID_X9_62_prime192v1) && (NID_X9_62_prime192v1) <= LONG_MAX)
    o = PyInt_FromLong((long)(NID_X9_62_prime192v1));
  else if ((NID_X9_62_prime192v1) <= 0)
    o = PyLong_FromLongLong((long long)(NID_X9_62_prime192v1));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(NID_X9_62_prime192v1));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "NID_X9_62_prime192v1", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_MBSTRING_UTF8(lib);
}

static int _cffi_const_NID_X9_62_prime192v2(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (NID_X9_62_prime192v2) && (NID_X9_62_prime192v2) <= LONG_MAX)
    o = PyInt_FromLong((long)(NID_X9_62_prime192v2));
  else if ((NID_X9_62_prime192v2) <= 0)
    o = PyLong_FromLongLong((long long)(NID_X9_62_prime192v2));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(NID_X9_62_prime192v2));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "NID_X9_62_prime192v2", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_NID_X9_62_prime192v1(lib);
}

static int _cffi_const_NID_X9_62_prime192v3(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (NID_X9_62_prime192v3) && (NID_X9_62_prime192v3) <= LONG_MAX)
    o = PyInt_FromLong((long)(NID_X9_62_prime192v3));
  else if ((NID_X9_62_prime192v3) <= 0)
    o = PyLong_FromLongLong((long long)(NID_X9_62_prime192v3));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(NID_X9_62_prime192v3));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "NID_X9_62_prime192v3", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_NID_X9_62_prime192v2(lib);
}

static int _cffi_const_NID_X9_62_prime239v1(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (NID_X9_62_prime239v1) && (NID_X9_62_prime239v1) <= LONG_MAX)
    o = PyInt_FromLong((long)(NID_X9_62_prime239v1));
  else if ((NID_X9_62_prime239v1) <= 0)
    o = PyLong_FromLongLong((long long)(NID_X9_62_prime239v1));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(NID_X9_62_prime239v1));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "NID_X9_62_prime239v1", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_NID_X9_62_prime192v3(lib);
}

static int _cffi_const_NID_X9_62_prime239v2(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (NID_X9_62_prime239v2) && (NID_X9_62_prime239v2) <= LONG_MAX)
    o = PyInt_FromLong((long)(NID_X9_62_prime239v2));
  else if ((NID_X9_62_prime239v2) <= 0)
    o = PyLong_FromLongLong((long long)(NID_X9_62_prime239v2));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(NID_X9_62_prime239v2));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "NID_X9_62_prime239v2", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_NID_X9_62_prime239v1(lib);
}

static int _cffi_const_NID_X9_62_prime239v3(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (NID_X9_62_prime239v3) && (NID_X9_62_prime239v3) <= LONG_MAX)
    o = PyInt_FromLong((long)(NID_X9_62_prime239v3));
  else if ((NID_X9_62_prime239v3) <= 0)
    o = PyLong_FromLongLong((long long)(NID_X9_62_prime239v3));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(NID_X9_62_prime239v3));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "NID_X9_62_prime239v3", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_NID_X9_62_prime239v2(lib);
}

static int _cffi_const_NID_X9_62_prime256v1(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (NID_X9_62_prime256v1) && (NID_X9_62_prime256v1) <= LONG_MAX)
    o = PyInt_FromLong((long)(NID_X9_62_prime256v1));
  else if ((NID_X9_62_prime256v1) <= 0)
    o = PyLong_FromLongLong((long long)(NID_X9_62_prime256v1));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(NID_X9_62_prime256v1));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "NID_X9_62_prime256v1", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_NID_X9_62_prime239v3(lib);
}

static int _cffi_const_NID_crl_reason(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (NID_crl_reason) && (NID_crl_reason) <= LONG_MAX)
    o = PyInt_FromLong((long)(NID_crl_reason));
  else if ((NID_crl_reason) <= 0)
    o = PyLong_FromLongLong((long long)(NID_crl_reason));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(NID_crl_reason));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "NID_crl_reason", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_NID_X9_62_prime256v1(lib);
}

static int _cffi_const_NID_dsa(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (NID_dsa) && (NID_dsa) <= LONG_MAX)
    o = PyInt_FromLong((long)(NID_dsa));
  else if ((NID_dsa) <= 0)
    o = PyLong_FromLongLong((long long)(NID_dsa));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(NID_dsa));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "NID_dsa", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_NID_crl_reason(lib);
}

static int _cffi_const_NID_dsaWithSHA(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (NID_dsaWithSHA) && (NID_dsaWithSHA) <= LONG_MAX)
    o = PyInt_FromLong((long)(NID_dsaWithSHA));
  else if ((NID_dsaWithSHA) <= 0)
    o = PyLong_FromLongLong((long long)(NID_dsaWithSHA));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(NID_dsaWithSHA));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "NID_dsaWithSHA", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_NID_dsa(lib);
}

static int _cffi_const_NID_dsaWithSHA1(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (NID_dsaWithSHA1) && (NID_dsaWithSHA1) <= LONG_MAX)
    o = PyInt_FromLong((long)(NID_dsaWithSHA1));
  else if ((NID_dsaWithSHA1) <= 0)
    o = PyLong_FromLongLong((long long)(NID_dsaWithSHA1));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(NID_dsaWithSHA1));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "NID_dsaWithSHA1", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_NID_dsaWithSHA(lib);
}

static int _cffi_const_NID_ecdsa_with_SHA1(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (NID_ecdsa_with_SHA1) && (NID_ecdsa_with_SHA1) <= LONG_MAX)
    o = PyInt_FromLong((long)(NID_ecdsa_with_SHA1));
  else if ((NID_ecdsa_with_SHA1) <= 0)
    o = PyLong_FromLongLong((long long)(NID_ecdsa_with_SHA1));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(NID_ecdsa_with_SHA1));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "NID_ecdsa_with_SHA1", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_NID_dsaWithSHA1(lib);
}

static int _cffi_const_NID_ecdsa_with_SHA224(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (NID_ecdsa_with_SHA224) && (NID_ecdsa_with_SHA224) <= LONG_MAX)
    o = PyInt_FromLong((long)(NID_ecdsa_with_SHA224));
  else if ((NID_ecdsa_with_SHA224) <= 0)
    o = PyLong_FromLongLong((long long)(NID_ecdsa_with_SHA224));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(NID_ecdsa_with_SHA224));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "NID_ecdsa_with_SHA224", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_NID_ecdsa_with_SHA1(lib);
}

static int _cffi_const_NID_ecdsa_with_SHA256(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (NID_ecdsa_with_SHA256) && (NID_ecdsa_with_SHA256) <= LONG_MAX)
    o = PyInt_FromLong((long)(NID_ecdsa_with_SHA256));
  else if ((NID_ecdsa_with_SHA256) <= 0)
    o = PyLong_FromLongLong((long long)(NID_ecdsa_with_SHA256));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(NID_ecdsa_with_SHA256));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "NID_ecdsa_with_SHA256", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_NID_ecdsa_with_SHA224(lib);
}

static int _cffi_const_NID_ecdsa_with_SHA384(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (NID_ecdsa_with_SHA384) && (NID_ecdsa_with_SHA384) <= LONG_MAX)
    o = PyInt_FromLong((long)(NID_ecdsa_with_SHA384));
  else if ((NID_ecdsa_with_SHA384) <= 0)
    o = PyLong_FromLongLong((long long)(NID_ecdsa_with_SHA384));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(NID_ecdsa_with_SHA384));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "NID_ecdsa_with_SHA384", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_NID_ecdsa_with_SHA256(lib);
}

static int _cffi_const_NID_ecdsa_with_SHA512(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (NID_ecdsa_with_SHA512) && (NID_ecdsa_with_SHA512) <= LONG_MAX)
    o = PyInt_FromLong((long)(NID_ecdsa_with_SHA512));
  else if ((NID_ecdsa_with_SHA512) <= 0)
    o = PyLong_FromLongLong((long long)(NID_ecdsa_with_SHA512));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(NID_ecdsa_with_SHA512));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "NID_ecdsa_with_SHA512", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_NID_ecdsa_with_SHA384(lib);
}

static int _cffi_const_NID_md2(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (NID_md2) && (NID_md2) <= LONG_MAX)
    o = PyInt_FromLong((long)(NID_md2));
  else if ((NID_md2) <= 0)
    o = PyLong_FromLongLong((long long)(NID_md2));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(NID_md2));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "NID_md2", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_NID_ecdsa_with_SHA512(lib);
}

static int _cffi_const_NID_md4(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (NID_md4) && (NID_md4) <= LONG_MAX)
    o = PyInt_FromLong((long)(NID_md4));
  else if ((NID_md4) <= 0)
    o = PyLong_FromLongLong((long long)(NID_md4));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(NID_md4));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "NID_md4", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_NID_md2(lib);
}

static int _cffi_const_NID_md5(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (NID_md5) && (NID_md5) <= LONG_MAX)
    o = PyInt_FromLong((long)(NID_md5));
  else if ((NID_md5) <= 0)
    o = PyLong_FromLongLong((long long)(NID_md5));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(NID_md5));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "NID_md5", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_NID_md4(lib);
}

static int _cffi_const_NID_mdc2(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (NID_mdc2) && (NID_mdc2) <= LONG_MAX)
    o = PyInt_FromLong((long)(NID_mdc2));
  else if ((NID_mdc2) <= 0)
    o = PyLong_FromLongLong((long long)(NID_mdc2));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(NID_mdc2));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "NID_mdc2", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_NID_md5(lib);
}

static int _cffi_const_NID_pbe_WithSHA1And3_Key_TripleDES_CBC(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (NID_pbe_WithSHA1And3_Key_TripleDES_CBC) && (NID_pbe_WithSHA1And3_Key_TripleDES_CBC) <= LONG_MAX)
    o = PyInt_FromLong((long)(NID_pbe_WithSHA1And3_Key_TripleDES_CBC));
  else if ((NID_pbe_WithSHA1And3_Key_TripleDES_CBC) <= 0)
    o = PyLong_FromLongLong((long long)(NID_pbe_WithSHA1And3_Key_TripleDES_CBC));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(NID_pbe_WithSHA1And3_Key_TripleDES_CBC));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "NID_pbe_WithSHA1And3_Key_TripleDES_CBC", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_NID_mdc2(lib);
}

static int _cffi_const_NID_ripemd160(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (NID_ripemd160) && (NID_ripemd160) <= LONG_MAX)
    o = PyInt_FromLong((long)(NID_ripemd160));
  else if ((NID_ripemd160) <= 0)
    o = PyLong_FromLongLong((long long)(NID_ripemd160));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(NID_ripemd160));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "NID_ripemd160", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_NID_pbe_WithSHA1And3_Key_TripleDES_CBC(lib);
}

static int _cffi_const_NID_sha(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (NID_sha) && (NID_sha) <= LONG_MAX)
    o = PyInt_FromLong((long)(NID_sha));
  else if ((NID_sha) <= 0)
    o = PyLong_FromLongLong((long long)(NID_sha));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(NID_sha));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "NID_sha", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_NID_ripemd160(lib);
}

static int _cffi_const_NID_sha1(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (NID_sha1) && (NID_sha1) <= LONG_MAX)
    o = PyInt_FromLong((long)(NID_sha1));
  else if ((NID_sha1) <= 0)
    o = PyLong_FromLongLong((long long)(NID_sha1));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(NID_sha1));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "NID_sha1", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_NID_sha(lib);
}

static int _cffi_const_NID_sha224(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (NID_sha224) && (NID_sha224) <= LONG_MAX)
    o = PyInt_FromLong((long)(NID_sha224));
  else if ((NID_sha224) <= 0)
    o = PyLong_FromLongLong((long long)(NID_sha224));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(NID_sha224));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "NID_sha224", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_NID_sha1(lib);
}

static int _cffi_const_NID_sha256(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (NID_sha256) && (NID_sha256) <= LONG_MAX)
    o = PyInt_FromLong((long)(NID_sha256));
  else if ((NID_sha256) <= 0)
    o = PyLong_FromLongLong((long long)(NID_sha256));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(NID_sha256));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "NID_sha256", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_NID_sha224(lib);
}

static int _cffi_const_NID_sha384(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (NID_sha384) && (NID_sha384) <= LONG_MAX)
    o = PyInt_FromLong((long)(NID_sha384));
  else if ((NID_sha384) <= 0)
    o = PyLong_FromLongLong((long long)(NID_sha384));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(NID_sha384));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "NID_sha384", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_NID_sha256(lib);
}

static int _cffi_const_NID_sha512(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (NID_sha512) && (NID_sha512) <= LONG_MAX)
    o = PyInt_FromLong((long)(NID_sha512));
  else if ((NID_sha512) <= 0)
    o = PyLong_FromLongLong((long long)(NID_sha512));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(NID_sha512));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "NID_sha512", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_NID_sha384(lib);
}

static int _cffi_const_NID_subject_alt_name(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (NID_subject_alt_name) && (NID_subject_alt_name) <= LONG_MAX)
    o = PyInt_FromLong((long)(NID_subject_alt_name));
  else if ((NID_subject_alt_name) <= 0)
    o = PyLong_FromLongLong((long long)(NID_subject_alt_name));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(NID_subject_alt_name));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "NID_subject_alt_name", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_NID_sha512(lib);
}

static int _cffi_const_NID_undef(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (NID_undef) && (NID_undef) <= LONG_MAX)
    o = PyInt_FromLong((long)(NID_undef));
  else if ((NID_undef) <= 0)
    o = PyLong_FromLongLong((long long)(NID_undef));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(NID_undef));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "NID_undef", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_NID_subject_alt_name(lib);
}

static int _cffi_const_OPENSSL_VERSION_NUMBER(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (OPENSSL_VERSION_NUMBER) && (OPENSSL_VERSION_NUMBER) <= LONG_MAX)
    o = PyInt_FromLong((long)(OPENSSL_VERSION_NUMBER));
  else if ((OPENSSL_VERSION_NUMBER) <= 0)
    o = PyLong_FromLongLong((long long)(OPENSSL_VERSION_NUMBER));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(OPENSSL_VERSION_NUMBER));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "OPENSSL_VERSION_NUMBER", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_NID_undef(lib);
}

static int _cffi_const_OPENSSL_VERSION_TEXT(PyObject *lib)
{
  PyObject *o;
  int res;
  char const * i;
  i = (OPENSSL_VERSION_TEXT);
  o = _cffi_from_c_pointer((char *)i, _cffi_type(0));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "OPENSSL_VERSION_TEXT", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_OPENSSL_VERSION_NUMBER(lib);
}

static int _cffi_const_PEM_F_D2I_PKCS8PRIVATEKEY_BIO(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (PEM_F_D2I_PKCS8PRIVATEKEY_BIO) && (PEM_F_D2I_PKCS8PRIVATEKEY_BIO) <= LONG_MAX)
    o = PyInt_FromLong((long)(PEM_F_D2I_PKCS8PRIVATEKEY_BIO));
  else if ((PEM_F_D2I_PKCS8PRIVATEKEY_BIO) <= 0)
    o = PyLong_FromLongLong((long long)(PEM_F_D2I_PKCS8PRIVATEKEY_BIO));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(PEM_F_D2I_PKCS8PRIVATEKEY_BIO));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "PEM_F_D2I_PKCS8PRIVATEKEY_BIO", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_OPENSSL_VERSION_TEXT(lib);
}

static int _cffi_const_PEM_F_D2I_PKCS8PRIVATEKEY_FP(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (PEM_F_D2I_PKCS8PRIVATEKEY_FP) && (PEM_F_D2I_PKCS8PRIVATEKEY_FP) <= LONG_MAX)
    o = PyInt_FromLong((long)(PEM_F_D2I_PKCS8PRIVATEKEY_FP));
  else if ((PEM_F_D2I_PKCS8PRIVATEKEY_FP) <= 0)
    o = PyLong_FromLongLong((long long)(PEM_F_D2I_PKCS8PRIVATEKEY_FP));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(PEM_F_D2I_PKCS8PRIVATEKEY_FP));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "PEM_F_D2I_PKCS8PRIVATEKEY_FP", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_PEM_F_D2I_PKCS8PRIVATEKEY_BIO(lib);
}

static int _cffi_const_PEM_F_DO_PK8PKEY(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (PEM_F_DO_PK8PKEY) && (PEM_F_DO_PK8PKEY) <= LONG_MAX)
    o = PyInt_FromLong((long)(PEM_F_DO_PK8PKEY));
  else if ((PEM_F_DO_PK8PKEY) <= 0)
    o = PyLong_FromLongLong((long long)(PEM_F_DO_PK8PKEY));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(PEM_F_DO_PK8PKEY));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "PEM_F_DO_PK8PKEY", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_PEM_F_D2I_PKCS8PRIVATEKEY_FP(lib);
}

static int _cffi_const_PEM_F_DO_PK8PKEY_FP(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (PEM_F_DO_PK8PKEY_FP) && (PEM_F_DO_PK8PKEY_FP) <= LONG_MAX)
    o = PyInt_FromLong((long)(PEM_F_DO_PK8PKEY_FP));
  else if ((PEM_F_DO_PK8PKEY_FP) <= 0)
    o = PyLong_FromLongLong((long long)(PEM_F_DO_PK8PKEY_FP));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(PEM_F_DO_PK8PKEY_FP));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "PEM_F_DO_PK8PKEY_FP", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_PEM_F_DO_PK8PKEY(lib);
}

static int _cffi_const_PEM_F_LOAD_IV(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (PEM_F_LOAD_IV) && (PEM_F_LOAD_IV) <= LONG_MAX)
    o = PyInt_FromLong((long)(PEM_F_LOAD_IV));
  else if ((PEM_F_LOAD_IV) <= 0)
    o = PyLong_FromLongLong((long long)(PEM_F_LOAD_IV));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(PEM_F_LOAD_IV));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "PEM_F_LOAD_IV", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_PEM_F_DO_PK8PKEY_FP(lib);
}

static int _cffi_const_PEM_F_PEM_ASN1_READ(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (PEM_F_PEM_ASN1_READ) && (PEM_F_PEM_ASN1_READ) <= LONG_MAX)
    o = PyInt_FromLong((long)(PEM_F_PEM_ASN1_READ));
  else if ((PEM_F_PEM_ASN1_READ) <= 0)
    o = PyLong_FromLongLong((long long)(PEM_F_PEM_ASN1_READ));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(PEM_F_PEM_ASN1_READ));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "PEM_F_PEM_ASN1_READ", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_PEM_F_LOAD_IV(lib);
}

static int _cffi_const_PEM_F_PEM_ASN1_READ_BIO(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (PEM_F_PEM_ASN1_READ_BIO) && (PEM_F_PEM_ASN1_READ_BIO) <= LONG_MAX)
    o = PyInt_FromLong((long)(PEM_F_PEM_ASN1_READ_BIO));
  else if ((PEM_F_PEM_ASN1_READ_BIO) <= 0)
    o = PyLong_FromLongLong((long long)(PEM_F_PEM_ASN1_READ_BIO));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(PEM_F_PEM_ASN1_READ_BIO));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "PEM_F_PEM_ASN1_READ_BIO", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_PEM_F_PEM_ASN1_READ(lib);
}

static int _cffi_const_PEM_F_PEM_ASN1_WRITE(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (PEM_F_PEM_ASN1_WRITE) && (PEM_F_PEM_ASN1_WRITE) <= LONG_MAX)
    o = PyInt_FromLong((long)(PEM_F_PEM_ASN1_WRITE));
  else if ((PEM_F_PEM_ASN1_WRITE) <= 0)
    o = PyLong_FromLongLong((long long)(PEM_F_PEM_ASN1_WRITE));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(PEM_F_PEM_ASN1_WRITE));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "PEM_F_PEM_ASN1_WRITE", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_PEM_F_PEM_ASN1_READ_BIO(lib);
}

static int _cffi_const_PEM_F_PEM_ASN1_WRITE_BIO(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (PEM_F_PEM_ASN1_WRITE_BIO) && (PEM_F_PEM_ASN1_WRITE_BIO) <= LONG_MAX)
    o = PyInt_FromLong((long)(PEM_F_PEM_ASN1_WRITE_BIO));
  else if ((PEM_F_PEM_ASN1_WRITE_BIO) <= 0)
    o = PyLong_FromLongLong((long long)(PEM_F_PEM_ASN1_WRITE_BIO));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(PEM_F_PEM_ASN1_WRITE_BIO));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "PEM_F_PEM_ASN1_WRITE_BIO", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_PEM_F_PEM_ASN1_WRITE(lib);
}

static int _cffi_const_PEM_F_PEM_DEF_CALLBACK(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (PEM_F_PEM_DEF_CALLBACK) && (PEM_F_PEM_DEF_CALLBACK) <= LONG_MAX)
    o = PyInt_FromLong((long)(PEM_F_PEM_DEF_CALLBACK));
  else if ((PEM_F_PEM_DEF_CALLBACK) <= 0)
    o = PyLong_FromLongLong((long long)(PEM_F_PEM_DEF_CALLBACK));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(PEM_F_PEM_DEF_CALLBACK));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "PEM_F_PEM_DEF_CALLBACK", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_PEM_F_PEM_ASN1_WRITE_BIO(lib);
}

static int _cffi_const_PEM_F_PEM_DO_HEADER(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (PEM_F_PEM_DO_HEADER) && (PEM_F_PEM_DO_HEADER) <= LONG_MAX)
    o = PyInt_FromLong((long)(PEM_F_PEM_DO_HEADER));
  else if ((PEM_F_PEM_DO_HEADER) <= 0)
    o = PyLong_FromLongLong((long long)(PEM_F_PEM_DO_HEADER));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(PEM_F_PEM_DO_HEADER));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "PEM_F_PEM_DO_HEADER", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_PEM_F_PEM_DEF_CALLBACK(lib);
}

static int _cffi_const_PEM_F_PEM_F_PEM_WRITE_PKCS8PRIVATEKEY(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (PEM_F_PEM_F_PEM_WRITE_PKCS8PRIVATEKEY) && (PEM_F_PEM_F_PEM_WRITE_PKCS8PRIVATEKEY) <= LONG_MAX)
    o = PyInt_FromLong((long)(PEM_F_PEM_F_PEM_WRITE_PKCS8PRIVATEKEY));
  else if ((PEM_F_PEM_F_PEM_WRITE_PKCS8PRIVATEKEY) <= 0)
    o = PyLong_FromLongLong((long long)(PEM_F_PEM_F_PEM_WRITE_PKCS8PRIVATEKEY));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(PEM_F_PEM_F_PEM_WRITE_PKCS8PRIVATEKEY));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "PEM_F_PEM_F_PEM_WRITE_PKCS8PRIVATEKEY", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_PEM_F_PEM_DO_HEADER(lib);
}

static int _cffi_const_PEM_F_PEM_GET_EVP_CIPHER_INFO(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (PEM_F_PEM_GET_EVP_CIPHER_INFO) && (PEM_F_PEM_GET_EVP_CIPHER_INFO) <= LONG_MAX)
    o = PyInt_FromLong((long)(PEM_F_PEM_GET_EVP_CIPHER_INFO));
  else if ((PEM_F_PEM_GET_EVP_CIPHER_INFO) <= 0)
    o = PyLong_FromLongLong((long long)(PEM_F_PEM_GET_EVP_CIPHER_INFO));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(PEM_F_PEM_GET_EVP_CIPHER_INFO));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "PEM_F_PEM_GET_EVP_CIPHER_INFO", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_PEM_F_PEM_F_PEM_WRITE_PKCS8PRIVATEKEY(lib);
}

static int _cffi_const_PEM_F_PEM_PK8PKEY(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (PEM_F_PEM_PK8PKEY) && (PEM_F_PEM_PK8PKEY) <= LONG_MAX)
    o = PyInt_FromLong((long)(PEM_F_PEM_PK8PKEY));
  else if ((PEM_F_PEM_PK8PKEY) <= 0)
    o = PyLong_FromLongLong((long long)(PEM_F_PEM_PK8PKEY));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(PEM_F_PEM_PK8PKEY));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "PEM_F_PEM_PK8PKEY", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_PEM_F_PEM_GET_EVP_CIPHER_INFO(lib);
}

static int _cffi_const_PEM_F_PEM_READ(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (PEM_F_PEM_READ) && (PEM_F_PEM_READ) <= LONG_MAX)
    o = PyInt_FromLong((long)(PEM_F_PEM_READ));
  else if ((PEM_F_PEM_READ) <= 0)
    o = PyLong_FromLongLong((long long)(PEM_F_PEM_READ));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(PEM_F_PEM_READ));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "PEM_F_PEM_READ", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_PEM_F_PEM_PK8PKEY(lib);
}

static int _cffi_const_PEM_F_PEM_READ_BIO(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (PEM_F_PEM_READ_BIO) && (PEM_F_PEM_READ_BIO) <= LONG_MAX)
    o = PyInt_FromLong((long)(PEM_F_PEM_READ_BIO));
  else if ((PEM_F_PEM_READ_BIO) <= 0)
    o = PyLong_FromLongLong((long long)(PEM_F_PEM_READ_BIO));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(PEM_F_PEM_READ_BIO));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "PEM_F_PEM_READ_BIO", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_PEM_F_PEM_READ(lib);
}

static int _cffi_const_PEM_F_PEM_READ_BIO_PRIVATEKEY(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (PEM_F_PEM_READ_BIO_PRIVATEKEY) && (PEM_F_PEM_READ_BIO_PRIVATEKEY) <= LONG_MAX)
    o = PyInt_FromLong((long)(PEM_F_PEM_READ_BIO_PRIVATEKEY));
  else if ((PEM_F_PEM_READ_BIO_PRIVATEKEY) <= 0)
    o = PyLong_FromLongLong((long long)(PEM_F_PEM_READ_BIO_PRIVATEKEY));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(PEM_F_PEM_READ_BIO_PRIVATEKEY));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "PEM_F_PEM_READ_BIO_PRIVATEKEY", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_PEM_F_PEM_READ_BIO(lib);
}

static int _cffi_const_PEM_F_PEM_READ_PRIVATEKEY(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (PEM_F_PEM_READ_PRIVATEKEY) && (PEM_F_PEM_READ_PRIVATEKEY) <= LONG_MAX)
    o = PyInt_FromLong((long)(PEM_F_PEM_READ_PRIVATEKEY));
  else if ((PEM_F_PEM_READ_PRIVATEKEY) <= 0)
    o = PyLong_FromLongLong((long long)(PEM_F_PEM_READ_PRIVATEKEY));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(PEM_F_PEM_READ_PRIVATEKEY));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "PEM_F_PEM_READ_PRIVATEKEY", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_PEM_F_PEM_READ_BIO_PRIVATEKEY(lib);
}

static int _cffi_const_PEM_F_PEM_SEALFINAL(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (PEM_F_PEM_SEALFINAL) && (PEM_F_PEM_SEALFINAL) <= LONG_MAX)
    o = PyInt_FromLong((long)(PEM_F_PEM_SEALFINAL));
  else if ((PEM_F_PEM_SEALFINAL) <= 0)
    o = PyLong_FromLongLong((long long)(PEM_F_PEM_SEALFINAL));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(PEM_F_PEM_SEALFINAL));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "PEM_F_PEM_SEALFINAL", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_PEM_F_PEM_READ_PRIVATEKEY(lib);
}

static int _cffi_const_PEM_F_PEM_SEALINIT(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (PEM_F_PEM_SEALINIT) && (PEM_F_PEM_SEALINIT) <= LONG_MAX)
    o = PyInt_FromLong((long)(PEM_F_PEM_SEALINIT));
  else if ((PEM_F_PEM_SEALINIT) <= 0)
    o = PyLong_FromLongLong((long long)(PEM_F_PEM_SEALINIT));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(PEM_F_PEM_SEALINIT));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "PEM_F_PEM_SEALINIT", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_PEM_F_PEM_SEALFINAL(lib);
}

static int _cffi_const_PEM_F_PEM_SIGNFINAL(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (PEM_F_PEM_SIGNFINAL) && (PEM_F_PEM_SIGNFINAL) <= LONG_MAX)
    o = PyInt_FromLong((long)(PEM_F_PEM_SIGNFINAL));
  else if ((PEM_F_PEM_SIGNFINAL) <= 0)
    o = PyLong_FromLongLong((long long)(PEM_F_PEM_SIGNFINAL));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(PEM_F_PEM_SIGNFINAL));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "PEM_F_PEM_SIGNFINAL", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_PEM_F_PEM_SEALINIT(lib);
}

static int _cffi_const_PEM_F_PEM_WRITE(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (PEM_F_PEM_WRITE) && (PEM_F_PEM_WRITE) <= LONG_MAX)
    o = PyInt_FromLong((long)(PEM_F_PEM_WRITE));
  else if ((PEM_F_PEM_WRITE) <= 0)
    o = PyLong_FromLongLong((long long)(PEM_F_PEM_WRITE));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(PEM_F_PEM_WRITE));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "PEM_F_PEM_WRITE", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_PEM_F_PEM_SIGNFINAL(lib);
}

static int _cffi_const_PEM_F_PEM_WRITE_BIO(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (PEM_F_PEM_WRITE_BIO) && (PEM_F_PEM_WRITE_BIO) <= LONG_MAX)
    o = PyInt_FromLong((long)(PEM_F_PEM_WRITE_BIO));
  else if ((PEM_F_PEM_WRITE_BIO) <= 0)
    o = PyLong_FromLongLong((long long)(PEM_F_PEM_WRITE_BIO));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(PEM_F_PEM_WRITE_BIO));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "PEM_F_PEM_WRITE_BIO", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_PEM_F_PEM_WRITE(lib);
}

static int _cffi_const_PEM_F_PEM_X509_INFO_READ(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (PEM_F_PEM_X509_INFO_READ) && (PEM_F_PEM_X509_INFO_READ) <= LONG_MAX)
    o = PyInt_FromLong((long)(PEM_F_PEM_X509_INFO_READ));
  else if ((PEM_F_PEM_X509_INFO_READ) <= 0)
    o = PyLong_FromLongLong((long long)(PEM_F_PEM_X509_INFO_READ));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(PEM_F_PEM_X509_INFO_READ));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "PEM_F_PEM_X509_INFO_READ", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_PEM_F_PEM_WRITE_BIO(lib);
}

static int _cffi_const_PEM_F_PEM_X509_INFO_READ_BIO(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (PEM_F_PEM_X509_INFO_READ_BIO) && (PEM_F_PEM_X509_INFO_READ_BIO) <= LONG_MAX)
    o = PyInt_FromLong((long)(PEM_F_PEM_X509_INFO_READ_BIO));
  else if ((PEM_F_PEM_X509_INFO_READ_BIO) <= 0)
    o = PyLong_FromLongLong((long long)(PEM_F_PEM_X509_INFO_READ_BIO));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(PEM_F_PEM_X509_INFO_READ_BIO));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "PEM_F_PEM_X509_INFO_READ_BIO", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_PEM_F_PEM_X509_INFO_READ(lib);
}

static int _cffi_const_PEM_F_PEM_X509_INFO_WRITE_BIO(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (PEM_F_PEM_X509_INFO_WRITE_BIO) && (PEM_F_PEM_X509_INFO_WRITE_BIO) <= LONG_MAX)
    o = PyInt_FromLong((long)(PEM_F_PEM_X509_INFO_WRITE_BIO));
  else if ((PEM_F_PEM_X509_INFO_WRITE_BIO) <= 0)
    o = PyLong_FromLongLong((long long)(PEM_F_PEM_X509_INFO_WRITE_BIO));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(PEM_F_PEM_X509_INFO_WRITE_BIO));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "PEM_F_PEM_X509_INFO_WRITE_BIO", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_PEM_F_PEM_X509_INFO_READ_BIO(lib);
}

static int _cffi_const_PEM_R_BAD_BASE64_DECODE(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (PEM_R_BAD_BASE64_DECODE) && (PEM_R_BAD_BASE64_DECODE) <= LONG_MAX)
    o = PyInt_FromLong((long)(PEM_R_BAD_BASE64_DECODE));
  else if ((PEM_R_BAD_BASE64_DECODE) <= 0)
    o = PyLong_FromLongLong((long long)(PEM_R_BAD_BASE64_DECODE));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(PEM_R_BAD_BASE64_DECODE));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "PEM_R_BAD_BASE64_DECODE", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_PEM_F_PEM_X509_INFO_WRITE_BIO(lib);
}

static int _cffi_const_PEM_R_BAD_DECRYPT(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (PEM_R_BAD_DECRYPT) && (PEM_R_BAD_DECRYPT) <= LONG_MAX)
    o = PyInt_FromLong((long)(PEM_R_BAD_DECRYPT));
  else if ((PEM_R_BAD_DECRYPT) <= 0)
    o = PyLong_FromLongLong((long long)(PEM_R_BAD_DECRYPT));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(PEM_R_BAD_DECRYPT));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "PEM_R_BAD_DECRYPT", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_PEM_R_BAD_BASE64_DECODE(lib);
}

static int _cffi_const_PEM_R_BAD_END_LINE(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (PEM_R_BAD_END_LINE) && (PEM_R_BAD_END_LINE) <= LONG_MAX)
    o = PyInt_FromLong((long)(PEM_R_BAD_END_LINE));
  else if ((PEM_R_BAD_END_LINE) <= 0)
    o = PyLong_FromLongLong((long long)(PEM_R_BAD_END_LINE));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(PEM_R_BAD_END_LINE));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "PEM_R_BAD_END_LINE", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_PEM_R_BAD_DECRYPT(lib);
}

static int _cffi_const_PEM_R_BAD_IV_CHARS(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (PEM_R_BAD_IV_CHARS) && (PEM_R_BAD_IV_CHARS) <= LONG_MAX)
    o = PyInt_FromLong((long)(PEM_R_BAD_IV_CHARS));
  else if ((PEM_R_BAD_IV_CHARS) <= 0)
    o = PyLong_FromLongLong((long long)(PEM_R_BAD_IV_CHARS));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(PEM_R_BAD_IV_CHARS));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "PEM_R_BAD_IV_CHARS", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_PEM_R_BAD_END_LINE(lib);
}

static int _cffi_const_PEM_R_BAD_PASSWORD_READ(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (PEM_R_BAD_PASSWORD_READ) && (PEM_R_BAD_PASSWORD_READ) <= LONG_MAX)
    o = PyInt_FromLong((long)(PEM_R_BAD_PASSWORD_READ));
  else if ((PEM_R_BAD_PASSWORD_READ) <= 0)
    o = PyLong_FromLongLong((long long)(PEM_R_BAD_PASSWORD_READ));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(PEM_R_BAD_PASSWORD_READ));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "PEM_R_BAD_PASSWORD_READ", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_PEM_R_BAD_IV_CHARS(lib);
}

static int _cffi_const_PEM_R_ERROR_CONVERTING_PRIVATE_KEY(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (PEM_R_ERROR_CONVERTING_PRIVATE_KEY) && (PEM_R_ERROR_CONVERTING_PRIVATE_KEY) <= LONG_MAX)
    o = PyInt_FromLong((long)(PEM_R_ERROR_CONVERTING_PRIVATE_KEY));
  else if ((PEM_R_ERROR_CONVERTING_PRIVATE_KEY) <= 0)
    o = PyLong_FromLongLong((long long)(PEM_R_ERROR_CONVERTING_PRIVATE_KEY));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(PEM_R_ERROR_CONVERTING_PRIVATE_KEY));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "PEM_R_ERROR_CONVERTING_PRIVATE_KEY", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_PEM_R_BAD_PASSWORD_READ(lib);
}

static int _cffi_const_PEM_R_NOT_DEK_INFO(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (PEM_R_NOT_DEK_INFO) && (PEM_R_NOT_DEK_INFO) <= LONG_MAX)
    o = PyInt_FromLong((long)(PEM_R_NOT_DEK_INFO));
  else if ((PEM_R_NOT_DEK_INFO) <= 0)
    o = PyLong_FromLongLong((long long)(PEM_R_NOT_DEK_INFO));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(PEM_R_NOT_DEK_INFO));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "PEM_R_NOT_DEK_INFO", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_PEM_R_ERROR_CONVERTING_PRIVATE_KEY(lib);
}

static int _cffi_const_PEM_R_NOT_ENCRYPTED(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (PEM_R_NOT_ENCRYPTED) && (PEM_R_NOT_ENCRYPTED) <= LONG_MAX)
    o = PyInt_FromLong((long)(PEM_R_NOT_ENCRYPTED));
  else if ((PEM_R_NOT_ENCRYPTED) <= 0)
    o = PyLong_FromLongLong((long long)(PEM_R_NOT_ENCRYPTED));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(PEM_R_NOT_ENCRYPTED));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "PEM_R_NOT_ENCRYPTED", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_PEM_R_NOT_DEK_INFO(lib);
}

static int _cffi_const_PEM_R_NOT_PROC_TYPE(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (PEM_R_NOT_PROC_TYPE) && (PEM_R_NOT_PROC_TYPE) <= LONG_MAX)
    o = PyInt_FromLong((long)(PEM_R_NOT_PROC_TYPE));
  else if ((PEM_R_NOT_PROC_TYPE) <= 0)
    o = PyLong_FromLongLong((long long)(PEM_R_NOT_PROC_TYPE));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(PEM_R_NOT_PROC_TYPE));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "PEM_R_NOT_PROC_TYPE", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_PEM_R_NOT_ENCRYPTED(lib);
}

static int _cffi_const_PEM_R_NO_START_LINE(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (PEM_R_NO_START_LINE) && (PEM_R_NO_START_LINE) <= LONG_MAX)
    o = PyInt_FromLong((long)(PEM_R_NO_START_LINE));
  else if ((PEM_R_NO_START_LINE) <= 0)
    o = PyLong_FromLongLong((long long)(PEM_R_NO_START_LINE));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(PEM_R_NO_START_LINE));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "PEM_R_NO_START_LINE", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_PEM_R_NOT_PROC_TYPE(lib);
}

static int _cffi_const_PEM_R_PROBLEMS_GETTING_PASSWORD(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (PEM_R_PROBLEMS_GETTING_PASSWORD) && (PEM_R_PROBLEMS_GETTING_PASSWORD) <= LONG_MAX)
    o = PyInt_FromLong((long)(PEM_R_PROBLEMS_GETTING_PASSWORD));
  else if ((PEM_R_PROBLEMS_GETTING_PASSWORD) <= 0)
    o = PyLong_FromLongLong((long long)(PEM_R_PROBLEMS_GETTING_PASSWORD));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(PEM_R_PROBLEMS_GETTING_PASSWORD));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "PEM_R_PROBLEMS_GETTING_PASSWORD", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_PEM_R_NO_START_LINE(lib);
}

static int _cffi_const_PEM_R_PUBLIC_KEY_NO_RSA(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (PEM_R_PUBLIC_KEY_NO_RSA) && (PEM_R_PUBLIC_KEY_NO_RSA) <= LONG_MAX)
    o = PyInt_FromLong((long)(PEM_R_PUBLIC_KEY_NO_RSA));
  else if ((PEM_R_PUBLIC_KEY_NO_RSA) <= 0)
    o = PyLong_FromLongLong((long long)(PEM_R_PUBLIC_KEY_NO_RSA));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(PEM_R_PUBLIC_KEY_NO_RSA));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "PEM_R_PUBLIC_KEY_NO_RSA", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_PEM_R_PROBLEMS_GETTING_PASSWORD(lib);
}

static int _cffi_const_PEM_R_READ_KEY(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (PEM_R_READ_KEY) && (PEM_R_READ_KEY) <= LONG_MAX)
    o = PyInt_FromLong((long)(PEM_R_READ_KEY));
  else if ((PEM_R_READ_KEY) <= 0)
    o = PyLong_FromLongLong((long long)(PEM_R_READ_KEY));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(PEM_R_READ_KEY));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "PEM_R_READ_KEY", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_PEM_R_PUBLIC_KEY_NO_RSA(lib);
}

static int _cffi_const_PEM_R_SHORT_HEADER(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (PEM_R_SHORT_HEADER) && (PEM_R_SHORT_HEADER) <= LONG_MAX)
    o = PyInt_FromLong((long)(PEM_R_SHORT_HEADER));
  else if ((PEM_R_SHORT_HEADER) <= 0)
    o = PyLong_FromLongLong((long long)(PEM_R_SHORT_HEADER));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(PEM_R_SHORT_HEADER));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "PEM_R_SHORT_HEADER", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_PEM_R_READ_KEY(lib);
}

static int _cffi_const_PEM_R_UNSUPPORTED_CIPHER(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (PEM_R_UNSUPPORTED_CIPHER) && (PEM_R_UNSUPPORTED_CIPHER) <= LONG_MAX)
    o = PyInt_FromLong((long)(PEM_R_UNSUPPORTED_CIPHER));
  else if ((PEM_R_UNSUPPORTED_CIPHER) <= 0)
    o = PyLong_FromLongLong((long long)(PEM_R_UNSUPPORTED_CIPHER));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(PEM_R_UNSUPPORTED_CIPHER));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "PEM_R_UNSUPPORTED_CIPHER", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_PEM_R_SHORT_HEADER(lib);
}

static int _cffi_const_PEM_R_UNSUPPORTED_ENCRYPTION(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (PEM_R_UNSUPPORTED_ENCRYPTION) && (PEM_R_UNSUPPORTED_ENCRYPTION) <= LONG_MAX)
    o = PyInt_FromLong((long)(PEM_R_UNSUPPORTED_ENCRYPTION));
  else if ((PEM_R_UNSUPPORTED_ENCRYPTION) <= 0)
    o = PyLong_FromLongLong((long long)(PEM_R_UNSUPPORTED_ENCRYPTION));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(PEM_R_UNSUPPORTED_ENCRYPTION));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "PEM_R_UNSUPPORTED_ENCRYPTION", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_PEM_R_UNSUPPORTED_CIPHER(lib);
}

static int _cffi_const_RSA_F4(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (RSA_F4) && (RSA_F4) <= LONG_MAX)
    o = PyInt_FromLong((long)(RSA_F4));
  else if ((RSA_F4) <= 0)
    o = PyLong_FromLongLong((long long)(RSA_F4));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(RSA_F4));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "RSA_F4", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_PEM_R_UNSUPPORTED_ENCRYPTION(lib);
}

static int _cffi_const_RSA_NO_PADDING(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (RSA_NO_PADDING) && (RSA_NO_PADDING) <= LONG_MAX)
    o = PyInt_FromLong((long)(RSA_NO_PADDING));
  else if ((RSA_NO_PADDING) <= 0)
    o = PyLong_FromLongLong((long long)(RSA_NO_PADDING));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(RSA_NO_PADDING));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "RSA_NO_PADDING", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_RSA_F4(lib);
}

static int _cffi_const_RSA_PKCS1_OAEP_PADDING(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (RSA_PKCS1_OAEP_PADDING) && (RSA_PKCS1_OAEP_PADDING) <= LONG_MAX)
    o = PyInt_FromLong((long)(RSA_PKCS1_OAEP_PADDING));
  else if ((RSA_PKCS1_OAEP_PADDING) <= 0)
    o = PyLong_FromLongLong((long long)(RSA_PKCS1_OAEP_PADDING));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(RSA_PKCS1_OAEP_PADDING));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "RSA_PKCS1_OAEP_PADDING", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_RSA_NO_PADDING(lib);
}

static int _cffi_const_RSA_PKCS1_PADDING(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (RSA_PKCS1_PADDING) && (RSA_PKCS1_PADDING) <= LONG_MAX)
    o = PyInt_FromLong((long)(RSA_PKCS1_PADDING));
  else if ((RSA_PKCS1_PADDING) <= 0)
    o = PyLong_FromLongLong((long long)(RSA_PKCS1_PADDING));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(RSA_PKCS1_PADDING));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "RSA_PKCS1_PADDING", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_RSA_PKCS1_OAEP_PADDING(lib);
}

static int _cffi_const_RSA_PKCS1_PSS_PADDING(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (RSA_PKCS1_PSS_PADDING) && (RSA_PKCS1_PSS_PADDING) <= LONG_MAX)
    o = PyInt_FromLong((long)(RSA_PKCS1_PSS_PADDING));
  else if ((RSA_PKCS1_PSS_PADDING) <= 0)
    o = PyLong_FromLongLong((long long)(RSA_PKCS1_PSS_PADDING));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(RSA_PKCS1_PSS_PADDING));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "RSA_PKCS1_PSS_PADDING", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_RSA_PKCS1_PADDING(lib);
}

static int _cffi_const_RSA_SSLV23_PADDING(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (RSA_SSLV23_PADDING) && (RSA_SSLV23_PADDING) <= LONG_MAX)
    o = PyInt_FromLong((long)(RSA_SSLV23_PADDING));
  else if ((RSA_SSLV23_PADDING) <= 0)
    o = PyLong_FromLongLong((long long)(RSA_SSLV23_PADDING));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(RSA_SSLV23_PADDING));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "RSA_SSLV23_PADDING", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_RSA_PKCS1_PSS_PADDING(lib);
}

static int _cffi_const_RSA_X931_PADDING(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (RSA_X931_PADDING) && (RSA_X931_PADDING) <= LONG_MAX)
    o = PyInt_FromLong((long)(RSA_X931_PADDING));
  else if ((RSA_X931_PADDING) <= 0)
    o = PyLong_FromLongLong((long long)(RSA_X931_PADDING));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(RSA_X931_PADDING));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "RSA_X931_PADDING", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_RSA_SSLV23_PADDING(lib);
}

static int _cffi_const_SSL3_RANDOM_SIZE(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (SSL3_RANDOM_SIZE) && (SSL3_RANDOM_SIZE) <= LONG_MAX)
    o = PyInt_FromLong((long)(SSL3_RANDOM_SIZE));
  else if ((SSL3_RANDOM_SIZE) <= 0)
    o = PyLong_FromLongLong((long long)(SSL3_RANDOM_SIZE));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(SSL3_RANDOM_SIZE));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "SSL3_RANDOM_SIZE", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_RSA_X931_PADDING(lib);
}

static int _cffi_const_SSLEAY_BUILT_ON(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (SSLEAY_BUILT_ON) && (SSLEAY_BUILT_ON) <= LONG_MAX)
    o = PyInt_FromLong((long)(SSLEAY_BUILT_ON));
  else if ((SSLEAY_BUILT_ON) <= 0)
    o = PyLong_FromLongLong((long long)(SSLEAY_BUILT_ON));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(SSLEAY_BUILT_ON));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "SSLEAY_BUILT_ON", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_SSL3_RANDOM_SIZE(lib);
}

static int _cffi_const_SSLEAY_CFLAGS(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (SSLEAY_CFLAGS) && (SSLEAY_CFLAGS) <= LONG_MAX)
    o = PyInt_FromLong((long)(SSLEAY_CFLAGS));
  else if ((SSLEAY_CFLAGS) <= 0)
    o = PyLong_FromLongLong((long long)(SSLEAY_CFLAGS));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(SSLEAY_CFLAGS));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "SSLEAY_CFLAGS", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_SSLEAY_BUILT_ON(lib);
}

static int _cffi_const_SSLEAY_DIR(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (SSLEAY_DIR) && (SSLEAY_DIR) <= LONG_MAX)
    o = PyInt_FromLong((long)(SSLEAY_DIR));
  else if ((SSLEAY_DIR) <= 0)
    o = PyLong_FromLongLong((long long)(SSLEAY_DIR));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(SSLEAY_DIR));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "SSLEAY_DIR", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_SSLEAY_CFLAGS(lib);
}

static int _cffi_const_SSLEAY_PLATFORM(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (SSLEAY_PLATFORM) && (SSLEAY_PLATFORM) <= LONG_MAX)
    o = PyInt_FromLong((long)(SSLEAY_PLATFORM));
  else if ((SSLEAY_PLATFORM) <= 0)
    o = PyLong_FromLongLong((long long)(SSLEAY_PLATFORM));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(SSLEAY_PLATFORM));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "SSLEAY_PLATFORM", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_SSLEAY_DIR(lib);
}

static int _cffi_const_SSLEAY_VERSION(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (SSLEAY_VERSION) && (SSLEAY_VERSION) <= LONG_MAX)
    o = PyInt_FromLong((long)(SSLEAY_VERSION));
  else if ((SSLEAY_VERSION) <= 0)
    o = PyLong_FromLongLong((long long)(SSLEAY_VERSION));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(SSLEAY_VERSION));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "SSLEAY_VERSION", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_SSLEAY_PLATFORM(lib);
}

static int _cffi_const_SSL_CB_ACCEPT_EXIT(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (SSL_CB_ACCEPT_EXIT) && (SSL_CB_ACCEPT_EXIT) <= LONG_MAX)
    o = PyInt_FromLong((long)(SSL_CB_ACCEPT_EXIT));
  else if ((SSL_CB_ACCEPT_EXIT) <= 0)
    o = PyLong_FromLongLong((long long)(SSL_CB_ACCEPT_EXIT));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(SSL_CB_ACCEPT_EXIT));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "SSL_CB_ACCEPT_EXIT", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_SSLEAY_VERSION(lib);
}

static int _cffi_const_SSL_CB_ACCEPT_LOOP(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (SSL_CB_ACCEPT_LOOP) && (SSL_CB_ACCEPT_LOOP) <= LONG_MAX)
    o = PyInt_FromLong((long)(SSL_CB_ACCEPT_LOOP));
  else if ((SSL_CB_ACCEPT_LOOP) <= 0)
    o = PyLong_FromLongLong((long long)(SSL_CB_ACCEPT_LOOP));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(SSL_CB_ACCEPT_LOOP));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "SSL_CB_ACCEPT_LOOP", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_SSL_CB_ACCEPT_EXIT(lib);
}

static int _cffi_const_SSL_CB_ALERT(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (SSL_CB_ALERT) && (SSL_CB_ALERT) <= LONG_MAX)
    o = PyInt_FromLong((long)(SSL_CB_ALERT));
  else if ((SSL_CB_ALERT) <= 0)
    o = PyLong_FromLongLong((long long)(SSL_CB_ALERT));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(SSL_CB_ALERT));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "SSL_CB_ALERT", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_SSL_CB_ACCEPT_LOOP(lib);
}

static int _cffi_const_SSL_CB_CONNECT_EXIT(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (SSL_CB_CONNECT_EXIT) && (SSL_CB_CONNECT_EXIT) <= LONG_MAX)
    o = PyInt_FromLong((long)(SSL_CB_CONNECT_EXIT));
  else if ((SSL_CB_CONNECT_EXIT) <= 0)
    o = PyLong_FromLongLong((long long)(SSL_CB_CONNECT_EXIT));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(SSL_CB_CONNECT_EXIT));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "SSL_CB_CONNECT_EXIT", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_SSL_CB_ALERT(lib);
}

static int _cffi_const_SSL_CB_CONNECT_LOOP(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (SSL_CB_CONNECT_LOOP) && (SSL_CB_CONNECT_LOOP) <= LONG_MAX)
    o = PyInt_FromLong((long)(SSL_CB_CONNECT_LOOP));
  else if ((SSL_CB_CONNECT_LOOP) <= 0)
    o = PyLong_FromLongLong((long long)(SSL_CB_CONNECT_LOOP));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(SSL_CB_CONNECT_LOOP));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "SSL_CB_CONNECT_LOOP", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_SSL_CB_CONNECT_EXIT(lib);
}

static int _cffi_const_SSL_CB_EXIT(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (SSL_CB_EXIT) && (SSL_CB_EXIT) <= LONG_MAX)
    o = PyInt_FromLong((long)(SSL_CB_EXIT));
  else if ((SSL_CB_EXIT) <= 0)
    o = PyLong_FromLongLong((long long)(SSL_CB_EXIT));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(SSL_CB_EXIT));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "SSL_CB_EXIT", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_SSL_CB_CONNECT_LOOP(lib);
}

static int _cffi_const_SSL_CB_HANDSHAKE_DONE(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (SSL_CB_HANDSHAKE_DONE) && (SSL_CB_HANDSHAKE_DONE) <= LONG_MAX)
    o = PyInt_FromLong((long)(SSL_CB_HANDSHAKE_DONE));
  else if ((SSL_CB_HANDSHAKE_DONE) <= 0)
    o = PyLong_FromLongLong((long long)(SSL_CB_HANDSHAKE_DONE));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(SSL_CB_HANDSHAKE_DONE));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "SSL_CB_HANDSHAKE_DONE", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_SSL_CB_EXIT(lib);
}

static int _cffi_const_SSL_CB_HANDSHAKE_START(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (SSL_CB_HANDSHAKE_START) && (SSL_CB_HANDSHAKE_START) <= LONG_MAX)
    o = PyInt_FromLong((long)(SSL_CB_HANDSHAKE_START));
  else if ((SSL_CB_HANDSHAKE_START) <= 0)
    o = PyLong_FromLongLong((long long)(SSL_CB_HANDSHAKE_START));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(SSL_CB_HANDSHAKE_START));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "SSL_CB_HANDSHAKE_START", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_SSL_CB_HANDSHAKE_DONE(lib);
}

static int _cffi_const_SSL_CB_LOOP(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (SSL_CB_LOOP) && (SSL_CB_LOOP) <= LONG_MAX)
    o = PyInt_FromLong((long)(SSL_CB_LOOP));
  else if ((SSL_CB_LOOP) <= 0)
    o = PyLong_FromLongLong((long long)(SSL_CB_LOOP));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(SSL_CB_LOOP));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "SSL_CB_LOOP", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_SSL_CB_HANDSHAKE_START(lib);
}

static int _cffi_const_SSL_CB_READ(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (SSL_CB_READ) && (SSL_CB_READ) <= LONG_MAX)
    o = PyInt_FromLong((long)(SSL_CB_READ));
  else if ((SSL_CB_READ) <= 0)
    o = PyLong_FromLongLong((long long)(SSL_CB_READ));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(SSL_CB_READ));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "SSL_CB_READ", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_SSL_CB_LOOP(lib);
}

static int _cffi_const_SSL_CB_READ_ALERT(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (SSL_CB_READ_ALERT) && (SSL_CB_READ_ALERT) <= LONG_MAX)
    o = PyInt_FromLong((long)(SSL_CB_READ_ALERT));
  else if ((SSL_CB_READ_ALERT) <= 0)
    o = PyLong_FromLongLong((long long)(SSL_CB_READ_ALERT));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(SSL_CB_READ_ALERT));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "SSL_CB_READ_ALERT", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_SSL_CB_READ(lib);
}

static int _cffi_const_SSL_CB_WRITE(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (SSL_CB_WRITE) && (SSL_CB_WRITE) <= LONG_MAX)
    o = PyInt_FromLong((long)(SSL_CB_WRITE));
  else if ((SSL_CB_WRITE) <= 0)
    o = PyLong_FromLongLong((long long)(SSL_CB_WRITE));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(SSL_CB_WRITE));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "SSL_CB_WRITE", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_SSL_CB_READ_ALERT(lib);
}

static int _cffi_const_SSL_CB_WRITE_ALERT(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (SSL_CB_WRITE_ALERT) && (SSL_CB_WRITE_ALERT) <= LONG_MAX)
    o = PyInt_FromLong((long)(SSL_CB_WRITE_ALERT));
  else if ((SSL_CB_WRITE_ALERT) <= 0)
    o = PyLong_FromLongLong((long long)(SSL_CB_WRITE_ALERT));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(SSL_CB_WRITE_ALERT));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "SSL_CB_WRITE_ALERT", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_SSL_CB_WRITE(lib);
}

static int _cffi_const_SSL_ERROR_NONE(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (SSL_ERROR_NONE) && (SSL_ERROR_NONE) <= LONG_MAX)
    o = PyInt_FromLong((long)(SSL_ERROR_NONE));
  else if ((SSL_ERROR_NONE) <= 0)
    o = PyLong_FromLongLong((long long)(SSL_ERROR_NONE));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(SSL_ERROR_NONE));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "SSL_ERROR_NONE", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_SSL_CB_WRITE_ALERT(lib);
}

static int _cffi_const_SSL_ERROR_SSL(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (SSL_ERROR_SSL) && (SSL_ERROR_SSL) <= LONG_MAX)
    o = PyInt_FromLong((long)(SSL_ERROR_SSL));
  else if ((SSL_ERROR_SSL) <= 0)
    o = PyLong_FromLongLong((long long)(SSL_ERROR_SSL));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(SSL_ERROR_SSL));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "SSL_ERROR_SSL", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_SSL_ERROR_NONE(lib);
}

static int _cffi_const_SSL_ERROR_SYSCALL(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (SSL_ERROR_SYSCALL) && (SSL_ERROR_SYSCALL) <= LONG_MAX)
    o = PyInt_FromLong((long)(SSL_ERROR_SYSCALL));
  else if ((SSL_ERROR_SYSCALL) <= 0)
    o = PyLong_FromLongLong((long long)(SSL_ERROR_SYSCALL));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(SSL_ERROR_SYSCALL));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "SSL_ERROR_SYSCALL", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_SSL_ERROR_SSL(lib);
}

static int _cffi_const_SSL_ERROR_WANT_READ(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (SSL_ERROR_WANT_READ) && (SSL_ERROR_WANT_READ) <= LONG_MAX)
    o = PyInt_FromLong((long)(SSL_ERROR_WANT_READ));
  else if ((SSL_ERROR_WANT_READ) <= 0)
    o = PyLong_FromLongLong((long long)(SSL_ERROR_WANT_READ));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(SSL_ERROR_WANT_READ));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "SSL_ERROR_WANT_READ", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_SSL_ERROR_SYSCALL(lib);
}

static int _cffi_const_SSL_ERROR_WANT_WRITE(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (SSL_ERROR_WANT_WRITE) && (SSL_ERROR_WANT_WRITE) <= LONG_MAX)
    o = PyInt_FromLong((long)(SSL_ERROR_WANT_WRITE));
  else if ((SSL_ERROR_WANT_WRITE) <= 0)
    o = PyLong_FromLongLong((long long)(SSL_ERROR_WANT_WRITE));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(SSL_ERROR_WANT_WRITE));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "SSL_ERROR_WANT_WRITE", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_SSL_ERROR_WANT_READ(lib);
}

static int _cffi_const_SSL_ERROR_WANT_X509_LOOKUP(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (SSL_ERROR_WANT_X509_LOOKUP) && (SSL_ERROR_WANT_X509_LOOKUP) <= LONG_MAX)
    o = PyInt_FromLong((long)(SSL_ERROR_WANT_X509_LOOKUP));
  else if ((SSL_ERROR_WANT_X509_LOOKUP) <= 0)
    o = PyLong_FromLongLong((long long)(SSL_ERROR_WANT_X509_LOOKUP));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(SSL_ERROR_WANT_X509_LOOKUP));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "SSL_ERROR_WANT_X509_LOOKUP", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_SSL_ERROR_WANT_WRITE(lib);
}

static int _cffi_const_SSL_ERROR_ZERO_RETURN(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (SSL_ERROR_ZERO_RETURN) && (SSL_ERROR_ZERO_RETURN) <= LONG_MAX)
    o = PyInt_FromLong((long)(SSL_ERROR_ZERO_RETURN));
  else if ((SSL_ERROR_ZERO_RETURN) <= 0)
    o = PyLong_FromLongLong((long long)(SSL_ERROR_ZERO_RETURN));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(SSL_ERROR_ZERO_RETURN));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "SSL_ERROR_ZERO_RETURN", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_SSL_ERROR_WANT_X509_LOOKUP(lib);
}

static int _cffi_const_SSL_FILETYPE_ASN1(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (SSL_FILETYPE_ASN1) && (SSL_FILETYPE_ASN1) <= LONG_MAX)
    o = PyInt_FromLong((long)(SSL_FILETYPE_ASN1));
  else if ((SSL_FILETYPE_ASN1) <= 0)
    o = PyLong_FromLongLong((long long)(SSL_FILETYPE_ASN1));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(SSL_FILETYPE_ASN1));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "SSL_FILETYPE_ASN1", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_SSL_ERROR_ZERO_RETURN(lib);
}

static int _cffi_const_SSL_FILETYPE_PEM(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (SSL_FILETYPE_PEM) && (SSL_FILETYPE_PEM) <= LONG_MAX)
    o = PyInt_FromLong((long)(SSL_FILETYPE_PEM));
  else if ((SSL_FILETYPE_PEM) <= 0)
    o = PyLong_FromLongLong((long long)(SSL_FILETYPE_PEM));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(SSL_FILETYPE_PEM));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "SSL_FILETYPE_PEM", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_SSL_FILETYPE_ASN1(lib);
}

static int _cffi_const_SSL_MODE_ACCEPT_MOVING_WRITE_BUFFER(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (SSL_MODE_ACCEPT_MOVING_WRITE_BUFFER) && (SSL_MODE_ACCEPT_MOVING_WRITE_BUFFER) <= LONG_MAX)
    o = PyInt_FromLong((long)(SSL_MODE_ACCEPT_MOVING_WRITE_BUFFER));
  else if ((SSL_MODE_ACCEPT_MOVING_WRITE_BUFFER) <= 0)
    o = PyLong_FromLongLong((long long)(SSL_MODE_ACCEPT_MOVING_WRITE_BUFFER));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(SSL_MODE_ACCEPT_MOVING_WRITE_BUFFER));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "SSL_MODE_ACCEPT_MOVING_WRITE_BUFFER", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_SSL_FILETYPE_PEM(lib);
}

static int _cffi_const_SSL_MODE_AUTO_RETRY(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (SSL_MODE_AUTO_RETRY) && (SSL_MODE_AUTO_RETRY) <= LONG_MAX)
    o = PyInt_FromLong((long)(SSL_MODE_AUTO_RETRY));
  else if ((SSL_MODE_AUTO_RETRY) <= 0)
    o = PyLong_FromLongLong((long long)(SSL_MODE_AUTO_RETRY));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(SSL_MODE_AUTO_RETRY));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "SSL_MODE_AUTO_RETRY", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_SSL_MODE_ACCEPT_MOVING_WRITE_BUFFER(lib);
}

static int _cffi_const_SSL_MODE_ENABLE_PARTIAL_WRITE(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (SSL_MODE_ENABLE_PARTIAL_WRITE) && (SSL_MODE_ENABLE_PARTIAL_WRITE) <= LONG_MAX)
    o = PyInt_FromLong((long)(SSL_MODE_ENABLE_PARTIAL_WRITE));
  else if ((SSL_MODE_ENABLE_PARTIAL_WRITE) <= 0)
    o = PyLong_FromLongLong((long long)(SSL_MODE_ENABLE_PARTIAL_WRITE));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(SSL_MODE_ENABLE_PARTIAL_WRITE));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "SSL_MODE_ENABLE_PARTIAL_WRITE", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_SSL_MODE_AUTO_RETRY(lib);
}

static int _cffi_const_SSL_MODE_RELEASE_BUFFERS(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (SSL_MODE_RELEASE_BUFFERS) && (SSL_MODE_RELEASE_BUFFERS) <= LONG_MAX)
    o = PyInt_FromLong((long)(SSL_MODE_RELEASE_BUFFERS));
  else if ((SSL_MODE_RELEASE_BUFFERS) <= 0)
    o = PyLong_FromLongLong((long long)(SSL_MODE_RELEASE_BUFFERS));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(SSL_MODE_RELEASE_BUFFERS));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "SSL_MODE_RELEASE_BUFFERS", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_SSL_MODE_ENABLE_PARTIAL_WRITE(lib);
}

static int _cffi_const_SSL_OP_ALL(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (SSL_OP_ALL) && (SSL_OP_ALL) <= LONG_MAX)
    o = PyInt_FromLong((long)(SSL_OP_ALL));
  else if ((SSL_OP_ALL) <= 0)
    o = PyLong_FromLongLong((long long)(SSL_OP_ALL));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(SSL_OP_ALL));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "SSL_OP_ALL", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_SSL_MODE_RELEASE_BUFFERS(lib);
}

static int _cffi_const_SSL_OP_CIPHER_SERVER_PREFERENCE(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (SSL_OP_CIPHER_SERVER_PREFERENCE) && (SSL_OP_CIPHER_SERVER_PREFERENCE) <= LONG_MAX)
    o = PyInt_FromLong((long)(SSL_OP_CIPHER_SERVER_PREFERENCE));
  else if ((SSL_OP_CIPHER_SERVER_PREFERENCE) <= 0)
    o = PyLong_FromLongLong((long long)(SSL_OP_CIPHER_SERVER_PREFERENCE));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(SSL_OP_CIPHER_SERVER_PREFERENCE));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "SSL_OP_CIPHER_SERVER_PREFERENCE", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_SSL_OP_ALL(lib);
}

static int _cffi_const_SSL_OP_COOKIE_EXCHANGE(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (SSL_OP_COOKIE_EXCHANGE) && (SSL_OP_COOKIE_EXCHANGE) <= LONG_MAX)
    o = PyInt_FromLong((long)(SSL_OP_COOKIE_EXCHANGE));
  else if ((SSL_OP_COOKIE_EXCHANGE) <= 0)
    o = PyLong_FromLongLong((long long)(SSL_OP_COOKIE_EXCHANGE));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(SSL_OP_COOKIE_EXCHANGE));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "SSL_OP_COOKIE_EXCHANGE", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_SSL_OP_CIPHER_SERVER_PREFERENCE(lib);
}

static int _cffi_const_SSL_OP_DONT_INSERT_EMPTY_FRAGMENTS(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (SSL_OP_DONT_INSERT_EMPTY_FRAGMENTS) && (SSL_OP_DONT_INSERT_EMPTY_FRAGMENTS) <= LONG_MAX)
    o = PyInt_FromLong((long)(SSL_OP_DONT_INSERT_EMPTY_FRAGMENTS));
  else if ((SSL_OP_DONT_INSERT_EMPTY_FRAGMENTS) <= 0)
    o = PyLong_FromLongLong((long long)(SSL_OP_DONT_INSERT_EMPTY_FRAGMENTS));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(SSL_OP_DONT_INSERT_EMPTY_FRAGMENTS));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "SSL_OP_DONT_INSERT_EMPTY_FRAGMENTS", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_SSL_OP_COOKIE_EXCHANGE(lib);
}

static int _cffi_const_SSL_OP_EPHEMERAL_RSA(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (SSL_OP_EPHEMERAL_RSA) && (SSL_OP_EPHEMERAL_RSA) <= LONG_MAX)
    o = PyInt_FromLong((long)(SSL_OP_EPHEMERAL_RSA));
  else if ((SSL_OP_EPHEMERAL_RSA) <= 0)
    o = PyLong_FromLongLong((long long)(SSL_OP_EPHEMERAL_RSA));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(SSL_OP_EPHEMERAL_RSA));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "SSL_OP_EPHEMERAL_RSA", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_SSL_OP_DONT_INSERT_EMPTY_FRAGMENTS(lib);
}

static int _cffi_const_SSL_OP_MICROSOFT_BIG_SSLV3_BUFFER(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (SSL_OP_MICROSOFT_BIG_SSLV3_BUFFER) && (SSL_OP_MICROSOFT_BIG_SSLV3_BUFFER) <= LONG_MAX)
    o = PyInt_FromLong((long)(SSL_OP_MICROSOFT_BIG_SSLV3_BUFFER));
  else if ((SSL_OP_MICROSOFT_BIG_SSLV3_BUFFER) <= 0)
    o = PyLong_FromLongLong((long long)(SSL_OP_MICROSOFT_BIG_SSLV3_BUFFER));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(SSL_OP_MICROSOFT_BIG_SSLV3_BUFFER));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "SSL_OP_MICROSOFT_BIG_SSLV3_BUFFER", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_SSL_OP_EPHEMERAL_RSA(lib);
}

static int _cffi_const_SSL_OP_MICROSOFT_SESS_ID_BUG(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (SSL_OP_MICROSOFT_SESS_ID_BUG) && (SSL_OP_MICROSOFT_SESS_ID_BUG) <= LONG_MAX)
    o = PyInt_FromLong((long)(SSL_OP_MICROSOFT_SESS_ID_BUG));
  else if ((SSL_OP_MICROSOFT_SESS_ID_BUG) <= 0)
    o = PyLong_FromLongLong((long long)(SSL_OP_MICROSOFT_SESS_ID_BUG));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(SSL_OP_MICROSOFT_SESS_ID_BUG));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "SSL_OP_MICROSOFT_SESS_ID_BUG", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_SSL_OP_MICROSOFT_BIG_SSLV3_BUFFER(lib);
}

static int _cffi_const_SSL_OP_MSIE_SSLV2_RSA_PADDING(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (SSL_OP_MSIE_SSLV2_RSA_PADDING) && (SSL_OP_MSIE_SSLV2_RSA_PADDING) <= LONG_MAX)
    o = PyInt_FromLong((long)(SSL_OP_MSIE_SSLV2_RSA_PADDING));
  else if ((SSL_OP_MSIE_SSLV2_RSA_PADDING) <= 0)
    o = PyLong_FromLongLong((long long)(SSL_OP_MSIE_SSLV2_RSA_PADDING));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(SSL_OP_MSIE_SSLV2_RSA_PADDING));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "SSL_OP_MSIE_SSLV2_RSA_PADDING", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_SSL_OP_MICROSOFT_SESS_ID_BUG(lib);
}

static int _cffi_const_SSL_OP_NETSCAPE_CA_DN_BUG(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (SSL_OP_NETSCAPE_CA_DN_BUG) && (SSL_OP_NETSCAPE_CA_DN_BUG) <= LONG_MAX)
    o = PyInt_FromLong((long)(SSL_OP_NETSCAPE_CA_DN_BUG));
  else if ((SSL_OP_NETSCAPE_CA_DN_BUG) <= 0)
    o = PyLong_FromLongLong((long long)(SSL_OP_NETSCAPE_CA_DN_BUG));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(SSL_OP_NETSCAPE_CA_DN_BUG));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "SSL_OP_NETSCAPE_CA_DN_BUG", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_SSL_OP_MSIE_SSLV2_RSA_PADDING(lib);
}

static int _cffi_const_SSL_OP_NETSCAPE_CHALLENGE_BUG(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (SSL_OP_NETSCAPE_CHALLENGE_BUG) && (SSL_OP_NETSCAPE_CHALLENGE_BUG) <= LONG_MAX)
    o = PyInt_FromLong((long)(SSL_OP_NETSCAPE_CHALLENGE_BUG));
  else if ((SSL_OP_NETSCAPE_CHALLENGE_BUG) <= 0)
    o = PyLong_FromLongLong((long long)(SSL_OP_NETSCAPE_CHALLENGE_BUG));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(SSL_OP_NETSCAPE_CHALLENGE_BUG));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "SSL_OP_NETSCAPE_CHALLENGE_BUG", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_SSL_OP_NETSCAPE_CA_DN_BUG(lib);
}

static int _cffi_const_SSL_OP_NETSCAPE_DEMO_CIPHER_CHANGE_BUG(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (SSL_OP_NETSCAPE_DEMO_CIPHER_CHANGE_BUG) && (SSL_OP_NETSCAPE_DEMO_CIPHER_CHANGE_BUG) <= LONG_MAX)
    o = PyInt_FromLong((long)(SSL_OP_NETSCAPE_DEMO_CIPHER_CHANGE_BUG));
  else if ((SSL_OP_NETSCAPE_DEMO_CIPHER_CHANGE_BUG) <= 0)
    o = PyLong_FromLongLong((long long)(SSL_OP_NETSCAPE_DEMO_CIPHER_CHANGE_BUG));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(SSL_OP_NETSCAPE_DEMO_CIPHER_CHANGE_BUG));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "SSL_OP_NETSCAPE_DEMO_CIPHER_CHANGE_BUG", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_SSL_OP_NETSCAPE_CHALLENGE_BUG(lib);
}

static int _cffi_const_SSL_OP_NETSCAPE_REUSE_CIPHER_CHANGE_BUG(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (SSL_OP_NETSCAPE_REUSE_CIPHER_CHANGE_BUG) && (SSL_OP_NETSCAPE_REUSE_CIPHER_CHANGE_BUG) <= LONG_MAX)
    o = PyInt_FromLong((long)(SSL_OP_NETSCAPE_REUSE_CIPHER_CHANGE_BUG));
  else if ((SSL_OP_NETSCAPE_REUSE_CIPHER_CHANGE_BUG) <= 0)
    o = PyLong_FromLongLong((long long)(SSL_OP_NETSCAPE_REUSE_CIPHER_CHANGE_BUG));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(SSL_OP_NETSCAPE_REUSE_CIPHER_CHANGE_BUG));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "SSL_OP_NETSCAPE_REUSE_CIPHER_CHANGE_BUG", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_SSL_OP_NETSCAPE_DEMO_CIPHER_CHANGE_BUG(lib);
}

static int _cffi_const_SSL_OP_NO_COMPRESSION(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (SSL_OP_NO_COMPRESSION) && (SSL_OP_NO_COMPRESSION) <= LONG_MAX)
    o = PyInt_FromLong((long)(SSL_OP_NO_COMPRESSION));
  else if ((SSL_OP_NO_COMPRESSION) <= 0)
    o = PyLong_FromLongLong((long long)(SSL_OP_NO_COMPRESSION));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(SSL_OP_NO_COMPRESSION));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "SSL_OP_NO_COMPRESSION", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_SSL_OP_NETSCAPE_REUSE_CIPHER_CHANGE_BUG(lib);
}

static int _cffi_const_SSL_OP_NO_QUERY_MTU(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (SSL_OP_NO_QUERY_MTU) && (SSL_OP_NO_QUERY_MTU) <= LONG_MAX)
    o = PyInt_FromLong((long)(SSL_OP_NO_QUERY_MTU));
  else if ((SSL_OP_NO_QUERY_MTU) <= 0)
    o = PyLong_FromLongLong((long long)(SSL_OP_NO_QUERY_MTU));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(SSL_OP_NO_QUERY_MTU));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "SSL_OP_NO_QUERY_MTU", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_SSL_OP_NO_COMPRESSION(lib);
}

static int _cffi_const_SSL_OP_NO_SSLv2(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (SSL_OP_NO_SSLv2) && (SSL_OP_NO_SSLv2) <= LONG_MAX)
    o = PyInt_FromLong((long)(SSL_OP_NO_SSLv2));
  else if ((SSL_OP_NO_SSLv2) <= 0)
    o = PyLong_FromLongLong((long long)(SSL_OP_NO_SSLv2));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(SSL_OP_NO_SSLv2));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "SSL_OP_NO_SSLv2", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_SSL_OP_NO_QUERY_MTU(lib);
}

static int _cffi_const_SSL_OP_NO_SSLv3(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (SSL_OP_NO_SSLv3) && (SSL_OP_NO_SSLv3) <= LONG_MAX)
    o = PyInt_FromLong((long)(SSL_OP_NO_SSLv3));
  else if ((SSL_OP_NO_SSLv3) <= 0)
    o = PyLong_FromLongLong((long long)(SSL_OP_NO_SSLv3));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(SSL_OP_NO_SSLv3));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "SSL_OP_NO_SSLv3", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_SSL_OP_NO_SSLv2(lib);
}

static int _cffi_const_SSL_OP_NO_TICKET(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (SSL_OP_NO_TICKET) && (SSL_OP_NO_TICKET) <= LONG_MAX)
    o = PyInt_FromLong((long)(SSL_OP_NO_TICKET));
  else if ((SSL_OP_NO_TICKET) <= 0)
    o = PyLong_FromLongLong((long long)(SSL_OP_NO_TICKET));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(SSL_OP_NO_TICKET));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "SSL_OP_NO_TICKET", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_SSL_OP_NO_SSLv3(lib);
}

static int _cffi_const_SSL_OP_NO_TLSv1(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (SSL_OP_NO_TLSv1) && (SSL_OP_NO_TLSv1) <= LONG_MAX)
    o = PyInt_FromLong((long)(SSL_OP_NO_TLSv1));
  else if ((SSL_OP_NO_TLSv1) <= 0)
    o = PyLong_FromLongLong((long long)(SSL_OP_NO_TLSv1));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(SSL_OP_NO_TLSv1));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "SSL_OP_NO_TLSv1", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_SSL_OP_NO_TICKET(lib);
}

static int _cffi_const_SSL_OP_NO_TLSv1_1(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (SSL_OP_NO_TLSv1_1) && (SSL_OP_NO_TLSv1_1) <= LONG_MAX)
    o = PyInt_FromLong((long)(SSL_OP_NO_TLSv1_1));
  else if ((SSL_OP_NO_TLSv1_1) <= 0)
    o = PyLong_FromLongLong((long long)(SSL_OP_NO_TLSv1_1));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(SSL_OP_NO_TLSv1_1));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "SSL_OP_NO_TLSv1_1", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_SSL_OP_NO_TLSv1(lib);
}

static int _cffi_const_SSL_OP_NO_TLSv1_2(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (SSL_OP_NO_TLSv1_2) && (SSL_OP_NO_TLSv1_2) <= LONG_MAX)
    o = PyInt_FromLong((long)(SSL_OP_NO_TLSv1_2));
  else if ((SSL_OP_NO_TLSv1_2) <= 0)
    o = PyLong_FromLongLong((long long)(SSL_OP_NO_TLSv1_2));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(SSL_OP_NO_TLSv1_2));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "SSL_OP_NO_TLSv1_2", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_SSL_OP_NO_TLSv1_1(lib);
}

static int _cffi_const_SSL_OP_PKCS1_CHECK_1(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (SSL_OP_PKCS1_CHECK_1) && (SSL_OP_PKCS1_CHECK_1) <= LONG_MAX)
    o = PyInt_FromLong((long)(SSL_OP_PKCS1_CHECK_1));
  else if ((SSL_OP_PKCS1_CHECK_1) <= 0)
    o = PyLong_FromLongLong((long long)(SSL_OP_PKCS1_CHECK_1));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(SSL_OP_PKCS1_CHECK_1));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "SSL_OP_PKCS1_CHECK_1", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_SSL_OP_NO_TLSv1_2(lib);
}

static int _cffi_const_SSL_OP_PKCS1_CHECK_2(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (SSL_OP_PKCS1_CHECK_2) && (SSL_OP_PKCS1_CHECK_2) <= LONG_MAX)
    o = PyInt_FromLong((long)(SSL_OP_PKCS1_CHECK_2));
  else if ((SSL_OP_PKCS1_CHECK_2) <= 0)
    o = PyLong_FromLongLong((long long)(SSL_OP_PKCS1_CHECK_2));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(SSL_OP_PKCS1_CHECK_2));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "SSL_OP_PKCS1_CHECK_2", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_SSL_OP_PKCS1_CHECK_1(lib);
}

static int _cffi_const_SSL_OP_SINGLE_DH_USE(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (SSL_OP_SINGLE_DH_USE) && (SSL_OP_SINGLE_DH_USE) <= LONG_MAX)
    o = PyInt_FromLong((long)(SSL_OP_SINGLE_DH_USE));
  else if ((SSL_OP_SINGLE_DH_USE) <= 0)
    o = PyLong_FromLongLong((long long)(SSL_OP_SINGLE_DH_USE));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(SSL_OP_SINGLE_DH_USE));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "SSL_OP_SINGLE_DH_USE", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_SSL_OP_PKCS1_CHECK_2(lib);
}

static int _cffi_const_SSL_OP_SINGLE_ECDH_USE(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (SSL_OP_SINGLE_ECDH_USE) && (SSL_OP_SINGLE_ECDH_USE) <= LONG_MAX)
    o = PyInt_FromLong((long)(SSL_OP_SINGLE_ECDH_USE));
  else if ((SSL_OP_SINGLE_ECDH_USE) <= 0)
    o = PyLong_FromLongLong((long long)(SSL_OP_SINGLE_ECDH_USE));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(SSL_OP_SINGLE_ECDH_USE));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "SSL_OP_SINGLE_ECDH_USE", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_SSL_OP_SINGLE_DH_USE(lib);
}

static int _cffi_const_SSL_OP_SSLEAY_080_CLIENT_DH_BUG(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (SSL_OP_SSLEAY_080_CLIENT_DH_BUG) && (SSL_OP_SSLEAY_080_CLIENT_DH_BUG) <= LONG_MAX)
    o = PyInt_FromLong((long)(SSL_OP_SSLEAY_080_CLIENT_DH_BUG));
  else if ((SSL_OP_SSLEAY_080_CLIENT_DH_BUG) <= 0)
    o = PyLong_FromLongLong((long long)(SSL_OP_SSLEAY_080_CLIENT_DH_BUG));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(SSL_OP_SSLEAY_080_CLIENT_DH_BUG));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "SSL_OP_SSLEAY_080_CLIENT_DH_BUG", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_SSL_OP_SINGLE_ECDH_USE(lib);
}

static int _cffi_const_SSL_OP_SSLREF2_REUSE_CERT_TYPE_BUG(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (SSL_OP_SSLREF2_REUSE_CERT_TYPE_BUG) && (SSL_OP_SSLREF2_REUSE_CERT_TYPE_BUG) <= LONG_MAX)
    o = PyInt_FromLong((long)(SSL_OP_SSLREF2_REUSE_CERT_TYPE_BUG));
  else if ((SSL_OP_SSLREF2_REUSE_CERT_TYPE_BUG) <= 0)
    o = PyLong_FromLongLong((long long)(SSL_OP_SSLREF2_REUSE_CERT_TYPE_BUG));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(SSL_OP_SSLREF2_REUSE_CERT_TYPE_BUG));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "SSL_OP_SSLREF2_REUSE_CERT_TYPE_BUG", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_SSL_OP_SSLEAY_080_CLIENT_DH_BUG(lib);
}

static int _cffi_const_SSL_OP_TLS_BLOCK_PADDING_BUG(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (SSL_OP_TLS_BLOCK_PADDING_BUG) && (SSL_OP_TLS_BLOCK_PADDING_BUG) <= LONG_MAX)
    o = PyInt_FromLong((long)(SSL_OP_TLS_BLOCK_PADDING_BUG));
  else if ((SSL_OP_TLS_BLOCK_PADDING_BUG) <= 0)
    o = PyLong_FromLongLong((long long)(SSL_OP_TLS_BLOCK_PADDING_BUG));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(SSL_OP_TLS_BLOCK_PADDING_BUG));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "SSL_OP_TLS_BLOCK_PADDING_BUG", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_SSL_OP_SSLREF2_REUSE_CERT_TYPE_BUG(lib);
}

static int _cffi_const_SSL_OP_TLS_D5_BUG(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (SSL_OP_TLS_D5_BUG) && (SSL_OP_TLS_D5_BUG) <= LONG_MAX)
    o = PyInt_FromLong((long)(SSL_OP_TLS_D5_BUG));
  else if ((SSL_OP_TLS_D5_BUG) <= 0)
    o = PyLong_FromLongLong((long long)(SSL_OP_TLS_D5_BUG));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(SSL_OP_TLS_D5_BUG));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "SSL_OP_TLS_D5_BUG", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_SSL_OP_TLS_BLOCK_PADDING_BUG(lib);
}

static int _cffi_const_SSL_OP_TLS_ROLLBACK_BUG(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (SSL_OP_TLS_ROLLBACK_BUG) && (SSL_OP_TLS_ROLLBACK_BUG) <= LONG_MAX)
    o = PyInt_FromLong((long)(SSL_OP_TLS_ROLLBACK_BUG));
  else if ((SSL_OP_TLS_ROLLBACK_BUG) <= 0)
    o = PyLong_FromLongLong((long long)(SSL_OP_TLS_ROLLBACK_BUG));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(SSL_OP_TLS_ROLLBACK_BUG));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "SSL_OP_TLS_ROLLBACK_BUG", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_SSL_OP_TLS_D5_BUG(lib);
}

static int _cffi_const_SSL_RECEIVED_SHUTDOWN(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (SSL_RECEIVED_SHUTDOWN) && (SSL_RECEIVED_SHUTDOWN) <= LONG_MAX)
    o = PyInt_FromLong((long)(SSL_RECEIVED_SHUTDOWN));
  else if ((SSL_RECEIVED_SHUTDOWN) <= 0)
    o = PyLong_FromLongLong((long long)(SSL_RECEIVED_SHUTDOWN));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(SSL_RECEIVED_SHUTDOWN));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "SSL_RECEIVED_SHUTDOWN", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_SSL_OP_TLS_ROLLBACK_BUG(lib);
}

static int _cffi_const_SSL_SENT_SHUTDOWN(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (SSL_SENT_SHUTDOWN) && (SSL_SENT_SHUTDOWN) <= LONG_MAX)
    o = PyInt_FromLong((long)(SSL_SENT_SHUTDOWN));
  else if ((SSL_SENT_SHUTDOWN) <= 0)
    o = PyLong_FromLongLong((long long)(SSL_SENT_SHUTDOWN));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(SSL_SENT_SHUTDOWN));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "SSL_SENT_SHUTDOWN", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_SSL_RECEIVED_SHUTDOWN(lib);
}

static int _cffi_const_SSL_SESS_CACHE_BOTH(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (SSL_SESS_CACHE_BOTH) && (SSL_SESS_CACHE_BOTH) <= LONG_MAX)
    o = PyInt_FromLong((long)(SSL_SESS_CACHE_BOTH));
  else if ((SSL_SESS_CACHE_BOTH) <= 0)
    o = PyLong_FromLongLong((long long)(SSL_SESS_CACHE_BOTH));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(SSL_SESS_CACHE_BOTH));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "SSL_SESS_CACHE_BOTH", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_SSL_SENT_SHUTDOWN(lib);
}

static int _cffi_const_SSL_SESS_CACHE_CLIENT(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (SSL_SESS_CACHE_CLIENT) && (SSL_SESS_CACHE_CLIENT) <= LONG_MAX)
    o = PyInt_FromLong((long)(SSL_SESS_CACHE_CLIENT));
  else if ((SSL_SESS_CACHE_CLIENT) <= 0)
    o = PyLong_FromLongLong((long long)(SSL_SESS_CACHE_CLIENT));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(SSL_SESS_CACHE_CLIENT));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "SSL_SESS_CACHE_CLIENT", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_SSL_SESS_CACHE_BOTH(lib);
}

static int _cffi_const_SSL_SESS_CACHE_NO_AUTO_CLEAR(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (SSL_SESS_CACHE_NO_AUTO_CLEAR) && (SSL_SESS_CACHE_NO_AUTO_CLEAR) <= LONG_MAX)
    o = PyInt_FromLong((long)(SSL_SESS_CACHE_NO_AUTO_CLEAR));
  else if ((SSL_SESS_CACHE_NO_AUTO_CLEAR) <= 0)
    o = PyLong_FromLongLong((long long)(SSL_SESS_CACHE_NO_AUTO_CLEAR));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(SSL_SESS_CACHE_NO_AUTO_CLEAR));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "SSL_SESS_CACHE_NO_AUTO_CLEAR", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_SSL_SESS_CACHE_CLIENT(lib);
}

static int _cffi_const_SSL_SESS_CACHE_NO_INTERNAL(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (SSL_SESS_CACHE_NO_INTERNAL) && (SSL_SESS_CACHE_NO_INTERNAL) <= LONG_MAX)
    o = PyInt_FromLong((long)(SSL_SESS_CACHE_NO_INTERNAL));
  else if ((SSL_SESS_CACHE_NO_INTERNAL) <= 0)
    o = PyLong_FromLongLong((long long)(SSL_SESS_CACHE_NO_INTERNAL));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(SSL_SESS_CACHE_NO_INTERNAL));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "SSL_SESS_CACHE_NO_INTERNAL", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_SSL_SESS_CACHE_NO_AUTO_CLEAR(lib);
}

static int _cffi_const_SSL_SESS_CACHE_NO_INTERNAL_LOOKUP(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (SSL_SESS_CACHE_NO_INTERNAL_LOOKUP) && (SSL_SESS_CACHE_NO_INTERNAL_LOOKUP) <= LONG_MAX)
    o = PyInt_FromLong((long)(SSL_SESS_CACHE_NO_INTERNAL_LOOKUP));
  else if ((SSL_SESS_CACHE_NO_INTERNAL_LOOKUP) <= 0)
    o = PyLong_FromLongLong((long long)(SSL_SESS_CACHE_NO_INTERNAL_LOOKUP));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(SSL_SESS_CACHE_NO_INTERNAL_LOOKUP));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "SSL_SESS_CACHE_NO_INTERNAL_LOOKUP", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_SSL_SESS_CACHE_NO_INTERNAL(lib);
}

static int _cffi_const_SSL_SESS_CACHE_NO_INTERNAL_STORE(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (SSL_SESS_CACHE_NO_INTERNAL_STORE) && (SSL_SESS_CACHE_NO_INTERNAL_STORE) <= LONG_MAX)
    o = PyInt_FromLong((long)(SSL_SESS_CACHE_NO_INTERNAL_STORE));
  else if ((SSL_SESS_CACHE_NO_INTERNAL_STORE) <= 0)
    o = PyLong_FromLongLong((long long)(SSL_SESS_CACHE_NO_INTERNAL_STORE));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(SSL_SESS_CACHE_NO_INTERNAL_STORE));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "SSL_SESS_CACHE_NO_INTERNAL_STORE", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_SSL_SESS_CACHE_NO_INTERNAL_LOOKUP(lib);
}

static int _cffi_const_SSL_SESS_CACHE_OFF(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (SSL_SESS_CACHE_OFF) && (SSL_SESS_CACHE_OFF) <= LONG_MAX)
    o = PyInt_FromLong((long)(SSL_SESS_CACHE_OFF));
  else if ((SSL_SESS_CACHE_OFF) <= 0)
    o = PyLong_FromLongLong((long long)(SSL_SESS_CACHE_OFF));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(SSL_SESS_CACHE_OFF));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "SSL_SESS_CACHE_OFF", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_SSL_SESS_CACHE_NO_INTERNAL_STORE(lib);
}

static int _cffi_const_SSL_SESS_CACHE_SERVER(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (SSL_SESS_CACHE_SERVER) && (SSL_SESS_CACHE_SERVER) <= LONG_MAX)
    o = PyInt_FromLong((long)(SSL_SESS_CACHE_SERVER));
  else if ((SSL_SESS_CACHE_SERVER) <= 0)
    o = PyLong_FromLongLong((long long)(SSL_SESS_CACHE_SERVER));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(SSL_SESS_CACHE_SERVER));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "SSL_SESS_CACHE_SERVER", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_SSL_SESS_CACHE_OFF(lib);
}

static int _cffi_const_SSL_ST_ACCEPT(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (SSL_ST_ACCEPT) && (SSL_ST_ACCEPT) <= LONG_MAX)
    o = PyInt_FromLong((long)(SSL_ST_ACCEPT));
  else if ((SSL_ST_ACCEPT) <= 0)
    o = PyLong_FromLongLong((long long)(SSL_ST_ACCEPT));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(SSL_ST_ACCEPT));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "SSL_ST_ACCEPT", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_SSL_SESS_CACHE_SERVER(lib);
}

static int _cffi_const_SSL_ST_BEFORE(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (SSL_ST_BEFORE) && (SSL_ST_BEFORE) <= LONG_MAX)
    o = PyInt_FromLong((long)(SSL_ST_BEFORE));
  else if ((SSL_ST_BEFORE) <= 0)
    o = PyLong_FromLongLong((long long)(SSL_ST_BEFORE));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(SSL_ST_BEFORE));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "SSL_ST_BEFORE", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_SSL_ST_ACCEPT(lib);
}

static int _cffi_const_SSL_ST_CONNECT(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (SSL_ST_CONNECT) && (SSL_ST_CONNECT) <= LONG_MAX)
    o = PyInt_FromLong((long)(SSL_ST_CONNECT));
  else if ((SSL_ST_CONNECT) <= 0)
    o = PyLong_FromLongLong((long long)(SSL_ST_CONNECT));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(SSL_ST_CONNECT));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "SSL_ST_CONNECT", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_SSL_ST_BEFORE(lib);
}

static int _cffi_const_SSL_ST_INIT(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (SSL_ST_INIT) && (SSL_ST_INIT) <= LONG_MAX)
    o = PyInt_FromLong((long)(SSL_ST_INIT));
  else if ((SSL_ST_INIT) <= 0)
    o = PyLong_FromLongLong((long long)(SSL_ST_INIT));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(SSL_ST_INIT));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "SSL_ST_INIT", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_SSL_ST_CONNECT(lib);
}

static int _cffi_const_SSL_ST_MASK(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (SSL_ST_MASK) && (SSL_ST_MASK) <= LONG_MAX)
    o = PyInt_FromLong((long)(SSL_ST_MASK));
  else if ((SSL_ST_MASK) <= 0)
    o = PyLong_FromLongLong((long long)(SSL_ST_MASK));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(SSL_ST_MASK));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "SSL_ST_MASK", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_SSL_ST_INIT(lib);
}

static int _cffi_const_SSL_ST_OK(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (SSL_ST_OK) && (SSL_ST_OK) <= LONG_MAX)
    o = PyInt_FromLong((long)(SSL_ST_OK));
  else if ((SSL_ST_OK) <= 0)
    o = PyLong_FromLongLong((long long)(SSL_ST_OK));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(SSL_ST_OK));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "SSL_ST_OK", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_SSL_ST_MASK(lib);
}

static int _cffi_const_SSL_ST_RENEGOTIATE(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (SSL_ST_RENEGOTIATE) && (SSL_ST_RENEGOTIATE) <= LONG_MAX)
    o = PyInt_FromLong((long)(SSL_ST_RENEGOTIATE));
  else if ((SSL_ST_RENEGOTIATE) <= 0)
    o = PyLong_FromLongLong((long long)(SSL_ST_RENEGOTIATE));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(SSL_ST_RENEGOTIATE));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "SSL_ST_RENEGOTIATE", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_SSL_ST_OK(lib);
}

static int _cffi_const_SSL_VERIFY_CLIENT_ONCE(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (SSL_VERIFY_CLIENT_ONCE) && (SSL_VERIFY_CLIENT_ONCE) <= LONG_MAX)
    o = PyInt_FromLong((long)(SSL_VERIFY_CLIENT_ONCE));
  else if ((SSL_VERIFY_CLIENT_ONCE) <= 0)
    o = PyLong_FromLongLong((long long)(SSL_VERIFY_CLIENT_ONCE));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(SSL_VERIFY_CLIENT_ONCE));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "SSL_VERIFY_CLIENT_ONCE", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_SSL_ST_RENEGOTIATE(lib);
}

static int _cffi_const_SSL_VERIFY_FAIL_IF_NO_PEER_CERT(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (SSL_VERIFY_FAIL_IF_NO_PEER_CERT) && (SSL_VERIFY_FAIL_IF_NO_PEER_CERT) <= LONG_MAX)
    o = PyInt_FromLong((long)(SSL_VERIFY_FAIL_IF_NO_PEER_CERT));
  else if ((SSL_VERIFY_FAIL_IF_NO_PEER_CERT) <= 0)
    o = PyLong_FromLongLong((long long)(SSL_VERIFY_FAIL_IF_NO_PEER_CERT));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(SSL_VERIFY_FAIL_IF_NO_PEER_CERT));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "SSL_VERIFY_FAIL_IF_NO_PEER_CERT", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_SSL_VERIFY_CLIENT_ONCE(lib);
}

static int _cffi_const_SSL_VERIFY_NONE(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (SSL_VERIFY_NONE) && (SSL_VERIFY_NONE) <= LONG_MAX)
    o = PyInt_FromLong((long)(SSL_VERIFY_NONE));
  else if ((SSL_VERIFY_NONE) <= 0)
    o = PyLong_FromLongLong((long long)(SSL_VERIFY_NONE));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(SSL_VERIFY_NONE));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "SSL_VERIFY_NONE", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_SSL_VERIFY_FAIL_IF_NO_PEER_CERT(lib);
}

static int _cffi_const_SSL_VERIFY_PEER(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (SSL_VERIFY_PEER) && (SSL_VERIFY_PEER) <= LONG_MAX)
    o = PyInt_FromLong((long)(SSL_VERIFY_PEER));
  else if ((SSL_VERIFY_PEER) <= 0)
    o = PyLong_FromLongLong((long long)(SSL_VERIFY_PEER));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(SSL_VERIFY_PEER));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "SSL_VERIFY_PEER", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_SSL_VERIFY_NONE(lib);
}

static int _cffi_const_TLSEXT_NAMETYPE_host_name(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (TLSEXT_NAMETYPE_host_name) && (TLSEXT_NAMETYPE_host_name) <= LONG_MAX)
    o = PyInt_FromLong((long)(TLSEXT_NAMETYPE_host_name));
  else if ((TLSEXT_NAMETYPE_host_name) <= 0)
    o = PyLong_FromLongLong((long long)(TLSEXT_NAMETYPE_host_name));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(TLSEXT_NAMETYPE_host_name));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "TLSEXT_NAMETYPE_host_name", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_SSL_VERIFY_PEER(lib);
}

static int _cffi_const_V_ASN1_GENERALIZEDTIME(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (V_ASN1_GENERALIZEDTIME) && (V_ASN1_GENERALIZEDTIME) <= LONG_MAX)
    o = PyInt_FromLong((long)(V_ASN1_GENERALIZEDTIME));
  else if ((V_ASN1_GENERALIZEDTIME) <= 0)
    o = PyLong_FromLongLong((long long)(V_ASN1_GENERALIZEDTIME));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(V_ASN1_GENERALIZEDTIME));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "V_ASN1_GENERALIZEDTIME", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_TLSEXT_NAMETYPE_host_name(lib);
}

static int _cffi_const_X509_V_ERR_APPLICATION_VERIFICATION(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (X509_V_ERR_APPLICATION_VERIFICATION) && (X509_V_ERR_APPLICATION_VERIFICATION) <= LONG_MAX)
    o = PyInt_FromLong((long)(X509_V_ERR_APPLICATION_VERIFICATION));
  else if ((X509_V_ERR_APPLICATION_VERIFICATION) <= 0)
    o = PyLong_FromLongLong((long long)(X509_V_ERR_APPLICATION_VERIFICATION));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(X509_V_ERR_APPLICATION_VERIFICATION));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "X509_V_ERR_APPLICATION_VERIFICATION", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_V_ASN1_GENERALIZEDTIME(lib);
}

static int _cffi_const_X509_V_OK(PyObject *lib)
{
  PyObject *o;
  int res;
  if (LONG_MIN <= (X509_V_OK) && (X509_V_OK) <= LONG_MAX)
    o = PyInt_FromLong((long)(X509_V_OK));
  else if ((X509_V_OK) <= 0)
    o = PyLong_FromLongLong((long long)(X509_V_OK));
  else
    o = PyLong_FromUnsignedLongLong((unsigned long long)(X509_V_OK));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "X509_V_OK", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_X509_V_ERR_APPLICATION_VERIFICATION(lib);
}

static PyObject *
_cffi_f_AES_set_decrypt_key(PyObject *self, PyObject *args)
{
  unsigned char const * x0;
  int x1;
  AES_KEY * x2;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;

  if (!PyArg_ParseTuple(args, "OOO:AES_set_decrypt_key", &arg0, &arg1, &arg2))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(1), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(1), arg0) < 0)
      return NULL;
  }

  x1 = _cffi_to_c_SIGNED(arg1, int);
  if (x1 == (int)-1 && PyErr_Occurred())
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(2), arg2, (char **)&x2);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x2 = alloca(datasize);
    memset((void *)x2, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x2, _cffi_type(2), arg2) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = AES_set_decrypt_key(x0, x1, x2); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_AES_set_encrypt_key(PyObject *self, PyObject *args)
{
  unsigned char const * x0;
  int x1;
  AES_KEY * x2;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;

  if (!PyArg_ParseTuple(args, "OOO:AES_set_encrypt_key", &arg0, &arg1, &arg2))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(1), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(1), arg0) < 0)
      return NULL;
  }

  x1 = _cffi_to_c_SIGNED(arg1, int);
  if (x1 == (int)-1 && PyErr_Occurred())
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(2), arg2, (char **)&x2);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x2 = alloca(datasize);
    memset((void *)x2, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x2, _cffi_type(2), arg2) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = AES_set_encrypt_key(x0, x1, x2); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_AES_unwrap_key(PyObject *self, PyObject *args)
{
  AES_KEY * x0;
  unsigned char const * x1;
  unsigned char * x2;
  unsigned char const * x3;
  unsigned int x4;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;
  PyObject *arg4;

  if (!PyArg_ParseTuple(args, "OOOOO:AES_unwrap_key", &arg0, &arg1, &arg2, &arg3, &arg4))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(2), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(2), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(1), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(1), arg1) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(3), arg2, (char **)&x2);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x2 = alloca(datasize);
    memset((void *)x2, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x2, _cffi_type(3), arg2) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(1), arg3, (char **)&x3);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x3 = alloca(datasize);
    memset((void *)x3, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x3, _cffi_type(1), arg3) < 0)
      return NULL;
  }

  x4 = _cffi_to_c_UNSIGNED(arg4, unsigned int);
  if (x4 == (unsigned int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = AES_unwrap_key(x0, x1, x2, x3, x4); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_AES_wrap_key(PyObject *self, PyObject *args)
{
  AES_KEY * x0;
  unsigned char const * x1;
  unsigned char * x2;
  unsigned char const * x3;
  unsigned int x4;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;
  PyObject *arg4;

  if (!PyArg_ParseTuple(args, "OOOOO:AES_wrap_key", &arg0, &arg1, &arg2, &arg3, &arg4))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(2), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(2), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(1), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(1), arg1) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(3), arg2, (char **)&x2);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x2 = alloca(datasize);
    memset((void *)x2, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x2, _cffi_type(3), arg2) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(1), arg3, (char **)&x3);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x3 = alloca(datasize);
    memset((void *)x3, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x3, _cffi_type(1), arg3) < 0)
      return NULL;
  }

  x4 = _cffi_to_c_UNSIGNED(arg4, unsigned int);
  if (x4 == (unsigned int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = AES_wrap_key(x0, x1, x2, x3, x4); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_ASN1_ENUMERATED_free(PyObject *self, PyObject *arg0)
{
  ASN1_ENUMERATED * x0;
  Py_ssize_t datasize;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(4), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(4), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { ASN1_ENUMERATED_free(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_ASN1_ENUMERATED_new(PyObject *self, PyObject *no_arg)
{
  ASN1_ENUMERATED * result;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ASN1_ENUMERATED_new(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(4));
}

static PyObject *
_cffi_f_ASN1_ENUMERATED_set(PyObject *self, PyObject *args)
{
  ASN1_ENUMERATED * x0;
  long x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:ASN1_ENUMERATED_set", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(4), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(4), arg0) < 0)
      return NULL;
  }

  x1 = _cffi_to_c_SIGNED(arg1, long);
  if (x1 == (long)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ASN1_ENUMERATED_set(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_ASN1_GENERALIZEDTIME_check(PyObject *self, PyObject *arg0)
{
  ASN1_GENERALIZEDTIME * x0;
  Py_ssize_t datasize;
  int result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(6), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(6), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ASN1_GENERALIZEDTIME_check(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_ASN1_GENERALIZEDTIME_free(PyObject *self, PyObject *arg0)
{
  ASN1_GENERALIZEDTIME * x0;
  Py_ssize_t datasize;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(6), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(6), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { ASN1_GENERALIZEDTIME_free(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_ASN1_GENERALIZEDTIME_set_string(PyObject *self, PyObject *args)
{
  ASN1_GENERALIZEDTIME * x0;
  char const * x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:ASN1_GENERALIZEDTIME_set_string", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(6), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(6), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(0), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(0), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ASN1_GENERALIZEDTIME_set_string(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_ASN1_INTEGER_cmp(PyObject *self, PyObject *args)
{
  ASN1_INTEGER * x0;
  ASN1_INTEGER * x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:ASN1_INTEGER_cmp", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(7), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(7), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(7), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(7), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ASN1_INTEGER_cmp(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_ASN1_INTEGER_dup(PyObject *self, PyObject *arg0)
{
  ASN1_INTEGER * x0;
  Py_ssize_t datasize;
  ASN1_INTEGER * result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(7), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(7), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ASN1_INTEGER_dup(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(7));
}

static PyObject *
_cffi_f_ASN1_INTEGER_free(PyObject *self, PyObject *arg0)
{
  ASN1_INTEGER * x0;
  Py_ssize_t datasize;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(7), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(7), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { ASN1_INTEGER_free(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_ASN1_INTEGER_get(PyObject *self, PyObject *arg0)
{
  ASN1_INTEGER * x0;
  Py_ssize_t datasize;
  long result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(7), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(7), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ASN1_INTEGER_get(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, long);
}

static PyObject *
_cffi_f_ASN1_INTEGER_new(PyObject *self, PyObject *no_arg)
{
  ASN1_INTEGER * result;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ASN1_INTEGER_new(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(7));
}

static PyObject *
_cffi_f_ASN1_INTEGER_set(PyObject *self, PyObject *args)
{
  ASN1_INTEGER * x0;
  long x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:ASN1_INTEGER_set", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(7), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(7), arg0) < 0)
      return NULL;
  }

  x1 = _cffi_to_c_SIGNED(arg1, long);
  if (x1 == (long)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ASN1_INTEGER_set(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_ASN1_INTEGER_to_BN(PyObject *self, PyObject *args)
{
  ASN1_INTEGER * x0;
  BIGNUM * x1;
  Py_ssize_t datasize;
  BIGNUM * result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:ASN1_INTEGER_to_BN", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(7), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(7), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(8), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(8), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ASN1_INTEGER_to_BN(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(8));
}

static PyObject *
_cffi_f_ASN1_ITEM_ptr(PyObject *self, PyObject *arg0)
{
  ASN1_ITEM_EXP * x0;
  Py_ssize_t datasize;
  ASN1_ITEM const * result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(9), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(9), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ASN1_ITEM_ptr(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(10));
}

static PyObject *
_cffi_f_ASN1_OBJECT_free(PyObject *self, PyObject *arg0)
{
  ASN1_OBJECT * x0;
  Py_ssize_t datasize;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(11), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(11), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { ASN1_OBJECT_free(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_ASN1_OBJECT_new(PyObject *self, PyObject *no_arg)
{
  ASN1_OBJECT * result;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ASN1_OBJECT_new(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(11));
}

static PyObject *
_cffi_f_ASN1_OCTET_STRING_cmp(PyObject *self, PyObject *args)
{
  ASN1_OCTET_STRING * x0;
  ASN1_OCTET_STRING * x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:ASN1_OCTET_STRING_cmp", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(12), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(12), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(12), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(12), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ASN1_OCTET_STRING_cmp(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_ASN1_OCTET_STRING_dup(PyObject *self, PyObject *arg0)
{
  ASN1_OCTET_STRING * x0;
  Py_ssize_t datasize;
  ASN1_OCTET_STRING * result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(12), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(12), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ASN1_OCTET_STRING_dup(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(12));
}

static PyObject *
_cffi_f_ASN1_OCTET_STRING_free(PyObject *self, PyObject *arg0)
{
  ASN1_OCTET_STRING * x0;
  Py_ssize_t datasize;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(12), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(12), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { ASN1_OCTET_STRING_free(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_ASN1_OCTET_STRING_new(PyObject *self, PyObject *no_arg)
{
  ASN1_OCTET_STRING * result;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ASN1_OCTET_STRING_new(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(12));
}

static PyObject *
_cffi_f_ASN1_OCTET_STRING_set(PyObject *self, PyObject *args)
{
  ASN1_OCTET_STRING * x0;
  unsigned char const * x1;
  int x2;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;

  if (!PyArg_ParseTuple(args, "OOO:ASN1_OCTET_STRING_set", &arg0, &arg1, &arg2))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(12), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(12), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(1), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(1), arg1) < 0)
      return NULL;
  }

  x2 = _cffi_to_c_SIGNED(arg2, int);
  if (x2 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ASN1_OCTET_STRING_set(x0, x1, x2); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_ASN1_STRING_cmp(PyObject *self, PyObject *args)
{
  ASN1_STRING * x0;
  ASN1_STRING * x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:ASN1_STRING_cmp", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(13), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(13), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(13), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(13), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ASN1_STRING_cmp(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_ASN1_STRING_data(PyObject *self, PyObject *arg0)
{
  ASN1_STRING * x0;
  Py_ssize_t datasize;
  unsigned char * result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(13), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(13), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ASN1_STRING_data(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(3));
}

static PyObject *
_cffi_f_ASN1_STRING_dup(PyObject *self, PyObject *arg0)
{
  ASN1_STRING * x0;
  Py_ssize_t datasize;
  ASN1_STRING * result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(13), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(13), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ASN1_STRING_dup(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(13));
}

static PyObject *
_cffi_f_ASN1_STRING_free(PyObject *self, PyObject *arg0)
{
  ASN1_STRING * x0;
  Py_ssize_t datasize;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(13), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(13), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { ASN1_STRING_free(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_ASN1_STRING_length(PyObject *self, PyObject *arg0)
{
  ASN1_STRING * x0;
  Py_ssize_t datasize;
  int result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(13), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(13), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ASN1_STRING_length(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_ASN1_STRING_new(PyObject *self, PyObject *no_arg)
{
  ASN1_STRING * result;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ASN1_STRING_new(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(13));
}

static PyObject *
_cffi_f_ASN1_STRING_set(PyObject *self, PyObject *args)
{
  ASN1_STRING * x0;
  void const * x1;
  int x2;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;

  if (!PyArg_ParseTuple(args, "OOO:ASN1_STRING_set", &arg0, &arg1, &arg2))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(13), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(13), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(14), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(14), arg1) < 0)
      return NULL;
  }

  x2 = _cffi_to_c_SIGNED(arg2, int);
  if (x2 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ASN1_STRING_set(x0, x1, x2); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_ASN1_STRING_to_UTF8(PyObject *self, PyObject *args)
{
  unsigned char * * x0;
  ASN1_STRING * x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:ASN1_STRING_to_UTF8", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(15), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(15), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(13), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(13), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ASN1_STRING_to_UTF8(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_ASN1_STRING_type(PyObject *self, PyObject *arg0)
{
  ASN1_STRING * x0;
  Py_ssize_t datasize;
  int result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(13), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(13), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ASN1_STRING_type(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_ASN1_STRING_type_new(PyObject *self, PyObject *arg0)
{
  int x0;
  ASN1_STRING * result;

  x0 = _cffi_to_c_SIGNED(arg0, int);
  if (x0 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ASN1_STRING_type_new(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(13));
}

static PyObject *
_cffi_f_ASN1_TIME_new(PyObject *self, PyObject *no_arg)
{
  ASN1_TIME * result;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ASN1_TIME_new(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(16));
}

static PyObject *
_cffi_f_ASN1_TIME_to_generalizedtime(PyObject *self, PyObject *args)
{
  ASN1_TIME * x0;
  ASN1_GENERALIZEDTIME * * x1;
  Py_ssize_t datasize;
  ASN1_GENERALIZEDTIME * result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:ASN1_TIME_to_generalizedtime", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(16), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(16), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(17), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(17), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ASN1_TIME_to_generalizedtime(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(6));
}

static PyObject *
_cffi_f_ASN1_UTCTIME_cmp_time_t(PyObject *self, PyObject *args)
{
  ASN1_UTCTIME const * x0;
  intptr_t x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:ASN1_UTCTIME_cmp_time_t", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(18), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(18), arg0) < 0)
      return NULL;
  }

  x1 = _cffi_to_c_SIGNED(arg1, intptr_t);
  if (x1 == (intptr_t)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ASN1_UTCTIME_cmp_time_t(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_ASN1_item_d2i(PyObject *self, PyObject *args)
{
  ASN1_VALUE * * x0;
  unsigned char const * * x1;
  long x2;
  ASN1_ITEM const * x3;
  Py_ssize_t datasize;
  ASN1_VALUE * result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;

  if (!PyArg_ParseTuple(args, "OOOO:ASN1_item_d2i", &arg0, &arg1, &arg2, &arg3))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(19), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(19), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(20), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(20), arg1) < 0)
      return NULL;
  }

  x2 = _cffi_to_c_SIGNED(arg2, long);
  if (x2 == (long)-1 && PyErr_Occurred())
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(10), arg3, (char **)&x3);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x3 = alloca(datasize);
    memset((void *)x3, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x3, _cffi_type(10), arg3) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ASN1_item_d2i(x0, x1, x2, x3); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(21));
}

static PyObject *
_cffi_f_BIO_append_filename(PyObject *self, PyObject *args)
{
  BIO * x0;
  char * x1;
  Py_ssize_t datasize;
  long result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:BIO_append_filename", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(22), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(22), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(23), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(23), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = BIO_append_filename(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, long);
}

static PyObject *
_cffi_f_BIO_callback_ctrl(PyObject *self, PyObject *args)
{
  BIO * x0;
  int x1;
  void(* x2)(BIO *, int, char const *, int, long, long);
  Py_ssize_t datasize;
  long result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;

  if (!PyArg_ParseTuple(args, "OOO:BIO_callback_ctrl", &arg0, &arg1, &arg2))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(22), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(22), arg0) < 0)
      return NULL;
  }

  x1 = _cffi_to_c_SIGNED(arg1, int);
  if (x1 == (int)-1 && PyErr_Occurred())
    return NULL;

  x2 = (void(*)(BIO *, int, char const *, int, long, long))_cffi_to_c_pointer(arg2, _cffi_type(24));
  if (x2 == (void(*)(BIO *, int, char const *, int, long, long))NULL && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = BIO_callback_ctrl(x0, x1, x2); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, long);
}

static PyObject *
_cffi_f_BIO_ctrl(PyObject *self, PyObject *args)
{
  BIO * x0;
  int x1;
  long x2;
  void * x3;
  Py_ssize_t datasize;
  long result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;

  if (!PyArg_ParseTuple(args, "OOOO:BIO_ctrl", &arg0, &arg1, &arg2, &arg3))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(22), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(22), arg0) < 0)
      return NULL;
  }

  x1 = _cffi_to_c_SIGNED(arg1, int);
  if (x1 == (int)-1 && PyErr_Occurred())
    return NULL;

  x2 = _cffi_to_c_SIGNED(arg2, long);
  if (x2 == (long)-1 && PyErr_Occurred())
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(25), arg3, (char **)&x3);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x3 = alloca(datasize);
    memset((void *)x3, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x3, _cffi_type(25), arg3) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = BIO_ctrl(x0, x1, x2, x3); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, long);
}

static PyObject *
_cffi_f_BIO_ctrl_pending(PyObject *self, PyObject *arg0)
{
  BIO * x0;
  Py_ssize_t datasize;
  size_t result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(22), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(22), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = BIO_ctrl_pending(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_UNSIGNED(result, size_t);
}

static PyObject *
_cffi_f_BIO_ctrl_wpending(PyObject *self, PyObject *arg0)
{
  BIO * x0;
  Py_ssize_t datasize;
  size_t result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(22), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(22), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = BIO_ctrl_wpending(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_UNSIGNED(result, size_t);
}

static PyObject *
_cffi_f_BIO_eof(PyObject *self, PyObject *arg0)
{
  BIO * x0;
  Py_ssize_t datasize;
  int result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(22), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(22), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = BIO_eof(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_BIO_f_buffer(PyObject *self, PyObject *no_arg)
{
  BIO_METHOD * result;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = BIO_f_buffer(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(26));
}

static PyObject *
_cffi_f_BIO_f_null(PyObject *self, PyObject *no_arg)
{
  BIO_METHOD * result;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = BIO_f_null(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(26));
}

static PyObject *
_cffi_f_BIO_find_type(PyObject *self, PyObject *args)
{
  BIO * x0;
  int x1;
  Py_ssize_t datasize;
  BIO * result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:BIO_find_type", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(22), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(22), arg0) < 0)
      return NULL;
  }

  x1 = _cffi_to_c_SIGNED(arg1, int);
  if (x1 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = BIO_find_type(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(22));
}

static PyObject *
_cffi_f_BIO_flush(PyObject *self, PyObject *arg0)
{
  BIO * x0;
  Py_ssize_t datasize;
  int result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(22), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(22), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = BIO_flush(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_BIO_free(PyObject *self, PyObject *arg0)
{
  BIO * x0;
  Py_ssize_t datasize;
  int result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(22), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(22), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = BIO_free(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_BIO_free_all(PyObject *self, PyObject *arg0)
{
  BIO * x0;
  Py_ssize_t datasize;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(22), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(22), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { BIO_free_all(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_BIO_get_buffer_num_lines(PyObject *self, PyObject *arg0)
{
  BIO * x0;
  Py_ssize_t datasize;
  long result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(22), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(22), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = BIO_get_buffer_num_lines(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, long);
}

static PyObject *
_cffi_f_BIO_get_close(PyObject *self, PyObject *arg0)
{
  BIO * x0;
  Py_ssize_t datasize;
  int result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(22), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(22), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = BIO_get_close(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_BIO_get_fd(PyObject *self, PyObject *args)
{
  BIO * x0;
  char * x1;
  Py_ssize_t datasize;
  long result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:BIO_get_fd", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(22), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(22), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(23), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(23), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = BIO_get_fd(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, long);
}

static PyObject *
_cffi_f_BIO_get_fp(PyObject *self, PyObject *args)
{
  BIO * x0;
  FILE * * x1;
  Py_ssize_t datasize;
  long result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:BIO_get_fp", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(22), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(22), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(27), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(27), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = BIO_get_fp(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, long);
}

static PyObject *
_cffi_f_BIO_get_info_callback(PyObject *self, PyObject *args)
{
  BIO * x0;
  void(* * x1)(BIO *, int, char const *, int, long, long);
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:BIO_get_info_callback", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(22), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(22), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(28), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(28), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = BIO_get_info_callback(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_BIO_get_mem_data(PyObject *self, PyObject *args)
{
  BIO * x0;
  char * * x1;
  Py_ssize_t datasize;
  long result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:BIO_get_mem_data", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(22), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(22), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(29), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(29), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = BIO_get_mem_data(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, long);
}

static PyObject *
_cffi_f_BIO_get_mem_ptr(PyObject *self, PyObject *args)
{
  BIO * x0;
  BUF_MEM * * x1;
  Py_ssize_t datasize;
  long result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:BIO_get_mem_ptr", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(22), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(22), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(30), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(30), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = BIO_get_mem_ptr(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, long);
}

static PyObject *
_cffi_f_BIO_gets(PyObject *self, PyObject *args)
{
  BIO * x0;
  char * x1;
  int x2;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;

  if (!PyArg_ParseTuple(args, "OOO:BIO_gets", &arg0, &arg1, &arg2))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(22), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(22), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(23), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(23), arg1) < 0)
      return NULL;
  }

  x2 = _cffi_to_c_SIGNED(arg2, int);
  if (x2 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = BIO_gets(x0, x1, x2); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_BIO_int_ctrl(PyObject *self, PyObject *args)
{
  BIO * x0;
  int x1;
  long x2;
  int x3;
  Py_ssize_t datasize;
  long result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;

  if (!PyArg_ParseTuple(args, "OOOO:BIO_int_ctrl", &arg0, &arg1, &arg2, &arg3))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(22), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(22), arg0) < 0)
      return NULL;
  }

  x1 = _cffi_to_c_SIGNED(arg1, int);
  if (x1 == (int)-1 && PyErr_Occurred())
    return NULL;

  x2 = _cffi_to_c_SIGNED(arg2, long);
  if (x2 == (long)-1 && PyErr_Occurred())
    return NULL;

  x3 = _cffi_to_c_SIGNED(arg3, int);
  if (x3 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = BIO_int_ctrl(x0, x1, x2, x3); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, long);
}

static PyObject *
_cffi_f_BIO_method_type(PyObject *self, PyObject *arg0)
{
  BIO const * x0;
  Py_ssize_t datasize;
  int result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(31), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(31), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = BIO_method_type(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_BIO_new(PyObject *self, PyObject *arg0)
{
  BIO_METHOD * x0;
  Py_ssize_t datasize;
  BIO * result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(26), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(26), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = BIO_new(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(22));
}

static PyObject *
_cffi_f_BIO_new_fd(PyObject *self, PyObject *args)
{
  int x0;
  int x1;
  BIO * result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:BIO_new_fd", &arg0, &arg1))
    return NULL;

  x0 = _cffi_to_c_SIGNED(arg0, int);
  if (x0 == (int)-1 && PyErr_Occurred())
    return NULL;

  x1 = _cffi_to_c_SIGNED(arg1, int);
  if (x1 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = BIO_new_fd(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(22));
}

static PyObject *
_cffi_f_BIO_new_file(PyObject *self, PyObject *args)
{
  char const * x0;
  char const * x1;
  Py_ssize_t datasize;
  BIO * result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:BIO_new_file", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(0), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(0), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(0), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(0), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = BIO_new_file(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(22));
}

static PyObject *
_cffi_f_BIO_new_fp(PyObject *self, PyObject *args)
{
  FILE * x0;
  int x1;
  Py_ssize_t datasize;
  BIO * result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:BIO_new_fp", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(32), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(32), arg0) < 0)
      return NULL;
  }

  x1 = _cffi_to_c_SIGNED(arg1, int);
  if (x1 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = BIO_new_fp(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(22));
}

static PyObject *
_cffi_f_BIO_new_mem_buf(PyObject *self, PyObject *args)
{
  void * x0;
  int x1;
  Py_ssize_t datasize;
  BIO * result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:BIO_new_mem_buf", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(25), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(25), arg0) < 0)
      return NULL;
  }

  x1 = _cffi_to_c_SIGNED(arg1, int);
  if (x1 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = BIO_new_mem_buf(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(22));
}

static PyObject *
_cffi_f_BIO_new_socket(PyObject *self, PyObject *args)
{
  int x0;
  int x1;
  BIO * result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:BIO_new_socket", &arg0, &arg1))
    return NULL;

  x0 = _cffi_to_c_SIGNED(arg0, int);
  if (x0 == (int)-1 && PyErr_Occurred())
    return NULL;

  x1 = _cffi_to_c_SIGNED(arg1, int);
  if (x1 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = BIO_new_socket(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(22));
}

static PyObject *
_cffi_f_BIO_next(PyObject *self, PyObject *arg0)
{
  BIO * x0;
  Py_ssize_t datasize;
  BIO * result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(22), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(22), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = BIO_next(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(22));
}

static PyObject *
_cffi_f_BIO_pending(PyObject *self, PyObject *arg0)
{
  BIO * x0;
  Py_ssize_t datasize;
  int result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(22), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(22), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = BIO_pending(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_BIO_pop(PyObject *self, PyObject *arg0)
{
  BIO * x0;
  Py_ssize_t datasize;
  BIO * result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(22), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(22), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = BIO_pop(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(22));
}

static PyObject *
_cffi_f_BIO_ptr_ctrl(PyObject *self, PyObject *args)
{
  BIO * x0;
  int x1;
  long x2;
  Py_ssize_t datasize;
  char * result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;

  if (!PyArg_ParseTuple(args, "OOO:BIO_ptr_ctrl", &arg0, &arg1, &arg2))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(22), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(22), arg0) < 0)
      return NULL;
  }

  x1 = _cffi_to_c_SIGNED(arg1, int);
  if (x1 == (int)-1 && PyErr_Occurred())
    return NULL;

  x2 = _cffi_to_c_SIGNED(arg2, long);
  if (x2 == (long)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = BIO_ptr_ctrl(x0, x1, x2); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(23));
}

static PyObject *
_cffi_f_BIO_push(PyObject *self, PyObject *args)
{
  BIO * x0;
  BIO * x1;
  Py_ssize_t datasize;
  BIO * result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:BIO_push", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(22), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(22), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(22), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(22), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = BIO_push(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(22));
}

static PyObject *
_cffi_f_BIO_puts(PyObject *self, PyObject *args)
{
  BIO * x0;
  char const * x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:BIO_puts", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(22), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(22), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(0), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(0), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = BIO_puts(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_BIO_read(PyObject *self, PyObject *args)
{
  BIO * x0;
  void * x1;
  int x2;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;

  if (!PyArg_ParseTuple(args, "OOO:BIO_read", &arg0, &arg1, &arg2))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(22), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(22), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(25), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(25), arg1) < 0)
      return NULL;
  }

  x2 = _cffi_to_c_SIGNED(arg2, int);
  if (x2 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = BIO_read(x0, x1, x2); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_BIO_read_filename(PyObject *self, PyObject *args)
{
  BIO * x0;
  char * x1;
  Py_ssize_t datasize;
  long result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:BIO_read_filename", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(22), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(22), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(23), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(23), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = BIO_read_filename(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, long);
}

static PyObject *
_cffi_f_BIO_reset(PyObject *self, PyObject *arg0)
{
  BIO * x0;
  Py_ssize_t datasize;
  int result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(22), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(22), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = BIO_reset(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_BIO_retry_type(PyObject *self, PyObject *arg0)
{
  BIO * x0;
  Py_ssize_t datasize;
  int result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(22), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(22), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = BIO_retry_type(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_BIO_rw_filename(PyObject *self, PyObject *args)
{
  BIO * x0;
  char * x1;
  Py_ssize_t datasize;
  long result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:BIO_rw_filename", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(22), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(22), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(23), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(23), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = BIO_rw_filename(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, long);
}

static PyObject *
_cffi_f_BIO_s_fd(PyObject *self, PyObject *no_arg)
{
  BIO_METHOD * result;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = BIO_s_fd(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(26));
}

static PyObject *
_cffi_f_BIO_s_file(PyObject *self, PyObject *no_arg)
{
  BIO_METHOD * result;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = BIO_s_file(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(26));
}

static PyObject *
_cffi_f_BIO_s_mem(PyObject *self, PyObject *no_arg)
{
  BIO_METHOD * result;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = BIO_s_mem(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(26));
}

static PyObject *
_cffi_f_BIO_s_null(PyObject *self, PyObject *no_arg)
{
  BIO_METHOD * result;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = BIO_s_null(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(26));
}

static PyObject *
_cffi_f_BIO_s_socket(PyObject *self, PyObject *no_arg)
{
  BIO_METHOD * result;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = BIO_s_socket(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(26));
}

static PyObject *
_cffi_f_BIO_seek(PyObject *self, PyObject *args)
{
  BIO * x0;
  int x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:BIO_seek", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(22), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(22), arg0) < 0)
      return NULL;
  }

  x1 = _cffi_to_c_SIGNED(arg1, int);
  if (x1 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = BIO_seek(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_BIO_set(PyObject *self, PyObject *args)
{
  BIO * x0;
  BIO_METHOD * x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:BIO_set", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(22), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(22), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(26), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(26), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = BIO_set(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_BIO_set_buffer_read_data(PyObject *self, PyObject *args)
{
  BIO * x0;
  void * x1;
  long x2;
  Py_ssize_t datasize;
  long result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;

  if (!PyArg_ParseTuple(args, "OOO:BIO_set_buffer_read_data", &arg0, &arg1, &arg2))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(22), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(22), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(25), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(25), arg1) < 0)
      return NULL;
  }

  x2 = _cffi_to_c_SIGNED(arg2, long);
  if (x2 == (long)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = BIO_set_buffer_read_data(x0, x1, x2); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, long);
}

static PyObject *
_cffi_f_BIO_set_buffer_size(PyObject *self, PyObject *args)
{
  BIO * x0;
  long x1;
  Py_ssize_t datasize;
  long result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:BIO_set_buffer_size", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(22), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(22), arg0) < 0)
      return NULL;
  }

  x1 = _cffi_to_c_SIGNED(arg1, long);
  if (x1 == (long)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = BIO_set_buffer_size(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, long);
}

static PyObject *
_cffi_f_BIO_set_close(PyObject *self, PyObject *args)
{
  BIO * x0;
  long x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:BIO_set_close", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(22), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(22), arg0) < 0)
      return NULL;
  }

  x1 = _cffi_to_c_SIGNED(arg1, long);
  if (x1 == (long)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = BIO_set_close(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_BIO_set_fd(PyObject *self, PyObject *args)
{
  BIO * x0;
  long x1;
  int x2;
  Py_ssize_t datasize;
  long result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;

  if (!PyArg_ParseTuple(args, "OOO:BIO_set_fd", &arg0, &arg1, &arg2))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(22), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(22), arg0) < 0)
      return NULL;
  }

  x1 = _cffi_to_c_SIGNED(arg1, long);
  if (x1 == (long)-1 && PyErr_Occurred())
    return NULL;

  x2 = _cffi_to_c_SIGNED(arg2, int);
  if (x2 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = BIO_set_fd(x0, x1, x2); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, long);
}

static PyObject *
_cffi_f_BIO_set_fp(PyObject *self, PyObject *args)
{
  BIO * x0;
  FILE * x1;
  int x2;
  Py_ssize_t datasize;
  long result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;

  if (!PyArg_ParseTuple(args, "OOO:BIO_set_fp", &arg0, &arg1, &arg2))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(22), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(22), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(32), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(32), arg1) < 0)
      return NULL;
  }

  x2 = _cffi_to_c_SIGNED(arg2, int);
  if (x2 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = BIO_set_fp(x0, x1, x2); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, long);
}

static PyObject *
_cffi_f_BIO_set_info_callback(PyObject *self, PyObject *args)
{
  BIO * x0;
  void(* x1)(BIO *, int, char const *, int, long, long);
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:BIO_set_info_callback", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(22), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(22), arg0) < 0)
      return NULL;
  }

  x1 = (void(*)(BIO *, int, char const *, int, long, long))_cffi_to_c_pointer(arg1, _cffi_type(24));
  if (x1 == (void(*)(BIO *, int, char const *, int, long, long))NULL && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = BIO_set_info_callback(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_BIO_set_mem_buf(PyObject *self, PyObject *args)
{
  BIO * x0;
  BUF_MEM * x1;
  int x2;
  Py_ssize_t datasize;
  long result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;

  if (!PyArg_ParseTuple(args, "OOO:BIO_set_mem_buf", &arg0, &arg1, &arg2))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(22), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(22), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(33), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(33), arg1) < 0)
      return NULL;
  }

  x2 = _cffi_to_c_SIGNED(arg2, int);
  if (x2 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = BIO_set_mem_buf(x0, x1, x2); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, long);
}

static PyObject *
_cffi_f_BIO_set_mem_eof_return(PyObject *self, PyObject *args)
{
  BIO * x0;
  int x1;
  Py_ssize_t datasize;
  long result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:BIO_set_mem_eof_return", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(22), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(22), arg0) < 0)
      return NULL;
  }

  x1 = _cffi_to_c_SIGNED(arg1, int);
  if (x1 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = BIO_set_mem_eof_return(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, long);
}

static PyObject *
_cffi_f_BIO_set_read_buffer_size(PyObject *self, PyObject *args)
{
  BIO * x0;
  long x1;
  Py_ssize_t datasize;
  long result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:BIO_set_read_buffer_size", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(22), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(22), arg0) < 0)
      return NULL;
  }

  x1 = _cffi_to_c_SIGNED(arg1, long);
  if (x1 == (long)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = BIO_set_read_buffer_size(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, long);
}

static PyObject *
_cffi_f_BIO_set_write_buffer_size(PyObject *self, PyObject *args)
{
  BIO * x0;
  long x1;
  Py_ssize_t datasize;
  long result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:BIO_set_write_buffer_size", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(22), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(22), arg0) < 0)
      return NULL;
  }

  x1 = _cffi_to_c_SIGNED(arg1, long);
  if (x1 == (long)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = BIO_set_write_buffer_size(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, long);
}

static PyObject *
_cffi_f_BIO_should_io_special(PyObject *self, PyObject *arg0)
{
  BIO * x0;
  Py_ssize_t datasize;
  int result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(22), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(22), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = BIO_should_io_special(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_BIO_should_read(PyObject *self, PyObject *arg0)
{
  BIO * x0;
  Py_ssize_t datasize;
  int result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(22), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(22), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = BIO_should_read(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_BIO_should_retry(PyObject *self, PyObject *arg0)
{
  BIO * x0;
  Py_ssize_t datasize;
  int result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(22), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(22), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = BIO_should_retry(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_BIO_should_write(PyObject *self, PyObject *arg0)
{
  BIO * x0;
  Py_ssize_t datasize;
  int result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(22), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(22), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = BIO_should_write(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_BIO_tell(PyObject *self, PyObject *arg0)
{
  BIO * x0;
  Py_ssize_t datasize;
  int result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(22), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(22), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = BIO_tell(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_BIO_vfree(PyObject *self, PyObject *arg0)
{
  BIO * x0;
  Py_ssize_t datasize;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(22), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(22), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { BIO_vfree(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_BIO_wpending(PyObject *self, PyObject *arg0)
{
  BIO * x0;
  Py_ssize_t datasize;
  int result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(22), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(22), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = BIO_wpending(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_BIO_write(PyObject *self, PyObject *args)
{
  BIO * x0;
  void const * x1;
  int x2;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;

  if (!PyArg_ParseTuple(args, "OOO:BIO_write", &arg0, &arg1, &arg2))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(22), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(22), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(14), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(14), arg1) < 0)
      return NULL;
  }

  x2 = _cffi_to_c_SIGNED(arg2, int);
  if (x2 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = BIO_write(x0, x1, x2); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_BIO_write_filename(PyObject *self, PyObject *args)
{
  BIO * x0;
  char * x1;
  Py_ssize_t datasize;
  long result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:BIO_write_filename", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(22), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(22), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(23), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(23), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = BIO_write_filename(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, long);
}

static PyObject *
_cffi_f_BN_add(PyObject *self, PyObject *args)
{
  BIGNUM * x0;
  BIGNUM const * x1;
  BIGNUM const * x2;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;

  if (!PyArg_ParseTuple(args, "OOO:BN_add", &arg0, &arg1, &arg2))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(8), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(8), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(34), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(34), arg1) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(34), arg2, (char **)&x2);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x2 = alloca(datasize);
    memset((void *)x2, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x2, _cffi_type(34), arg2) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = BN_add(x0, x1, x2); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_BN_bin2bn(PyObject *self, PyObject *args)
{
  unsigned char const * x0;
  int x1;
  BIGNUM * x2;
  Py_ssize_t datasize;
  BIGNUM * result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;

  if (!PyArg_ParseTuple(args, "OOO:BN_bin2bn", &arg0, &arg1, &arg2))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(1), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(1), arg0) < 0)
      return NULL;
  }

  x1 = _cffi_to_c_SIGNED(arg1, int);
  if (x1 == (int)-1 && PyErr_Occurred())
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(8), arg2, (char **)&x2);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x2 = alloca(datasize);
    memset((void *)x2, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x2, _cffi_type(8), arg2) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = BN_bin2bn(x0, x1, x2); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(8));
}

static PyObject *
_cffi_f_BN_bn2bin(PyObject *self, PyObject *args)
{
  BIGNUM const * x0;
  unsigned char * x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:BN_bn2bin", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(34), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(34), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(3), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(3), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = BN_bn2bin(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_BN_bn2hex(PyObject *self, PyObject *arg0)
{
  BIGNUM const * x0;
  Py_ssize_t datasize;
  char * result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(34), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(34), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = BN_bn2hex(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(23));
}

static PyObject *
_cffi_f_BN_copy(PyObject *self, PyObject *args)
{
  BIGNUM * x0;
  BIGNUM const * x1;
  Py_ssize_t datasize;
  BIGNUM * result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:BN_copy", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(8), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(8), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(34), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(34), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = BN_copy(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(8));
}

static PyObject *
_cffi_f_BN_dec2bn(PyObject *self, PyObject *args)
{
  BIGNUM * * x0;
  char const * x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:BN_dec2bn", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(35), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(35), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(0), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(0), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = BN_dec2bn(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_BN_div(PyObject *self, PyObject *args)
{
  BIGNUM * x0;
  BIGNUM * x1;
  BIGNUM const * x2;
  BIGNUM const * x3;
  BN_CTX * x4;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;
  PyObject *arg4;

  if (!PyArg_ParseTuple(args, "OOOOO:BN_div", &arg0, &arg1, &arg2, &arg3, &arg4))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(8), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(8), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(8), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(8), arg1) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(34), arg2, (char **)&x2);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x2 = alloca(datasize);
    memset((void *)x2, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x2, _cffi_type(34), arg2) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(34), arg3, (char **)&x3);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x3 = alloca(datasize);
    memset((void *)x3, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x3, _cffi_type(34), arg3) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(36), arg4, (char **)&x4);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x4 = alloca(datasize);
    memset((void *)x4, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x4, _cffi_type(36), arg4) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = BN_div(x0, x1, x2, x3, x4); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_BN_dup(PyObject *self, PyObject *arg0)
{
  BIGNUM const * x0;
  Py_ssize_t datasize;
  BIGNUM * result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(34), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(34), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = BN_dup(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(8));
}

static PyObject *
_cffi_f_BN_exp(PyObject *self, PyObject *args)
{
  BIGNUM * x0;
  BIGNUM const * x1;
  BIGNUM const * x2;
  BN_CTX * x3;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;

  if (!PyArg_ParseTuple(args, "OOOO:BN_exp", &arg0, &arg1, &arg2, &arg3))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(8), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(8), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(34), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(34), arg1) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(34), arg2, (char **)&x2);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x2 = alloca(datasize);
    memset((void *)x2, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x2, _cffi_type(34), arg2) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(36), arg3, (char **)&x3);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x3 = alloca(datasize);
    memset((void *)x3, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x3, _cffi_type(36), arg3) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = BN_exp(x0, x1, x2, x3); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_BN_free(PyObject *self, PyObject *arg0)
{
  BIGNUM * x0;
  Py_ssize_t datasize;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(8), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(8), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { BN_free(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_BN_gcd(PyObject *self, PyObject *args)
{
  BIGNUM * x0;
  BIGNUM const * x1;
  BIGNUM const * x2;
  BN_CTX * x3;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;

  if (!PyArg_ParseTuple(args, "OOOO:BN_gcd", &arg0, &arg1, &arg2, &arg3))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(8), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(8), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(34), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(34), arg1) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(34), arg2, (char **)&x2);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x2 = alloca(datasize);
    memset((void *)x2, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x2, _cffi_type(34), arg2) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(36), arg3, (char **)&x3);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x3 = alloca(datasize);
    memset((void *)x3, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x3, _cffi_type(36), arg3) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = BN_gcd(x0, x1, x2, x3); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_BN_get_word(PyObject *self, PyObject *arg0)
{
  BIGNUM const * x0;
  Py_ssize_t datasize;
  uintptr_t result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(34), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(34), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = BN_get_word(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_UNSIGNED(result, uintptr_t);
}

static PyObject *
_cffi_f_BN_hex2bn(PyObject *self, PyObject *args)
{
  BIGNUM * * x0;
  char const * x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:BN_hex2bn", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(35), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(35), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(0), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(0), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = BN_hex2bn(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_BN_mod(PyObject *self, PyObject *args)
{
  BIGNUM * x0;
  BIGNUM const * x1;
  BIGNUM const * x2;
  BN_CTX * x3;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;

  if (!PyArg_ParseTuple(args, "OOOO:BN_mod", &arg0, &arg1, &arg2, &arg3))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(8), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(8), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(34), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(34), arg1) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(34), arg2, (char **)&x2);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x2 = alloca(datasize);
    memset((void *)x2, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x2, _cffi_type(34), arg2) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(36), arg3, (char **)&x3);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x3 = alloca(datasize);
    memset((void *)x3, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x3, _cffi_type(36), arg3) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = BN_mod(x0, x1, x2, x3); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_BN_mod_add(PyObject *self, PyObject *args)
{
  BIGNUM * x0;
  BIGNUM const * x1;
  BIGNUM const * x2;
  BIGNUM const * x3;
  BN_CTX * x4;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;
  PyObject *arg4;

  if (!PyArg_ParseTuple(args, "OOOOO:BN_mod_add", &arg0, &arg1, &arg2, &arg3, &arg4))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(8), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(8), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(34), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(34), arg1) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(34), arg2, (char **)&x2);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x2 = alloca(datasize);
    memset((void *)x2, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x2, _cffi_type(34), arg2) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(34), arg3, (char **)&x3);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x3 = alloca(datasize);
    memset((void *)x3, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x3, _cffi_type(34), arg3) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(36), arg4, (char **)&x4);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x4 = alloca(datasize);
    memset((void *)x4, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x4, _cffi_type(36), arg4) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = BN_mod_add(x0, x1, x2, x3, x4); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_BN_mod_exp(PyObject *self, PyObject *args)
{
  BIGNUM * x0;
  BIGNUM const * x1;
  BIGNUM const * x2;
  BIGNUM const * x3;
  BN_CTX * x4;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;
  PyObject *arg4;

  if (!PyArg_ParseTuple(args, "OOOOO:BN_mod_exp", &arg0, &arg1, &arg2, &arg3, &arg4))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(8), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(8), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(34), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(34), arg1) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(34), arg2, (char **)&x2);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x2 = alloca(datasize);
    memset((void *)x2, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x2, _cffi_type(34), arg2) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(34), arg3, (char **)&x3);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x3 = alloca(datasize);
    memset((void *)x3, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x3, _cffi_type(34), arg3) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(36), arg4, (char **)&x4);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x4 = alloca(datasize);
    memset((void *)x4, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x4, _cffi_type(36), arg4) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = BN_mod_exp(x0, x1, x2, x3, x4); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_BN_mod_inverse(PyObject *self, PyObject *args)
{
  BIGNUM * x0;
  BIGNUM const * x1;
  BIGNUM const * x2;
  BN_CTX * x3;
  Py_ssize_t datasize;
  BIGNUM * result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;

  if (!PyArg_ParseTuple(args, "OOOO:BN_mod_inverse", &arg0, &arg1, &arg2, &arg3))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(8), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(8), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(34), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(34), arg1) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(34), arg2, (char **)&x2);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x2 = alloca(datasize);
    memset((void *)x2, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x2, _cffi_type(34), arg2) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(36), arg3, (char **)&x3);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x3 = alloca(datasize);
    memset((void *)x3, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x3, _cffi_type(36), arg3) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = BN_mod_inverse(x0, x1, x2, x3); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(8));
}

static PyObject *
_cffi_f_BN_mod_mul(PyObject *self, PyObject *args)
{
  BIGNUM * x0;
  BIGNUM const * x1;
  BIGNUM const * x2;
  BIGNUM const * x3;
  BN_CTX * x4;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;
  PyObject *arg4;

  if (!PyArg_ParseTuple(args, "OOOOO:BN_mod_mul", &arg0, &arg1, &arg2, &arg3, &arg4))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(8), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(8), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(34), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(34), arg1) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(34), arg2, (char **)&x2);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x2 = alloca(datasize);
    memset((void *)x2, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x2, _cffi_type(34), arg2) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(34), arg3, (char **)&x3);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x3 = alloca(datasize);
    memset((void *)x3, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x3, _cffi_type(34), arg3) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(36), arg4, (char **)&x4);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x4 = alloca(datasize);
    memset((void *)x4, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x4, _cffi_type(36), arg4) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = BN_mod_mul(x0, x1, x2, x3, x4); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_BN_mod_sqr(PyObject *self, PyObject *args)
{
  BIGNUM * x0;
  BIGNUM const * x1;
  BIGNUM const * x2;
  BN_CTX * x3;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;

  if (!PyArg_ParseTuple(args, "OOOO:BN_mod_sqr", &arg0, &arg1, &arg2, &arg3))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(8), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(8), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(34), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(34), arg1) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(34), arg2, (char **)&x2);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x2 = alloca(datasize);
    memset((void *)x2, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x2, _cffi_type(34), arg2) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(36), arg3, (char **)&x3);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x3 = alloca(datasize);
    memset((void *)x3, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x3, _cffi_type(36), arg3) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = BN_mod_sqr(x0, x1, x2, x3); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_BN_mod_sub(PyObject *self, PyObject *args)
{
  BIGNUM * x0;
  BIGNUM const * x1;
  BIGNUM const * x2;
  BIGNUM const * x3;
  BN_CTX * x4;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;
  PyObject *arg4;

  if (!PyArg_ParseTuple(args, "OOOOO:BN_mod_sub", &arg0, &arg1, &arg2, &arg3, &arg4))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(8), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(8), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(34), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(34), arg1) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(34), arg2, (char **)&x2);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x2 = alloca(datasize);
    memset((void *)x2, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x2, _cffi_type(34), arg2) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(34), arg3, (char **)&x3);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x3 = alloca(datasize);
    memset((void *)x3, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x3, _cffi_type(34), arg3) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(36), arg4, (char **)&x4);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x4 = alloca(datasize);
    memset((void *)x4, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x4, _cffi_type(36), arg4) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = BN_mod_sub(x0, x1, x2, x3, x4); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_BN_mul(PyObject *self, PyObject *args)
{
  BIGNUM * x0;
  BIGNUM const * x1;
  BIGNUM const * x2;
  BN_CTX * x3;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;

  if (!PyArg_ParseTuple(args, "OOOO:BN_mul", &arg0, &arg1, &arg2, &arg3))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(8), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(8), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(34), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(34), arg1) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(34), arg2, (char **)&x2);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x2 = alloca(datasize);
    memset((void *)x2, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x2, _cffi_type(34), arg2) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(36), arg3, (char **)&x3);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x3 = alloca(datasize);
    memset((void *)x3, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x3, _cffi_type(36), arg3) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = BN_mul(x0, x1, x2, x3); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_BN_new(PyObject *self, PyObject *no_arg)
{
  BIGNUM * result;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = BN_new(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(8));
}

static PyObject *
_cffi_f_BN_nnmod(PyObject *self, PyObject *args)
{
  BIGNUM * x0;
  BIGNUM const * x1;
  BIGNUM const * x2;
  BN_CTX * x3;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;

  if (!PyArg_ParseTuple(args, "OOOO:BN_nnmod", &arg0, &arg1, &arg2, &arg3))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(8), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(8), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(34), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(34), arg1) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(34), arg2, (char **)&x2);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x2 = alloca(datasize);
    memset((void *)x2, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x2, _cffi_type(34), arg2) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(36), arg3, (char **)&x3);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x3 = alloca(datasize);
    memset((void *)x3, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x3, _cffi_type(36), arg3) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = BN_nnmod(x0, x1, x2, x3); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_BN_num_bits(PyObject *self, PyObject *arg0)
{
  BIGNUM const * x0;
  Py_ssize_t datasize;
  int result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(34), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(34), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = BN_num_bits(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_BN_one(PyObject *self, PyObject *arg0)
{
  BIGNUM * x0;
  Py_ssize_t datasize;
  int result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(8), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(8), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = BN_one(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_BN_set_word(PyObject *self, PyObject *args)
{
  BIGNUM * x0;
  uintptr_t x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:BN_set_word", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(8), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(8), arg0) < 0)
      return NULL;
  }

  x1 = _cffi_to_c_UNSIGNED(arg1, uintptr_t);
  if (x1 == (uintptr_t)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = BN_set_word(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_BN_sqr(PyObject *self, PyObject *args)
{
  BIGNUM * x0;
  BIGNUM const * x1;
  BN_CTX * x2;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;

  if (!PyArg_ParseTuple(args, "OOO:BN_sqr", &arg0, &arg1, &arg2))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(8), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(8), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(34), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(34), arg1) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(36), arg2, (char **)&x2);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x2 = alloca(datasize);
    memset((void *)x2, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x2, _cffi_type(36), arg2) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = BN_sqr(x0, x1, x2); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_BN_sub(PyObject *self, PyObject *args)
{
  BIGNUM * x0;
  BIGNUM const * x1;
  BIGNUM const * x2;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;

  if (!PyArg_ParseTuple(args, "OOO:BN_sub", &arg0, &arg1, &arg2))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(8), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(8), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(34), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(34), arg1) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(34), arg2, (char **)&x2);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x2 = alloca(datasize);
    memset((void *)x2, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x2, _cffi_type(34), arg2) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = BN_sub(x0, x1, x2); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_BN_to_ASN1_INTEGER(PyObject *self, PyObject *args)
{
  BIGNUM * x0;
  ASN1_INTEGER * x1;
  Py_ssize_t datasize;
  ASN1_INTEGER * result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:BN_to_ASN1_INTEGER", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(8), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(8), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(7), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(7), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = BN_to_ASN1_INTEGER(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(7));
}

static PyObject *
_cffi_f_BN_value_one(PyObject *self, PyObject *no_arg)
{
  BIGNUM const * result;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = BN_value_one(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(34));
}

static PyObject *
_cffi_f_BN_zero(PyObject *self, PyObject *arg0)
{
  BIGNUM * x0;
  Py_ssize_t datasize;
  int result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(8), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(8), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = BN_zero(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_CRYPTO_add(PyObject *self, PyObject *args)
{
  int * x0;
  int x1;
  int x2;
  Py_ssize_t datasize;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;

  if (!PyArg_ParseTuple(args, "OOO:CRYPTO_add", &arg0, &arg1, &arg2))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(37), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(37), arg0) < 0)
      return NULL;
  }

  x1 = _cffi_to_c_SIGNED(arg1, int);
  if (x1 == (int)-1 && PyErr_Occurred())
    return NULL;

  x2 = _cffi_to_c_SIGNED(arg2, int);
  if (x2 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { CRYPTO_add(x0, x1, x2); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_CRYPTO_cleanup_all_ex_data(PyObject *self, PyObject *no_arg)
{

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { CRYPTO_cleanup_all_ex_data(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_CRYPTO_free(PyObject *self, PyObject *arg0)
{
  void * x0;
  Py_ssize_t datasize;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(25), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(25), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { CRYPTO_free(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_CRYPTO_get_id_callback(PyObject *self, PyObject *no_arg)
{
  unsigned long(* result)(void);

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = CRYPTO_get_id_callback(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(38));
}

static PyObject *
_cffi_f_CRYPTO_get_locking_callback(PyObject *self, PyObject *no_arg)
{
  void(* result)(int, int, char const *, int);

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = CRYPTO_get_locking_callback(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(39));
}

static PyObject *
_cffi_f_CRYPTO_is_mem_check_on(PyObject *self, PyObject *no_arg)
{
  int result;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = CRYPTO_is_mem_check_on(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_CRYPTO_lock(PyObject *self, PyObject *args)
{
  int x0;
  int x1;
  char const * x2;
  int x3;
  Py_ssize_t datasize;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;

  if (!PyArg_ParseTuple(args, "OOOO:CRYPTO_lock", &arg0, &arg1, &arg2, &arg3))
    return NULL;

  x0 = _cffi_to_c_SIGNED(arg0, int);
  if (x0 == (int)-1 && PyErr_Occurred())
    return NULL;

  x1 = _cffi_to_c_SIGNED(arg1, int);
  if (x1 == (int)-1 && PyErr_Occurred())
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(0), arg2, (char **)&x2);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x2 = alloca(datasize);
    memset((void *)x2, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x2, _cffi_type(0), arg2) < 0)
      return NULL;
  }

  x3 = _cffi_to_c_SIGNED(arg3, int);
  if (x3 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { CRYPTO_lock(x0, x1, x2, x3); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_CRYPTO_malloc_debug_init(PyObject *self, PyObject *no_arg)
{

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { CRYPTO_malloc_debug_init(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_CRYPTO_malloc_init(PyObject *self, PyObject *no_arg)
{

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { CRYPTO_malloc_init(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_CRYPTO_mem_ctrl(PyObject *self, PyObject *arg0)
{
  int x0;
  int result;

  x0 = _cffi_to_c_SIGNED(arg0, int);
  if (x0 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = CRYPTO_mem_ctrl(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_CRYPTO_mem_leaks(PyObject *self, PyObject *arg0)
{
  BIO * x0;
  Py_ssize_t datasize;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(22), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(22), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { CRYPTO_mem_leaks(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_CRYPTO_num_locks(PyObject *self, PyObject *no_arg)
{
  int result;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = CRYPTO_num_locks(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_CRYPTO_set_id_callback(PyObject *self, PyObject *arg0)
{
  unsigned long(* x0)(void);

  x0 = (unsigned long(*)(void))_cffi_to_c_pointer(arg0, _cffi_type(38));
  if (x0 == (unsigned long(*)(void))NULL && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { CRYPTO_set_id_callback(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_CRYPTO_set_locking_callback(PyObject *self, PyObject *arg0)
{
  void(* x0)(int, int, char const *, int);

  x0 = (void(*)(int, int, char const *, int))_cffi_to_c_pointer(arg0, _cffi_type(39));
  if (x0 == (void(*)(int, int, char const *, int))NULL && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { CRYPTO_set_locking_callback(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_Cryptography_HMAC_CTX_copy(PyObject *self, PyObject *args)
{
  HMAC_CTX * x0;
  HMAC_CTX * x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:Cryptography_HMAC_CTX_copy", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(40), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(40), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(40), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(40), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = Cryptography_HMAC_CTX_copy(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_Cryptography_HMAC_Final(PyObject *self, PyObject *args)
{
  HMAC_CTX * x0;
  unsigned char * x1;
  unsigned int * x2;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;

  if (!PyArg_ParseTuple(args, "OOO:Cryptography_HMAC_Final", &arg0, &arg1, &arg2))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(40), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(40), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(3), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(3), arg1) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(41), arg2, (char **)&x2);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x2 = alloca(datasize);
    memset((void *)x2, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x2, _cffi_type(41), arg2) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = Cryptography_HMAC_Final(x0, x1, x2); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_Cryptography_HMAC_Init_ex(PyObject *self, PyObject *args)
{
  HMAC_CTX * x0;
  void const * x1;
  int x2;
  EVP_MD const * x3;
  ENGINE * x4;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;
  PyObject *arg4;

  if (!PyArg_ParseTuple(args, "OOOOO:Cryptography_HMAC_Init_ex", &arg0, &arg1, &arg2, &arg3, &arg4))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(40), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(40), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(14), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(14), arg1) < 0)
      return NULL;
  }

  x2 = _cffi_to_c_SIGNED(arg2, int);
  if (x2 == (int)-1 && PyErr_Occurred())
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(42), arg3, (char **)&x3);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x3 = alloca(datasize);
    memset((void *)x3, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x3, _cffi_type(42), arg3) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(43), arg4, (char **)&x4);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x4 = alloca(datasize);
    memset((void *)x4, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x4, _cffi_type(43), arg4) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = Cryptography_HMAC_Init_ex(x0, x1, x2, x3, x4); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_Cryptography_HMAC_Update(PyObject *self, PyObject *args)
{
  HMAC_CTX * x0;
  unsigned char const * x1;
  size_t x2;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;

  if (!PyArg_ParseTuple(args, "OOO:Cryptography_HMAC_Update", &arg0, &arg1, &arg2))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(40), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(40), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(1), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(1), arg1) < 0)
      return NULL;
  }

  x2 = _cffi_to_c_UNSIGNED(arg2, size_t);
  if (x2 == (size_t)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = Cryptography_HMAC_Update(x0, x1, x2); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_Cryptography_add_osrandom_engine(PyObject *self, PyObject *no_arg)
{
  int result;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = Cryptography_add_osrandom_engine(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_DH_free(PyObject *self, PyObject *arg0)
{
  DH * x0;
  Py_ssize_t datasize;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(44), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(44), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { DH_free(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_DH_new(PyObject *self, PyObject *no_arg)
{
  DH * result;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = DH_new(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(44));
}

static PyObject *
_cffi_f_DSA_free(PyObject *self, PyObject *arg0)
{
  DSA * x0;
  Py_ssize_t datasize;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(45), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(45), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { DSA_free(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_DSA_generate_key(PyObject *self, PyObject *arg0)
{
  DSA * x0;
  Py_ssize_t datasize;
  int result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(45), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(45), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = DSA_generate_key(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_DSA_generate_parameters(PyObject *self, PyObject *args)
{
  int x0;
  unsigned char * x1;
  int x2;
  int * x3;
  unsigned long * x4;
  void(* x5)(int, int, void *);
  void * x6;
  Py_ssize_t datasize;
  DSA * result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;
  PyObject *arg4;
  PyObject *arg5;
  PyObject *arg6;

  if (!PyArg_ParseTuple(args, "OOOOOOO:DSA_generate_parameters", &arg0, &arg1, &arg2, &arg3, &arg4, &arg5, &arg6))
    return NULL;

  x0 = _cffi_to_c_SIGNED(arg0, int);
  if (x0 == (int)-1 && PyErr_Occurred())
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(3), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(3), arg1) < 0)
      return NULL;
  }

  x2 = _cffi_to_c_SIGNED(arg2, int);
  if (x2 == (int)-1 && PyErr_Occurred())
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(37), arg3, (char **)&x3);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x3 = alloca(datasize);
    memset((void *)x3, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x3, _cffi_type(37), arg3) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(46), arg4, (char **)&x4);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x4 = alloca(datasize);
    memset((void *)x4, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x4, _cffi_type(46), arg4) < 0)
      return NULL;
  }

  x5 = (void(*)(int, int, void *))_cffi_to_c_pointer(arg5, _cffi_type(47));
  if (x5 == (void(*)(int, int, void *))NULL && PyErr_Occurred())
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(25), arg6, (char **)&x6);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x6 = alloca(datasize);
    memset((void *)x6, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x6, _cffi_type(25), arg6) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = DSA_generate_parameters(x0, x1, x2, x3, x4, x5, x6); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(45));
}

static PyObject *
_cffi_f_DTLSv1_client_method(PyObject *self, PyObject *no_arg)
{
  SSL_METHOD const * result;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = DTLSv1_client_method(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(48));
}

static PyObject *
_cffi_f_DTLSv1_method(PyObject *self, PyObject *no_arg)
{
  SSL_METHOD const * result;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = DTLSv1_method(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(48));
}

static PyObject *
_cffi_f_DTLSv1_server_method(PyObject *self, PyObject *no_arg)
{
  SSL_METHOD const * result;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = DTLSv1_server_method(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(48));
}

static PyObject *
_cffi_f_EC_KEY_free(PyObject *self, PyObject *arg0)
{
  EC_KEY * x0;
  Py_ssize_t datasize;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(49), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(49), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { EC_KEY_free(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_EC_KEY_new_by_curve_name(PyObject *self, PyObject *arg0)
{
  int x0;
  EC_KEY * result;

  x0 = _cffi_to_c_SIGNED(arg0, int);
  if (x0 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = EC_KEY_new_by_curve_name(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(49));
}

static PyObject *
_cffi_f_ENGINE_add(PyObject *self, PyObject *arg0)
{
  ENGINE * x0;
  Py_ssize_t datasize;
  int result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(43), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(43), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ENGINE_add(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_ENGINE_add_conf_module(PyObject *self, PyObject *no_arg)
{

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { ENGINE_add_conf_module(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_ENGINE_by_id(PyObject *self, PyObject *arg0)
{
  char const * x0;
  Py_ssize_t datasize;
  ENGINE * result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(0), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(0), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ENGINE_by_id(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(43));
}

static PyObject *
_cffi_f_ENGINE_cleanup(PyObject *self, PyObject *no_arg)
{

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { ENGINE_cleanup(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_ENGINE_cmd_is_executable(PyObject *self, PyObject *args)
{
  ENGINE * x0;
  int x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:ENGINE_cmd_is_executable", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(43), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(43), arg0) < 0)
      return NULL;
  }

  x1 = _cffi_to_c_SIGNED(arg1, int);
  if (x1 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ENGINE_cmd_is_executable(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_ENGINE_ctrl(PyObject *self, PyObject *args)
{
  ENGINE * x0;
  int x1;
  long x2;
  void * x3;
  void(* x4)(void);
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;
  PyObject *arg4;

  if (!PyArg_ParseTuple(args, "OOOOO:ENGINE_ctrl", &arg0, &arg1, &arg2, &arg3, &arg4))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(43), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(43), arg0) < 0)
      return NULL;
  }

  x1 = _cffi_to_c_SIGNED(arg1, int);
  if (x1 == (int)-1 && PyErr_Occurred())
    return NULL;

  x2 = _cffi_to_c_SIGNED(arg2, long);
  if (x2 == (long)-1 && PyErr_Occurred())
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(25), arg3, (char **)&x3);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x3 = alloca(datasize);
    memset((void *)x3, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x3, _cffi_type(25), arg3) < 0)
      return NULL;
  }

  x4 = (void(*)(void))_cffi_to_c_pointer(arg4, _cffi_type(50));
  if (x4 == (void(*)(void))NULL && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ENGINE_ctrl(x0, x1, x2, x3, x4); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_ENGINE_ctrl_cmd(PyObject *self, PyObject *args)
{
  ENGINE * x0;
  char const * x1;
  long x2;
  void * x3;
  void(* x4)(void);
  int x5;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;
  PyObject *arg4;
  PyObject *arg5;

  if (!PyArg_ParseTuple(args, "OOOOOO:ENGINE_ctrl_cmd", &arg0, &arg1, &arg2, &arg3, &arg4, &arg5))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(43), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(43), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(0), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(0), arg1) < 0)
      return NULL;
  }

  x2 = _cffi_to_c_SIGNED(arg2, long);
  if (x2 == (long)-1 && PyErr_Occurred())
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(25), arg3, (char **)&x3);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x3 = alloca(datasize);
    memset((void *)x3, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x3, _cffi_type(25), arg3) < 0)
      return NULL;
  }

  x4 = (void(*)(void))_cffi_to_c_pointer(arg4, _cffi_type(50));
  if (x4 == (void(*)(void))NULL && PyErr_Occurred())
    return NULL;

  x5 = _cffi_to_c_SIGNED(arg5, int);
  if (x5 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ENGINE_ctrl_cmd(x0, x1, x2, x3, x4, x5); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_ENGINE_ctrl_cmd_string(PyObject *self, PyObject *args)
{
  ENGINE * x0;
  char const * x1;
  char const * x2;
  int x3;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;

  if (!PyArg_ParseTuple(args, "OOOO:ENGINE_ctrl_cmd_string", &arg0, &arg1, &arg2, &arg3))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(43), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(43), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(0), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(0), arg1) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(0), arg2, (char **)&x2);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x2 = alloca(datasize);
    memset((void *)x2, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x2, _cffi_type(0), arg2) < 0)
      return NULL;
  }

  x3 = _cffi_to_c_SIGNED(arg3, int);
  if (x3 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ENGINE_ctrl_cmd_string(x0, x1, x2, x3); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_ENGINE_finish(PyObject *self, PyObject *arg0)
{
  ENGINE * x0;
  Py_ssize_t datasize;
  int result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(43), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(43), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ENGINE_finish(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_ENGINE_free(PyObject *self, PyObject *arg0)
{
  ENGINE * x0;
  Py_ssize_t datasize;
  int result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(43), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(43), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ENGINE_free(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_ENGINE_get_DH(PyObject *self, PyObject *arg0)
{
  ENGINE const * x0;
  Py_ssize_t datasize;
  DH_METHOD const * result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(51), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(51), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ENGINE_get_DH(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(52));
}

static PyObject *
_cffi_f_ENGINE_get_DSA(PyObject *self, PyObject *arg0)
{
  ENGINE const * x0;
  Py_ssize_t datasize;
  DSA_METHOD const * result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(51), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(51), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ENGINE_get_DSA(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(53));
}

static PyObject *
_cffi_f_ENGINE_get_ECDH(PyObject *self, PyObject *arg0)
{
  ENGINE const * x0;
  Py_ssize_t datasize;
  ECDH_METHOD const * result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(51), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(51), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ENGINE_get_ECDH(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(54));
}

static PyObject *
_cffi_f_ENGINE_get_ECDSA(PyObject *self, PyObject *arg0)
{
  ENGINE const * x0;
  Py_ssize_t datasize;
  ECDSA_METHOD const * result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(51), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(51), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ENGINE_get_ECDSA(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(55));
}

static PyObject *
_cffi_f_ENGINE_get_RAND(PyObject *self, PyObject *arg0)
{
  ENGINE const * x0;
  Py_ssize_t datasize;
  RAND_METHOD const * result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(51), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(51), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ENGINE_get_RAND(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(56));
}

static PyObject *
_cffi_f_ENGINE_get_RSA(PyObject *self, PyObject *arg0)
{
  ENGINE const * x0;
  Py_ssize_t datasize;
  RSA_METHOD const * result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(51), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(51), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ENGINE_get_RSA(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(57));
}

static PyObject *
_cffi_f_ENGINE_get_STORE(PyObject *self, PyObject *arg0)
{
  ENGINE const * x0;
  Py_ssize_t datasize;
  STORE_METHOD const * result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(51), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(51), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ENGINE_get_STORE(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(58));
}

static PyObject *
_cffi_f_ENGINE_get_cipher(PyObject *self, PyObject *args)
{
  ENGINE * x0;
  int x1;
  Py_ssize_t datasize;
  EVP_CIPHER const * result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:ENGINE_get_cipher", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(43), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(43), arg0) < 0)
      return NULL;
  }

  x1 = _cffi_to_c_SIGNED(arg1, int);
  if (x1 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ENGINE_get_cipher(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(59));
}

static PyObject *
_cffi_f_ENGINE_get_cipher_engine(PyObject *self, PyObject *arg0)
{
  int x0;
  ENGINE * result;

  x0 = _cffi_to_c_SIGNED(arg0, int);
  if (x0 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ENGINE_get_cipher_engine(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(43));
}

static PyObject *
_cffi_f_ENGINE_get_cmd_defns(PyObject *self, PyObject *arg0)
{
  ENGINE const * x0;
  Py_ssize_t datasize;
  ENGINE_CMD_DEFN const * result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(51), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(51), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ENGINE_get_cmd_defns(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(60));
}

static PyObject *
_cffi_f_ENGINE_get_default_DH(PyObject *self, PyObject *no_arg)
{
  ENGINE * result;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ENGINE_get_default_DH(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(43));
}

static PyObject *
_cffi_f_ENGINE_get_default_DSA(PyObject *self, PyObject *no_arg)
{
  ENGINE * result;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ENGINE_get_default_DSA(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(43));
}

static PyObject *
_cffi_f_ENGINE_get_default_ECDH(PyObject *self, PyObject *no_arg)
{
  ENGINE * result;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ENGINE_get_default_ECDH(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(43));
}

static PyObject *
_cffi_f_ENGINE_get_default_ECDSA(PyObject *self, PyObject *no_arg)
{
  ENGINE * result;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ENGINE_get_default_ECDSA(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(43));
}

static PyObject *
_cffi_f_ENGINE_get_default_RAND(PyObject *self, PyObject *no_arg)
{
  ENGINE * result;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ENGINE_get_default_RAND(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(43));
}

static PyObject *
_cffi_f_ENGINE_get_default_RSA(PyObject *self, PyObject *no_arg)
{
  ENGINE * result;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ENGINE_get_default_RSA(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(43));
}

static PyObject *
_cffi_f_ENGINE_get_digest(PyObject *self, PyObject *args)
{
  ENGINE * x0;
  int x1;
  Py_ssize_t datasize;
  EVP_MD const * result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:ENGINE_get_digest", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(43), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(43), arg0) < 0)
      return NULL;
  }

  x1 = _cffi_to_c_SIGNED(arg1, int);
  if (x1 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ENGINE_get_digest(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(42));
}

static PyObject *
_cffi_f_ENGINE_get_digest_engine(PyObject *self, PyObject *arg0)
{
  int x0;
  ENGINE * result;

  x0 = _cffi_to_c_SIGNED(arg0, int);
  if (x0 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ENGINE_get_digest_engine(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(43));
}

static PyObject *
_cffi_f_ENGINE_get_first(PyObject *self, PyObject *no_arg)
{
  ENGINE * result;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ENGINE_get_first(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(43));
}

static PyObject *
_cffi_f_ENGINE_get_flags(PyObject *self, PyObject *arg0)
{
  ENGINE const * x0;
  Py_ssize_t datasize;
  int result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(51), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(51), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ENGINE_get_flags(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_ENGINE_get_id(PyObject *self, PyObject *arg0)
{
  ENGINE const * x0;
  Py_ssize_t datasize;
  char const * result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(51), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(51), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ENGINE_get_id(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(0));
}

static PyObject *
_cffi_f_ENGINE_get_last(PyObject *self, PyObject *no_arg)
{
  ENGINE * result;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ENGINE_get_last(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(43));
}

static PyObject *
_cffi_f_ENGINE_get_name(PyObject *self, PyObject *arg0)
{
  ENGINE const * x0;
  Py_ssize_t datasize;
  char const * result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(51), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(51), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ENGINE_get_name(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(0));
}

static PyObject *
_cffi_f_ENGINE_get_next(PyObject *self, PyObject *arg0)
{
  ENGINE * x0;
  Py_ssize_t datasize;
  ENGINE * result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(43), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(43), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ENGINE_get_next(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(43));
}

static PyObject *
_cffi_f_ENGINE_get_prev(PyObject *self, PyObject *arg0)
{
  ENGINE * x0;
  Py_ssize_t datasize;
  ENGINE * result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(43), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(43), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ENGINE_get_prev(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(43));
}

static PyObject *
_cffi_f_ENGINE_get_table_flags(PyObject *self, PyObject *no_arg)
{
  unsigned int result;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ENGINE_get_table_flags(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_UNSIGNED(result, unsigned int);
}

static PyObject *
_cffi_f_ENGINE_init(PyObject *self, PyObject *arg0)
{
  ENGINE * x0;
  Py_ssize_t datasize;
  int result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(43), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(43), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ENGINE_init(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_ENGINE_load_builtin_engines(PyObject *self, PyObject *no_arg)
{

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { ENGINE_load_builtin_engines(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_ENGINE_load_cryptodev(PyObject *self, PyObject *no_arg)
{

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { ENGINE_load_cryptodev(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_ENGINE_load_dynamic(PyObject *self, PyObject *no_arg)
{

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { ENGINE_load_dynamic(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_ENGINE_load_openssl(PyObject *self, PyObject *no_arg)
{

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { ENGINE_load_openssl(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_ENGINE_load_private_key(PyObject *self, PyObject *args)
{
  ENGINE * x0;
  char const * x1;
  UI_METHOD * x2;
  void * x3;
  Py_ssize_t datasize;
  EVP_PKEY * result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;

  if (!PyArg_ParseTuple(args, "OOOO:ENGINE_load_private_key", &arg0, &arg1, &arg2, &arg3))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(43), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(43), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(0), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(0), arg1) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(61), arg2, (char **)&x2);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x2 = alloca(datasize);
    memset((void *)x2, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x2, _cffi_type(61), arg2) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(25), arg3, (char **)&x3);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x3 = alloca(datasize);
    memset((void *)x3, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x3, _cffi_type(25), arg3) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ENGINE_load_private_key(x0, x1, x2, x3); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(62));
}

static PyObject *
_cffi_f_ENGINE_load_public_key(PyObject *self, PyObject *args)
{
  ENGINE * x0;
  char const * x1;
  UI_METHOD * x2;
  void * x3;
  Py_ssize_t datasize;
  EVP_PKEY * result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;

  if (!PyArg_ParseTuple(args, "OOOO:ENGINE_load_public_key", &arg0, &arg1, &arg2, &arg3))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(43), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(43), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(0), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(0), arg1) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(61), arg2, (char **)&x2);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x2 = alloca(datasize);
    memset((void *)x2, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x2, _cffi_type(61), arg2) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(25), arg3, (char **)&x3);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x3 = alloca(datasize);
    memset((void *)x3, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x3, _cffi_type(25), arg3) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ENGINE_load_public_key(x0, x1, x2, x3); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(62));
}

static PyObject *
_cffi_f_ENGINE_new(PyObject *self, PyObject *no_arg)
{
  ENGINE * result;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ENGINE_new(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(43));
}

static PyObject *
_cffi_f_ENGINE_register_DH(PyObject *self, PyObject *arg0)
{
  ENGINE * x0;
  Py_ssize_t datasize;
  int result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(43), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(43), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ENGINE_register_DH(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_ENGINE_register_DSA(PyObject *self, PyObject *arg0)
{
  ENGINE * x0;
  Py_ssize_t datasize;
  int result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(43), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(43), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ENGINE_register_DSA(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_ENGINE_register_ECDH(PyObject *self, PyObject *arg0)
{
  ENGINE * x0;
  Py_ssize_t datasize;
  int result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(43), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(43), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ENGINE_register_ECDH(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_ENGINE_register_ECDSA(PyObject *self, PyObject *arg0)
{
  ENGINE * x0;
  Py_ssize_t datasize;
  int result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(43), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(43), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ENGINE_register_ECDSA(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_ENGINE_register_RAND(PyObject *self, PyObject *arg0)
{
  ENGINE * x0;
  Py_ssize_t datasize;
  int result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(43), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(43), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ENGINE_register_RAND(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_ENGINE_register_RSA(PyObject *self, PyObject *arg0)
{
  ENGINE * x0;
  Py_ssize_t datasize;
  int result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(43), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(43), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ENGINE_register_RSA(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_ENGINE_register_STORE(PyObject *self, PyObject *arg0)
{
  ENGINE * x0;
  Py_ssize_t datasize;
  int result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(43), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(43), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ENGINE_register_STORE(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_ENGINE_register_all_DH(PyObject *self, PyObject *no_arg)
{

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { ENGINE_register_all_DH(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_ENGINE_register_all_DSA(PyObject *self, PyObject *no_arg)
{

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { ENGINE_register_all_DSA(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_ENGINE_register_all_ECDH(PyObject *self, PyObject *no_arg)
{

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { ENGINE_register_all_ECDH(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_ENGINE_register_all_ECDSA(PyObject *self, PyObject *no_arg)
{

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { ENGINE_register_all_ECDSA(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_ENGINE_register_all_RAND(PyObject *self, PyObject *no_arg)
{

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { ENGINE_register_all_RAND(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_ENGINE_register_all_RSA(PyObject *self, PyObject *no_arg)
{

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { ENGINE_register_all_RSA(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_ENGINE_register_all_STORE(PyObject *self, PyObject *no_arg)
{

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { ENGINE_register_all_STORE(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_ENGINE_register_all_ciphers(PyObject *self, PyObject *no_arg)
{

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { ENGINE_register_all_ciphers(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_ENGINE_register_all_complete(PyObject *self, PyObject *no_arg)
{
  int result;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ENGINE_register_all_complete(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_ENGINE_register_all_digests(PyObject *self, PyObject *no_arg)
{

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { ENGINE_register_all_digests(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_ENGINE_register_ciphers(PyObject *self, PyObject *arg0)
{
  ENGINE * x0;
  Py_ssize_t datasize;
  int result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(43), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(43), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ENGINE_register_ciphers(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_ENGINE_register_complete(PyObject *self, PyObject *arg0)
{
  ENGINE * x0;
  Py_ssize_t datasize;
  int result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(43), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(43), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ENGINE_register_complete(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_ENGINE_register_digests(PyObject *self, PyObject *arg0)
{
  ENGINE * x0;
  Py_ssize_t datasize;
  int result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(43), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(43), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ENGINE_register_digests(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_ENGINE_remove(PyObject *self, PyObject *arg0)
{
  ENGINE * x0;
  Py_ssize_t datasize;
  int result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(43), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(43), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ENGINE_remove(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_ENGINE_set_DH(PyObject *self, PyObject *args)
{
  ENGINE * x0;
  DH_METHOD const * x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:ENGINE_set_DH", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(43), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(43), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(52), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(52), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ENGINE_set_DH(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_ENGINE_set_DSA(PyObject *self, PyObject *args)
{
  ENGINE * x0;
  DSA_METHOD const * x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:ENGINE_set_DSA", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(43), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(43), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(53), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(53), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ENGINE_set_DSA(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_ENGINE_set_ECDH(PyObject *self, PyObject *args)
{
  ENGINE * x0;
  ECDH_METHOD const * x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:ENGINE_set_ECDH", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(43), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(43), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(54), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(54), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ENGINE_set_ECDH(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_ENGINE_set_ECDSA(PyObject *self, PyObject *args)
{
  ENGINE * x0;
  ECDSA_METHOD const * x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:ENGINE_set_ECDSA", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(43), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(43), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(55), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(55), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ENGINE_set_ECDSA(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_ENGINE_set_RAND(PyObject *self, PyObject *args)
{
  ENGINE * x0;
  RAND_METHOD const * x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:ENGINE_set_RAND", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(43), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(43), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(56), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(56), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ENGINE_set_RAND(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_ENGINE_set_RSA(PyObject *self, PyObject *args)
{
  ENGINE * x0;
  RSA_METHOD const * x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:ENGINE_set_RSA", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(43), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(43), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(57), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(57), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ENGINE_set_RSA(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_ENGINE_set_STORE(PyObject *self, PyObject *args)
{
  ENGINE * x0;
  STORE_METHOD const * x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:ENGINE_set_STORE", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(43), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(43), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(58), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(58), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ENGINE_set_STORE(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_ENGINE_set_ciphers(PyObject *self, PyObject *args)
{
  ENGINE * x0;
  ENGINE_CIPHERS_PTR x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:ENGINE_set_ciphers", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(43), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(43), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(63), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(63), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ENGINE_set_ciphers(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_ENGINE_set_cmd_defns(PyObject *self, PyObject *args)
{
  ENGINE * x0;
  ENGINE_CMD_DEFN const * x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:ENGINE_set_cmd_defns", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(43), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(43), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(60), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(60), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ENGINE_set_cmd_defns(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_ENGINE_set_ctrl_function(PyObject *self, PyObject *args)
{
  ENGINE * x0;
  ENGINE_CTRL_FUNC_PTR x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:ENGINE_set_ctrl_function", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(43), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(43), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(64), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(64), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ENGINE_set_ctrl_function(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_ENGINE_set_default(PyObject *self, PyObject *args)
{
  ENGINE * x0;
  unsigned int x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:ENGINE_set_default", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(43), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(43), arg0) < 0)
      return NULL;
  }

  x1 = _cffi_to_c_UNSIGNED(arg1, unsigned int);
  if (x1 == (unsigned int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ENGINE_set_default(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_ENGINE_set_default_DH(PyObject *self, PyObject *arg0)
{
  ENGINE * x0;
  Py_ssize_t datasize;
  int result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(43), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(43), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ENGINE_set_default_DH(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_ENGINE_set_default_DSA(PyObject *self, PyObject *arg0)
{
  ENGINE * x0;
  Py_ssize_t datasize;
  int result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(43), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(43), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ENGINE_set_default_DSA(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_ENGINE_set_default_ECDH(PyObject *self, PyObject *arg0)
{
  ENGINE * x0;
  Py_ssize_t datasize;
  int result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(43), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(43), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ENGINE_set_default_ECDH(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_ENGINE_set_default_ECDSA(PyObject *self, PyObject *arg0)
{
  ENGINE * x0;
  Py_ssize_t datasize;
  int result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(43), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(43), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ENGINE_set_default_ECDSA(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_ENGINE_set_default_RAND(PyObject *self, PyObject *arg0)
{
  ENGINE * x0;
  Py_ssize_t datasize;
  int result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(43), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(43), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ENGINE_set_default_RAND(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_ENGINE_set_default_RSA(PyObject *self, PyObject *arg0)
{
  ENGINE * x0;
  Py_ssize_t datasize;
  int result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(43), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(43), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ENGINE_set_default_RSA(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_ENGINE_set_default_ciphers(PyObject *self, PyObject *arg0)
{
  ENGINE * x0;
  Py_ssize_t datasize;
  int result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(43), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(43), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ENGINE_set_default_ciphers(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_ENGINE_set_default_digests(PyObject *self, PyObject *arg0)
{
  ENGINE * x0;
  Py_ssize_t datasize;
  int result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(43), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(43), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ENGINE_set_default_digests(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_ENGINE_set_default_string(PyObject *self, PyObject *args)
{
  ENGINE * x0;
  char const * x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:ENGINE_set_default_string", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(43), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(43), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(0), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(0), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ENGINE_set_default_string(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_ENGINE_set_destroy_function(PyObject *self, PyObject *args)
{
  ENGINE * x0;
  ENGINE_GEN_INT_FUNC_PTR x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:ENGINE_set_destroy_function", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(43), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(43), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(65), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(65), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ENGINE_set_destroy_function(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_ENGINE_set_digests(PyObject *self, PyObject *args)
{
  ENGINE * x0;
  ENGINE_DIGESTS_PTR x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:ENGINE_set_digests", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(43), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(43), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(66), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(66), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ENGINE_set_digests(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_ENGINE_set_finish_function(PyObject *self, PyObject *args)
{
  ENGINE * x0;
  ENGINE_GEN_INT_FUNC_PTR x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:ENGINE_set_finish_function", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(43), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(43), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(65), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(65), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ENGINE_set_finish_function(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_ENGINE_set_flags(PyObject *self, PyObject *args)
{
  ENGINE * x0;
  int x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:ENGINE_set_flags", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(43), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(43), arg0) < 0)
      return NULL;
  }

  x1 = _cffi_to_c_SIGNED(arg1, int);
  if (x1 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ENGINE_set_flags(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_ENGINE_set_id(PyObject *self, PyObject *args)
{
  ENGINE * x0;
  char const * x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:ENGINE_set_id", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(43), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(43), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(0), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(0), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ENGINE_set_id(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_ENGINE_set_init_function(PyObject *self, PyObject *args)
{
  ENGINE * x0;
  ENGINE_GEN_INT_FUNC_PTR x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:ENGINE_set_init_function", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(43), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(43), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(65), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(65), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ENGINE_set_init_function(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_ENGINE_set_load_privkey_function(PyObject *self, PyObject *args)
{
  ENGINE * x0;
  ENGINE_LOAD_KEY_PTR x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:ENGINE_set_load_privkey_function", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(43), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(43), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(67), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(67), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ENGINE_set_load_privkey_function(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_ENGINE_set_load_pubkey_function(PyObject *self, PyObject *args)
{
  ENGINE * x0;
  ENGINE_LOAD_KEY_PTR x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:ENGINE_set_load_pubkey_function", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(43), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(43), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(67), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(67), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ENGINE_set_load_pubkey_function(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_ENGINE_set_name(PyObject *self, PyObject *args)
{
  ENGINE * x0;
  char const * x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:ENGINE_set_name", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(43), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(43), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(0), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(0), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ENGINE_set_name(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_ENGINE_set_table_flags(PyObject *self, PyObject *arg0)
{
  unsigned int x0;

  x0 = _cffi_to_c_UNSIGNED(arg0, unsigned int);
  if (x0 == (unsigned int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { ENGINE_set_table_flags(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_ENGINE_unregister_DH(PyObject *self, PyObject *arg0)
{
  ENGINE * x0;
  Py_ssize_t datasize;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(43), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(43), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { ENGINE_unregister_DH(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_ENGINE_unregister_DSA(PyObject *self, PyObject *arg0)
{
  ENGINE * x0;
  Py_ssize_t datasize;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(43), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(43), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { ENGINE_unregister_DSA(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_ENGINE_unregister_ECDH(PyObject *self, PyObject *arg0)
{
  ENGINE * x0;
  Py_ssize_t datasize;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(43), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(43), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { ENGINE_unregister_ECDH(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_ENGINE_unregister_ECDSA(PyObject *self, PyObject *arg0)
{
  ENGINE * x0;
  Py_ssize_t datasize;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(43), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(43), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { ENGINE_unregister_ECDSA(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_ENGINE_unregister_RAND(PyObject *self, PyObject *arg0)
{
  ENGINE * x0;
  Py_ssize_t datasize;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(43), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(43), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { ENGINE_unregister_RAND(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_ENGINE_unregister_RSA(PyObject *self, PyObject *arg0)
{
  ENGINE * x0;
  Py_ssize_t datasize;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(43), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(43), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { ENGINE_unregister_RSA(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_ENGINE_unregister_STORE(PyObject *self, PyObject *arg0)
{
  ENGINE * x0;
  Py_ssize_t datasize;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(43), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(43), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { ENGINE_unregister_STORE(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_ENGINE_unregister_ciphers(PyObject *self, PyObject *arg0)
{
  ENGINE * x0;
  Py_ssize_t datasize;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(43), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(43), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { ENGINE_unregister_ciphers(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_ENGINE_unregister_digests(PyObject *self, PyObject *arg0)
{
  ENGINE * x0;
  Py_ssize_t datasize;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(43), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(43), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { ENGINE_unregister_digests(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_ENGINE_up_ref(PyObject *self, PyObject *arg0)
{
  ENGINE * x0;
  Py_ssize_t datasize;
  int result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(43), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(43), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ENGINE_up_ref(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_ERR_FATAL_ERROR(PyObject *self, PyObject *arg0)
{
  unsigned long x0;
  int result;

  x0 = _cffi_to_c_UNSIGNED(arg0, unsigned long);
  if (x0 == (unsigned long)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ERR_FATAL_ERROR(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_ERR_GET_FUNC(PyObject *self, PyObject *arg0)
{
  unsigned long x0;
  int result;

  x0 = _cffi_to_c_UNSIGNED(arg0, unsigned long);
  if (x0 == (unsigned long)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ERR_GET_FUNC(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_ERR_GET_LIB(PyObject *self, PyObject *arg0)
{
  unsigned long x0;
  int result;

  x0 = _cffi_to_c_UNSIGNED(arg0, unsigned long);
  if (x0 == (unsigned long)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ERR_GET_LIB(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_ERR_GET_REASON(PyObject *self, PyObject *arg0)
{
  unsigned long x0;
  int result;

  x0 = _cffi_to_c_UNSIGNED(arg0, unsigned long);
  if (x0 == (unsigned long)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ERR_GET_REASON(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_ERR_PACK(PyObject *self, PyObject *args)
{
  int x0;
  int x1;
  int x2;
  unsigned long result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;

  if (!PyArg_ParseTuple(args, "OOO:ERR_PACK", &arg0, &arg1, &arg2))
    return NULL;

  x0 = _cffi_to_c_SIGNED(arg0, int);
  if (x0 == (int)-1 && PyErr_Occurred())
    return NULL;

  x1 = _cffi_to_c_SIGNED(arg1, int);
  if (x1 == (int)-1 && PyErr_Occurred())
    return NULL;

  x2 = _cffi_to_c_SIGNED(arg2, int);
  if (x2 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ERR_PACK(x0, x1, x2); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_UNSIGNED(result, unsigned long);
}

static int _cffi_const_ERR_add_error_data(PyObject *lib)
{
  PyObject *o;
  int res;
  void(* i)(int, ...);
  i = (ERR_add_error_data);
  o = _cffi_from_c_pointer((char *)i, _cffi_type(68));
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "ERR_add_error_data", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_X509_V_OK(lib);
}

static PyObject *
_cffi_f_ERR_error_string(PyObject *self, PyObject *args)
{
  unsigned long x0;
  char * x1;
  Py_ssize_t datasize;
  char * result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:ERR_error_string", &arg0, &arg1))
    return NULL;

  x0 = _cffi_to_c_UNSIGNED(arg0, unsigned long);
  if (x0 == (unsigned long)-1 && PyErr_Occurred())
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(23), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(23), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ERR_error_string(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(23));
}

static PyObject *
_cffi_f_ERR_error_string_n(PyObject *self, PyObject *args)
{
  unsigned long x0;
  char * x1;
  size_t x2;
  Py_ssize_t datasize;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;

  if (!PyArg_ParseTuple(args, "OOO:ERR_error_string_n", &arg0, &arg1, &arg2))
    return NULL;

  x0 = _cffi_to_c_UNSIGNED(arg0, unsigned long);
  if (x0 == (unsigned long)-1 && PyErr_Occurred())
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(23), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(23), arg1) < 0)
      return NULL;
  }

  x2 = _cffi_to_c_UNSIGNED(arg2, size_t);
  if (x2 == (size_t)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { ERR_error_string_n(x0, x1, x2); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_ERR_free_strings(PyObject *self, PyObject *no_arg)
{

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { ERR_free_strings(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_ERR_func_error_string(PyObject *self, PyObject *arg0)
{
  unsigned long x0;
  char const * result;

  x0 = _cffi_to_c_UNSIGNED(arg0, unsigned long);
  if (x0 == (unsigned long)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ERR_func_error_string(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(0));
}

static PyObject *
_cffi_f_ERR_get_error(PyObject *self, PyObject *no_arg)
{
  unsigned long result;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ERR_get_error(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_UNSIGNED(result, unsigned long);
}

static PyObject *
_cffi_f_ERR_get_error_line(PyObject *self, PyObject *args)
{
  char const * * x0;
  int * x1;
  Py_ssize_t datasize;
  unsigned long result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:ERR_get_error_line", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(69), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(69), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(37), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(37), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ERR_get_error_line(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_UNSIGNED(result, unsigned long);
}

static PyObject *
_cffi_f_ERR_get_error_line_data(PyObject *self, PyObject *args)
{
  char const * * x0;
  int * x1;
  char const * * x2;
  int * x3;
  Py_ssize_t datasize;
  unsigned long result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;

  if (!PyArg_ParseTuple(args, "OOOO:ERR_get_error_line_data", &arg0, &arg1, &arg2, &arg3))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(69), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(69), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(37), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(37), arg1) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(69), arg2, (char **)&x2);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x2 = alloca(datasize);
    memset((void *)x2, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x2, _cffi_type(69), arg2) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(37), arg3, (char **)&x3);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x3 = alloca(datasize);
    memset((void *)x3, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x3, _cffi_type(37), arg3) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ERR_get_error_line_data(x0, x1, x2, x3); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_UNSIGNED(result, unsigned long);
}

static PyObject *
_cffi_f_ERR_get_next_error_library(PyObject *self, PyObject *no_arg)
{
  int result;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ERR_get_next_error_library(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_ERR_lib_error_string(PyObject *self, PyObject *arg0)
{
  unsigned long x0;
  char const * result;

  x0 = _cffi_to_c_UNSIGNED(arg0, unsigned long);
  if (x0 == (unsigned long)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ERR_lib_error_string(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(0));
}

static PyObject *
_cffi_f_ERR_load_RAND_strings(PyObject *self, PyObject *no_arg)
{

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { ERR_load_RAND_strings(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_ERR_load_SSL_strings(PyObject *self, PyObject *no_arg)
{

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { ERR_load_SSL_strings(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_ERR_load_crypto_strings(PyObject *self, PyObject *no_arg)
{

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { ERR_load_crypto_strings(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_ERR_peek_error(PyObject *self, PyObject *no_arg)
{
  unsigned long result;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ERR_peek_error(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_UNSIGNED(result, unsigned long);
}

static PyObject *
_cffi_f_ERR_peek_error_line(PyObject *self, PyObject *args)
{
  char const * * x0;
  int * x1;
  Py_ssize_t datasize;
  unsigned long result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:ERR_peek_error_line", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(69), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(69), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(37), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(37), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ERR_peek_error_line(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_UNSIGNED(result, unsigned long);
}

static PyObject *
_cffi_f_ERR_peek_error_line_data(PyObject *self, PyObject *args)
{
  char const * * x0;
  int * x1;
  char const * * x2;
  int * x3;
  Py_ssize_t datasize;
  unsigned long result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;

  if (!PyArg_ParseTuple(args, "OOOO:ERR_peek_error_line_data", &arg0, &arg1, &arg2, &arg3))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(69), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(69), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(37), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(37), arg1) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(69), arg2, (char **)&x2);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x2 = alloca(datasize);
    memset((void *)x2, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x2, _cffi_type(69), arg2) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(37), arg3, (char **)&x3);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x3 = alloca(datasize);
    memset((void *)x3, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x3, _cffi_type(37), arg3) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ERR_peek_error_line_data(x0, x1, x2, x3); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_UNSIGNED(result, unsigned long);
}

static PyObject *
_cffi_f_ERR_peek_last_error(PyObject *self, PyObject *no_arg)
{
  unsigned long result;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ERR_peek_last_error(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_UNSIGNED(result, unsigned long);
}

static PyObject *
_cffi_f_ERR_peek_last_error_line(PyObject *self, PyObject *args)
{
  char const * * x0;
  int * x1;
  Py_ssize_t datasize;
  unsigned long result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:ERR_peek_last_error_line", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(69), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(69), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(37), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(37), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ERR_peek_last_error_line(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_UNSIGNED(result, unsigned long);
}

static PyObject *
_cffi_f_ERR_peek_last_error_line_data(PyObject *self, PyObject *args)
{
  char const * * x0;
  int * x1;
  char const * * x2;
  int * x3;
  Py_ssize_t datasize;
  unsigned long result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;

  if (!PyArg_ParseTuple(args, "OOOO:ERR_peek_last_error_line_data", &arg0, &arg1, &arg2, &arg3))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(69), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(69), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(37), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(37), arg1) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(69), arg2, (char **)&x2);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x2 = alloca(datasize);
    memset((void *)x2, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x2, _cffi_type(69), arg2) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(37), arg3, (char **)&x3);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x3 = alloca(datasize);
    memset((void *)x3, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x3, _cffi_type(37), arg3) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ERR_peek_last_error_line_data(x0, x1, x2, x3); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_UNSIGNED(result, unsigned long);
}

static PyObject *
_cffi_f_ERR_print_errors(PyObject *self, PyObject *arg0)
{
  BIO * x0;
  Py_ssize_t datasize;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(22), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(22), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { ERR_print_errors(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_ERR_print_errors_fp(PyObject *self, PyObject *arg0)
{
  FILE * x0;
  Py_ssize_t datasize;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(32), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(32), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { ERR_print_errors_fp(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_ERR_put_error(PyObject *self, PyObject *args)
{
  int x0;
  int x1;
  int x2;
  char const * x3;
  int x4;
  Py_ssize_t datasize;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;
  PyObject *arg4;

  if (!PyArg_ParseTuple(args, "OOOOO:ERR_put_error", &arg0, &arg1, &arg2, &arg3, &arg4))
    return NULL;

  x0 = _cffi_to_c_SIGNED(arg0, int);
  if (x0 == (int)-1 && PyErr_Occurred())
    return NULL;

  x1 = _cffi_to_c_SIGNED(arg1, int);
  if (x1 == (int)-1 && PyErr_Occurred())
    return NULL;

  x2 = _cffi_to_c_SIGNED(arg2, int);
  if (x2 == (int)-1 && PyErr_Occurred())
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(0), arg3, (char **)&x3);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x3 = alloca(datasize);
    memset((void *)x3, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x3, _cffi_type(0), arg3) < 0)
      return NULL;
  }

  x4 = _cffi_to_c_SIGNED(arg4, int);
  if (x4 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { ERR_put_error(x0, x1, x2, x3, x4); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_ERR_reason_error_string(PyObject *self, PyObject *arg0)
{
  unsigned long x0;
  char const * result;

  x0 = _cffi_to_c_UNSIGNED(arg0, unsigned long);
  if (x0 == (unsigned long)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ERR_reason_error_string(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(0));
}

static PyObject *
_cffi_f_ERR_remove_thread_state(PyObject *self, PyObject *arg0)
{
  CRYPTO_THREADID const * x0;
  Py_ssize_t datasize;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(70), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(70), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { ERR_remove_thread_state(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_EVP_CIPHER_CTX_block_size(PyObject *self, PyObject *arg0)
{
  EVP_CIPHER_CTX const * x0;
  Py_ssize_t datasize;
  int result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(71), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(71), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = EVP_CIPHER_CTX_block_size(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_EVP_CIPHER_CTX_cipher(PyObject *self, PyObject *arg0)
{
  EVP_CIPHER_CTX const * x0;
  Py_ssize_t datasize;
  EVP_CIPHER const * result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(71), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(71), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = EVP_CIPHER_CTX_cipher(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(59));
}

static PyObject *
_cffi_f_EVP_CIPHER_CTX_cleanup(PyObject *self, PyObject *arg0)
{
  EVP_CIPHER_CTX * x0;
  Py_ssize_t datasize;
  int result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(72), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(72), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = EVP_CIPHER_CTX_cleanup(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_EVP_CIPHER_CTX_ctrl(PyObject *self, PyObject *args)
{
  EVP_CIPHER_CTX * x0;
  int x1;
  int x2;
  void * x3;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;

  if (!PyArg_ParseTuple(args, "OOOO:EVP_CIPHER_CTX_ctrl", &arg0, &arg1, &arg2, &arg3))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(72), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(72), arg0) < 0)
      return NULL;
  }

  x1 = _cffi_to_c_SIGNED(arg1, int);
  if (x1 == (int)-1 && PyErr_Occurred())
    return NULL;

  x2 = _cffi_to_c_SIGNED(arg2, int);
  if (x2 == (int)-1 && PyErr_Occurred())
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(25), arg3, (char **)&x3);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x3 = alloca(datasize);
    memset((void *)x3, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x3, _cffi_type(25), arg3) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = EVP_CIPHER_CTX_ctrl(x0, x1, x2, x3); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_EVP_CIPHER_CTX_free(PyObject *self, PyObject *arg0)
{
  EVP_CIPHER_CTX * x0;
  Py_ssize_t datasize;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(72), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(72), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { EVP_CIPHER_CTX_free(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_EVP_CIPHER_CTX_init(PyObject *self, PyObject *arg0)
{
  EVP_CIPHER_CTX * x0;
  Py_ssize_t datasize;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(72), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(72), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { EVP_CIPHER_CTX_init(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_EVP_CIPHER_CTX_new(PyObject *self, PyObject *no_arg)
{
  EVP_CIPHER_CTX * result;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = EVP_CIPHER_CTX_new(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(72));
}

static PyObject *
_cffi_f_EVP_CIPHER_CTX_set_key_length(PyObject *self, PyObject *args)
{
  EVP_CIPHER_CTX * x0;
  int x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:EVP_CIPHER_CTX_set_key_length", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(72), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(72), arg0) < 0)
      return NULL;
  }

  x1 = _cffi_to_c_SIGNED(arg1, int);
  if (x1 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = EVP_CIPHER_CTX_set_key_length(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_EVP_CIPHER_CTX_set_padding(PyObject *self, PyObject *args)
{
  EVP_CIPHER_CTX * x0;
  int x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:EVP_CIPHER_CTX_set_padding", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(72), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(72), arg0) < 0)
      return NULL;
  }

  x1 = _cffi_to_c_SIGNED(arg1, int);
  if (x1 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = EVP_CIPHER_CTX_set_padding(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_EVP_CIPHER_block_size(PyObject *self, PyObject *arg0)
{
  EVP_CIPHER const * x0;
  Py_ssize_t datasize;
  int result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(59), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(59), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = EVP_CIPHER_block_size(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_EVP_CipherFinal_ex(PyObject *self, PyObject *args)
{
  EVP_CIPHER_CTX * x0;
  unsigned char * x1;
  int * x2;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;

  if (!PyArg_ParseTuple(args, "OOO:EVP_CipherFinal_ex", &arg0, &arg1, &arg2))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(72), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(72), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(3), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(3), arg1) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(37), arg2, (char **)&x2);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x2 = alloca(datasize);
    memset((void *)x2, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x2, _cffi_type(37), arg2) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = EVP_CipherFinal_ex(x0, x1, x2); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_EVP_CipherInit_ex(PyObject *self, PyObject *args)
{
  EVP_CIPHER_CTX * x0;
  EVP_CIPHER const * x1;
  ENGINE * x2;
  unsigned char const * x3;
  unsigned char const * x4;
  int x5;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;
  PyObject *arg4;
  PyObject *arg5;

  if (!PyArg_ParseTuple(args, "OOOOOO:EVP_CipherInit_ex", &arg0, &arg1, &arg2, &arg3, &arg4, &arg5))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(72), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(72), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(59), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(59), arg1) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(43), arg2, (char **)&x2);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x2 = alloca(datasize);
    memset((void *)x2, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x2, _cffi_type(43), arg2) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(1), arg3, (char **)&x3);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x3 = alloca(datasize);
    memset((void *)x3, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x3, _cffi_type(1), arg3) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(1), arg4, (char **)&x4);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x4 = alloca(datasize);
    memset((void *)x4, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x4, _cffi_type(1), arg4) < 0)
      return NULL;
  }

  x5 = _cffi_to_c_SIGNED(arg5, int);
  if (x5 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = EVP_CipherInit_ex(x0, x1, x2, x3, x4, x5); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_EVP_CipherUpdate(PyObject *self, PyObject *args)
{
  EVP_CIPHER_CTX * x0;
  unsigned char * x1;
  int * x2;
  unsigned char const * x3;
  int x4;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;
  PyObject *arg4;

  if (!PyArg_ParseTuple(args, "OOOOO:EVP_CipherUpdate", &arg0, &arg1, &arg2, &arg3, &arg4))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(72), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(72), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(3), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(3), arg1) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(37), arg2, (char **)&x2);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x2 = alloca(datasize);
    memset((void *)x2, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x2, _cffi_type(37), arg2) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(1), arg3, (char **)&x3);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x3 = alloca(datasize);
    memset((void *)x3, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x3, _cffi_type(1), arg3) < 0)
      return NULL;
  }

  x4 = _cffi_to_c_SIGNED(arg4, int);
  if (x4 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = EVP_CipherUpdate(x0, x1, x2, x3, x4); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_EVP_DecryptFinal_ex(PyObject *self, PyObject *args)
{
  EVP_CIPHER_CTX * x0;
  unsigned char * x1;
  int * x2;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;

  if (!PyArg_ParseTuple(args, "OOO:EVP_DecryptFinal_ex", &arg0, &arg1, &arg2))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(72), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(72), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(3), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(3), arg1) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(37), arg2, (char **)&x2);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x2 = alloca(datasize);
    memset((void *)x2, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x2, _cffi_type(37), arg2) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = EVP_DecryptFinal_ex(x0, x1, x2); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_EVP_DecryptInit_ex(PyObject *self, PyObject *args)
{
  EVP_CIPHER_CTX * x0;
  EVP_CIPHER const * x1;
  ENGINE * x2;
  unsigned char const * x3;
  unsigned char const * x4;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;
  PyObject *arg4;

  if (!PyArg_ParseTuple(args, "OOOOO:EVP_DecryptInit_ex", &arg0, &arg1, &arg2, &arg3, &arg4))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(72), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(72), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(59), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(59), arg1) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(43), arg2, (char **)&x2);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x2 = alloca(datasize);
    memset((void *)x2, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x2, _cffi_type(43), arg2) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(1), arg3, (char **)&x3);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x3 = alloca(datasize);
    memset((void *)x3, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x3, _cffi_type(1), arg3) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(1), arg4, (char **)&x4);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x4 = alloca(datasize);
    memset((void *)x4, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x4, _cffi_type(1), arg4) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = EVP_DecryptInit_ex(x0, x1, x2, x3, x4); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_EVP_DecryptUpdate(PyObject *self, PyObject *args)
{
  EVP_CIPHER_CTX * x0;
  unsigned char * x1;
  int * x2;
  unsigned char const * x3;
  int x4;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;
  PyObject *arg4;

  if (!PyArg_ParseTuple(args, "OOOOO:EVP_DecryptUpdate", &arg0, &arg1, &arg2, &arg3, &arg4))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(72), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(72), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(3), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(3), arg1) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(37), arg2, (char **)&x2);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x2 = alloca(datasize);
    memset((void *)x2, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x2, _cffi_type(37), arg2) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(1), arg3, (char **)&x3);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x3 = alloca(datasize);
    memset((void *)x3, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x3, _cffi_type(1), arg3) < 0)
      return NULL;
  }

  x4 = _cffi_to_c_SIGNED(arg4, int);
  if (x4 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = EVP_DecryptUpdate(x0, x1, x2, x3, x4); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_EVP_DigestFinal_ex(PyObject *self, PyObject *args)
{
  EVP_MD_CTX * x0;
  unsigned char * x1;
  unsigned int * x2;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;

  if (!PyArg_ParseTuple(args, "OOO:EVP_DigestFinal_ex", &arg0, &arg1, &arg2))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(73), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(73), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(3), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(3), arg1) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(41), arg2, (char **)&x2);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x2 = alloca(datasize);
    memset((void *)x2, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x2, _cffi_type(41), arg2) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = EVP_DigestFinal_ex(x0, x1, x2); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_EVP_DigestInit_ex(PyObject *self, PyObject *args)
{
  EVP_MD_CTX * x0;
  EVP_MD const * x1;
  ENGINE * x2;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;

  if (!PyArg_ParseTuple(args, "OOO:EVP_DigestInit_ex", &arg0, &arg1, &arg2))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(73), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(73), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(42), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(42), arg1) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(43), arg2, (char **)&x2);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x2 = alloca(datasize);
    memset((void *)x2, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x2, _cffi_type(43), arg2) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = EVP_DigestInit_ex(x0, x1, x2); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_EVP_DigestUpdate(PyObject *self, PyObject *args)
{
  EVP_MD_CTX * x0;
  void const * x1;
  size_t x2;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;

  if (!PyArg_ParseTuple(args, "OOO:EVP_DigestUpdate", &arg0, &arg1, &arg2))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(73), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(73), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(14), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(14), arg1) < 0)
      return NULL;
  }

  x2 = _cffi_to_c_UNSIGNED(arg2, size_t);
  if (x2 == (size_t)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = EVP_DigestUpdate(x0, x1, x2); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_EVP_EncryptFinal_ex(PyObject *self, PyObject *args)
{
  EVP_CIPHER_CTX * x0;
  unsigned char * x1;
  int * x2;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;

  if (!PyArg_ParseTuple(args, "OOO:EVP_EncryptFinal_ex", &arg0, &arg1, &arg2))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(72), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(72), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(3), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(3), arg1) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(37), arg2, (char **)&x2);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x2 = alloca(datasize);
    memset((void *)x2, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x2, _cffi_type(37), arg2) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = EVP_EncryptFinal_ex(x0, x1, x2); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_EVP_EncryptInit_ex(PyObject *self, PyObject *args)
{
  EVP_CIPHER_CTX * x0;
  EVP_CIPHER const * x1;
  ENGINE * x2;
  unsigned char const * x3;
  unsigned char const * x4;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;
  PyObject *arg4;

  if (!PyArg_ParseTuple(args, "OOOOO:EVP_EncryptInit_ex", &arg0, &arg1, &arg2, &arg3, &arg4))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(72), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(72), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(59), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(59), arg1) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(43), arg2, (char **)&x2);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x2 = alloca(datasize);
    memset((void *)x2, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x2, _cffi_type(43), arg2) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(1), arg3, (char **)&x3);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x3 = alloca(datasize);
    memset((void *)x3, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x3, _cffi_type(1), arg3) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(1), arg4, (char **)&x4);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x4 = alloca(datasize);
    memset((void *)x4, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x4, _cffi_type(1), arg4) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = EVP_EncryptInit_ex(x0, x1, x2, x3, x4); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_EVP_EncryptUpdate(PyObject *self, PyObject *args)
{
  EVP_CIPHER_CTX * x0;
  unsigned char * x1;
  int * x2;
  unsigned char const * x3;
  int x4;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;
  PyObject *arg4;

  if (!PyArg_ParseTuple(args, "OOOOO:EVP_EncryptUpdate", &arg0, &arg1, &arg2, &arg3, &arg4))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(72), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(72), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(3), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(3), arg1) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(37), arg2, (char **)&x2);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x2 = alloca(datasize);
    memset((void *)x2, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x2, _cffi_type(37), arg2) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(1), arg3, (char **)&x3);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x3 = alloca(datasize);
    memset((void *)x3, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x3, _cffi_type(1), arg3) < 0)
      return NULL;
  }

  x4 = _cffi_to_c_SIGNED(arg4, int);
  if (x4 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = EVP_EncryptUpdate(x0, x1, x2, x3, x4); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_EVP_MD_CTX_cleanup(PyObject *self, PyObject *arg0)
{
  EVP_MD_CTX * x0;
  Py_ssize_t datasize;
  int result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(73), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(73), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = EVP_MD_CTX_cleanup(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_EVP_MD_CTX_copy_ex(PyObject *self, PyObject *args)
{
  EVP_MD_CTX * x0;
  EVP_MD_CTX const * x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:EVP_MD_CTX_copy_ex", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(73), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(73), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(74), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(74), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = EVP_MD_CTX_copy_ex(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_EVP_MD_CTX_create(PyObject *self, PyObject *no_arg)
{
  EVP_MD_CTX * result;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = EVP_MD_CTX_create(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(73));
}

static PyObject *
_cffi_f_EVP_MD_CTX_destroy(PyObject *self, PyObject *arg0)
{
  EVP_MD_CTX * x0;
  Py_ssize_t datasize;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(73), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(73), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { EVP_MD_CTX_destroy(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_EVP_MD_CTX_md(PyObject *self, PyObject *arg0)
{
  EVP_MD_CTX const * x0;
  Py_ssize_t datasize;
  EVP_MD const * result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(74), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(74), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = EVP_MD_CTX_md(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(42));
}

static PyObject *
_cffi_f_EVP_MD_size(PyObject *self, PyObject *arg0)
{
  EVP_MD const * x0;
  Py_ssize_t datasize;
  int result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(42), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(42), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = EVP_MD_size(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_EVP_PKEY_CTX_dup(PyObject *self, PyObject *arg0)
{
  EVP_PKEY_CTX * x0;
  Py_ssize_t datasize;
  EVP_PKEY_CTX * result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(75), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(75), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = EVP_PKEY_CTX_dup(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(75));
}

static PyObject *
_cffi_f_EVP_PKEY_CTX_free(PyObject *self, PyObject *arg0)
{
  EVP_PKEY_CTX * x0;
  Py_ssize_t datasize;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(75), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(75), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { EVP_PKEY_CTX_free(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_EVP_PKEY_CTX_new(PyObject *self, PyObject *args)
{
  EVP_PKEY * x0;
  ENGINE * x1;
  Py_ssize_t datasize;
  EVP_PKEY_CTX * result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:EVP_PKEY_CTX_new", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(62), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(62), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(43), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(43), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = EVP_PKEY_CTX_new(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(75));
}

static PyObject *
_cffi_f_EVP_PKEY_CTX_new_id(PyObject *self, PyObject *args)
{
  int x0;
  ENGINE * x1;
  Py_ssize_t datasize;
  EVP_PKEY_CTX * result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:EVP_PKEY_CTX_new_id", &arg0, &arg1))
    return NULL;

  x0 = _cffi_to_c_SIGNED(arg0, int);
  if (x0 == (int)-1 && PyErr_Occurred())
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(43), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(43), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = EVP_PKEY_CTX_new_id(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(75));
}

static PyObject *
_cffi_f_EVP_PKEY_CTX_set_rsa_padding(PyObject *self, PyObject *args)
{
  EVP_PKEY_CTX * x0;
  int x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:EVP_PKEY_CTX_set_rsa_padding", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(75), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(75), arg0) < 0)
      return NULL;
  }

  x1 = _cffi_to_c_SIGNED(arg1, int);
  if (x1 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = EVP_PKEY_CTX_set_rsa_padding(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_EVP_PKEY_CTX_set_rsa_pss_saltlen(PyObject *self, PyObject *args)
{
  EVP_PKEY_CTX * x0;
  int x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:EVP_PKEY_CTX_set_rsa_pss_saltlen", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(75), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(75), arg0) < 0)
      return NULL;
  }

  x1 = _cffi_to_c_SIGNED(arg1, int);
  if (x1 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = EVP_PKEY_CTX_set_rsa_pss_saltlen(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_EVP_PKEY_CTX_set_signature_md(PyObject *self, PyObject *args)
{
  EVP_PKEY_CTX * x0;
  EVP_MD const * x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:EVP_PKEY_CTX_set_signature_md", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(75), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(75), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(42), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(42), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = EVP_PKEY_CTX_set_signature_md(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_EVP_PKEY_assign_DSA(PyObject *self, PyObject *args)
{
  EVP_PKEY * x0;
  DSA * x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:EVP_PKEY_assign_DSA", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(62), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(62), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(45), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(45), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = EVP_PKEY_assign_DSA(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_EVP_PKEY_assign_RSA(PyObject *self, PyObject *args)
{
  EVP_PKEY * x0;
  RSA * x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:EVP_PKEY_assign_RSA", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(62), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(62), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(76), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(76), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = EVP_PKEY_assign_RSA(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_EVP_PKEY_bits(PyObject *self, PyObject *arg0)
{
  EVP_PKEY * x0;
  Py_ssize_t datasize;
  int result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(62), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(62), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = EVP_PKEY_bits(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_EVP_PKEY_free(PyObject *self, PyObject *arg0)
{
  EVP_PKEY * x0;
  Py_ssize_t datasize;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(62), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(62), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { EVP_PKEY_free(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_EVP_PKEY_get1_RSA(PyObject *self, PyObject *arg0)
{
  EVP_PKEY * x0;
  Py_ssize_t datasize;
  RSA * result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(62), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(62), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = EVP_PKEY_get1_RSA(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(76));
}

static PyObject *
_cffi_f_EVP_PKEY_new(PyObject *self, PyObject *no_arg)
{
  EVP_PKEY * result;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = EVP_PKEY_new(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(62));
}

static PyObject *
_cffi_f_EVP_PKEY_set1_DSA(PyObject *self, PyObject *args)
{
  EVP_PKEY * x0;
  DSA * x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:EVP_PKEY_set1_DSA", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(62), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(62), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(45), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(45), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = EVP_PKEY_set1_DSA(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_EVP_PKEY_set1_RSA(PyObject *self, PyObject *args)
{
  EVP_PKEY * x0;
  RSA * x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:EVP_PKEY_set1_RSA", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(62), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(62), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(76), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(76), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = EVP_PKEY_set1_RSA(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_EVP_PKEY_sign(PyObject *self, PyObject *args)
{
  EVP_PKEY_CTX * x0;
  unsigned char * x1;
  size_t * x2;
  unsigned char const * x3;
  size_t x4;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;
  PyObject *arg4;

  if (!PyArg_ParseTuple(args, "OOOOO:EVP_PKEY_sign", &arg0, &arg1, &arg2, &arg3, &arg4))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(75), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(75), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(3), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(3), arg1) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(77), arg2, (char **)&x2);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x2 = alloca(datasize);
    memset((void *)x2, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x2, _cffi_type(77), arg2) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(1), arg3, (char **)&x3);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x3 = alloca(datasize);
    memset((void *)x3, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x3, _cffi_type(1), arg3) < 0)
      return NULL;
  }

  x4 = _cffi_to_c_UNSIGNED(arg4, size_t);
  if (x4 == (size_t)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = EVP_PKEY_sign(x0, x1, x2, x3, x4); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_EVP_PKEY_sign_init(PyObject *self, PyObject *arg0)
{
  EVP_PKEY_CTX * x0;
  Py_ssize_t datasize;
  int result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(75), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(75), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = EVP_PKEY_sign_init(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_EVP_PKEY_size(PyObject *self, PyObject *arg0)
{
  EVP_PKEY * x0;
  Py_ssize_t datasize;
  int result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(62), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(62), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = EVP_PKEY_size(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_EVP_PKEY_type(PyObject *self, PyObject *arg0)
{
  int x0;
  int result;

  x0 = _cffi_to_c_SIGNED(arg0, int);
  if (x0 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = EVP_PKEY_type(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_EVP_PKEY_verify(PyObject *self, PyObject *args)
{
  EVP_PKEY_CTX * x0;
  unsigned char const * x1;
  size_t x2;
  unsigned char const * x3;
  size_t x4;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;
  PyObject *arg4;

  if (!PyArg_ParseTuple(args, "OOOOO:EVP_PKEY_verify", &arg0, &arg1, &arg2, &arg3, &arg4))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(75), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(75), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(1), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(1), arg1) < 0)
      return NULL;
  }

  x2 = _cffi_to_c_UNSIGNED(arg2, size_t);
  if (x2 == (size_t)-1 && PyErr_Occurred())
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(1), arg3, (char **)&x3);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x3 = alloca(datasize);
    memset((void *)x3, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x3, _cffi_type(1), arg3) < 0)
      return NULL;
  }

  x4 = _cffi_to_c_UNSIGNED(arg4, size_t);
  if (x4 == (size_t)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = EVP_PKEY_verify(x0, x1, x2, x3, x4); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_EVP_PKEY_verify_init(PyObject *self, PyObject *arg0)
{
  EVP_PKEY_CTX * x0;
  Py_ssize_t datasize;
  int result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(75), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(75), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = EVP_PKEY_verify_init(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_EVP_SignFinal(PyObject *self, PyObject *args)
{
  EVP_MD_CTX * x0;
  unsigned char * x1;
  unsigned int * x2;
  EVP_PKEY * x3;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;

  if (!PyArg_ParseTuple(args, "OOOO:EVP_SignFinal", &arg0, &arg1, &arg2, &arg3))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(73), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(73), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(3), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(3), arg1) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(41), arg2, (char **)&x2);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x2 = alloca(datasize);
    memset((void *)x2, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x2, _cffi_type(41), arg2) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(62), arg3, (char **)&x3);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x3 = alloca(datasize);
    memset((void *)x3, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x3, _cffi_type(62), arg3) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = EVP_SignFinal(x0, x1, x2, x3); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_EVP_SignInit(PyObject *self, PyObject *args)
{
  EVP_MD_CTX * x0;
  EVP_MD const * x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:EVP_SignInit", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(73), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(73), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(42), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(42), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = EVP_SignInit(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_EVP_SignUpdate(PyObject *self, PyObject *args)
{
  EVP_MD_CTX * x0;
  void const * x1;
  size_t x2;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;

  if (!PyArg_ParseTuple(args, "OOO:EVP_SignUpdate", &arg0, &arg1, &arg2))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(73), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(73), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(14), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(14), arg1) < 0)
      return NULL;
  }

  x2 = _cffi_to_c_UNSIGNED(arg2, size_t);
  if (x2 == (size_t)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = EVP_SignUpdate(x0, x1, x2); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_EVP_VerifyFinal(PyObject *self, PyObject *args)
{
  EVP_MD_CTX * x0;
  unsigned char const * x1;
  unsigned int x2;
  EVP_PKEY * x3;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;

  if (!PyArg_ParseTuple(args, "OOOO:EVP_VerifyFinal", &arg0, &arg1, &arg2, &arg3))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(73), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(73), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(1), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(1), arg1) < 0)
      return NULL;
  }

  x2 = _cffi_to_c_UNSIGNED(arg2, unsigned int);
  if (x2 == (unsigned int)-1 && PyErr_Occurred())
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(62), arg3, (char **)&x3);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x3 = alloca(datasize);
    memset((void *)x3, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x3, _cffi_type(62), arg3) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = EVP_VerifyFinal(x0, x1, x2, x3); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_EVP_VerifyInit(PyObject *self, PyObject *args)
{
  EVP_MD_CTX * x0;
  EVP_MD const * x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:EVP_VerifyInit", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(73), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(73), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(42), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(42), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = EVP_VerifyInit(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_EVP_VerifyUpdate(PyObject *self, PyObject *args)
{
  EVP_MD_CTX * x0;
  void const * x1;
  size_t x2;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;

  if (!PyArg_ParseTuple(args, "OOO:EVP_VerifyUpdate", &arg0, &arg1, &arg2))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(73), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(73), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(14), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(14), arg1) < 0)
      return NULL;
  }

  x2 = _cffi_to_c_UNSIGNED(arg2, size_t);
  if (x2 == (size_t)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = EVP_VerifyUpdate(x0, x1, x2); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_EVP_get_cipherbyname(PyObject *self, PyObject *arg0)
{
  char const * x0;
  Py_ssize_t datasize;
  EVP_CIPHER const * result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(0), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(0), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = EVP_get_cipherbyname(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(59));
}

static PyObject *
_cffi_f_EVP_get_digestbyname(PyObject *self, PyObject *arg0)
{
  char const * x0;
  Py_ssize_t datasize;
  EVP_MD const * result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(0), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(0), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = EVP_get_digestbyname(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(42));
}

static PyObject *
_cffi_f_EVP_md5(PyObject *self, PyObject *no_arg)
{
  EVP_MD const * result;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = EVP_md5(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(42));
}

static PyObject *
_cffi_f_GENERAL_NAME_print(PyObject *self, PyObject *args)
{
  BIO * x0;
  GENERAL_NAME * x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:GENERAL_NAME_print", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(22), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(22), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(78), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(78), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = GENERAL_NAME_print(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_HMAC_CTX_cleanup(PyObject *self, PyObject *arg0)
{
  HMAC_CTX * x0;
  Py_ssize_t datasize;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(40), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(40), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { HMAC_CTX_cleanup(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_HMAC_CTX_init(PyObject *self, PyObject *arg0)
{
  HMAC_CTX * x0;
  Py_ssize_t datasize;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(40), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(40), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { HMAC_CTX_init(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_M_ASN1_TIME_dup(PyObject *self, PyObject *arg0)
{
  void * x0;
  Py_ssize_t datasize;
  ASN1_TIME * result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(25), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(25), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = M_ASN1_TIME_dup(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(16));
}

static PyObject *
_cffi_f_NETSCAPE_SPKI_b64_encode(PyObject *self, PyObject *arg0)
{
  NETSCAPE_SPKI * x0;
  Py_ssize_t datasize;
  char * result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(79), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(79), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = NETSCAPE_SPKI_b64_encode(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(23));
}

static PyObject *
_cffi_f_NETSCAPE_SPKI_free(PyObject *self, PyObject *arg0)
{
  NETSCAPE_SPKI * x0;
  Py_ssize_t datasize;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(79), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(79), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { NETSCAPE_SPKI_free(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_NETSCAPE_SPKI_get_pubkey(PyObject *self, PyObject *arg0)
{
  NETSCAPE_SPKI * x0;
  Py_ssize_t datasize;
  EVP_PKEY * result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(79), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(79), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = NETSCAPE_SPKI_get_pubkey(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(62));
}

static PyObject *
_cffi_f_NETSCAPE_SPKI_new(PyObject *self, PyObject *no_arg)
{
  NETSCAPE_SPKI * result;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = NETSCAPE_SPKI_new(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(79));
}

static PyObject *
_cffi_f_NETSCAPE_SPKI_set_pubkey(PyObject *self, PyObject *args)
{
  NETSCAPE_SPKI * x0;
  EVP_PKEY * x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:NETSCAPE_SPKI_set_pubkey", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(79), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(79), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(62), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(62), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = NETSCAPE_SPKI_set_pubkey(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_NETSCAPE_SPKI_sign(PyObject *self, PyObject *args)
{
  NETSCAPE_SPKI * x0;
  EVP_PKEY * x1;
  EVP_MD const * x2;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;

  if (!PyArg_ParseTuple(args, "OOO:NETSCAPE_SPKI_sign", &arg0, &arg1, &arg2))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(79), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(79), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(62), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(62), arg1) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(42), arg2, (char **)&x2);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x2 = alloca(datasize);
    memset((void *)x2, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x2, _cffi_type(42), arg2) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = NETSCAPE_SPKI_sign(x0, x1, x2); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_NETSCAPE_SPKI_verify(PyObject *self, PyObject *args)
{
  NETSCAPE_SPKI * x0;
  EVP_PKEY * x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:NETSCAPE_SPKI_verify", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(79), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(79), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(62), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(62), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = NETSCAPE_SPKI_verify(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_OBJ_cleanup(PyObject *self, PyObject *no_arg)
{

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { OBJ_cleanup(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_OBJ_cmp(PyObject *self, PyObject *args)
{
  ASN1_OBJECT const * x0;
  ASN1_OBJECT const * x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:OBJ_cmp", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(80), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(80), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(80), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(80), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = OBJ_cmp(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_OBJ_create(PyObject *self, PyObject *args)
{
  char const * x0;
  char const * x1;
  char const * x2;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;

  if (!PyArg_ParseTuple(args, "OOO:OBJ_create", &arg0, &arg1, &arg2))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(0), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(0), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(0), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(0), arg1) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(0), arg2, (char **)&x2);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x2 = alloca(datasize);
    memset((void *)x2, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x2, _cffi_type(0), arg2) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = OBJ_create(x0, x1, x2); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_OBJ_dup(PyObject *self, PyObject *arg0)
{
  ASN1_OBJECT const * x0;
  Py_ssize_t datasize;
  ASN1_OBJECT * result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(80), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(80), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = OBJ_dup(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(11));
}

static PyObject *
_cffi_f_OBJ_ln2nid(PyObject *self, PyObject *arg0)
{
  char const * x0;
  Py_ssize_t datasize;
  int result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(0), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(0), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = OBJ_ln2nid(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_OBJ_nid2ln(PyObject *self, PyObject *arg0)
{
  int x0;
  char const * result;

  x0 = _cffi_to_c_SIGNED(arg0, int);
  if (x0 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = OBJ_nid2ln(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(0));
}

static PyObject *
_cffi_f_OBJ_nid2obj(PyObject *self, PyObject *arg0)
{
  int x0;
  ASN1_OBJECT * result;

  x0 = _cffi_to_c_SIGNED(arg0, int);
  if (x0 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = OBJ_nid2obj(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(11));
}

static PyObject *
_cffi_f_OBJ_nid2sn(PyObject *self, PyObject *arg0)
{
  int x0;
  char const * result;

  x0 = _cffi_to_c_SIGNED(arg0, int);
  if (x0 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = OBJ_nid2sn(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(0));
}

static PyObject *
_cffi_f_OBJ_obj2nid(PyObject *self, PyObject *arg0)
{
  ASN1_OBJECT const * x0;
  Py_ssize_t datasize;
  int result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(80), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(80), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = OBJ_obj2nid(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_OBJ_obj2txt(PyObject *self, PyObject *args)
{
  char * x0;
  int x1;
  ASN1_OBJECT const * x2;
  int x3;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;

  if (!PyArg_ParseTuple(args, "OOOO:OBJ_obj2txt", &arg0, &arg1, &arg2, &arg3))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(23), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(23), arg0) < 0)
      return NULL;
  }

  x1 = _cffi_to_c_SIGNED(arg1, int);
  if (x1 == (int)-1 && PyErr_Occurred())
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(80), arg2, (char **)&x2);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x2 = alloca(datasize);
    memset((void *)x2, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x2, _cffi_type(80), arg2) < 0)
      return NULL;
  }

  x3 = _cffi_to_c_SIGNED(arg3, int);
  if (x3 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = OBJ_obj2txt(x0, x1, x2, x3); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_OBJ_sn2nid(PyObject *self, PyObject *arg0)
{
  char const * x0;
  Py_ssize_t datasize;
  int result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(0), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(0), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = OBJ_sn2nid(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_OBJ_txt2nid(PyObject *self, PyObject *arg0)
{
  char const * x0;
  Py_ssize_t datasize;
  int result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(0), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(0), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = OBJ_txt2nid(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_OBJ_txt2obj(PyObject *self, PyObject *args)
{
  char const * x0;
  int x1;
  Py_ssize_t datasize;
  ASN1_OBJECT * result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:OBJ_txt2obj", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(0), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(0), arg0) < 0)
      return NULL;
  }

  x1 = _cffi_to_c_SIGNED(arg1, int);
  if (x1 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = OBJ_txt2obj(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(11));
}

static PyObject *
_cffi_f_OPENSSL_free(PyObject *self, PyObject *arg0)
{
  void * x0;
  Py_ssize_t datasize;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(25), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(25), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { OPENSSL_free(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_OpenSSL_add_all_algorithms(PyObject *self, PyObject *no_arg)
{

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { OpenSSL_add_all_algorithms(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_PEM_read_bio_DHparams(PyObject *self, PyObject *args)
{
  BIO * x0;
  DH * * x1;
  int(* x2)(char *, int, int, void *);
  void * x3;
  Py_ssize_t datasize;
  DH * result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;

  if (!PyArg_ParseTuple(args, "OOOO:PEM_read_bio_DHparams", &arg0, &arg1, &arg2, &arg3))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(22), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(22), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(81), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(81), arg1) < 0)
      return NULL;
  }

  x2 = (int(*)(char *, int, int, void *))_cffi_to_c_pointer(arg2, _cffi_type(82));
  if (x2 == (int(*)(char *, int, int, void *))NULL && PyErr_Occurred())
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(25), arg3, (char **)&x3);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x3 = alloca(datasize);
    memset((void *)x3, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x3, _cffi_type(25), arg3) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = PEM_read_bio_DHparams(x0, x1, x2, x3); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(44));
}

static PyObject *
_cffi_f_PEM_read_bio_DSAPrivateKey(PyObject *self, PyObject *args)
{
  BIO * x0;
  DSA * * x1;
  int(* x2)(char *, int, int, void *);
  void * x3;
  Py_ssize_t datasize;
  DSA * result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;

  if (!PyArg_ParseTuple(args, "OOOO:PEM_read_bio_DSAPrivateKey", &arg0, &arg1, &arg2, &arg3))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(22), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(22), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(83), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(83), arg1) < 0)
      return NULL;
  }

  x2 = (int(*)(char *, int, int, void *))_cffi_to_c_pointer(arg2, _cffi_type(82));
  if (x2 == (int(*)(char *, int, int, void *))NULL && PyErr_Occurred())
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(25), arg3, (char **)&x3);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x3 = alloca(datasize);
    memset((void *)x3, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x3, _cffi_type(25), arg3) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = PEM_read_bio_DSAPrivateKey(x0, x1, x2, x3); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(45));
}

static PyObject *
_cffi_f_PEM_read_bio_DSA_PUBKEY(PyObject *self, PyObject *args)
{
  BIO * x0;
  DSA * * x1;
  int(* x2)(char *, int, int, void *);
  void * x3;
  Py_ssize_t datasize;
  DSA * result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;

  if (!PyArg_ParseTuple(args, "OOOO:PEM_read_bio_DSA_PUBKEY", &arg0, &arg1, &arg2, &arg3))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(22), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(22), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(83), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(83), arg1) < 0)
      return NULL;
  }

  x2 = (int(*)(char *, int, int, void *))_cffi_to_c_pointer(arg2, _cffi_type(82));
  if (x2 == (int(*)(char *, int, int, void *))NULL && PyErr_Occurred())
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(25), arg3, (char **)&x3);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x3 = alloca(datasize);
    memset((void *)x3, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x3, _cffi_type(25), arg3) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = PEM_read_bio_DSA_PUBKEY(x0, x1, x2, x3); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(45));
}

static PyObject *
_cffi_f_PEM_read_bio_PKCS7(PyObject *self, PyObject *args)
{
  BIO * x0;
  PKCS7 * * x1;
  int(* x2)(char *, int, int, void *);
  void * x3;
  Py_ssize_t datasize;
  PKCS7 * result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;

  if (!PyArg_ParseTuple(args, "OOOO:PEM_read_bio_PKCS7", &arg0, &arg1, &arg2, &arg3))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(22), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(22), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(84), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(84), arg1) < 0)
      return NULL;
  }

  x2 = (int(*)(char *, int, int, void *))_cffi_to_c_pointer(arg2, _cffi_type(82));
  if (x2 == (int(*)(char *, int, int, void *))NULL && PyErr_Occurred())
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(25), arg3, (char **)&x3);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x3 = alloca(datasize);
    memset((void *)x3, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x3, _cffi_type(25), arg3) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = PEM_read_bio_PKCS7(x0, x1, x2, x3); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(85));
}

static PyObject *
_cffi_f_PEM_read_bio_PUBKEY(PyObject *self, PyObject *args)
{
  BIO * x0;
  EVP_PKEY * * x1;
  int(* x2)(char *, int, int, void *);
  void * x3;
  Py_ssize_t datasize;
  EVP_PKEY * result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;

  if (!PyArg_ParseTuple(args, "OOOO:PEM_read_bio_PUBKEY", &arg0, &arg1, &arg2, &arg3))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(22), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(22), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(86), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(86), arg1) < 0)
      return NULL;
  }

  x2 = (int(*)(char *, int, int, void *))_cffi_to_c_pointer(arg2, _cffi_type(82));
  if (x2 == (int(*)(char *, int, int, void *))NULL && PyErr_Occurred())
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(25), arg3, (char **)&x3);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x3 = alloca(datasize);
    memset((void *)x3, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x3, _cffi_type(25), arg3) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = PEM_read_bio_PUBKEY(x0, x1, x2, x3); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(62));
}

static PyObject *
_cffi_f_PEM_read_bio_PrivateKey(PyObject *self, PyObject *args)
{
  BIO * x0;
  EVP_PKEY * * x1;
  int(* x2)(char *, int, int, void *);
  void * x3;
  Py_ssize_t datasize;
  EVP_PKEY * result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;

  if (!PyArg_ParseTuple(args, "OOOO:PEM_read_bio_PrivateKey", &arg0, &arg1, &arg2, &arg3))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(22), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(22), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(86), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(86), arg1) < 0)
      return NULL;
  }

  x2 = (int(*)(char *, int, int, void *))_cffi_to_c_pointer(arg2, _cffi_type(82));
  if (x2 == (int(*)(char *, int, int, void *))NULL && PyErr_Occurred())
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(25), arg3, (char **)&x3);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x3 = alloca(datasize);
    memset((void *)x3, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x3, _cffi_type(25), arg3) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = PEM_read_bio_PrivateKey(x0, x1, x2, x3); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(62));
}

static PyObject *
_cffi_f_PEM_read_bio_RSAPrivateKey(PyObject *self, PyObject *args)
{
  BIO * x0;
  RSA * * x1;
  int(* x2)(char *, int, int, void *);
  void * x3;
  Py_ssize_t datasize;
  RSA * result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;

  if (!PyArg_ParseTuple(args, "OOOO:PEM_read_bio_RSAPrivateKey", &arg0, &arg1, &arg2, &arg3))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(22), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(22), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(87), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(87), arg1) < 0)
      return NULL;
  }

  x2 = (int(*)(char *, int, int, void *))_cffi_to_c_pointer(arg2, _cffi_type(82));
  if (x2 == (int(*)(char *, int, int, void *))NULL && PyErr_Occurred())
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(25), arg3, (char **)&x3);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x3 = alloca(datasize);
    memset((void *)x3, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x3, _cffi_type(25), arg3) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = PEM_read_bio_RSAPrivateKey(x0, x1, x2, x3); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(76));
}

static PyObject *
_cffi_f_PEM_read_bio_RSAPublicKey(PyObject *self, PyObject *args)
{
  BIO * x0;
  RSA * * x1;
  int(* x2)(char *, int, int, void *);
  void * x3;
  Py_ssize_t datasize;
  RSA * result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;

  if (!PyArg_ParseTuple(args, "OOOO:PEM_read_bio_RSAPublicKey", &arg0, &arg1, &arg2, &arg3))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(22), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(22), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(87), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(87), arg1) < 0)
      return NULL;
  }

  x2 = (int(*)(char *, int, int, void *))_cffi_to_c_pointer(arg2, _cffi_type(82));
  if (x2 == (int(*)(char *, int, int, void *))NULL && PyErr_Occurred())
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(25), arg3, (char **)&x3);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x3 = alloca(datasize);
    memset((void *)x3, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x3, _cffi_type(25), arg3) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = PEM_read_bio_RSAPublicKey(x0, x1, x2, x3); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(76));
}

static PyObject *
_cffi_f_PEM_read_bio_X509(PyObject *self, PyObject *args)
{
  BIO * x0;
  X509 * * x1;
  int(* x2)(char *, int, int, void *);
  void * x3;
  Py_ssize_t datasize;
  X509 * result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;

  if (!PyArg_ParseTuple(args, "OOOO:PEM_read_bio_X509", &arg0, &arg1, &arg2, &arg3))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(22), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(22), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(88), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(88), arg1) < 0)
      return NULL;
  }

  x2 = (int(*)(char *, int, int, void *))_cffi_to_c_pointer(arg2, _cffi_type(82));
  if (x2 == (int(*)(char *, int, int, void *))NULL && PyErr_Occurred())
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(25), arg3, (char **)&x3);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x3 = alloca(datasize);
    memset((void *)x3, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x3, _cffi_type(25), arg3) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = PEM_read_bio_X509(x0, x1, x2, x3); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(89));
}

static PyObject *
_cffi_f_PEM_read_bio_X509_CRL(PyObject *self, PyObject *args)
{
  BIO * x0;
  X509_CRL * * x1;
  int(* x2)(char *, int, int, void *);
  void * x3;
  Py_ssize_t datasize;
  X509_CRL * result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;

  if (!PyArg_ParseTuple(args, "OOOO:PEM_read_bio_X509_CRL", &arg0, &arg1, &arg2, &arg3))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(22), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(22), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(90), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(90), arg1) < 0)
      return NULL;
  }

  x2 = (int(*)(char *, int, int, void *))_cffi_to_c_pointer(arg2, _cffi_type(82));
  if (x2 == (int(*)(char *, int, int, void *))NULL && PyErr_Occurred())
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(25), arg3, (char **)&x3);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x3 = alloca(datasize);
    memset((void *)x3, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x3, _cffi_type(25), arg3) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = PEM_read_bio_X509_CRL(x0, x1, x2, x3); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(91));
}

static PyObject *
_cffi_f_PEM_read_bio_X509_REQ(PyObject *self, PyObject *args)
{
  BIO * x0;
  X509_REQ * * x1;
  int(* x2)(char *, int, int, void *);
  void * x3;
  Py_ssize_t datasize;
  X509_REQ * result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;

  if (!PyArg_ParseTuple(args, "OOOO:PEM_read_bio_X509_REQ", &arg0, &arg1, &arg2, &arg3))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(22), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(22), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(92), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(92), arg1) < 0)
      return NULL;
  }

  x2 = (int(*)(char *, int, int, void *))_cffi_to_c_pointer(arg2, _cffi_type(82));
  if (x2 == (int(*)(char *, int, int, void *))NULL && PyErr_Occurred())
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(25), arg3, (char **)&x3);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x3 = alloca(datasize);
    memset((void *)x3, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x3, _cffi_type(25), arg3) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = PEM_read_bio_X509_REQ(x0, x1, x2, x3); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(93));
}

static PyObject *
_cffi_f_PEM_write_bio_DSAPrivateKey(PyObject *self, PyObject *args)
{
  BIO * x0;
  DSA * x1;
  EVP_CIPHER const * x2;
  unsigned char * x3;
  int x4;
  int(* x5)(char *, int, int, void *);
  void * x6;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;
  PyObject *arg4;
  PyObject *arg5;
  PyObject *arg6;

  if (!PyArg_ParseTuple(args, "OOOOOOO:PEM_write_bio_DSAPrivateKey", &arg0, &arg1, &arg2, &arg3, &arg4, &arg5, &arg6))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(22), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(22), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(45), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(45), arg1) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(59), arg2, (char **)&x2);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x2 = alloca(datasize);
    memset((void *)x2, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x2, _cffi_type(59), arg2) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(3), arg3, (char **)&x3);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x3 = alloca(datasize);
    memset((void *)x3, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x3, _cffi_type(3), arg3) < 0)
      return NULL;
  }

  x4 = _cffi_to_c_SIGNED(arg4, int);
  if (x4 == (int)-1 && PyErr_Occurred())
    return NULL;

  x5 = (int(*)(char *, int, int, void *))_cffi_to_c_pointer(arg5, _cffi_type(82));
  if (x5 == (int(*)(char *, int, int, void *))NULL && PyErr_Occurred())
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(25), arg6, (char **)&x6);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x6 = alloca(datasize);
    memset((void *)x6, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x6, _cffi_type(25), arg6) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = PEM_write_bio_DSAPrivateKey(x0, x1, x2, x3, x4, x5, x6); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_PEM_write_bio_DSA_PUBKEY(PyObject *self, PyObject *args)
{
  BIO * x0;
  DSA * x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:PEM_write_bio_DSA_PUBKEY", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(22), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(22), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(45), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(45), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = PEM_write_bio_DSA_PUBKEY(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_PEM_write_bio_PKCS8PrivateKey(PyObject *self, PyObject *args)
{
  BIO * x0;
  EVP_PKEY * x1;
  EVP_CIPHER const * x2;
  char * x3;
  int x4;
  int(* x5)(char *, int, int, void *);
  void * x6;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;
  PyObject *arg4;
  PyObject *arg5;
  PyObject *arg6;

  if (!PyArg_ParseTuple(args, "OOOOOOO:PEM_write_bio_PKCS8PrivateKey", &arg0, &arg1, &arg2, &arg3, &arg4, &arg5, &arg6))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(22), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(22), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(62), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(62), arg1) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(59), arg2, (char **)&x2);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x2 = alloca(datasize);
    memset((void *)x2, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x2, _cffi_type(59), arg2) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(23), arg3, (char **)&x3);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x3 = alloca(datasize);
    memset((void *)x3, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x3, _cffi_type(23), arg3) < 0)
      return NULL;
  }

  x4 = _cffi_to_c_SIGNED(arg4, int);
  if (x4 == (int)-1 && PyErr_Occurred())
    return NULL;

  x5 = (int(*)(char *, int, int, void *))_cffi_to_c_pointer(arg5, _cffi_type(82));
  if (x5 == (int(*)(char *, int, int, void *))NULL && PyErr_Occurred())
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(25), arg6, (char **)&x6);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x6 = alloca(datasize);
    memset((void *)x6, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x6, _cffi_type(25), arg6) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = PEM_write_bio_PKCS8PrivateKey(x0, x1, x2, x3, x4, x5, x6); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_PEM_write_bio_PUBKEY(PyObject *self, PyObject *args)
{
  BIO * x0;
  EVP_PKEY * x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:PEM_write_bio_PUBKEY", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(22), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(22), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(62), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(62), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = PEM_write_bio_PUBKEY(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_PEM_write_bio_PrivateKey(PyObject *self, PyObject *args)
{
  BIO * x0;
  EVP_PKEY * x1;
  EVP_CIPHER const * x2;
  unsigned char * x3;
  int x4;
  int(* x5)(char *, int, int, void *);
  void * x6;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;
  PyObject *arg4;
  PyObject *arg5;
  PyObject *arg6;

  if (!PyArg_ParseTuple(args, "OOOOOOO:PEM_write_bio_PrivateKey", &arg0, &arg1, &arg2, &arg3, &arg4, &arg5, &arg6))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(22), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(22), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(62), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(62), arg1) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(59), arg2, (char **)&x2);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x2 = alloca(datasize);
    memset((void *)x2, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x2, _cffi_type(59), arg2) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(3), arg3, (char **)&x3);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x3 = alloca(datasize);
    memset((void *)x3, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x3, _cffi_type(3), arg3) < 0)
      return NULL;
  }

  x4 = _cffi_to_c_SIGNED(arg4, int);
  if (x4 == (int)-1 && PyErr_Occurred())
    return NULL;

  x5 = (int(*)(char *, int, int, void *))_cffi_to_c_pointer(arg5, _cffi_type(82));
  if (x5 == (int(*)(char *, int, int, void *))NULL && PyErr_Occurred())
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(25), arg6, (char **)&x6);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x6 = alloca(datasize);
    memset((void *)x6, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x6, _cffi_type(25), arg6) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = PEM_write_bio_PrivateKey(x0, x1, x2, x3, x4, x5, x6); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_PEM_write_bio_RSAPrivateKey(PyObject *self, PyObject *args)
{
  BIO * x0;
  RSA * x1;
  EVP_CIPHER const * x2;
  unsigned char * x3;
  int x4;
  int(* x5)(char *, int, int, void *);
  void * x6;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;
  PyObject *arg4;
  PyObject *arg5;
  PyObject *arg6;

  if (!PyArg_ParseTuple(args, "OOOOOOO:PEM_write_bio_RSAPrivateKey", &arg0, &arg1, &arg2, &arg3, &arg4, &arg5, &arg6))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(22), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(22), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(76), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(76), arg1) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(59), arg2, (char **)&x2);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x2 = alloca(datasize);
    memset((void *)x2, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x2, _cffi_type(59), arg2) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(3), arg3, (char **)&x3);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x3 = alloca(datasize);
    memset((void *)x3, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x3, _cffi_type(3), arg3) < 0)
      return NULL;
  }

  x4 = _cffi_to_c_SIGNED(arg4, int);
  if (x4 == (int)-1 && PyErr_Occurred())
    return NULL;

  x5 = (int(*)(char *, int, int, void *))_cffi_to_c_pointer(arg5, _cffi_type(82));
  if (x5 == (int(*)(char *, int, int, void *))NULL && PyErr_Occurred())
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(25), arg6, (char **)&x6);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x6 = alloca(datasize);
    memset((void *)x6, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x6, _cffi_type(25), arg6) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = PEM_write_bio_RSAPrivateKey(x0, x1, x2, x3, x4, x5, x6); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_PEM_write_bio_RSAPublicKey(PyObject *self, PyObject *args)
{
  BIO * x0;
  RSA const * x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:PEM_write_bio_RSAPublicKey", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(22), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(22), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(94), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(94), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = PEM_write_bio_RSAPublicKey(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_PEM_write_bio_X509(PyObject *self, PyObject *args)
{
  BIO * x0;
  X509 * x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:PEM_write_bio_X509", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(22), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(22), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(89), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(89), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = PEM_write_bio_X509(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_PEM_write_bio_X509_CRL(PyObject *self, PyObject *args)
{
  BIO * x0;
  X509_CRL * x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:PEM_write_bio_X509_CRL", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(22), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(22), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(91), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(91), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = PEM_write_bio_X509_CRL(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_PEM_write_bio_X509_REQ(PyObject *self, PyObject *args)
{
  BIO * x0;
  X509_REQ * x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:PEM_write_bio_X509_REQ", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(22), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(22), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(93), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(93), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = PEM_write_bio_X509_REQ(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_PKCS12_create(PyObject *self, PyObject *args)
{
  char * x0;
  char * x1;
  EVP_PKEY * x2;
  X509 * x3;
  Cryptography_STACK_OF_X509 * x4;
  int x5;
  int x6;
  int x7;
  int x8;
  int x9;
  Py_ssize_t datasize;
  PKCS12 * result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;
  PyObject *arg4;
  PyObject *arg5;
  PyObject *arg6;
  PyObject *arg7;
  PyObject *arg8;
  PyObject *arg9;

  if (!PyArg_ParseTuple(args, "OOOOOOOOOO:PKCS12_create", &arg0, &arg1, &arg2, &arg3, &arg4, &arg5, &arg6, &arg7, &arg8, &arg9))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(23), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(23), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(23), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(23), arg1) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(62), arg2, (char **)&x2);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x2 = alloca(datasize);
    memset((void *)x2, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x2, _cffi_type(62), arg2) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(89), arg3, (char **)&x3);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x3 = alloca(datasize);
    memset((void *)x3, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x3, _cffi_type(89), arg3) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(95), arg4, (char **)&x4);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x4 = alloca(datasize);
    memset((void *)x4, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x4, _cffi_type(95), arg4) < 0)
      return NULL;
  }

  x5 = _cffi_to_c_SIGNED(arg5, int);
  if (x5 == (int)-1 && PyErr_Occurred())
    return NULL;

  x6 = _cffi_to_c_SIGNED(arg6, int);
  if (x6 == (int)-1 && PyErr_Occurred())
    return NULL;

  x7 = _cffi_to_c_SIGNED(arg7, int);
  if (x7 == (int)-1 && PyErr_Occurred())
    return NULL;

  x8 = _cffi_to_c_SIGNED(arg8, int);
  if (x8 == (int)-1 && PyErr_Occurred())
    return NULL;

  x9 = _cffi_to_c_SIGNED(arg9, int);
  if (x9 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = PKCS12_create(x0, x1, x2, x3, x4, x5, x6, x7, x8, x9); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(96));
}

static PyObject *
_cffi_f_PKCS12_free(PyObject *self, PyObject *arg0)
{
  PKCS12 * x0;
  Py_ssize_t datasize;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(96), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(96), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { PKCS12_free(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_PKCS12_parse(PyObject *self, PyObject *args)
{
  PKCS12 * x0;
  char const * x1;
  EVP_PKEY * * x2;
  X509 * * x3;
  Cryptography_STACK_OF_X509 * * x4;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;
  PyObject *arg4;

  if (!PyArg_ParseTuple(args, "OOOOO:PKCS12_parse", &arg0, &arg1, &arg2, &arg3, &arg4))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(96), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(96), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(0), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(0), arg1) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(86), arg2, (char **)&x2);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x2 = alloca(datasize);
    memset((void *)x2, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x2, _cffi_type(86), arg2) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(88), arg3, (char **)&x3);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x3 = alloca(datasize);
    memset((void *)x3, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x3, _cffi_type(88), arg3) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(97), arg4, (char **)&x4);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x4 = alloca(datasize);
    memset((void *)x4, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x4, _cffi_type(97), arg4) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = PKCS12_parse(x0, x1, x2, x3, x4); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_PKCS5_PBKDF2_HMAC(PyObject *self, PyObject *args)
{
  char const * x0;
  int x1;
  unsigned char const * x2;
  int x3;
  int x4;
  EVP_MD const * x5;
  int x6;
  unsigned char * x7;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;
  PyObject *arg4;
  PyObject *arg5;
  PyObject *arg6;
  PyObject *arg7;

  if (!PyArg_ParseTuple(args, "OOOOOOOO:PKCS5_PBKDF2_HMAC", &arg0, &arg1, &arg2, &arg3, &arg4, &arg5, &arg6, &arg7))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(0), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(0), arg0) < 0)
      return NULL;
  }

  x1 = _cffi_to_c_SIGNED(arg1, int);
  if (x1 == (int)-1 && PyErr_Occurred())
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(1), arg2, (char **)&x2);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x2 = alloca(datasize);
    memset((void *)x2, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x2, _cffi_type(1), arg2) < 0)
      return NULL;
  }

  x3 = _cffi_to_c_SIGNED(arg3, int);
  if (x3 == (int)-1 && PyErr_Occurred())
    return NULL;

  x4 = _cffi_to_c_SIGNED(arg4, int);
  if (x4 == (int)-1 && PyErr_Occurred())
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(42), arg5, (char **)&x5);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x5 = alloca(datasize);
    memset((void *)x5, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x5, _cffi_type(42), arg5) < 0)
      return NULL;
  }

  x6 = _cffi_to_c_SIGNED(arg6, int);
  if (x6 == (int)-1 && PyErr_Occurred())
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(3), arg7, (char **)&x7);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x7 = alloca(datasize);
    memset((void *)x7, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x7, _cffi_type(3), arg7) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = PKCS5_PBKDF2_HMAC(x0, x1, x2, x3, x4, x5, x6, x7); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_PKCS5_PBKDF2_HMAC_SHA1(PyObject *self, PyObject *args)
{
  char const * x0;
  int x1;
  unsigned char const * x2;
  int x3;
  int x4;
  int x5;
  unsigned char * x6;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;
  PyObject *arg4;
  PyObject *arg5;
  PyObject *arg6;

  if (!PyArg_ParseTuple(args, "OOOOOOO:PKCS5_PBKDF2_HMAC_SHA1", &arg0, &arg1, &arg2, &arg3, &arg4, &arg5, &arg6))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(0), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(0), arg0) < 0)
      return NULL;
  }

  x1 = _cffi_to_c_SIGNED(arg1, int);
  if (x1 == (int)-1 && PyErr_Occurred())
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(1), arg2, (char **)&x2);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x2 = alloca(datasize);
    memset((void *)x2, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x2, _cffi_type(1), arg2) < 0)
      return NULL;
  }

  x3 = _cffi_to_c_SIGNED(arg3, int);
  if (x3 == (int)-1 && PyErr_Occurred())
    return NULL;

  x4 = _cffi_to_c_SIGNED(arg4, int);
  if (x4 == (int)-1 && PyErr_Occurred())
    return NULL;

  x5 = _cffi_to_c_SIGNED(arg5, int);
  if (x5 == (int)-1 && PyErr_Occurred())
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(3), arg6, (char **)&x6);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x6 = alloca(datasize);
    memset((void *)x6, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x6, _cffi_type(3), arg6) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = PKCS5_PBKDF2_HMAC_SHA1(x0, x1, x2, x3, x4, x5, x6); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_PKCS7_free(PyObject *self, PyObject *arg0)
{
  PKCS7 * x0;
  Py_ssize_t datasize;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(85), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(85), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { PKCS7_free(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_PKCS7_type_is_data(PyObject *self, PyObject *arg0)
{
  PKCS7 * x0;
  Py_ssize_t datasize;
  int result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(85), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(85), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = PKCS7_type_is_data(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_PKCS7_type_is_enveloped(PyObject *self, PyObject *arg0)
{
  PKCS7 * x0;
  Py_ssize_t datasize;
  int result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(85), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(85), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = PKCS7_type_is_enveloped(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_PKCS7_type_is_signed(PyObject *self, PyObject *arg0)
{
  PKCS7 * x0;
  Py_ssize_t datasize;
  int result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(85), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(85), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = PKCS7_type_is_signed(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_PKCS7_type_is_signedAndEnveloped(PyObject *self, PyObject *arg0)
{
  PKCS7 * x0;
  Py_ssize_t datasize;
  int result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(85), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(85), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = PKCS7_type_is_signedAndEnveloped(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_RAND_add(PyObject *self, PyObject *args)
{
  void const * x0;
  int x1;
  double x2;
  Py_ssize_t datasize;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;

  if (!PyArg_ParseTuple(args, "OOO:RAND_add", &arg0, &arg1, &arg2))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(14), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(14), arg0) < 0)
      return NULL;
  }

  x1 = _cffi_to_c_SIGNED(arg1, int);
  if (x1 == (int)-1 && PyErr_Occurred())
    return NULL;

  x2 = _cffi_to_c_double(arg2);
  if (x2 == (double)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { RAND_add(x0, x1, x2); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_RAND_bytes(PyObject *self, PyObject *args)
{
  unsigned char * x0;
  int x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:RAND_bytes", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(3), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(3), arg0) < 0)
      return NULL;
  }

  x1 = _cffi_to_c_SIGNED(arg1, int);
  if (x1 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = RAND_bytes(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_RAND_cleanup(PyObject *self, PyObject *no_arg)
{

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { RAND_cleanup(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_RAND_egd(PyObject *self, PyObject *arg0)
{
  char const * x0;
  Py_ssize_t datasize;
  int result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(0), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(0), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = RAND_egd(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_RAND_egd_bytes(PyObject *self, PyObject *args)
{
  char const * x0;
  int x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:RAND_egd_bytes", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(0), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(0), arg0) < 0)
      return NULL;
  }

  x1 = _cffi_to_c_SIGNED(arg1, int);
  if (x1 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = RAND_egd_bytes(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_RAND_file_name(PyObject *self, PyObject *args)
{
  char * x0;
  size_t x1;
  Py_ssize_t datasize;
  char const * result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:RAND_file_name", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(23), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(23), arg0) < 0)
      return NULL;
  }

  x1 = _cffi_to_c_UNSIGNED(arg1, size_t);
  if (x1 == (size_t)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = RAND_file_name(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(0));
}

static PyObject *
_cffi_f_RAND_load_file(PyObject *self, PyObject *args)
{
  char const * x0;
  long x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:RAND_load_file", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(0), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(0), arg0) < 0)
      return NULL;
  }

  x1 = _cffi_to_c_SIGNED(arg1, long);
  if (x1 == (long)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = RAND_load_file(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_RAND_pseudo_bytes(PyObject *self, PyObject *args)
{
  unsigned char * x0;
  int x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:RAND_pseudo_bytes", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(3), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(3), arg0) < 0)
      return NULL;
  }

  x1 = _cffi_to_c_SIGNED(arg1, int);
  if (x1 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = RAND_pseudo_bytes(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_RAND_query_egd_bytes(PyObject *self, PyObject *args)
{
  char const * x0;
  unsigned char * x1;
  int x2;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;

  if (!PyArg_ParseTuple(args, "OOO:RAND_query_egd_bytes", &arg0, &arg1, &arg2))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(0), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(0), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(3), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(3), arg1) < 0)
      return NULL;
  }

  x2 = _cffi_to_c_SIGNED(arg2, int);
  if (x2 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = RAND_query_egd_bytes(x0, x1, x2); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_RAND_seed(PyObject *self, PyObject *args)
{
  void const * x0;
  int x1;
  Py_ssize_t datasize;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:RAND_seed", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(14), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(14), arg0) < 0)
      return NULL;
  }

  x1 = _cffi_to_c_SIGNED(arg1, int);
  if (x1 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { RAND_seed(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_RAND_status(PyObject *self, PyObject *no_arg)
{
  int result;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = RAND_status(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_RAND_write_file(PyObject *self, PyObject *arg0)
{
  char const * x0;
  Py_ssize_t datasize;
  int result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(0), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(0), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = RAND_write_file(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_RSAPublicKey_dup(PyObject *self, PyObject *arg0)
{
  RSA * x0;
  Py_ssize_t datasize;
  RSA * result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(76), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(76), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = RSAPublicKey_dup(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(76));
}

static PyObject *
_cffi_f_RSA_blinding_off(PyObject *self, PyObject *arg0)
{
  RSA * x0;
  Py_ssize_t datasize;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(76), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(76), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { RSA_blinding_off(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_RSA_blinding_on(PyObject *self, PyObject *args)
{
  RSA * x0;
  BN_CTX * x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:RSA_blinding_on", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(76), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(76), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(36), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(36), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = RSA_blinding_on(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_RSA_check_key(PyObject *self, PyObject *arg0)
{
  RSA const * x0;
  Py_ssize_t datasize;
  int result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(94), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(94), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = RSA_check_key(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_RSA_free(PyObject *self, PyObject *arg0)
{
  RSA * x0;
  Py_ssize_t datasize;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(76), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(76), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { RSA_free(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_RSA_generate_key_ex(PyObject *self, PyObject *args)
{
  RSA * x0;
  int x1;
  BIGNUM * x2;
  BN_GENCB * x3;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;

  if (!PyArg_ParseTuple(args, "OOOO:RSA_generate_key_ex", &arg0, &arg1, &arg2, &arg3))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(76), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(76), arg0) < 0)
      return NULL;
  }

  x1 = _cffi_to_c_SIGNED(arg1, int);
  if (x1 == (int)-1 && PyErr_Occurred())
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(8), arg2, (char **)&x2);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x2 = alloca(datasize);
    memset((void *)x2, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x2, _cffi_type(8), arg2) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(98), arg3, (char **)&x3);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x3 = alloca(datasize);
    memset((void *)x3, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x3, _cffi_type(98), arg3) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = RSA_generate_key_ex(x0, x1, x2, x3); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_RSA_new(PyObject *self, PyObject *no_arg)
{
  RSA * result;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = RSA_new(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(76));
}

static PyObject *
_cffi_f_RSA_padding_add_PKCS1_OAEP(PyObject *self, PyObject *args)
{
  unsigned char * x0;
  int x1;
  unsigned char const * x2;
  int x3;
  unsigned char const * x4;
  int x5;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;
  PyObject *arg4;
  PyObject *arg5;

  if (!PyArg_ParseTuple(args, "OOOOOO:RSA_padding_add_PKCS1_OAEP", &arg0, &arg1, &arg2, &arg3, &arg4, &arg5))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(3), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(3), arg0) < 0)
      return NULL;
  }

  x1 = _cffi_to_c_SIGNED(arg1, int);
  if (x1 == (int)-1 && PyErr_Occurred())
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(1), arg2, (char **)&x2);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x2 = alloca(datasize);
    memset((void *)x2, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x2, _cffi_type(1), arg2) < 0)
      return NULL;
  }

  x3 = _cffi_to_c_SIGNED(arg3, int);
  if (x3 == (int)-1 && PyErr_Occurred())
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(1), arg4, (char **)&x4);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x4 = alloca(datasize);
    memset((void *)x4, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x4, _cffi_type(1), arg4) < 0)
      return NULL;
  }

  x5 = _cffi_to_c_SIGNED(arg5, int);
  if (x5 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = RSA_padding_add_PKCS1_OAEP(x0, x1, x2, x3, x4, x5); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_RSA_padding_add_PKCS1_PSS(PyObject *self, PyObject *args)
{
  RSA * x0;
  unsigned char * x1;
  unsigned char const * x2;
  EVP_MD const * x3;
  int x4;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;
  PyObject *arg4;

  if (!PyArg_ParseTuple(args, "OOOOO:RSA_padding_add_PKCS1_PSS", &arg0, &arg1, &arg2, &arg3, &arg4))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(76), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(76), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(3), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(3), arg1) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(1), arg2, (char **)&x2);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x2 = alloca(datasize);
    memset((void *)x2, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x2, _cffi_type(1), arg2) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(42), arg3, (char **)&x3);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x3 = alloca(datasize);
    memset((void *)x3, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x3, _cffi_type(42), arg3) < 0)
      return NULL;
  }

  x4 = _cffi_to_c_SIGNED(arg4, int);
  if (x4 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = RSA_padding_add_PKCS1_PSS(x0, x1, x2, x3, x4); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_RSA_padding_check_PKCS1_OAEP(PyObject *self, PyObject *args)
{
  unsigned char * x0;
  int x1;
  unsigned char const * x2;
  int x3;
  int x4;
  unsigned char const * x5;
  int x6;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;
  PyObject *arg4;
  PyObject *arg5;
  PyObject *arg6;

  if (!PyArg_ParseTuple(args, "OOOOOOO:RSA_padding_check_PKCS1_OAEP", &arg0, &arg1, &arg2, &arg3, &arg4, &arg5, &arg6))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(3), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(3), arg0) < 0)
      return NULL;
  }

  x1 = _cffi_to_c_SIGNED(arg1, int);
  if (x1 == (int)-1 && PyErr_Occurred())
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(1), arg2, (char **)&x2);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x2 = alloca(datasize);
    memset((void *)x2, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x2, _cffi_type(1), arg2) < 0)
      return NULL;
  }

  x3 = _cffi_to_c_SIGNED(arg3, int);
  if (x3 == (int)-1 && PyErr_Occurred())
    return NULL;

  x4 = _cffi_to_c_SIGNED(arg4, int);
  if (x4 == (int)-1 && PyErr_Occurred())
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(1), arg5, (char **)&x5);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x5 = alloca(datasize);
    memset((void *)x5, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x5, _cffi_type(1), arg5) < 0)
      return NULL;
  }

  x6 = _cffi_to_c_SIGNED(arg6, int);
  if (x6 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = RSA_padding_check_PKCS1_OAEP(x0, x1, x2, x3, x4, x5, x6); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_RSA_print(PyObject *self, PyObject *args)
{
  BIO * x0;
  RSA const * x1;
  int x2;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;

  if (!PyArg_ParseTuple(args, "OOO:RSA_print", &arg0, &arg1, &arg2))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(22), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(22), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(94), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(94), arg1) < 0)
      return NULL;
  }

  x2 = _cffi_to_c_SIGNED(arg2, int);
  if (x2 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = RSA_print(x0, x1, x2); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_RSA_private_decrypt(PyObject *self, PyObject *args)
{
  int x0;
  unsigned char const * x1;
  unsigned char * x2;
  RSA * x3;
  int x4;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;
  PyObject *arg4;

  if (!PyArg_ParseTuple(args, "OOOOO:RSA_private_decrypt", &arg0, &arg1, &arg2, &arg3, &arg4))
    return NULL;

  x0 = _cffi_to_c_SIGNED(arg0, int);
  if (x0 == (int)-1 && PyErr_Occurred())
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(1), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(1), arg1) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(3), arg2, (char **)&x2);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x2 = alloca(datasize);
    memset((void *)x2, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x2, _cffi_type(3), arg2) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(76), arg3, (char **)&x3);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x3 = alloca(datasize);
    memset((void *)x3, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x3, _cffi_type(76), arg3) < 0)
      return NULL;
  }

  x4 = _cffi_to_c_SIGNED(arg4, int);
  if (x4 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = RSA_private_decrypt(x0, x1, x2, x3, x4); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_RSA_private_encrypt(PyObject *self, PyObject *args)
{
  int x0;
  unsigned char const * x1;
  unsigned char * x2;
  RSA * x3;
  int x4;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;
  PyObject *arg4;

  if (!PyArg_ParseTuple(args, "OOOOO:RSA_private_encrypt", &arg0, &arg1, &arg2, &arg3, &arg4))
    return NULL;

  x0 = _cffi_to_c_SIGNED(arg0, int);
  if (x0 == (int)-1 && PyErr_Occurred())
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(1), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(1), arg1) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(3), arg2, (char **)&x2);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x2 = alloca(datasize);
    memset((void *)x2, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x2, _cffi_type(3), arg2) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(76), arg3, (char **)&x3);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x3 = alloca(datasize);
    memset((void *)x3, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x3, _cffi_type(76), arg3) < 0)
      return NULL;
  }

  x4 = _cffi_to_c_SIGNED(arg4, int);
  if (x4 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = RSA_private_encrypt(x0, x1, x2, x3, x4); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_RSA_public_decrypt(PyObject *self, PyObject *args)
{
  int x0;
  unsigned char const * x1;
  unsigned char * x2;
  RSA * x3;
  int x4;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;
  PyObject *arg4;

  if (!PyArg_ParseTuple(args, "OOOOO:RSA_public_decrypt", &arg0, &arg1, &arg2, &arg3, &arg4))
    return NULL;

  x0 = _cffi_to_c_SIGNED(arg0, int);
  if (x0 == (int)-1 && PyErr_Occurred())
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(1), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(1), arg1) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(3), arg2, (char **)&x2);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x2 = alloca(datasize);
    memset((void *)x2, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x2, _cffi_type(3), arg2) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(76), arg3, (char **)&x3);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x3 = alloca(datasize);
    memset((void *)x3, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x3, _cffi_type(76), arg3) < 0)
      return NULL;
  }

  x4 = _cffi_to_c_SIGNED(arg4, int);
  if (x4 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = RSA_public_decrypt(x0, x1, x2, x3, x4); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_RSA_public_encrypt(PyObject *self, PyObject *args)
{
  int x0;
  unsigned char const * x1;
  unsigned char * x2;
  RSA * x3;
  int x4;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;
  PyObject *arg4;

  if (!PyArg_ParseTuple(args, "OOOOO:RSA_public_encrypt", &arg0, &arg1, &arg2, &arg3, &arg4))
    return NULL;

  x0 = _cffi_to_c_SIGNED(arg0, int);
  if (x0 == (int)-1 && PyErr_Occurred())
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(1), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(1), arg1) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(3), arg2, (char **)&x2);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x2 = alloca(datasize);
    memset((void *)x2, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x2, _cffi_type(3), arg2) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(76), arg3, (char **)&x3);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x3 = alloca(datasize);
    memset((void *)x3, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x3, _cffi_type(76), arg3) < 0)
      return NULL;
  }

  x4 = _cffi_to_c_SIGNED(arg4, int);
  if (x4 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = RSA_public_encrypt(x0, x1, x2, x3, x4); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_RSA_size(PyObject *self, PyObject *arg0)
{
  RSA const * x0;
  Py_ssize_t datasize;
  int result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(94), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(94), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = RSA_size(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_RSA_verify_PKCS1_PSS(PyObject *self, PyObject *args)
{
  RSA * x0;
  unsigned char const * x1;
  EVP_MD const * x2;
  unsigned char const * x3;
  int x4;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;
  PyObject *arg4;

  if (!PyArg_ParseTuple(args, "OOOOO:RSA_verify_PKCS1_PSS", &arg0, &arg1, &arg2, &arg3, &arg4))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(76), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(76), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(1), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(1), arg1) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(42), arg2, (char **)&x2);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x2 = alloca(datasize);
    memset((void *)x2, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x2, _cffi_type(42), arg2) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(1), arg3, (char **)&x3);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x3 = alloca(datasize);
    memset((void *)x3, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x3, _cffi_type(1), arg3) < 0)
      return NULL;
  }

  x4 = _cffi_to_c_SIGNED(arg4, int);
  if (x4 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = RSA_verify_PKCS1_PSS(x0, x1, x2, x3, x4); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_SSL_CIPHER_get_bits(PyObject *self, PyObject *args)
{
  SSL_CIPHER const * x0;
  int * x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:SSL_CIPHER_get_bits", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(99), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(99), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(37), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(37), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = SSL_CIPHER_get_bits(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_SSL_CIPHER_get_name(PyObject *self, PyObject *arg0)
{
  SSL_CIPHER const * x0;
  Py_ssize_t datasize;
  char const * result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(99), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(99), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = SSL_CIPHER_get_name(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(0));
}

static PyObject *
_cffi_f_SSL_CIPHER_get_version(PyObject *self, PyObject *arg0)
{
  SSL_CIPHER const * x0;
  Py_ssize_t datasize;
  char * result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(99), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(99), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = SSL_CIPHER_get_version(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(23));
}

static PyObject *
_cffi_f_SSL_CTX_add_client_CA(PyObject *self, PyObject *args)
{
  SSL_CTX * x0;
  X509 * x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:SSL_CTX_add_client_CA", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(100), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(100), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(89), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(89), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = SSL_CTX_add_client_CA(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_SSL_CTX_add_extra_chain_cert(PyObject *self, PyObject *args)
{
  SSL_CTX * x0;
  X509 * x1;
  Py_ssize_t datasize;
  long result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:SSL_CTX_add_extra_chain_cert", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(100), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(100), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(89), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(89), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = SSL_CTX_add_extra_chain_cert(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, long);
}

static PyObject *
_cffi_f_SSL_CTX_free(PyObject *self, PyObject *arg0)
{
  SSL_CTX * x0;
  Py_ssize_t datasize;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(100), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(100), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { SSL_CTX_free(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_SSL_CTX_get_cert_store(PyObject *self, PyObject *arg0)
{
  SSL_CTX const * x0;
  Py_ssize_t datasize;
  X509_STORE * result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(101), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(101), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = SSL_CTX_get_cert_store(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(102));
}

static PyObject *
_cffi_f_SSL_CTX_get_info_callback(PyObject *self, PyObject *arg0)
{
  SSL_CTX * x0;
  Py_ssize_t datasize;
  void(* result)(SSL const *, int, int);

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(100), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(100), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = SSL_CTX_get_info_callback(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(103));
}

static PyObject *
_cffi_f_SSL_CTX_get_mode(PyObject *self, PyObject *arg0)
{
  SSL_CTX * x0;
  Py_ssize_t datasize;
  long result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(100), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(100), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = SSL_CTX_get_mode(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, long);
}

static PyObject *
_cffi_f_SSL_CTX_get_options(PyObject *self, PyObject *arg0)
{
  SSL_CTX * x0;
  Py_ssize_t datasize;
  long result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(100), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(100), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = SSL_CTX_get_options(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, long);
}

static PyObject *
_cffi_f_SSL_CTX_get_session_cache_mode(PyObject *self, PyObject *arg0)
{
  SSL_CTX * x0;
  Py_ssize_t datasize;
  long result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(100), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(100), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = SSL_CTX_get_session_cache_mode(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, long);
}

static PyObject *
_cffi_f_SSL_CTX_get_timeout(PyObject *self, PyObject *arg0)
{
  SSL_CTX const * x0;
  Py_ssize_t datasize;
  long result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(101), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(101), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = SSL_CTX_get_timeout(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, long);
}

static PyObject *
_cffi_f_SSL_CTX_get_verify_callback(PyObject *self, PyObject *arg0)
{
  SSL_CTX const * x0;
  Py_ssize_t datasize;
  int(* result)(int, X509_STORE_CTX *);

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(101), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(101), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = SSL_CTX_get_verify_callback(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(104));
}

static PyObject *
_cffi_f_SSL_CTX_get_verify_depth(PyObject *self, PyObject *arg0)
{
  SSL_CTX const * x0;
  Py_ssize_t datasize;
  int result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(101), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(101), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = SSL_CTX_get_verify_depth(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_SSL_CTX_get_verify_mode(PyObject *self, PyObject *arg0)
{
  SSL_CTX const * x0;
  Py_ssize_t datasize;
  int result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(101), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(101), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = SSL_CTX_get_verify_mode(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_SSL_CTX_load_verify_locations(PyObject *self, PyObject *args)
{
  SSL_CTX * x0;
  char const * x1;
  char const * x2;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;

  if (!PyArg_ParseTuple(args, "OOO:SSL_CTX_load_verify_locations", &arg0, &arg1, &arg2))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(100), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(100), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(0), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(0), arg1) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(0), arg2, (char **)&x2);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x2 = alloca(datasize);
    memset((void *)x2, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x2, _cffi_type(0), arg2) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = SSL_CTX_load_verify_locations(x0, x1, x2); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_SSL_CTX_new(PyObject *self, PyObject *arg0)
{
  SSL_METHOD * x0;
  Py_ssize_t datasize;
  SSL_CTX * result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(105), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(105), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = SSL_CTX_new(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(100));
}

static PyObject *
_cffi_f_SSL_CTX_set_cert_store(PyObject *self, PyObject *args)
{
  SSL_CTX * x0;
  X509_STORE * x1;
  Py_ssize_t datasize;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:SSL_CTX_set_cert_store", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(100), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(100), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(102), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(102), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { SSL_CTX_set_cert_store(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_SSL_CTX_set_cipher_list(PyObject *self, PyObject *args)
{
  SSL_CTX * x0;
  char const * x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:SSL_CTX_set_cipher_list", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(100), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(100), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(0), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(0), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = SSL_CTX_set_cipher_list(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_SSL_CTX_set_client_CA_list(PyObject *self, PyObject *args)
{
  SSL_CTX * x0;
  Cryptography_STACK_OF_X509_NAME * x1;
  Py_ssize_t datasize;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:SSL_CTX_set_client_CA_list", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(100), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(100), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(106), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(106), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { SSL_CTX_set_client_CA_list(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_SSL_CTX_set_default_passwd_cb(PyObject *self, PyObject *args)
{
  SSL_CTX * x0;
  int(* x1)(char *, int, int, void *);
  Py_ssize_t datasize;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:SSL_CTX_set_default_passwd_cb", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(100), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(100), arg0) < 0)
      return NULL;
  }

  x1 = (int(*)(char *, int, int, void *))_cffi_to_c_pointer(arg1, _cffi_type(82));
  if (x1 == (int(*)(char *, int, int, void *))NULL && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { SSL_CTX_set_default_passwd_cb(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_SSL_CTX_set_default_passwd_cb_userdata(PyObject *self, PyObject *args)
{
  SSL_CTX * x0;
  void * x1;
  Py_ssize_t datasize;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:SSL_CTX_set_default_passwd_cb_userdata", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(100), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(100), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(25), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(25), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { SSL_CTX_set_default_passwd_cb_userdata(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_SSL_CTX_set_default_verify_paths(PyObject *self, PyObject *arg0)
{
  SSL_CTX * x0;
  Py_ssize_t datasize;
  int result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(100), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(100), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = SSL_CTX_set_default_verify_paths(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_SSL_CTX_set_info_callback(PyObject *self, PyObject *args)
{
  SSL_CTX * x0;
  void(* x1)(SSL const *, int, int);
  Py_ssize_t datasize;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:SSL_CTX_set_info_callback", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(100), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(100), arg0) < 0)
      return NULL;
  }

  x1 = (void(*)(SSL const *, int, int))_cffi_to_c_pointer(arg1, _cffi_type(103));
  if (x1 == (void(*)(SSL const *, int, int))NULL && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { SSL_CTX_set_info_callback(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_SSL_CTX_set_mode(PyObject *self, PyObject *args)
{
  SSL_CTX * x0;
  long x1;
  Py_ssize_t datasize;
  long result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:SSL_CTX_set_mode", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(100), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(100), arg0) < 0)
      return NULL;
  }

  x1 = _cffi_to_c_SIGNED(arg1, long);
  if (x1 == (long)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = SSL_CTX_set_mode(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, long);
}

static PyObject *
_cffi_f_SSL_CTX_set_options(PyObject *self, PyObject *args)
{
  SSL_CTX * x0;
  long x1;
  Py_ssize_t datasize;
  long result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:SSL_CTX_set_options", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(100), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(100), arg0) < 0)
      return NULL;
  }

  x1 = _cffi_to_c_SIGNED(arg1, long);
  if (x1 == (long)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = SSL_CTX_set_options(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, long);
}

static PyObject *
_cffi_f_SSL_CTX_set_session_cache_mode(PyObject *self, PyObject *args)
{
  SSL_CTX * x0;
  long x1;
  Py_ssize_t datasize;
  long result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:SSL_CTX_set_session_cache_mode", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(100), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(100), arg0) < 0)
      return NULL;
  }

  x1 = _cffi_to_c_SIGNED(arg1, long);
  if (x1 == (long)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = SSL_CTX_set_session_cache_mode(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, long);
}

static PyObject *
_cffi_f_SSL_CTX_set_timeout(PyObject *self, PyObject *args)
{
  SSL_CTX * x0;
  long x1;
  Py_ssize_t datasize;
  long result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:SSL_CTX_set_timeout", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(100), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(100), arg0) < 0)
      return NULL;
  }

  x1 = _cffi_to_c_SIGNED(arg1, long);
  if (x1 == (long)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = SSL_CTX_set_timeout(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, long);
}

static PyObject *
_cffi_f_SSL_CTX_set_tlsext_servername_callback(PyObject *self, PyObject *args)
{
  SSL_CTX * x0;
  int(* x1)(SSL const *, int *, void *);
  Py_ssize_t datasize;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:SSL_CTX_set_tlsext_servername_callback", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(100), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(100), arg0) < 0)
      return NULL;
  }

  x1 = (int(*)(SSL const *, int *, void *))_cffi_to_c_pointer(arg1, _cffi_type(107));
  if (x1 == (int(*)(SSL const *, int *, void *))NULL && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { SSL_CTX_set_tlsext_servername_callback(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_SSL_CTX_set_tmp_dh(PyObject *self, PyObject *args)
{
  SSL_CTX * x0;
  DH * x1;
  Py_ssize_t datasize;
  long result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:SSL_CTX_set_tmp_dh", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(100), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(100), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(44), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(44), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = SSL_CTX_set_tmp_dh(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, long);
}

static PyObject *
_cffi_f_SSL_CTX_set_tmp_ecdh(PyObject *self, PyObject *args)
{
  SSL_CTX * x0;
  EC_KEY * x1;
  Py_ssize_t datasize;
  long result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:SSL_CTX_set_tmp_ecdh", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(100), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(100), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(49), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(49), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = SSL_CTX_set_tmp_ecdh(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, long);
}

static PyObject *
_cffi_f_SSL_CTX_set_verify(PyObject *self, PyObject *args)
{
  SSL_CTX * x0;
  int x1;
  int(* x2)(int, X509_STORE_CTX *);
  Py_ssize_t datasize;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;

  if (!PyArg_ParseTuple(args, "OOO:SSL_CTX_set_verify", &arg0, &arg1, &arg2))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(100), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(100), arg0) < 0)
      return NULL;
  }

  x1 = _cffi_to_c_SIGNED(arg1, int);
  if (x1 == (int)-1 && PyErr_Occurred())
    return NULL;

  x2 = (int(*)(int, X509_STORE_CTX *))_cffi_to_c_pointer(arg2, _cffi_type(104));
  if (x2 == (int(*)(int, X509_STORE_CTX *))NULL && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { SSL_CTX_set_verify(x0, x1, x2); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_SSL_CTX_set_verify_depth(PyObject *self, PyObject *args)
{
  SSL_CTX * x0;
  int x1;
  Py_ssize_t datasize;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:SSL_CTX_set_verify_depth", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(100), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(100), arg0) < 0)
      return NULL;
  }

  x1 = _cffi_to_c_SIGNED(arg1, int);
  if (x1 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { SSL_CTX_set_verify_depth(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_SSL_CTX_use_PrivateKey(PyObject *self, PyObject *args)
{
  SSL_CTX * x0;
  EVP_PKEY * x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:SSL_CTX_use_PrivateKey", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(100), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(100), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(62), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(62), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = SSL_CTX_use_PrivateKey(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_SSL_CTX_use_PrivateKey_file(PyObject *self, PyObject *args)
{
  SSL_CTX * x0;
  char const * x1;
  int x2;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;

  if (!PyArg_ParseTuple(args, "OOO:SSL_CTX_use_PrivateKey_file", &arg0, &arg1, &arg2))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(100), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(100), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(0), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(0), arg1) < 0)
      return NULL;
  }

  x2 = _cffi_to_c_SIGNED(arg2, int);
  if (x2 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = SSL_CTX_use_PrivateKey_file(x0, x1, x2); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_SSL_CTX_use_certificate(PyObject *self, PyObject *args)
{
  SSL_CTX * x0;
  X509 * x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:SSL_CTX_use_certificate", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(100), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(100), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(89), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(89), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = SSL_CTX_use_certificate(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_SSL_CTX_use_certificate_chain_file(PyObject *self, PyObject *args)
{
  SSL_CTX * x0;
  char const * x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:SSL_CTX_use_certificate_chain_file", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(100), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(100), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(0), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(0), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = SSL_CTX_use_certificate_chain_file(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_SSL_CTX_use_certificate_file(PyObject *self, PyObject *args)
{
  SSL_CTX * x0;
  char const * x1;
  int x2;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;

  if (!PyArg_ParseTuple(args, "OOO:SSL_CTX_use_certificate_file", &arg0, &arg1, &arg2))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(100), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(100), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(0), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(0), arg1) < 0)
      return NULL;
  }

  x2 = _cffi_to_c_SIGNED(arg2, int);
  if (x2 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = SSL_CTX_use_certificate_file(x0, x1, x2); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_SSL_SESSION_free(PyObject *self, PyObject *arg0)
{
  SSL_SESSION * x0;
  Py_ssize_t datasize;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(108), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(108), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { SSL_SESSION_free(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_SSL_do_handshake(PyObject *self, PyObject *arg0)
{
  SSL * x0;
  Py_ssize_t datasize;
  int result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(109), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(109), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = SSL_do_handshake(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_SSL_free(PyObject *self, PyObject *arg0)
{
  SSL * x0;
  Py_ssize_t datasize;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(109), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(109), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { SSL_free(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_SSL_get1_session(PyObject *self, PyObject *arg0)
{
  SSL * x0;
  Py_ssize_t datasize;
  SSL_SESSION * result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(109), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(109), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = SSL_get1_session(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(108));
}

static PyObject *
_cffi_f_SSL_get_cipher_list(PyObject *self, PyObject *args)
{
  SSL const * x0;
  int x1;
  Py_ssize_t datasize;
  char const * result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:SSL_get_cipher_list", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(110), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(110), arg0) < 0)
      return NULL;
  }

  x1 = _cffi_to_c_SIGNED(arg1, int);
  if (x1 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = SSL_get_cipher_list(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(0));
}

static PyObject *
_cffi_f_SSL_get_client_CA_list(PyObject *self, PyObject *arg0)
{
  SSL const * x0;
  Py_ssize_t datasize;
  Cryptography_STACK_OF_X509_NAME * result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(110), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(110), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = SSL_get_client_CA_list(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(106));
}

static PyObject *
_cffi_f_SSL_get_current_cipher(PyObject *self, PyObject *arg0)
{
  SSL const * x0;
  Py_ssize_t datasize;
  SSL_CIPHER const * result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(110), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(110), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = SSL_get_current_cipher(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(99));
}

static PyObject *
_cffi_f_SSL_get_error(PyObject *self, PyObject *args)
{
  SSL const * x0;
  int x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:SSL_get_error", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(110), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(110), arg0) < 0)
      return NULL;
  }

  x1 = _cffi_to_c_SIGNED(arg1, int);
  if (x1 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = SSL_get_error(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_SSL_get_info_callback(PyObject *self, PyObject *arg0)
{
  SSL const * x0;
  Py_ssize_t datasize;
  void(* result)(SSL const *, int, int);

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(110), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(110), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = SSL_get_info_callback(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(103));
}

static PyObject *
_cffi_f_SSL_get_mode(PyObject *self, PyObject *arg0)
{
  SSL * x0;
  Py_ssize_t datasize;
  long result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(109), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(109), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = SSL_get_mode(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, long);
}

static PyObject *
_cffi_f_SSL_get_options(PyObject *self, PyObject *arg0)
{
  SSL * x0;
  Py_ssize_t datasize;
  long result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(109), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(109), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = SSL_get_options(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, long);
}

static PyObject *
_cffi_f_SSL_get_peer_cert_chain(PyObject *self, PyObject *arg0)
{
  SSL const * x0;
  Py_ssize_t datasize;
  Cryptography_STACK_OF_X509 * result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(110), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(110), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = SSL_get_peer_cert_chain(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(95));
}

static PyObject *
_cffi_f_SSL_get_peer_certificate(PyObject *self, PyObject *arg0)
{
  SSL const * x0;
  Py_ssize_t datasize;
  X509 * result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(110), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(110), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = SSL_get_peer_certificate(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(89));
}

static PyObject *
_cffi_f_SSL_get_servername(PyObject *self, PyObject *args)
{
  SSL const * x0;
  int x1;
  Py_ssize_t datasize;
  char const * result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:SSL_get_servername", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(110), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(110), arg0) < 0)
      return NULL;
  }

  x1 = _cffi_to_c_SIGNED(arg1, int);
  if (x1 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = SSL_get_servername(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(0));
}

static PyObject *
_cffi_f_SSL_get_shutdown(PyObject *self, PyObject *arg0)
{
  SSL const * x0;
  Py_ssize_t datasize;
  int result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(110), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(110), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = SSL_get_shutdown(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_SSL_get_verify_callback(PyObject *self, PyObject *arg0)
{
  SSL const * x0;
  Py_ssize_t datasize;
  int(* result)(int, X509_STORE_CTX *);

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(110), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(110), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = SSL_get_verify_callback(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(104));
}

static PyObject *
_cffi_f_SSL_get_verify_depth(PyObject *self, PyObject *arg0)
{
  SSL const * x0;
  Py_ssize_t datasize;
  int result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(110), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(110), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = SSL_get_verify_depth(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_SSL_get_verify_mode(PyObject *self, PyObject *arg0)
{
  SSL const * x0;
  Py_ssize_t datasize;
  int result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(110), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(110), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = SSL_get_verify_mode(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_SSL_library_init(PyObject *self, PyObject *no_arg)
{
  int result;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = SSL_library_init(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_SSL_load_error_strings(PyObject *self, PyObject *no_arg)
{

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { SSL_load_error_strings(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_SSL_new(PyObject *self, PyObject *arg0)
{
  SSL_CTX * x0;
  Py_ssize_t datasize;
  SSL * result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(100), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(100), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = SSL_new(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(109));
}

static PyObject *
_cffi_f_SSL_pending(PyObject *self, PyObject *arg0)
{
  SSL const * x0;
  Py_ssize_t datasize;
  int result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(110), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(110), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = SSL_pending(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_SSL_read(PyObject *self, PyObject *args)
{
  SSL * x0;
  void * x1;
  int x2;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;

  if (!PyArg_ParseTuple(args, "OOO:SSL_read", &arg0, &arg1, &arg2))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(109), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(109), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(25), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(25), arg1) < 0)
      return NULL;
  }

  x2 = _cffi_to_c_SIGNED(arg2, int);
  if (x2 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = SSL_read(x0, x1, x2); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_SSL_set_SSL_CTX(PyObject *self, PyObject *args)
{
  SSL * x0;
  SSL_CTX * x1;
  Py_ssize_t datasize;
  SSL_CTX * result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:SSL_set_SSL_CTX", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(109), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(109), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(100), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(100), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = SSL_set_SSL_CTX(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(100));
}

static PyObject *
_cffi_f_SSL_set_accept_state(PyObject *self, PyObject *arg0)
{
  SSL * x0;
  Py_ssize_t datasize;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(109), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(109), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { SSL_set_accept_state(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_SSL_set_bio(PyObject *self, PyObject *args)
{
  SSL * x0;
  BIO * x1;
  BIO * x2;
  Py_ssize_t datasize;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;

  if (!PyArg_ParseTuple(args, "OOO:SSL_set_bio", &arg0, &arg1, &arg2))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(109), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(109), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(22), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(22), arg1) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(22), arg2, (char **)&x2);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x2 = alloca(datasize);
    memset((void *)x2, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x2, _cffi_type(22), arg2) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { SSL_set_bio(x0, x1, x2); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_SSL_set_connect_state(PyObject *self, PyObject *arg0)
{
  SSL * x0;
  Py_ssize_t datasize;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(109), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(109), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { SSL_set_connect_state(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_SSL_set_fd(PyObject *self, PyObject *args)
{
  SSL * x0;
  int x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:SSL_set_fd", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(109), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(109), arg0) < 0)
      return NULL;
  }

  x1 = _cffi_to_c_SIGNED(arg1, int);
  if (x1 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = SSL_set_fd(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_SSL_set_info_callback(PyObject *self, PyObject *args)
{
  SSL * x0;
  void(* x1)(SSL const *, int, int);
  Py_ssize_t datasize;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:SSL_set_info_callback", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(109), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(109), arg0) < 0)
      return NULL;
  }

  x1 = (void(*)(SSL const *, int, int))_cffi_to_c_pointer(arg1, _cffi_type(103));
  if (x1 == (void(*)(SSL const *, int, int))NULL && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { SSL_set_info_callback(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_SSL_set_mode(PyObject *self, PyObject *args)
{
  SSL * x0;
  long x1;
  Py_ssize_t datasize;
  long result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:SSL_set_mode", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(109), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(109), arg0) < 0)
      return NULL;
  }

  x1 = _cffi_to_c_SIGNED(arg1, long);
  if (x1 == (long)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = SSL_set_mode(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, long);
}

static PyObject *
_cffi_f_SSL_set_options(PyObject *self, PyObject *args)
{
  SSL * x0;
  long x1;
  Py_ssize_t datasize;
  long result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:SSL_set_options", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(109), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(109), arg0) < 0)
      return NULL;
  }

  x1 = _cffi_to_c_SIGNED(arg1, long);
  if (x1 == (long)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = SSL_set_options(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, long);
}

static PyObject *
_cffi_f_SSL_set_session(PyObject *self, PyObject *args)
{
  SSL * x0;
  SSL_SESSION * x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:SSL_set_session", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(109), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(109), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(108), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(108), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = SSL_set_session(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_SSL_set_shutdown(PyObject *self, PyObject *args)
{
  SSL * x0;
  int x1;
  Py_ssize_t datasize;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:SSL_set_shutdown", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(109), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(109), arg0) < 0)
      return NULL;
  }

  x1 = _cffi_to_c_SIGNED(arg1, int);
  if (x1 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { SSL_set_shutdown(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_SSL_set_tlsext_host_name(PyObject *self, PyObject *args)
{
  SSL * x0;
  char * x1;
  Py_ssize_t datasize;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:SSL_set_tlsext_host_name", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(109), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(109), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(23), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(23), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { SSL_set_tlsext_host_name(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_SSL_set_verify_depth(PyObject *self, PyObject *args)
{
  SSL * x0;
  int x1;
  Py_ssize_t datasize;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:SSL_set_verify_depth", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(109), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(109), arg0) < 0)
      return NULL;
  }

  x1 = _cffi_to_c_SIGNED(arg1, int);
  if (x1 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { SSL_set_verify_depth(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_SSL_shutdown(PyObject *self, PyObject *arg0)
{
  SSL * x0;
  Py_ssize_t datasize;
  int result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(109), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(109), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = SSL_shutdown(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_SSL_total_renegotiations(PyObject *self, PyObject *arg0)
{
  SSL * x0;
  Py_ssize_t datasize;
  long result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(109), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(109), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = SSL_total_renegotiations(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, long);
}

static PyObject *
_cffi_f_SSL_want_read(PyObject *self, PyObject *arg0)
{
  SSL const * x0;
  Py_ssize_t datasize;
  int result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(110), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(110), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = SSL_want_read(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_SSL_want_write(PyObject *self, PyObject *arg0)
{
  SSL const * x0;
  Py_ssize_t datasize;
  int result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(110), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(110), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = SSL_want_write(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_SSL_write(PyObject *self, PyObject *args)
{
  SSL * x0;
  void const * x1;
  int x2;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;

  if (!PyArg_ParseTuple(args, "OOO:SSL_write", &arg0, &arg1, &arg2))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(109), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(109), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(14), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(14), arg1) < 0)
      return NULL;
  }

  x2 = _cffi_to_c_SIGNED(arg2, int);
  if (x2 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = SSL_write(x0, x1, x2); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_SSLeay(PyObject *self, PyObject *no_arg)
{
  unsigned long result;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = SSLeay(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_UNSIGNED(result, unsigned long);
}

static PyObject *
_cffi_f_SSLeay_version(PyObject *self, PyObject *arg0)
{
  int x0;
  char const * result;

  x0 = _cffi_to_c_SIGNED(arg0, int);
  if (x0 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = SSLeay_version(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(0));
}

static PyObject *
_cffi_f_SSLv23_client_method(PyObject *self, PyObject *no_arg)
{
  SSL_METHOD const * result;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = SSLv23_client_method(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(48));
}

static PyObject *
_cffi_f_SSLv23_method(PyObject *self, PyObject *no_arg)
{
  SSL_METHOD const * result;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = SSLv23_method(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(48));
}

static PyObject *
_cffi_f_SSLv23_server_method(PyObject *self, PyObject *no_arg)
{
  SSL_METHOD const * result;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = SSLv23_server_method(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(48));
}

static PyObject *
_cffi_f_SSLv2_client_method(PyObject *self, PyObject *no_arg)
{
  SSL_METHOD const * result;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = SSLv2_client_method(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(48));
}

static PyObject *
_cffi_f_SSLv2_method(PyObject *self, PyObject *no_arg)
{
  SSL_METHOD const * result;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = SSLv2_method(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(48));
}

static PyObject *
_cffi_f_SSLv2_server_method(PyObject *self, PyObject *no_arg)
{
  SSL_METHOD const * result;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = SSLv2_server_method(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(48));
}

static PyObject *
_cffi_f_SSLv3_client_method(PyObject *self, PyObject *no_arg)
{
  SSL_METHOD const * result;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = SSLv3_client_method(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(48));
}

static PyObject *
_cffi_f_SSLv3_method(PyObject *self, PyObject *no_arg)
{
  SSL_METHOD const * result;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = SSLv3_method(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(48));
}

static PyObject *
_cffi_f_SSLv3_server_method(PyObject *self, PyObject *no_arg)
{
  SSL_METHOD const * result;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = SSLv3_server_method(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(48));
}

static PyObject *
_cffi_f_TLSv1_1_client_method(PyObject *self, PyObject *no_arg)
{
  SSL_METHOD const * result;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = TLSv1_1_client_method(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(48));
}

static PyObject *
_cffi_f_TLSv1_1_method(PyObject *self, PyObject *no_arg)
{
  SSL_METHOD const * result;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = TLSv1_1_method(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(48));
}

static PyObject *
_cffi_f_TLSv1_1_server_method(PyObject *self, PyObject *no_arg)
{
  SSL_METHOD const * result;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = TLSv1_1_server_method(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(48));
}

static PyObject *
_cffi_f_TLSv1_2_client_method(PyObject *self, PyObject *no_arg)
{
  SSL_METHOD const * result;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = TLSv1_2_client_method(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(48));
}

static PyObject *
_cffi_f_TLSv1_2_method(PyObject *self, PyObject *no_arg)
{
  SSL_METHOD const * result;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = TLSv1_2_method(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(48));
}

static PyObject *
_cffi_f_TLSv1_2_server_method(PyObject *self, PyObject *no_arg)
{
  SSL_METHOD const * result;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = TLSv1_2_server_method(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(48));
}

static PyObject *
_cffi_f_TLSv1_client_method(PyObject *self, PyObject *no_arg)
{
  SSL_METHOD const * result;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = TLSv1_client_method(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(48));
}

static PyObject *
_cffi_f_TLSv1_method(PyObject *self, PyObject *no_arg)
{
  SSL_METHOD const * result;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = TLSv1_method(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(48));
}

static PyObject *
_cffi_f_TLSv1_server_method(PyObject *self, PyObject *no_arg)
{
  SSL_METHOD const * result;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = TLSv1_server_method(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(48));
}

static PyObject *
_cffi_f_X509V3_EXT_get(PyObject *self, PyObject *arg0)
{
  X509_EXTENSION * x0;
  Py_ssize_t datasize;
  X509V3_EXT_METHOD const * result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(111), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(111), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = X509V3_EXT_get(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(112));
}

static PyObject *
_cffi_f_X509V3_EXT_get_nid(PyObject *self, PyObject *arg0)
{
  int x0;
  X509V3_EXT_METHOD const * result;

  x0 = _cffi_to_c_SIGNED(arg0, int);
  if (x0 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = X509V3_EXT_get_nid(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(112));
}

static PyObject *
_cffi_f_X509V3_EXT_nconf(PyObject *self, PyObject *args)
{
  CONF * x0;
  X509V3_CTX * x1;
  char * x2;
  char * x3;
  Py_ssize_t datasize;
  X509_EXTENSION * result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;

  if (!PyArg_ParseTuple(args, "OOOO:X509V3_EXT_nconf", &arg0, &arg1, &arg2, &arg3))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(113), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(113), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(114), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(114), arg1) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(23), arg2, (char **)&x2);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x2 = alloca(datasize);
    memset((void *)x2, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x2, _cffi_type(23), arg2) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(23), arg3, (char **)&x3);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x3 = alloca(datasize);
    memset((void *)x3, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x3, _cffi_type(23), arg3) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = X509V3_EXT_nconf(x0, x1, x2, x3); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(111));
}

static PyObject *
_cffi_f_X509V3_EXT_print(PyObject *self, PyObject *args)
{
  BIO * x0;
  X509_EXTENSION * x1;
  unsigned long x2;
  int x3;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;

  if (!PyArg_ParseTuple(args, "OOOO:X509V3_EXT_print", &arg0, &arg1, &arg2, &arg3))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(22), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(22), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(111), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(111), arg1) < 0)
      return NULL;
  }

  x2 = _cffi_to_c_UNSIGNED(arg2, unsigned long);
  if (x2 == (unsigned long)-1 && PyErr_Occurred())
    return NULL;

  x3 = _cffi_to_c_SIGNED(arg3, int);
  if (x3 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = X509V3_EXT_print(x0, x1, x2, x3); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_X509V3_set_ctx(PyObject *self, PyObject *args)
{
  X509V3_CTX * x0;
  X509 * x1;
  X509 * x2;
  X509_REQ * x3;
  X509_CRL * x4;
  int x5;
  Py_ssize_t datasize;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;
  PyObject *arg4;
  PyObject *arg5;

  if (!PyArg_ParseTuple(args, "OOOOOO:X509V3_set_ctx", &arg0, &arg1, &arg2, &arg3, &arg4, &arg5))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(114), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(114), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(89), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(89), arg1) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(89), arg2, (char **)&x2);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x2 = alloca(datasize);
    memset((void *)x2, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x2, _cffi_type(89), arg2) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(93), arg3, (char **)&x3);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x3 = alloca(datasize);
    memset((void *)x3, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x3, _cffi_type(93), arg3) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(91), arg4, (char **)&x4);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x4 = alloca(datasize);
    memset((void *)x4, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x4, _cffi_type(91), arg4) < 0)
      return NULL;
  }

  x5 = _cffi_to_c_SIGNED(arg5, int);
  if (x5 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { X509V3_set_ctx(x0, x1, x2, x3, x4, x5); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_X509V3_set_ctx_nodb(PyObject *self, PyObject *arg0)
{
  X509V3_CTX * x0;
  Py_ssize_t datasize;
  void * result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(114), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(114), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = X509V3_set_ctx_nodb(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(25));
}

static PyObject *
_cffi_f_X509_CRL_add0_revoked(PyObject *self, PyObject *args)
{
  X509_CRL * x0;
  X509_REVOKED * x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:X509_CRL_add0_revoked", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(91), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(91), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(115), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(115), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = X509_CRL_add0_revoked(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_X509_CRL_free(PyObject *self, PyObject *arg0)
{
  X509_CRL * x0;
  Py_ssize_t datasize;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(91), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(91), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { X509_CRL_free(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_X509_CRL_new(PyObject *self, PyObject *no_arg)
{
  X509_CRL * result;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = X509_CRL_new(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(91));
}

static PyObject *
_cffi_f_X509_CRL_print(PyObject *self, PyObject *args)
{
  BIO * x0;
  X509_CRL * x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:X509_CRL_print", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(22), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(22), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(91), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(91), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = X509_CRL_print(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_X509_CRL_set_issuer_name(PyObject *self, PyObject *args)
{
  X509_CRL * x0;
  X509_NAME * x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:X509_CRL_set_issuer_name", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(91), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(91), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(116), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(116), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = X509_CRL_set_issuer_name(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_X509_CRL_set_lastUpdate(PyObject *self, PyObject *args)
{
  X509_CRL * x0;
  ASN1_TIME * x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:X509_CRL_set_lastUpdate", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(91), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(91), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(16), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(16), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = X509_CRL_set_lastUpdate(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_X509_CRL_set_nextUpdate(PyObject *self, PyObject *args)
{
  X509_CRL * x0;
  ASN1_TIME * x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:X509_CRL_set_nextUpdate", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(91), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(91), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(16), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(16), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = X509_CRL_set_nextUpdate(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_X509_CRL_sign(PyObject *self, PyObject *args)
{
  X509_CRL * x0;
  EVP_PKEY * x1;
  EVP_MD const * x2;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;

  if (!PyArg_ParseTuple(args, "OOO:X509_CRL_sign", &arg0, &arg1, &arg2))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(91), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(91), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(62), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(62), arg1) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(42), arg2, (char **)&x2);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x2 = alloca(datasize);
    memset((void *)x2, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x2, _cffi_type(42), arg2) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = X509_CRL_sign(x0, x1, x2); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_X509_EXTENSION_dup(PyObject *self, PyObject *arg0)
{
  X509_EXTENSION * x0;
  Py_ssize_t datasize;
  X509_EXTENSION * result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(111), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(111), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = X509_EXTENSION_dup(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(111));
}

static PyObject *
_cffi_f_X509_EXTENSION_free(PyObject *self, PyObject *arg0)
{
  X509_EXTENSION * x0;
  Py_ssize_t datasize;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(111), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(111), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { X509_EXTENSION_free(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_X509_EXTENSION_get_critical(PyObject *self, PyObject *arg0)
{
  X509_EXTENSION * x0;
  Py_ssize_t datasize;
  int result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(111), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(111), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = X509_EXTENSION_get_critical(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_X509_EXTENSION_get_data(PyObject *self, PyObject *arg0)
{
  X509_EXTENSION * x0;
  Py_ssize_t datasize;
  ASN1_OCTET_STRING * result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(111), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(111), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = X509_EXTENSION_get_data(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(12));
}

static PyObject *
_cffi_f_X509_EXTENSION_get_object(PyObject *self, PyObject *arg0)
{
  X509_EXTENSION * x0;
  Py_ssize_t datasize;
  ASN1_OBJECT * result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(111), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(111), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = X509_EXTENSION_get_object(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(11));
}

static PyObject *
_cffi_f_X509_NAME_ENTRY_free(PyObject *self, PyObject *arg0)
{
  X509_NAME_ENTRY * x0;
  Py_ssize_t datasize;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(117), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(117), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { X509_NAME_ENTRY_free(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_X509_NAME_ENTRY_get_data(PyObject *self, PyObject *arg0)
{
  X509_NAME_ENTRY * x0;
  Py_ssize_t datasize;
  ASN1_STRING * result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(117), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(117), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = X509_NAME_ENTRY_get_data(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(13));
}

static PyObject *
_cffi_f_X509_NAME_ENTRY_get_object(PyObject *self, PyObject *arg0)
{
  X509_NAME_ENTRY * x0;
  Py_ssize_t datasize;
  ASN1_OBJECT * result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(117), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(117), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = X509_NAME_ENTRY_get_object(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(11));
}

static PyObject *
_cffi_f_X509_NAME_add_entry_by_NID(PyObject *self, PyObject *args)
{
  X509_NAME * x0;
  int x1;
  int x2;
  unsigned char * x3;
  int x4;
  int x5;
  int x6;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;
  PyObject *arg4;
  PyObject *arg5;
  PyObject *arg6;

  if (!PyArg_ParseTuple(args, "OOOOOOO:X509_NAME_add_entry_by_NID", &arg0, &arg1, &arg2, &arg3, &arg4, &arg5, &arg6))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(116), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(116), arg0) < 0)
      return NULL;
  }

  x1 = _cffi_to_c_SIGNED(arg1, int);
  if (x1 == (int)-1 && PyErr_Occurred())
    return NULL;

  x2 = _cffi_to_c_SIGNED(arg2, int);
  if (x2 == (int)-1 && PyErr_Occurred())
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(3), arg3, (char **)&x3);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x3 = alloca(datasize);
    memset((void *)x3, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x3, _cffi_type(3), arg3) < 0)
      return NULL;
  }

  x4 = _cffi_to_c_SIGNED(arg4, int);
  if (x4 == (int)-1 && PyErr_Occurred())
    return NULL;

  x5 = _cffi_to_c_SIGNED(arg5, int);
  if (x5 == (int)-1 && PyErr_Occurred())
    return NULL;

  x6 = _cffi_to_c_SIGNED(arg6, int);
  if (x6 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = X509_NAME_add_entry_by_NID(x0, x1, x2, x3, x4, x5, x6); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_X509_NAME_cmp(PyObject *self, PyObject *args)
{
  X509_NAME const * x0;
  X509_NAME const * x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:X509_NAME_cmp", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(118), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(118), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(118), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(118), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = X509_NAME_cmp(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_X509_NAME_delete_entry(PyObject *self, PyObject *args)
{
  X509_NAME * x0;
  int x1;
  Py_ssize_t datasize;
  X509_NAME_ENTRY * result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:X509_NAME_delete_entry", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(116), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(116), arg0) < 0)
      return NULL;
  }

  x1 = _cffi_to_c_SIGNED(arg1, int);
  if (x1 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = X509_NAME_delete_entry(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(117));
}

static PyObject *
_cffi_f_X509_NAME_dup(PyObject *self, PyObject *arg0)
{
  X509_NAME * x0;
  Py_ssize_t datasize;
  X509_NAME * result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(116), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(116), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = X509_NAME_dup(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(116));
}

static PyObject *
_cffi_f_X509_NAME_entry_count(PyObject *self, PyObject *arg0)
{
  X509_NAME * x0;
  Py_ssize_t datasize;
  int result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(116), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(116), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = X509_NAME_entry_count(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_X509_NAME_free(PyObject *self, PyObject *arg0)
{
  X509_NAME * x0;
  Py_ssize_t datasize;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(116), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(116), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { X509_NAME_free(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_X509_NAME_get_entry(PyObject *self, PyObject *args)
{
  X509_NAME * x0;
  int x1;
  Py_ssize_t datasize;
  X509_NAME_ENTRY * result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:X509_NAME_get_entry", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(116), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(116), arg0) < 0)
      return NULL;
  }

  x1 = _cffi_to_c_SIGNED(arg1, int);
  if (x1 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = X509_NAME_get_entry(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(117));
}

static PyObject *
_cffi_f_X509_NAME_get_index_by_NID(PyObject *self, PyObject *args)
{
  X509_NAME * x0;
  int x1;
  int x2;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;

  if (!PyArg_ParseTuple(args, "OOO:X509_NAME_get_index_by_NID", &arg0, &arg1, &arg2))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(116), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(116), arg0) < 0)
      return NULL;
  }

  x1 = _cffi_to_c_SIGNED(arg1, int);
  if (x1 == (int)-1 && PyErr_Occurred())
    return NULL;

  x2 = _cffi_to_c_SIGNED(arg2, int);
  if (x2 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = X509_NAME_get_index_by_NID(x0, x1, x2); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_X509_NAME_hash(PyObject *self, PyObject *arg0)
{
  X509_NAME * x0;
  Py_ssize_t datasize;
  unsigned long result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(116), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(116), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = X509_NAME_hash(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_UNSIGNED(result, unsigned long);
}

static PyObject *
_cffi_f_X509_NAME_oneline(PyObject *self, PyObject *args)
{
  X509_NAME * x0;
  char * x1;
  int x2;
  Py_ssize_t datasize;
  char * result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;

  if (!PyArg_ParseTuple(args, "OOO:X509_NAME_oneline", &arg0, &arg1, &arg2))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(116), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(116), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(23), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(23), arg1) < 0)
      return NULL;
  }

  x2 = _cffi_to_c_SIGNED(arg2, int);
  if (x2 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = X509_NAME_oneline(x0, x1, x2); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(23));
}

static PyObject *
_cffi_f_X509_REQ_add_extensions(PyObject *self, PyObject *args)
{
  X509_REQ * x0;
  X509_EXTENSIONS * x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:X509_REQ_add_extensions", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(93), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(93), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(119), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(119), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = X509_REQ_add_extensions(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_X509_REQ_free(PyObject *self, PyObject *arg0)
{
  X509_REQ * x0;
  Py_ssize_t datasize;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(93), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(93), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { X509_REQ_free(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_X509_REQ_get_extensions(PyObject *self, PyObject *arg0)
{
  X509_REQ * x0;
  Py_ssize_t datasize;
  X509_EXTENSIONS * result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(93), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(93), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = X509_REQ_get_extensions(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(119));
}

static PyObject *
_cffi_f_X509_REQ_get_pubkey(PyObject *self, PyObject *arg0)
{
  X509_REQ * x0;
  Py_ssize_t datasize;
  EVP_PKEY * result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(93), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(93), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = X509_REQ_get_pubkey(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(62));
}

static PyObject *
_cffi_f_X509_REQ_get_subject_name(PyObject *self, PyObject *arg0)
{
  X509_REQ * x0;
  Py_ssize_t datasize;
  X509_NAME * result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(93), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(93), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = X509_REQ_get_subject_name(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(116));
}

static PyObject *
_cffi_f_X509_REQ_get_version(PyObject *self, PyObject *arg0)
{
  X509_REQ * x0;
  Py_ssize_t datasize;
  long result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(93), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(93), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = X509_REQ_get_version(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, long);
}

static PyObject *
_cffi_f_X509_REQ_new(PyObject *self, PyObject *no_arg)
{
  X509_REQ * result;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = X509_REQ_new(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(93));
}

static PyObject *
_cffi_f_X509_REQ_print_ex(PyObject *self, PyObject *args)
{
  BIO * x0;
  X509_REQ * x1;
  unsigned long x2;
  unsigned long x3;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;

  if (!PyArg_ParseTuple(args, "OOOO:X509_REQ_print_ex", &arg0, &arg1, &arg2, &arg3))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(22), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(22), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(93), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(93), arg1) < 0)
      return NULL;
  }

  x2 = _cffi_to_c_UNSIGNED(arg2, unsigned long);
  if (x2 == (unsigned long)-1 && PyErr_Occurred())
    return NULL;

  x3 = _cffi_to_c_UNSIGNED(arg3, unsigned long);
  if (x3 == (unsigned long)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = X509_REQ_print_ex(x0, x1, x2, x3); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_X509_REQ_set_pubkey(PyObject *self, PyObject *args)
{
  X509_REQ * x0;
  EVP_PKEY * x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:X509_REQ_set_pubkey", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(93), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(93), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(62), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(62), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = X509_REQ_set_pubkey(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_X509_REQ_set_version(PyObject *self, PyObject *args)
{
  X509_REQ * x0;
  long x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:X509_REQ_set_version", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(93), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(93), arg0) < 0)
      return NULL;
  }

  x1 = _cffi_to_c_SIGNED(arg1, long);
  if (x1 == (long)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = X509_REQ_set_version(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_X509_REQ_sign(PyObject *self, PyObject *args)
{
  X509_REQ * x0;
  EVP_PKEY * x1;
  EVP_MD const * x2;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;

  if (!PyArg_ParseTuple(args, "OOO:X509_REQ_sign", &arg0, &arg1, &arg2))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(93), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(93), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(62), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(62), arg1) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(42), arg2, (char **)&x2);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x2 = alloca(datasize);
    memset((void *)x2, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x2, _cffi_type(42), arg2) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = X509_REQ_sign(x0, x1, x2); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_X509_REQ_verify(PyObject *self, PyObject *args)
{
  X509_REQ * x0;
  EVP_PKEY * x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:X509_REQ_verify", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(93), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(93), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(62), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(62), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = X509_REQ_verify(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_X509_REVOKED_add1_ext_i2d(PyObject *self, PyObject *args)
{
  X509_REVOKED * x0;
  int x1;
  void * x2;
  int x3;
  unsigned long x4;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;
  PyObject *arg4;

  if (!PyArg_ParseTuple(args, "OOOOO:X509_REVOKED_add1_ext_i2d", &arg0, &arg1, &arg2, &arg3, &arg4))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(115), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(115), arg0) < 0)
      return NULL;
  }

  x1 = _cffi_to_c_SIGNED(arg1, int);
  if (x1 == (int)-1 && PyErr_Occurred())
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(25), arg2, (char **)&x2);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x2 = alloca(datasize);
    memset((void *)x2, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x2, _cffi_type(25), arg2) < 0)
      return NULL;
  }

  x3 = _cffi_to_c_SIGNED(arg3, int);
  if (x3 == (int)-1 && PyErr_Occurred())
    return NULL;

  x4 = _cffi_to_c_UNSIGNED(arg4, unsigned long);
  if (x4 == (unsigned long)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = X509_REVOKED_add1_ext_i2d(x0, x1, x2, x3, x4); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_X509_REVOKED_free(PyObject *self, PyObject *arg0)
{
  X509_REVOKED * x0;
  Py_ssize_t datasize;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(115), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(115), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { X509_REVOKED_free(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_X509_REVOKED_new(PyObject *self, PyObject *no_arg)
{
  X509_REVOKED * result;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = X509_REVOKED_new(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(115));
}

static PyObject *
_cffi_f_X509_REVOKED_set_serialNumber(PyObject *self, PyObject *args)
{
  X509_REVOKED * x0;
  ASN1_INTEGER * x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:X509_REVOKED_set_serialNumber", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(115), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(115), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(7), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(7), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = X509_REVOKED_set_serialNumber(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_X509_STORE_CTX_get_current_cert(PyObject *self, PyObject *arg0)
{
  X509_STORE_CTX * x0;
  Py_ssize_t datasize;
  X509 * result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(120), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(120), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = X509_STORE_CTX_get_current_cert(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(89));
}

static PyObject *
_cffi_f_X509_STORE_CTX_get_error(PyObject *self, PyObject *arg0)
{
  X509_STORE_CTX * x0;
  Py_ssize_t datasize;
  int result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(120), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(120), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = X509_STORE_CTX_get_error(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_X509_STORE_CTX_get_error_depth(PyObject *self, PyObject *arg0)
{
  X509_STORE_CTX * x0;
  Py_ssize_t datasize;
  int result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(120), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(120), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = X509_STORE_CTX_get_error_depth(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_X509_STORE_CTX_set_error(PyObject *self, PyObject *args)
{
  X509_STORE_CTX * x0;
  int x1;
  Py_ssize_t datasize;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:X509_STORE_CTX_set_error", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(120), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(120), arg0) < 0)
      return NULL;
  }

  x1 = _cffi_to_c_SIGNED(arg1, int);
  if (x1 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { X509_STORE_CTX_set_error(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_X509_STORE_add_cert(PyObject *self, PyObject *args)
{
  X509_STORE * x0;
  X509 * x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:X509_STORE_add_cert", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(102), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(102), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(89), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(89), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = X509_STORE_add_cert(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_X509_STORE_free(PyObject *self, PyObject *arg0)
{
  X509_STORE * x0;
  Py_ssize_t datasize;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(102), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(102), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { X509_STORE_free(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_X509_STORE_new(PyObject *self, PyObject *no_arg)
{
  X509_STORE * result;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = X509_STORE_new(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(102));
}

static PyObject *
_cffi_f_X509_add_ext(PyObject *self, PyObject *args)
{
  X509 * x0;
  X509_EXTENSION * x1;
  int x2;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;

  if (!PyArg_ParseTuple(args, "OOO:X509_add_ext", &arg0, &arg1, &arg2))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(89), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(89), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(111), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(111), arg1) < 0)
      return NULL;
  }

  x2 = _cffi_to_c_SIGNED(arg2, int);
  if (x2 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = X509_add_ext(x0, x1, x2); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_X509_alias_get0(PyObject *self, PyObject *args)
{
  X509 * x0;
  int * x1;
  Py_ssize_t datasize;
  unsigned char * result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:X509_alias_get0", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(89), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(89), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(37), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(37), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = X509_alias_get0(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(3));
}

static PyObject *
_cffi_f_X509_digest(PyObject *self, PyObject *args)
{
  X509 const * x0;
  EVP_MD const * x1;
  unsigned char * x2;
  unsigned int * x3;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;

  if (!PyArg_ParseTuple(args, "OOOO:X509_digest", &arg0, &arg1, &arg2, &arg3))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(121), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(121), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(42), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(42), arg1) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(3), arg2, (char **)&x2);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x2 = alloca(datasize);
    memset((void *)x2, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x2, _cffi_type(3), arg2) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(41), arg3, (char **)&x3);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x3 = alloca(datasize);
    memset((void *)x3, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x3, _cffi_type(41), arg3) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = X509_digest(x0, x1, x2, x3); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_X509_dup(PyObject *self, PyObject *arg0)
{
  X509 * x0;
  Py_ssize_t datasize;
  X509 * result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(89), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(89), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = X509_dup(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(89));
}

static PyObject *
_cffi_f_X509_free(PyObject *self, PyObject *arg0)
{
  X509 * x0;
  Py_ssize_t datasize;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(89), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(89), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { X509_free(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_X509_get_default_cert_area(PyObject *self, PyObject *no_arg)
{
  char const * result;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = X509_get_default_cert_area(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(0));
}

static PyObject *
_cffi_f_X509_get_default_cert_dir(PyObject *self, PyObject *no_arg)
{
  char const * result;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = X509_get_default_cert_dir(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(0));
}

static PyObject *
_cffi_f_X509_get_default_cert_dir_env(PyObject *self, PyObject *no_arg)
{
  char const * result;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = X509_get_default_cert_dir_env(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(0));
}

static PyObject *
_cffi_f_X509_get_default_cert_file(PyObject *self, PyObject *no_arg)
{
  char const * result;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = X509_get_default_cert_file(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(0));
}

static PyObject *
_cffi_f_X509_get_default_cert_file_env(PyObject *self, PyObject *no_arg)
{
  char const * result;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = X509_get_default_cert_file_env(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(0));
}

static PyObject *
_cffi_f_X509_get_default_private_dir(PyObject *self, PyObject *no_arg)
{
  char const * result;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = X509_get_default_private_dir(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(0));
}

static PyObject *
_cffi_f_X509_get_ext(PyObject *self, PyObject *args)
{
  X509 * x0;
  int x1;
  Py_ssize_t datasize;
  X509_EXTENSION * result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:X509_get_ext", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(89), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(89), arg0) < 0)
      return NULL;
  }

  x1 = _cffi_to_c_SIGNED(arg1, int);
  if (x1 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = X509_get_ext(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(111));
}

static PyObject *
_cffi_f_X509_get_ext_count(PyObject *self, PyObject *arg0)
{
  X509 * x0;
  Py_ssize_t datasize;
  int result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(89), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(89), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = X509_get_ext_count(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_X509_get_issuer_name(PyObject *self, PyObject *arg0)
{
  X509 * x0;
  Py_ssize_t datasize;
  X509_NAME * result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(89), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(89), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = X509_get_issuer_name(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(116));
}

static PyObject *
_cffi_f_X509_get_notAfter(PyObject *self, PyObject *arg0)
{
  X509 * x0;
  Py_ssize_t datasize;
  ASN1_TIME * result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(89), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(89), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = X509_get_notAfter(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(16));
}

static PyObject *
_cffi_f_X509_get_notBefore(PyObject *self, PyObject *arg0)
{
  X509 * x0;
  Py_ssize_t datasize;
  ASN1_TIME * result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(89), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(89), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = X509_get_notBefore(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(16));
}

static PyObject *
_cffi_f_X509_get_pubkey(PyObject *self, PyObject *arg0)
{
  X509 * x0;
  Py_ssize_t datasize;
  EVP_PKEY * result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(89), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(89), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = X509_get_pubkey(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(62));
}

static PyObject *
_cffi_f_X509_get_serialNumber(PyObject *self, PyObject *arg0)
{
  X509 * x0;
  Py_ssize_t datasize;
  ASN1_INTEGER * result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(89), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(89), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = X509_get_serialNumber(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(7));
}

static PyObject *
_cffi_f_X509_get_subject_name(PyObject *self, PyObject *arg0)
{
  X509 * x0;
  Py_ssize_t datasize;
  X509_NAME * result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(89), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(89), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = X509_get_subject_name(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(116));
}

static PyObject *
_cffi_f_X509_get_version(PyObject *self, PyObject *arg0)
{
  X509 * x0;
  Py_ssize_t datasize;
  long result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(89), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(89), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = X509_get_version(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, long);
}

static PyObject *
_cffi_f_X509_gmtime_adj(PyObject *self, PyObject *args)
{
  ASN1_TIME * x0;
  long x1;
  Py_ssize_t datasize;
  ASN1_TIME * result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:X509_gmtime_adj", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(16), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(16), arg0) < 0)
      return NULL;
  }

  x1 = _cffi_to_c_SIGNED(arg1, long);
  if (x1 == (long)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = X509_gmtime_adj(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(16));
}

static PyObject *
_cffi_f_X509_new(PyObject *self, PyObject *no_arg)
{
  X509 * result;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = X509_new(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(89));
}

static PyObject *
_cffi_f_X509_print_ex(PyObject *self, PyObject *args)
{
  BIO * x0;
  X509 * x1;
  unsigned long x2;
  unsigned long x3;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;

  if (!PyArg_ParseTuple(args, "OOOO:X509_print_ex", &arg0, &arg1, &arg2, &arg3))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(22), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(22), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(89), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(89), arg1) < 0)
      return NULL;
  }

  x2 = _cffi_to_c_UNSIGNED(arg2, unsigned long);
  if (x2 == (unsigned long)-1 && PyErr_Occurred())
    return NULL;

  x3 = _cffi_to_c_UNSIGNED(arg3, unsigned long);
  if (x3 == (unsigned long)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = X509_print_ex(x0, x1, x2, x3); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_X509_set_issuer_name(PyObject *self, PyObject *args)
{
  X509 * x0;
  X509_NAME * x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:X509_set_issuer_name", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(89), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(89), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(116), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(116), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = X509_set_issuer_name(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_X509_set_pubkey(PyObject *self, PyObject *args)
{
  X509 * x0;
  EVP_PKEY * x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:X509_set_pubkey", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(89), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(89), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(62), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(62), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = X509_set_pubkey(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_X509_set_serialNumber(PyObject *self, PyObject *args)
{
  X509 * x0;
  ASN1_INTEGER * x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:X509_set_serialNumber", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(89), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(89), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(7), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(7), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = X509_set_serialNumber(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_X509_set_subject_name(PyObject *self, PyObject *args)
{
  X509 * x0;
  X509_NAME * x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:X509_set_subject_name", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(89), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(89), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(116), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(116), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = X509_set_subject_name(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_X509_set_version(PyObject *self, PyObject *args)
{
  X509 * x0;
  long x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:X509_set_version", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(89), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(89), arg0) < 0)
      return NULL;
  }

  x1 = _cffi_to_c_SIGNED(arg1, long);
  if (x1 == (long)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = X509_set_version(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_X509_sign(PyObject *self, PyObject *args)
{
  X509 * x0;
  EVP_PKEY * x1;
  EVP_MD const * x2;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;

  if (!PyArg_ParseTuple(args, "OOO:X509_sign", &arg0, &arg1, &arg2))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(89), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(89), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(62), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(62), arg1) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(42), arg2, (char **)&x2);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x2 = alloca(datasize);
    memset((void *)x2, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x2, _cffi_type(42), arg2) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = X509_sign(x0, x1, x2); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_X509_subject_name_hash(PyObject *self, PyObject *arg0)
{
  X509 * x0;
  Py_ssize_t datasize;
  unsigned long result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(89), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(89), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = X509_subject_name_hash(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_UNSIGNED(result, unsigned long);
}

static PyObject *
_cffi_f_X509_verify_cert(PyObject *self, PyObject *arg0)
{
  X509_STORE_CTX * x0;
  Py_ssize_t datasize;
  int result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(120), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(120), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = X509_verify_cert(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_X509_verify_cert_error_string(PyObject *self, PyObject *arg0)
{
  long x0;
  char const * result;

  x0 = _cffi_to_c_SIGNED(arg0, long);
  if (x0 == (long)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = X509_verify_cert_error_string(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(0));
}

static PyObject *
_cffi_f_d2i_ASN1_OBJECT(PyObject *self, PyObject *args)
{
  ASN1_OBJECT * * x0;
  unsigned char const * * x1;
  long x2;
  Py_ssize_t datasize;
  ASN1_OBJECT * result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;

  if (!PyArg_ParseTuple(args, "OOO:d2i_ASN1_OBJECT", &arg0, &arg1, &arg2))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(122), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(122), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(20), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(20), arg1) < 0)
      return NULL;
  }

  x2 = _cffi_to_c_SIGNED(arg2, long);
  if (x2 == (long)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = d2i_ASN1_OBJECT(x0, x1, x2); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(11));
}

static PyObject *
_cffi_f_d2i_PKCS12_bio(PyObject *self, PyObject *args)
{
  BIO * x0;
  PKCS12 * * x1;
  Py_ssize_t datasize;
  PKCS12 * result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:d2i_PKCS12_bio", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(22), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(22), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(123), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(123), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = d2i_PKCS12_bio(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(96));
}

static PyObject *
_cffi_f_d2i_PKCS8PrivateKey_bio(PyObject *self, PyObject *args)
{
  BIO * x0;
  EVP_PKEY * * x1;
  int(* x2)(char *, int, int, void *);
  void * x3;
  Py_ssize_t datasize;
  EVP_PKEY * result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;

  if (!PyArg_ParseTuple(args, "OOOO:d2i_PKCS8PrivateKey_bio", &arg0, &arg1, &arg2, &arg3))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(22), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(22), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(86), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(86), arg1) < 0)
      return NULL;
  }

  x2 = (int(*)(char *, int, int, void *))_cffi_to_c_pointer(arg2, _cffi_type(82));
  if (x2 == (int(*)(char *, int, int, void *))NULL && PyErr_Occurred())
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(25), arg3, (char **)&x3);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x3 = alloca(datasize);
    memset((void *)x3, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x3, _cffi_type(25), arg3) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = d2i_PKCS8PrivateKey_bio(x0, x1, x2, x3); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(62));
}

static PyObject *
_cffi_f_d2i_PrivateKey_bio(PyObject *self, PyObject *args)
{
  BIO * x0;
  EVP_PKEY * * x1;
  Py_ssize_t datasize;
  EVP_PKEY * result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:d2i_PrivateKey_bio", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(22), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(22), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(86), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(86), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = d2i_PrivateKey_bio(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(62));
}

static PyObject *
_cffi_f_d2i_X509_CRL_bio(PyObject *self, PyObject *args)
{
  BIO * x0;
  X509_CRL * * x1;
  Py_ssize_t datasize;
  X509_CRL * result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:d2i_X509_CRL_bio", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(22), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(22), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(90), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(90), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = d2i_X509_CRL_bio(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(91));
}

static PyObject *
_cffi_f_d2i_X509_REQ_bio(PyObject *self, PyObject *args)
{
  BIO * x0;
  X509_REQ * * x1;
  Py_ssize_t datasize;
  X509_REQ * result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:d2i_X509_REQ_bio", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(22), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(22), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(92), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(92), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = d2i_X509_REQ_bio(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(93));
}

static PyObject *
_cffi_f_d2i_X509_bio(PyObject *self, PyObject *args)
{
  BIO * x0;
  X509 * * x1;
  Py_ssize_t datasize;
  X509 * result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:d2i_X509_bio", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(22), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(22), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(88), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(88), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = d2i_X509_bio(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(89));
}

static PyObject *
_cffi_f_i2a_ASN1_INTEGER(PyObject *self, PyObject *args)
{
  BIO * x0;
  ASN1_INTEGER * x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:i2a_ASN1_INTEGER", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(22), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(22), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(7), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(7), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = i2a_ASN1_INTEGER(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_i2d_ASN1_OBJECT(PyObject *self, PyObject *args)
{
  ASN1_OBJECT * x0;
  unsigned char * * x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:i2d_ASN1_OBJECT", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(11), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(11), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(15), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(15), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = i2d_ASN1_OBJECT(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_i2d_PKCS12_bio(PyObject *self, PyObject *args)
{
  BIO * x0;
  PKCS12 * x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:i2d_PKCS12_bio", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(22), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(22), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(96), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(96), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = i2d_PKCS12_bio(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_i2d_PKCS8PrivateKey_bio(PyObject *self, PyObject *args)
{
  BIO * x0;
  EVP_PKEY * x1;
  EVP_CIPHER const * x2;
  char * x3;
  int x4;
  int(* x5)(char *, int, int, void *);
  void * x6;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;
  PyObject *arg4;
  PyObject *arg5;
  PyObject *arg6;

  if (!PyArg_ParseTuple(args, "OOOOOOO:i2d_PKCS8PrivateKey_bio", &arg0, &arg1, &arg2, &arg3, &arg4, &arg5, &arg6))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(22), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(22), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(62), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(62), arg1) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(59), arg2, (char **)&x2);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x2 = alloca(datasize);
    memset((void *)x2, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x2, _cffi_type(59), arg2) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(23), arg3, (char **)&x3);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x3 = alloca(datasize);
    memset((void *)x3, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x3, _cffi_type(23), arg3) < 0)
      return NULL;
  }

  x4 = _cffi_to_c_SIGNED(arg4, int);
  if (x4 == (int)-1 && PyErr_Occurred())
    return NULL;

  x5 = (int(*)(char *, int, int, void *))_cffi_to_c_pointer(arg5, _cffi_type(82));
  if (x5 == (int(*)(char *, int, int, void *))NULL && PyErr_Occurred())
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(25), arg6, (char **)&x6);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x6 = alloca(datasize);
    memset((void *)x6, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x6, _cffi_type(25), arg6) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = i2d_PKCS8PrivateKey_bio(x0, x1, x2, x3, x4, x5, x6); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_i2d_PrivateKey_bio(PyObject *self, PyObject *args)
{
  BIO * x0;
  EVP_PKEY * x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:i2d_PrivateKey_bio", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(22), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(22), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(62), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(62), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = i2d_PrivateKey_bio(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_i2d_X509_CRL_bio(PyObject *self, PyObject *args)
{
  BIO * x0;
  X509_CRL * x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:i2d_X509_CRL_bio", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(22), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(22), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(91), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(91), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = i2d_X509_CRL_bio(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_i2d_X509_NAME(PyObject *self, PyObject *args)
{
  X509_NAME * x0;
  unsigned char * * x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:i2d_X509_NAME", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(116), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(116), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(15), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(15), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = i2d_X509_NAME(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_i2d_X509_REQ_bio(PyObject *self, PyObject *args)
{
  BIO * x0;
  X509_REQ * x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:i2d_X509_REQ_bio", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(22), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(22), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(93), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(93), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = i2d_X509_REQ_bio(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_i2d_X509_bio(PyObject *self, PyObject *args)
{
  BIO * x0;
  X509 * x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:i2d_X509_bio", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(22), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(22), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(89), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(89), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = i2d_X509_bio(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_sk_GENERAL_NAME_num(PyObject *self, PyObject *arg0)
{
  GENERAL_NAMES * x0;
  Py_ssize_t datasize;
  int result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(124), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(124), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = sk_GENERAL_NAME_num(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_sk_GENERAL_NAME_push(PyObject *self, PyObject *args)
{
  GENERAL_NAMES * x0;
  GENERAL_NAME * x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:sk_GENERAL_NAME_push", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(124), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(124), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(78), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(78), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = sk_GENERAL_NAME_push(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_sk_GENERAL_NAME_value(PyObject *self, PyObject *args)
{
  GENERAL_NAMES * x0;
  int x1;
  Py_ssize_t datasize;
  GENERAL_NAME * result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:sk_GENERAL_NAME_value", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(124), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(124), arg0) < 0)
      return NULL;
  }

  x1 = _cffi_to_c_SIGNED(arg1, int);
  if (x1 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = sk_GENERAL_NAME_value(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(78));
}

static PyObject *
_cffi_f_sk_X509_EXTENSION_delete(PyObject *self, PyObject *args)
{
  X509_EXTENSIONS * x0;
  int x1;
  Py_ssize_t datasize;
  X509_EXTENSION * result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:sk_X509_EXTENSION_delete", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(119), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(119), arg0) < 0)
      return NULL;
  }

  x1 = _cffi_to_c_SIGNED(arg1, int);
  if (x1 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = sk_X509_EXTENSION_delete(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(111));
}

static PyObject *
_cffi_f_sk_X509_EXTENSION_free(PyObject *self, PyObject *arg0)
{
  X509_EXTENSIONS * x0;
  Py_ssize_t datasize;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(119), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(119), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { sk_X509_EXTENSION_free(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_sk_X509_EXTENSION_new_null(PyObject *self, PyObject *no_arg)
{
  X509_EXTENSIONS * result;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = sk_X509_EXTENSION_new_null(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(119));
}

static PyObject *
_cffi_f_sk_X509_EXTENSION_num(PyObject *self, PyObject *arg0)
{
  X509_EXTENSIONS * x0;
  Py_ssize_t datasize;
  int result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(119), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(119), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = sk_X509_EXTENSION_num(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_sk_X509_EXTENSION_push(PyObject *self, PyObject *args)
{
  X509_EXTENSIONS * x0;
  X509_EXTENSION * x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:sk_X509_EXTENSION_push", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(119), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(119), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(111), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(111), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = sk_X509_EXTENSION_push(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_sk_X509_EXTENSION_value(PyObject *self, PyObject *args)
{
  X509_EXTENSIONS * x0;
  int x1;
  Py_ssize_t datasize;
  X509_EXTENSION * result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:sk_X509_EXTENSION_value", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(119), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(119), arg0) < 0)
      return NULL;
  }

  x1 = _cffi_to_c_SIGNED(arg1, int);
  if (x1 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = sk_X509_EXTENSION_value(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(111));
}

static PyObject *
_cffi_f_sk_X509_NAME_free(PyObject *self, PyObject *arg0)
{
  Cryptography_STACK_OF_X509_NAME * x0;
  Py_ssize_t datasize;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(106), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(106), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { sk_X509_NAME_free(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_sk_X509_NAME_new_null(PyObject *self, PyObject *no_arg)
{
  Cryptography_STACK_OF_X509_NAME * result;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = sk_X509_NAME_new_null(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(106));
}

static PyObject *
_cffi_f_sk_X509_NAME_num(PyObject *self, PyObject *arg0)
{
  Cryptography_STACK_OF_X509_NAME * x0;
  Py_ssize_t datasize;
  int result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(106), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(106), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = sk_X509_NAME_num(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_sk_X509_NAME_push(PyObject *self, PyObject *args)
{
  Cryptography_STACK_OF_X509_NAME * x0;
  X509_NAME * x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:sk_X509_NAME_push", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(106), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(106), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(116), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(116), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = sk_X509_NAME_push(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_sk_X509_NAME_value(PyObject *self, PyObject *args)
{
  Cryptography_STACK_OF_X509_NAME * x0;
  int x1;
  Py_ssize_t datasize;
  X509_NAME * result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:sk_X509_NAME_value", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(106), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(106), arg0) < 0)
      return NULL;
  }

  x1 = _cffi_to_c_SIGNED(arg1, int);
  if (x1 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = sk_X509_NAME_value(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(116));
}

static PyObject *
_cffi_f_sk_X509_REVOKED_num(PyObject *self, PyObject *arg0)
{
  Cryptography_STACK_OF_X509_REVOKED * x0;
  Py_ssize_t datasize;
  int result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(125), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(125), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = sk_X509_REVOKED_num(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_sk_X509_REVOKED_value(PyObject *self, PyObject *args)
{
  Cryptography_STACK_OF_X509_REVOKED * x0;
  int x1;
  Py_ssize_t datasize;
  X509_REVOKED * result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:sk_X509_REVOKED_value", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(125), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(125), arg0) < 0)
      return NULL;
  }

  x1 = _cffi_to_c_SIGNED(arg1, int);
  if (x1 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = sk_X509_REVOKED_value(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(115));
}

static PyObject *
_cffi_f_sk_X509_free(PyObject *self, PyObject *arg0)
{
  Cryptography_STACK_OF_X509 * x0;
  Py_ssize_t datasize;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(95), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(95), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { sk_X509_free(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_sk_X509_new_null(PyObject *self, PyObject *no_arg)
{
  Cryptography_STACK_OF_X509 * result;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = sk_X509_new_null(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(95));
}

static PyObject *
_cffi_f_sk_X509_num(PyObject *self, PyObject *arg0)
{
  Cryptography_STACK_OF_X509 * x0;
  Py_ssize_t datasize;
  int result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(95), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(95), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = sk_X509_num(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_sk_X509_push(PyObject *self, PyObject *args)
{
  Cryptography_STACK_OF_X509 * x0;
  X509 * x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:sk_X509_push", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(95), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(95), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(89), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(89), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = sk_X509_push(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_sk_X509_value(PyObject *self, PyObject *args)
{
  Cryptography_STACK_OF_X509 * x0;
  int x1;
  Py_ssize_t datasize;
  X509 * result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:sk_X509_value", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(95), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(95), arg0) < 0)
      return NULL;
  }

  x1 = _cffi_to_c_SIGNED(arg1, int);
  if (x1 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = sk_X509_value(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(89));
}

static void _cffi_check_struct_ERR_string_data_st(struct ERR_string_data_st *p)
{
  /* only to generate compile-time warnings or errors */
  (void)((p->error) << 1);
  { char const * *tmp = &p->string; (void)tmp; }
}
static PyObject *
_cffi_layout_struct_ERR_string_data_st(PyObject *self, PyObject *noarg)
{
  struct _cffi_aligncheck { char x; struct ERR_string_data_st y; };
  static Py_ssize_t nums[] = {
    sizeof(struct ERR_string_data_st),
    offsetof(struct _cffi_aligncheck, y),
    offsetof(struct ERR_string_data_st, error),
    sizeof(((struct ERR_string_data_st *)0)->error),
    offsetof(struct ERR_string_data_st, string),
    sizeof(((struct ERR_string_data_st *)0)->string),
    -1
  };
  return _cffi_get_struct_layout(nums);
  /* the next line is not executed, but compiled */
  _cffi_check_struct_ERR_string_data_st(0);
}

static void _cffi_check_struct_aes_key_st(struct aes_key_st *p)
{
  /* only to generate compile-time warnings or errors */
}
static PyObject *
_cffi_layout_struct_aes_key_st(PyObject *self, PyObject *noarg)
{
  struct _cffi_aligncheck { char x; struct aes_key_st y; };
  static Py_ssize_t nums[] = {
    sizeof(struct aes_key_st),
    offsetof(struct _cffi_aligncheck, y),
    -1
  };
  return _cffi_get_struct_layout(nums);
  /* the next line is not executed, but compiled */
  _cffi_check_struct_aes_key_st(0);
}

static void _cffi_check_struct_asn1_string_st(struct asn1_string_st *p)
{
  /* only to generate compile-time warnings or errors */
  (void)((p->length) << 1);
  (void)((p->type) << 1);
  { unsigned char * *tmp = &p->data; (void)tmp; }
  (void)((p->flags) << 1);
}
static PyObject *
_cffi_layout_struct_asn1_string_st(PyObject *self, PyObject *noarg)
{
  struct _cffi_aligncheck { char x; struct asn1_string_st y; };
  static Py_ssize_t nums[] = {
    sizeof(struct asn1_string_st),
    offsetof(struct _cffi_aligncheck, y),
    offsetof(struct asn1_string_st, length),
    sizeof(((struct asn1_string_st *)0)->length),
    offsetof(struct asn1_string_st, type),
    sizeof(((struct asn1_string_st *)0)->type),
    offsetof(struct asn1_string_st, data),
    sizeof(((struct asn1_string_st *)0)->data),
    offsetof(struct asn1_string_st, flags),
    sizeof(((struct asn1_string_st *)0)->flags),
    -1
  };
  return _cffi_get_struct_layout(nums);
  /* the next line is not executed, but compiled */
  _cffi_check_struct_asn1_string_st(0);
}

static void _cffi_check_struct_bio_method_st(struct bio_method_st *p)
{
  /* only to generate compile-time warnings or errors */
  (void)((p->type) << 1);
  { char const * *tmp = &p->name; (void)tmp; }
  { int(* *tmp)(BIO *, char const *, int) = &p->bwrite; (void)tmp; }
  { int(* *tmp)(BIO *, char *, int) = &p->bread; (void)tmp; }
  { int(* *tmp)(BIO *, char const *) = &p->bputs; (void)tmp; }
  { int(* *tmp)(BIO *, char *, int) = &p->bgets; (void)tmp; }
  { long(* *tmp)(BIO *, int, long, void *) = &p->ctrl; (void)tmp; }
  { int(* *tmp)(BIO *) = &p->create; (void)tmp; }
  { int(* *tmp)(BIO *) = &p->destroy; (void)tmp; }
  { long(* *tmp)(BIO *, int, void(*)(BIO *, int, char const *, int, long, long)) = &p->callback_ctrl; (void)tmp; }
}
static PyObject *
_cffi_layout_struct_bio_method_st(PyObject *self, PyObject *noarg)
{
  struct _cffi_aligncheck { char x; struct bio_method_st y; };
  static Py_ssize_t nums[] = {
    sizeof(struct bio_method_st),
    offsetof(struct _cffi_aligncheck, y),
    offsetof(struct bio_method_st, type),
    sizeof(((struct bio_method_st *)0)->type),
    offsetof(struct bio_method_st, name),
    sizeof(((struct bio_method_st *)0)->name),
    offsetof(struct bio_method_st, bwrite),
    sizeof(((struct bio_method_st *)0)->bwrite),
    offsetof(struct bio_method_st, bread),
    sizeof(((struct bio_method_st *)0)->bread),
    offsetof(struct bio_method_st, bputs),
    sizeof(((struct bio_method_st *)0)->bputs),
    offsetof(struct bio_method_st, bgets),
    sizeof(((struct bio_method_st *)0)->bgets),
    offsetof(struct bio_method_st, ctrl),
    sizeof(((struct bio_method_st *)0)->ctrl),
    offsetof(struct bio_method_st, create),
    sizeof(((struct bio_method_st *)0)->create),
    offsetof(struct bio_method_st, destroy),
    sizeof(((struct bio_method_st *)0)->destroy),
    offsetof(struct bio_method_st, callback_ctrl),
    sizeof(((struct bio_method_st *)0)->callback_ctrl),
    -1
  };
  return _cffi_get_struct_layout(nums);
  /* the next line is not executed, but compiled */
  _cffi_check_struct_bio_method_st(0);
}

static void _cffi_check_struct_bio_st(struct bio_st *p)
{
  /* only to generate compile-time warnings or errors */
  { BIO_METHOD * *tmp = &p->method; (void)tmp; }
  { long(* *tmp)(BIO *, int, char const *, int, long, long) = &p->callback; (void)tmp; }
  { char * *tmp = &p->cb_arg; (void)tmp; }
  (void)((p->init) << 1);
  (void)((p->shutdown) << 1);
  (void)((p->flags) << 1);
  (void)((p->retry_reason) << 1);
  (void)((p->num) << 1);
  { void * *tmp = &p->ptr; (void)tmp; }
  { BIO * *tmp = &p->next_bio; (void)tmp; }
  { BIO * *tmp = &p->prev_bio; (void)tmp; }
  (void)((p->references) << 1);
  (void)((p->num_read) << 1);
  (void)((p->num_write) << 1);
}
static PyObject *
_cffi_layout_struct_bio_st(PyObject *self, PyObject *noarg)
{
  struct _cffi_aligncheck { char x; struct bio_st y; };
  static Py_ssize_t nums[] = {
    sizeof(struct bio_st),
    offsetof(struct _cffi_aligncheck, y),
    offsetof(struct bio_st, method),
    sizeof(((struct bio_st *)0)->method),
    offsetof(struct bio_st, callback),
    sizeof(((struct bio_st *)0)->callback),
    offsetof(struct bio_st, cb_arg),
    sizeof(((struct bio_st *)0)->cb_arg),
    offsetof(struct bio_st, init),
    sizeof(((struct bio_st *)0)->init),
    offsetof(struct bio_st, shutdown),
    sizeof(((struct bio_st *)0)->shutdown),
    offsetof(struct bio_st, flags),
    sizeof(((struct bio_st *)0)->flags),
    offsetof(struct bio_st, retry_reason),
    sizeof(((struct bio_st *)0)->retry_reason),
    offsetof(struct bio_st, num),
    sizeof(((struct bio_st *)0)->num),
    offsetof(struct bio_st, ptr),
    sizeof(((struct bio_st *)0)->ptr),
    offsetof(struct bio_st, next_bio),
    sizeof(((struct bio_st *)0)->next_bio),
    offsetof(struct bio_st, prev_bio),
    sizeof(((struct bio_st *)0)->prev_bio),
    offsetof(struct bio_st, references),
    sizeof(((struct bio_st *)0)->references),
    offsetof(struct bio_st, num_read),
    sizeof(((struct bio_st *)0)->num_read),
    offsetof(struct bio_st, num_write),
    sizeof(((struct bio_st *)0)->num_write),
    -1
  };
  return _cffi_get_struct_layout(nums);
  /* the next line is not executed, but compiled */
  _cffi_check_struct_bio_st(0);
}

static void _cffi_check_struct_dh_st(struct dh_st *p)
{
  /* only to generate compile-time warnings or errors */
  { BIGNUM * *tmp = &p->p; (void)tmp; }
  { BIGNUM * *tmp = &p->g; (void)tmp; }
  { BIGNUM * *tmp = &p->priv_key; (void)tmp; }
  { BIGNUM * *tmp = &p->pub_key; (void)tmp; }
}
static PyObject *
_cffi_layout_struct_dh_st(PyObject *self, PyObject *noarg)
{
  struct _cffi_aligncheck { char x; struct dh_st y; };
  static Py_ssize_t nums[] = {
    sizeof(struct dh_st),
    offsetof(struct _cffi_aligncheck, y),
    offsetof(struct dh_st, p),
    sizeof(((struct dh_st *)0)->p),
    offsetof(struct dh_st, g),
    sizeof(((struct dh_st *)0)->g),
    offsetof(struct dh_st, priv_key),
    sizeof(((struct dh_st *)0)->priv_key),
    offsetof(struct dh_st, pub_key),
    sizeof(((struct dh_st *)0)->pub_key),
    -1
  };
  return _cffi_get_struct_layout(nums);
  /* the next line is not executed, but compiled */
  _cffi_check_struct_dh_st(0);
}

static void _cffi_check_struct_dsa_st(struct dsa_st *p)
{
  /* only to generate compile-time warnings or errors */
  { BIGNUM * *tmp = &p->p; (void)tmp; }
  { BIGNUM * *tmp = &p->q; (void)tmp; }
  { BIGNUM * *tmp = &p->g; (void)tmp; }
  { BIGNUM * *tmp = &p->priv_key; (void)tmp; }
  { BIGNUM * *tmp = &p->pub_key; (void)tmp; }
}
static PyObject *
_cffi_layout_struct_dsa_st(PyObject *self, PyObject *noarg)
{
  struct _cffi_aligncheck { char x; struct dsa_st y; };
  static Py_ssize_t nums[] = {
    sizeof(struct dsa_st),
    offsetof(struct _cffi_aligncheck, y),
    offsetof(struct dsa_st, p),
    sizeof(((struct dsa_st *)0)->p),
    offsetof(struct dsa_st, q),
    sizeof(((struct dsa_st *)0)->q),
    offsetof(struct dsa_st, g),
    sizeof(((struct dsa_st *)0)->g),
    offsetof(struct dsa_st, priv_key),
    sizeof(((struct dsa_st *)0)->priv_key),
    offsetof(struct dsa_st, pub_key),
    sizeof(((struct dsa_st *)0)->pub_key),
    -1
  };
  return _cffi_get_struct_layout(nums);
  /* the next line is not executed, but compiled */
  _cffi_check_struct_dsa_st(0);
}

static void _cffi_check_struct_env_md_ctx_st(struct env_md_ctx_st *p)
{
  /* only to generate compile-time warnings or errors */
}
static PyObject *
_cffi_layout_struct_env_md_ctx_st(PyObject *self, PyObject *noarg)
{
  struct _cffi_aligncheck { char x; struct env_md_ctx_st y; };
  static Py_ssize_t nums[] = {
    sizeof(struct env_md_ctx_st),
    offsetof(struct _cffi_aligncheck, y),
    -1
  };
  return _cffi_get_struct_layout(nums);
  /* the next line is not executed, but compiled */
  _cffi_check_struct_env_md_ctx_st(0);
}

static void _cffi_check_struct_evp_pkey_st(struct evp_pkey_st *p)
{
  /* only to generate compile-time warnings or errors */
  (void)((p->type) << 1);
}
static PyObject *
_cffi_layout_struct_evp_pkey_st(PyObject *self, PyObject *noarg)
{
  struct _cffi_aligncheck { char x; struct evp_pkey_st y; };
  static Py_ssize_t nums[] = {
    sizeof(struct evp_pkey_st),
    offsetof(struct _cffi_aligncheck, y),
    offsetof(struct evp_pkey_st, type),
    sizeof(((struct evp_pkey_st *)0)->type),
    -1
  };
  return _cffi_get_struct_layout(nums);
  /* the next line is not executed, but compiled */
  _cffi_check_struct_evp_pkey_st(0);
}

static void _cffi_check_struct_rsa_st(struct rsa_st *p)
{
  /* only to generate compile-time warnings or errors */
  { BIGNUM * *tmp = &p->n; (void)tmp; }
  { BIGNUM * *tmp = &p->e; (void)tmp; }
  { BIGNUM * *tmp = &p->d; (void)tmp; }
  { BIGNUM * *tmp = &p->p; (void)tmp; }
  { BIGNUM * *tmp = &p->q; (void)tmp; }
  { BIGNUM * *tmp = &p->dmp1; (void)tmp; }
  { BIGNUM * *tmp = &p->dmq1; (void)tmp; }
  { BIGNUM * *tmp = &p->iqmp; (void)tmp; }
}
static PyObject *
_cffi_layout_struct_rsa_st(PyObject *self, PyObject *noarg)
{
  struct _cffi_aligncheck { char x; struct rsa_st y; };
  static Py_ssize_t nums[] = {
    sizeof(struct rsa_st),
    offsetof(struct _cffi_aligncheck, y),
    offsetof(struct rsa_st, n),
    sizeof(((struct rsa_st *)0)->n),
    offsetof(struct rsa_st, e),
    sizeof(((struct rsa_st *)0)->e),
    offsetof(struct rsa_st, d),
    sizeof(((struct rsa_st *)0)->d),
    offsetof(struct rsa_st, p),
    sizeof(((struct rsa_st *)0)->p),
    offsetof(struct rsa_st, q),
    sizeof(((struct rsa_st *)0)->q),
    offsetof(struct rsa_st, dmp1),
    sizeof(((struct rsa_st *)0)->dmp1),
    offsetof(struct rsa_st, dmq1),
    sizeof(((struct rsa_st *)0)->dmq1),
    offsetof(struct rsa_st, iqmp),
    sizeof(((struct rsa_st *)0)->iqmp),
    -1
  };
  return _cffi_get_struct_layout(nums);
  /* the next line is not executed, but compiled */
  _cffi_check_struct_rsa_st(0);
}

static int _cffi_setup_custom(PyObject *lib)
{
  return _cffi_const_ERR_add_error_data(lib);
}

static PyMethodDef _cffi_methods[] = {
  {"_cffi_layout__ASN1_TIME", _cffi_layout__ASN1_TIME, METH_NOARGS},
  {"_cffi_layout__EDIPARTYNAME", _cffi_layout__EDIPARTYNAME, METH_NOARGS},
  {"_cffi_layout__EVP_CIPHER_CTX", _cffi_layout__EVP_CIPHER_CTX, METH_NOARGS},
  {"_cffi_layout__GENERAL_NAME", _cffi_layout__GENERAL_NAME, METH_NOARGS},
  {"_cffi_layout__HMAC_CTX", _cffi_layout__HMAC_CTX, METH_NOARGS},
  {"_cffi_layout__OTHERNAME", _cffi_layout__OTHERNAME, METH_NOARGS},
  {"_cffi_layout__PKCS7", _cffi_layout__PKCS7, METH_NOARGS},
  {"_cffi_layout__SSL", _cffi_layout__SSL, METH_NOARGS},
  {"_cffi_layout__SSL3_STATE", _cffi_layout__SSL3_STATE, METH_NOARGS},
  {"_cffi_layout__SSL_SESSION", _cffi_layout__SSL_SESSION, METH_NOARGS},
  {"_cffi_layout__X509", _cffi_layout__X509, METH_NOARGS},
  {"_cffi_layout__X509V3_CTX", _cffi_layout__X509V3_CTX, METH_NOARGS},
  {"_cffi_layout__X509V3_EXT_METHOD", _cffi_layout__X509V3_EXT_METHOD, METH_NOARGS},
  {"_cffi_layout__X509_ALGOR", _cffi_layout__X509_ALGOR, METH_NOARGS},
  {"_cffi_layout__X509_CINF", _cffi_layout__X509_CINF, METH_NOARGS},
  {"_cffi_layout__X509_CRL", _cffi_layout__X509_CRL, METH_NOARGS},
  {"_cffi_layout__X509_CRL_INFO", _cffi_layout__X509_CRL_INFO, METH_NOARGS},
  {"_cffi_layout__X509_EXTENSION", _cffi_layout__X509_EXTENSION, METH_NOARGS},
  {"_cffi_layout__X509_REVOKED", _cffi_layout__X509_REVOKED, METH_NOARGS},
  {"AES_set_decrypt_key", _cffi_f_AES_set_decrypt_key, METH_VARARGS},
  {"AES_set_encrypt_key", _cffi_f_AES_set_encrypt_key, METH_VARARGS},
  {"AES_unwrap_key", _cffi_f_AES_unwrap_key, METH_VARARGS},
  {"AES_wrap_key", _cffi_f_AES_wrap_key, METH_VARARGS},
  {"ASN1_ENUMERATED_free", _cffi_f_ASN1_ENUMERATED_free, METH_O},
  {"ASN1_ENUMERATED_new", _cffi_f_ASN1_ENUMERATED_new, METH_NOARGS},
  {"ASN1_ENUMERATED_set", _cffi_f_ASN1_ENUMERATED_set, METH_VARARGS},
  {"ASN1_GENERALIZEDTIME_check", _cffi_f_ASN1_GENERALIZEDTIME_check, METH_O},
  {"ASN1_GENERALIZEDTIME_free", _cffi_f_ASN1_GENERALIZEDTIME_free, METH_O},
  {"ASN1_GENERALIZEDTIME_set_string", _cffi_f_ASN1_GENERALIZEDTIME_set_string, METH_VARARGS},
  {"ASN1_INTEGER_cmp", _cffi_f_ASN1_INTEGER_cmp, METH_VARARGS},
  {"ASN1_INTEGER_dup", _cffi_f_ASN1_INTEGER_dup, METH_O},
  {"ASN1_INTEGER_free", _cffi_f_ASN1_INTEGER_free, METH_O},
  {"ASN1_INTEGER_get", _cffi_f_ASN1_INTEGER_get, METH_O},
  {"ASN1_INTEGER_new", _cffi_f_ASN1_INTEGER_new, METH_NOARGS},
  {"ASN1_INTEGER_set", _cffi_f_ASN1_INTEGER_set, METH_VARARGS},
  {"ASN1_INTEGER_to_BN", _cffi_f_ASN1_INTEGER_to_BN, METH_VARARGS},
  {"ASN1_ITEM_ptr", _cffi_f_ASN1_ITEM_ptr, METH_O},
  {"ASN1_OBJECT_free", _cffi_f_ASN1_OBJECT_free, METH_O},
  {"ASN1_OBJECT_new", _cffi_f_ASN1_OBJECT_new, METH_NOARGS},
  {"ASN1_OCTET_STRING_cmp", _cffi_f_ASN1_OCTET_STRING_cmp, METH_VARARGS},
  {"ASN1_OCTET_STRING_dup", _cffi_f_ASN1_OCTET_STRING_dup, METH_O},
  {"ASN1_OCTET_STRING_free", _cffi_f_ASN1_OCTET_STRING_free, METH_O},
  {"ASN1_OCTET_STRING_new", _cffi_f_ASN1_OCTET_STRING_new, METH_NOARGS},
  {"ASN1_OCTET_STRING_set", _cffi_f_ASN1_OCTET_STRING_set, METH_VARARGS},
  {"ASN1_STRING_cmp", _cffi_f_ASN1_STRING_cmp, METH_VARARGS},
  {"ASN1_STRING_data", _cffi_f_ASN1_STRING_data, METH_O},
  {"ASN1_STRING_dup", _cffi_f_ASN1_STRING_dup, METH_O},
  {"ASN1_STRING_free", _cffi_f_ASN1_STRING_free, METH_O},
  {"ASN1_STRING_length", _cffi_f_ASN1_STRING_length, METH_O},
  {"ASN1_STRING_new", _cffi_f_ASN1_STRING_new, METH_NOARGS},
  {"ASN1_STRING_set", _cffi_f_ASN1_STRING_set, METH_VARARGS},
  {"ASN1_STRING_to_UTF8", _cffi_f_ASN1_STRING_to_UTF8, METH_VARARGS},
  {"ASN1_STRING_type", _cffi_f_ASN1_STRING_type, METH_O},
  {"ASN1_STRING_type_new", _cffi_f_ASN1_STRING_type_new, METH_O},
  {"ASN1_TIME_new", _cffi_f_ASN1_TIME_new, METH_NOARGS},
  {"ASN1_TIME_to_generalizedtime", _cffi_f_ASN1_TIME_to_generalizedtime, METH_VARARGS},
  {"ASN1_UTCTIME_cmp_time_t", _cffi_f_ASN1_UTCTIME_cmp_time_t, METH_VARARGS},
  {"ASN1_item_d2i", _cffi_f_ASN1_item_d2i, METH_VARARGS},
  {"BIO_append_filename", _cffi_f_BIO_append_filename, METH_VARARGS},
  {"BIO_callback_ctrl", _cffi_f_BIO_callback_ctrl, METH_VARARGS},
  {"BIO_ctrl", _cffi_f_BIO_ctrl, METH_VARARGS},
  {"BIO_ctrl_pending", _cffi_f_BIO_ctrl_pending, METH_O},
  {"BIO_ctrl_wpending", _cffi_f_BIO_ctrl_wpending, METH_O},
  {"BIO_eof", _cffi_f_BIO_eof, METH_O},
  {"BIO_f_buffer", _cffi_f_BIO_f_buffer, METH_NOARGS},
  {"BIO_f_null", _cffi_f_BIO_f_null, METH_NOARGS},
  {"BIO_find_type", _cffi_f_BIO_find_type, METH_VARARGS},
  {"BIO_flush", _cffi_f_BIO_flush, METH_O},
  {"BIO_free", _cffi_f_BIO_free, METH_O},
  {"BIO_free_all", _cffi_f_BIO_free_all, METH_O},
  {"BIO_get_buffer_num_lines", _cffi_f_BIO_get_buffer_num_lines, METH_O},
  {"BIO_get_close", _cffi_f_BIO_get_close, METH_O},
  {"BIO_get_fd", _cffi_f_BIO_get_fd, METH_VARARGS},
  {"BIO_get_fp", _cffi_f_BIO_get_fp, METH_VARARGS},
  {"BIO_get_info_callback", _cffi_f_BIO_get_info_callback, METH_VARARGS},
  {"BIO_get_mem_data", _cffi_f_BIO_get_mem_data, METH_VARARGS},
  {"BIO_get_mem_ptr", _cffi_f_BIO_get_mem_ptr, METH_VARARGS},
  {"BIO_gets", _cffi_f_BIO_gets, METH_VARARGS},
  {"BIO_int_ctrl", _cffi_f_BIO_int_ctrl, METH_VARARGS},
  {"BIO_method_type", _cffi_f_BIO_method_type, METH_O},
  {"BIO_new", _cffi_f_BIO_new, METH_O},
  {"BIO_new_fd", _cffi_f_BIO_new_fd, METH_VARARGS},
  {"BIO_new_file", _cffi_f_BIO_new_file, METH_VARARGS},
  {"BIO_new_fp", _cffi_f_BIO_new_fp, METH_VARARGS},
  {"BIO_new_mem_buf", _cffi_f_BIO_new_mem_buf, METH_VARARGS},
  {"BIO_new_socket", _cffi_f_BIO_new_socket, METH_VARARGS},
  {"BIO_next", _cffi_f_BIO_next, METH_O},
  {"BIO_pending", _cffi_f_BIO_pending, METH_O},
  {"BIO_pop", _cffi_f_BIO_pop, METH_O},
  {"BIO_ptr_ctrl", _cffi_f_BIO_ptr_ctrl, METH_VARARGS},
  {"BIO_push", _cffi_f_BIO_push, METH_VARARGS},
  {"BIO_puts", _cffi_f_BIO_puts, METH_VARARGS},
  {"BIO_read", _cffi_f_BIO_read, METH_VARARGS},
  {"BIO_read_filename", _cffi_f_BIO_read_filename, METH_VARARGS},
  {"BIO_reset", _cffi_f_BIO_reset, METH_O},
  {"BIO_retry_type", _cffi_f_BIO_retry_type, METH_O},
  {"BIO_rw_filename", _cffi_f_BIO_rw_filename, METH_VARARGS},
  {"BIO_s_fd", _cffi_f_BIO_s_fd, METH_NOARGS},
  {"BIO_s_file", _cffi_f_BIO_s_file, METH_NOARGS},
  {"BIO_s_mem", _cffi_f_BIO_s_mem, METH_NOARGS},
  {"BIO_s_null", _cffi_f_BIO_s_null, METH_NOARGS},
  {"BIO_s_socket", _cffi_f_BIO_s_socket, METH_NOARGS},
  {"BIO_seek", _cffi_f_BIO_seek, METH_VARARGS},
  {"BIO_set", _cffi_f_BIO_set, METH_VARARGS},
  {"BIO_set_buffer_read_data", _cffi_f_BIO_set_buffer_read_data, METH_VARARGS},
  {"BIO_set_buffer_size", _cffi_f_BIO_set_buffer_size, METH_VARARGS},
  {"BIO_set_close", _cffi_f_BIO_set_close, METH_VARARGS},
  {"BIO_set_fd", _cffi_f_BIO_set_fd, METH_VARARGS},
  {"BIO_set_fp", _cffi_f_BIO_set_fp, METH_VARARGS},
  {"BIO_set_info_callback", _cffi_f_BIO_set_info_callback, METH_VARARGS},
  {"BIO_set_mem_buf", _cffi_f_BIO_set_mem_buf, METH_VARARGS},
  {"BIO_set_mem_eof_return", _cffi_f_BIO_set_mem_eof_return, METH_VARARGS},
  {"BIO_set_read_buffer_size", _cffi_f_BIO_set_read_buffer_size, METH_VARARGS},
  {"BIO_set_write_buffer_size", _cffi_f_BIO_set_write_buffer_size, METH_VARARGS},
  {"BIO_should_io_special", _cffi_f_BIO_should_io_special, METH_O},
  {"BIO_should_read", _cffi_f_BIO_should_read, METH_O},
  {"BIO_should_retry", _cffi_f_BIO_should_retry, METH_O},
  {"BIO_should_write", _cffi_f_BIO_should_write, METH_O},
  {"BIO_tell", _cffi_f_BIO_tell, METH_O},
  {"BIO_vfree", _cffi_f_BIO_vfree, METH_O},
  {"BIO_wpending", _cffi_f_BIO_wpending, METH_O},
  {"BIO_write", _cffi_f_BIO_write, METH_VARARGS},
  {"BIO_write_filename", _cffi_f_BIO_write_filename, METH_VARARGS},
  {"BN_add", _cffi_f_BN_add, METH_VARARGS},
  {"BN_bin2bn", _cffi_f_BN_bin2bn, METH_VARARGS},
  {"BN_bn2bin", _cffi_f_BN_bn2bin, METH_VARARGS},
  {"BN_bn2hex", _cffi_f_BN_bn2hex, METH_O},
  {"BN_copy", _cffi_f_BN_copy, METH_VARARGS},
  {"BN_dec2bn", _cffi_f_BN_dec2bn, METH_VARARGS},
  {"BN_div", _cffi_f_BN_div, METH_VARARGS},
  {"BN_dup", _cffi_f_BN_dup, METH_O},
  {"BN_exp", _cffi_f_BN_exp, METH_VARARGS},
  {"BN_free", _cffi_f_BN_free, METH_O},
  {"BN_gcd", _cffi_f_BN_gcd, METH_VARARGS},
  {"BN_get_word", _cffi_f_BN_get_word, METH_O},
  {"BN_hex2bn", _cffi_f_BN_hex2bn, METH_VARARGS},
  {"BN_mod", _cffi_f_BN_mod, METH_VARARGS},
  {"BN_mod_add", _cffi_f_BN_mod_add, METH_VARARGS},
  {"BN_mod_exp", _cffi_f_BN_mod_exp, METH_VARARGS},
  {"BN_mod_inverse", _cffi_f_BN_mod_inverse, METH_VARARGS},
  {"BN_mod_mul", _cffi_f_BN_mod_mul, METH_VARARGS},
  {"BN_mod_sqr", _cffi_f_BN_mod_sqr, METH_VARARGS},
  {"BN_mod_sub", _cffi_f_BN_mod_sub, METH_VARARGS},
  {"BN_mul", _cffi_f_BN_mul, METH_VARARGS},
  {"BN_new", _cffi_f_BN_new, METH_NOARGS},
  {"BN_nnmod", _cffi_f_BN_nnmod, METH_VARARGS},
  {"BN_num_bits", _cffi_f_BN_num_bits, METH_O},
  {"BN_one", _cffi_f_BN_one, METH_O},
  {"BN_set_word", _cffi_f_BN_set_word, METH_VARARGS},
  {"BN_sqr", _cffi_f_BN_sqr, METH_VARARGS},
  {"BN_sub", _cffi_f_BN_sub, METH_VARARGS},
  {"BN_to_ASN1_INTEGER", _cffi_f_BN_to_ASN1_INTEGER, METH_VARARGS},
  {"BN_value_one", _cffi_f_BN_value_one, METH_NOARGS},
  {"BN_zero", _cffi_f_BN_zero, METH_O},
  {"CRYPTO_add", _cffi_f_CRYPTO_add, METH_VARARGS},
  {"CRYPTO_cleanup_all_ex_data", _cffi_f_CRYPTO_cleanup_all_ex_data, METH_NOARGS},
  {"CRYPTO_free", _cffi_f_CRYPTO_free, METH_O},
  {"CRYPTO_get_id_callback", _cffi_f_CRYPTO_get_id_callback, METH_NOARGS},
  {"CRYPTO_get_locking_callback", _cffi_f_CRYPTO_get_locking_callback, METH_NOARGS},
  {"CRYPTO_is_mem_check_on", _cffi_f_CRYPTO_is_mem_check_on, METH_NOARGS},
  {"CRYPTO_lock", _cffi_f_CRYPTO_lock, METH_VARARGS},
  {"CRYPTO_malloc_debug_init", _cffi_f_CRYPTO_malloc_debug_init, METH_NOARGS},
  {"CRYPTO_malloc_init", _cffi_f_CRYPTO_malloc_init, METH_NOARGS},
  {"CRYPTO_mem_ctrl", _cffi_f_CRYPTO_mem_ctrl, METH_O},
  {"CRYPTO_mem_leaks", _cffi_f_CRYPTO_mem_leaks, METH_O},
  {"CRYPTO_num_locks", _cffi_f_CRYPTO_num_locks, METH_NOARGS},
  {"CRYPTO_set_id_callback", _cffi_f_CRYPTO_set_id_callback, METH_O},
  {"CRYPTO_set_locking_callback", _cffi_f_CRYPTO_set_locking_callback, METH_O},
  {"Cryptography_HMAC_CTX_copy", _cffi_f_Cryptography_HMAC_CTX_copy, METH_VARARGS},
  {"Cryptography_HMAC_Final", _cffi_f_Cryptography_HMAC_Final, METH_VARARGS},
  {"Cryptography_HMAC_Init_ex", _cffi_f_Cryptography_HMAC_Init_ex, METH_VARARGS},
  {"Cryptography_HMAC_Update", _cffi_f_Cryptography_HMAC_Update, METH_VARARGS},
  {"Cryptography_add_osrandom_engine", _cffi_f_Cryptography_add_osrandom_engine, METH_NOARGS},
  {"DH_free", _cffi_f_DH_free, METH_O},
  {"DH_new", _cffi_f_DH_new, METH_NOARGS},
  {"DSA_free", _cffi_f_DSA_free, METH_O},
  {"DSA_generate_key", _cffi_f_DSA_generate_key, METH_O},
  {"DSA_generate_parameters", _cffi_f_DSA_generate_parameters, METH_VARARGS},
  {"DTLSv1_client_method", _cffi_f_DTLSv1_client_method, METH_NOARGS},
  {"DTLSv1_method", _cffi_f_DTLSv1_method, METH_NOARGS},
  {"DTLSv1_server_method", _cffi_f_DTLSv1_server_method, METH_NOARGS},
  {"EC_KEY_free", _cffi_f_EC_KEY_free, METH_O},
  {"EC_KEY_new_by_curve_name", _cffi_f_EC_KEY_new_by_curve_name, METH_O},
  {"ENGINE_add", _cffi_f_ENGINE_add, METH_O},
  {"ENGINE_add_conf_module", _cffi_f_ENGINE_add_conf_module, METH_NOARGS},
  {"ENGINE_by_id", _cffi_f_ENGINE_by_id, METH_O},
  {"ENGINE_cleanup", _cffi_f_ENGINE_cleanup, METH_NOARGS},
  {"ENGINE_cmd_is_executable", _cffi_f_ENGINE_cmd_is_executable, METH_VARARGS},
  {"ENGINE_ctrl", _cffi_f_ENGINE_ctrl, METH_VARARGS},
  {"ENGINE_ctrl_cmd", _cffi_f_ENGINE_ctrl_cmd, METH_VARARGS},
  {"ENGINE_ctrl_cmd_string", _cffi_f_ENGINE_ctrl_cmd_string, METH_VARARGS},
  {"ENGINE_finish", _cffi_f_ENGINE_finish, METH_O},
  {"ENGINE_free", _cffi_f_ENGINE_free, METH_O},
  {"ENGINE_get_DH", _cffi_f_ENGINE_get_DH, METH_O},
  {"ENGINE_get_DSA", _cffi_f_ENGINE_get_DSA, METH_O},
  {"ENGINE_get_ECDH", _cffi_f_ENGINE_get_ECDH, METH_O},
  {"ENGINE_get_ECDSA", _cffi_f_ENGINE_get_ECDSA, METH_O},
  {"ENGINE_get_RAND", _cffi_f_ENGINE_get_RAND, METH_O},
  {"ENGINE_get_RSA", _cffi_f_ENGINE_get_RSA, METH_O},
  {"ENGINE_get_STORE", _cffi_f_ENGINE_get_STORE, METH_O},
  {"ENGINE_get_cipher", _cffi_f_ENGINE_get_cipher, METH_VARARGS},
  {"ENGINE_get_cipher_engine", _cffi_f_ENGINE_get_cipher_engine, METH_O},
  {"ENGINE_get_cmd_defns", _cffi_f_ENGINE_get_cmd_defns, METH_O},
  {"ENGINE_get_default_DH", _cffi_f_ENGINE_get_default_DH, METH_NOARGS},
  {"ENGINE_get_default_DSA", _cffi_f_ENGINE_get_default_DSA, METH_NOARGS},
  {"ENGINE_get_default_ECDH", _cffi_f_ENGINE_get_default_ECDH, METH_NOARGS},
  {"ENGINE_get_default_ECDSA", _cffi_f_ENGINE_get_default_ECDSA, METH_NOARGS},
  {"ENGINE_get_default_RAND", _cffi_f_ENGINE_get_default_RAND, METH_NOARGS},
  {"ENGINE_get_default_RSA", _cffi_f_ENGINE_get_default_RSA, METH_NOARGS},
  {"ENGINE_get_digest", _cffi_f_ENGINE_get_digest, METH_VARARGS},
  {"ENGINE_get_digest_engine", _cffi_f_ENGINE_get_digest_engine, METH_O},
  {"ENGINE_get_first", _cffi_f_ENGINE_get_first, METH_NOARGS},
  {"ENGINE_get_flags", _cffi_f_ENGINE_get_flags, METH_O},
  {"ENGINE_get_id", _cffi_f_ENGINE_get_id, METH_O},
  {"ENGINE_get_last", _cffi_f_ENGINE_get_last, METH_NOARGS},
  {"ENGINE_get_name", _cffi_f_ENGINE_get_name, METH_O},
  {"ENGINE_get_next", _cffi_f_ENGINE_get_next, METH_O},
  {"ENGINE_get_prev", _cffi_f_ENGINE_get_prev, METH_O},
  {"ENGINE_get_table_flags", _cffi_f_ENGINE_get_table_flags, METH_NOARGS},
  {"ENGINE_init", _cffi_f_ENGINE_init, METH_O},
  {"ENGINE_load_builtin_engines", _cffi_f_ENGINE_load_builtin_engines, METH_NOARGS},
  {"ENGINE_load_cryptodev", _cffi_f_ENGINE_load_cryptodev, METH_NOARGS},
  {"ENGINE_load_dynamic", _cffi_f_ENGINE_load_dynamic, METH_NOARGS},
  {"ENGINE_load_openssl", _cffi_f_ENGINE_load_openssl, METH_NOARGS},
  {"ENGINE_load_private_key", _cffi_f_ENGINE_load_private_key, METH_VARARGS},
  {"ENGINE_load_public_key", _cffi_f_ENGINE_load_public_key, METH_VARARGS},
  {"ENGINE_new", _cffi_f_ENGINE_new, METH_NOARGS},
  {"ENGINE_register_DH", _cffi_f_ENGINE_register_DH, METH_O},
  {"ENGINE_register_DSA", _cffi_f_ENGINE_register_DSA, METH_O},
  {"ENGINE_register_ECDH", _cffi_f_ENGINE_register_ECDH, METH_O},
  {"ENGINE_register_ECDSA", _cffi_f_ENGINE_register_ECDSA, METH_O},
  {"ENGINE_register_RAND", _cffi_f_ENGINE_register_RAND, METH_O},
  {"ENGINE_register_RSA", _cffi_f_ENGINE_register_RSA, METH_O},
  {"ENGINE_register_STORE", _cffi_f_ENGINE_register_STORE, METH_O},
  {"ENGINE_register_all_DH", _cffi_f_ENGINE_register_all_DH, METH_NOARGS},
  {"ENGINE_register_all_DSA", _cffi_f_ENGINE_register_all_DSA, METH_NOARGS},
  {"ENGINE_register_all_ECDH", _cffi_f_ENGINE_register_all_ECDH, METH_NOARGS},
  {"ENGINE_register_all_ECDSA", _cffi_f_ENGINE_register_all_ECDSA, METH_NOARGS},
  {"ENGINE_register_all_RAND", _cffi_f_ENGINE_register_all_RAND, METH_NOARGS},
  {"ENGINE_register_all_RSA", _cffi_f_ENGINE_register_all_RSA, METH_NOARGS},
  {"ENGINE_register_all_STORE", _cffi_f_ENGINE_register_all_STORE, METH_NOARGS},
  {"ENGINE_register_all_ciphers", _cffi_f_ENGINE_register_all_ciphers, METH_NOARGS},
  {"ENGINE_register_all_complete", _cffi_f_ENGINE_register_all_complete, METH_NOARGS},
  {"ENGINE_register_all_digests", _cffi_f_ENGINE_register_all_digests, METH_NOARGS},
  {"ENGINE_register_ciphers", _cffi_f_ENGINE_register_ciphers, METH_O},
  {"ENGINE_register_complete", _cffi_f_ENGINE_register_complete, METH_O},
  {"ENGINE_register_digests", _cffi_f_ENGINE_register_digests, METH_O},
  {"ENGINE_remove", _cffi_f_ENGINE_remove, METH_O},
  {"ENGINE_set_DH", _cffi_f_ENGINE_set_DH, METH_VARARGS},
  {"ENGINE_set_DSA", _cffi_f_ENGINE_set_DSA, METH_VARARGS},
  {"ENGINE_set_ECDH", _cffi_f_ENGINE_set_ECDH, METH_VARARGS},
  {"ENGINE_set_ECDSA", _cffi_f_ENGINE_set_ECDSA, METH_VARARGS},
  {"ENGINE_set_RAND", _cffi_f_ENGINE_set_RAND, METH_VARARGS},
  {"ENGINE_set_RSA", _cffi_f_ENGINE_set_RSA, METH_VARARGS},
  {"ENGINE_set_STORE", _cffi_f_ENGINE_set_STORE, METH_VARARGS},
  {"ENGINE_set_ciphers", _cffi_f_ENGINE_set_ciphers, METH_VARARGS},
  {"ENGINE_set_cmd_defns", _cffi_f_ENGINE_set_cmd_defns, METH_VARARGS},
  {"ENGINE_set_ctrl_function", _cffi_f_ENGINE_set_ctrl_function, METH_VARARGS},
  {"ENGINE_set_default", _cffi_f_ENGINE_set_default, METH_VARARGS},
  {"ENGINE_set_default_DH", _cffi_f_ENGINE_set_default_DH, METH_O},
  {"ENGINE_set_default_DSA", _cffi_f_ENGINE_set_default_DSA, METH_O},
  {"ENGINE_set_default_ECDH", _cffi_f_ENGINE_set_default_ECDH, METH_O},
  {"ENGINE_set_default_ECDSA", _cffi_f_ENGINE_set_default_ECDSA, METH_O},
  {"ENGINE_set_default_RAND", _cffi_f_ENGINE_set_default_RAND, METH_O},
  {"ENGINE_set_default_RSA", _cffi_f_ENGINE_set_default_RSA, METH_O},
  {"ENGINE_set_default_ciphers", _cffi_f_ENGINE_set_default_ciphers, METH_O},
  {"ENGINE_set_default_digests", _cffi_f_ENGINE_set_default_digests, METH_O},
  {"ENGINE_set_default_string", _cffi_f_ENGINE_set_default_string, METH_VARARGS},
  {"ENGINE_set_destroy_function", _cffi_f_ENGINE_set_destroy_function, METH_VARARGS},
  {"ENGINE_set_digests", _cffi_f_ENGINE_set_digests, METH_VARARGS},
  {"ENGINE_set_finish_function", _cffi_f_ENGINE_set_finish_function, METH_VARARGS},
  {"ENGINE_set_flags", _cffi_f_ENGINE_set_flags, METH_VARARGS},
  {"ENGINE_set_id", _cffi_f_ENGINE_set_id, METH_VARARGS},
  {"ENGINE_set_init_function", _cffi_f_ENGINE_set_init_function, METH_VARARGS},
  {"ENGINE_set_load_privkey_function", _cffi_f_ENGINE_set_load_privkey_function, METH_VARARGS},
  {"ENGINE_set_load_pubkey_function", _cffi_f_ENGINE_set_load_pubkey_function, METH_VARARGS},
  {"ENGINE_set_name", _cffi_f_ENGINE_set_name, METH_VARARGS},
  {"ENGINE_set_table_flags", _cffi_f_ENGINE_set_table_flags, METH_O},
  {"ENGINE_unregister_DH", _cffi_f_ENGINE_unregister_DH, METH_O},
  {"ENGINE_unregister_DSA", _cffi_f_ENGINE_unregister_DSA, METH_O},
  {"ENGINE_unregister_ECDH", _cffi_f_ENGINE_unregister_ECDH, METH_O},
  {"ENGINE_unregister_ECDSA", _cffi_f_ENGINE_unregister_ECDSA, METH_O},
  {"ENGINE_unregister_RAND", _cffi_f_ENGINE_unregister_RAND, METH_O},
  {"ENGINE_unregister_RSA", _cffi_f_ENGINE_unregister_RSA, METH_O},
  {"ENGINE_unregister_STORE", _cffi_f_ENGINE_unregister_STORE, METH_O},
  {"ENGINE_unregister_ciphers", _cffi_f_ENGINE_unregister_ciphers, METH_O},
  {"ENGINE_unregister_digests", _cffi_f_ENGINE_unregister_digests, METH_O},
  {"ENGINE_up_ref", _cffi_f_ENGINE_up_ref, METH_O},
  {"ERR_FATAL_ERROR", _cffi_f_ERR_FATAL_ERROR, METH_O},
  {"ERR_GET_FUNC", _cffi_f_ERR_GET_FUNC, METH_O},
  {"ERR_GET_LIB", _cffi_f_ERR_GET_LIB, METH_O},
  {"ERR_GET_REASON", _cffi_f_ERR_GET_REASON, METH_O},
  {"ERR_PACK", _cffi_f_ERR_PACK, METH_VARARGS},
  {"ERR_error_string", _cffi_f_ERR_error_string, METH_VARARGS},
  {"ERR_error_string_n", _cffi_f_ERR_error_string_n, METH_VARARGS},
  {"ERR_free_strings", _cffi_f_ERR_free_strings, METH_NOARGS},
  {"ERR_func_error_string", _cffi_f_ERR_func_error_string, METH_O},
  {"ERR_get_error", _cffi_f_ERR_get_error, METH_NOARGS},
  {"ERR_get_error_line", _cffi_f_ERR_get_error_line, METH_VARARGS},
  {"ERR_get_error_line_data", _cffi_f_ERR_get_error_line_data, METH_VARARGS},
  {"ERR_get_next_error_library", _cffi_f_ERR_get_next_error_library, METH_NOARGS},
  {"ERR_lib_error_string", _cffi_f_ERR_lib_error_string, METH_O},
  {"ERR_load_RAND_strings", _cffi_f_ERR_load_RAND_strings, METH_NOARGS},
  {"ERR_load_SSL_strings", _cffi_f_ERR_load_SSL_strings, METH_NOARGS},
  {"ERR_load_crypto_strings", _cffi_f_ERR_load_crypto_strings, METH_NOARGS},
  {"ERR_peek_error", _cffi_f_ERR_peek_error, METH_NOARGS},
  {"ERR_peek_error_line", _cffi_f_ERR_peek_error_line, METH_VARARGS},
  {"ERR_peek_error_line_data", _cffi_f_ERR_peek_error_line_data, METH_VARARGS},
  {"ERR_peek_last_error", _cffi_f_ERR_peek_last_error, METH_NOARGS},
  {"ERR_peek_last_error_line", _cffi_f_ERR_peek_last_error_line, METH_VARARGS},
  {"ERR_peek_last_error_line_data", _cffi_f_ERR_peek_last_error_line_data, METH_VARARGS},
  {"ERR_print_errors", _cffi_f_ERR_print_errors, METH_O},
  {"ERR_print_errors_fp", _cffi_f_ERR_print_errors_fp, METH_O},
  {"ERR_put_error", _cffi_f_ERR_put_error, METH_VARARGS},
  {"ERR_reason_error_string", _cffi_f_ERR_reason_error_string, METH_O},
  {"ERR_remove_thread_state", _cffi_f_ERR_remove_thread_state, METH_O},
  {"EVP_CIPHER_CTX_block_size", _cffi_f_EVP_CIPHER_CTX_block_size, METH_O},
  {"EVP_CIPHER_CTX_cipher", _cffi_f_EVP_CIPHER_CTX_cipher, METH_O},
  {"EVP_CIPHER_CTX_cleanup", _cffi_f_EVP_CIPHER_CTX_cleanup, METH_O},
  {"EVP_CIPHER_CTX_ctrl", _cffi_f_EVP_CIPHER_CTX_ctrl, METH_VARARGS},
  {"EVP_CIPHER_CTX_free", _cffi_f_EVP_CIPHER_CTX_free, METH_O},
  {"EVP_CIPHER_CTX_init", _cffi_f_EVP_CIPHER_CTX_init, METH_O},
  {"EVP_CIPHER_CTX_new", _cffi_f_EVP_CIPHER_CTX_new, METH_NOARGS},
  {"EVP_CIPHER_CTX_set_key_length", _cffi_f_EVP_CIPHER_CTX_set_key_length, METH_VARARGS},
  {"EVP_CIPHER_CTX_set_padding", _cffi_f_EVP_CIPHER_CTX_set_padding, METH_VARARGS},
  {"EVP_CIPHER_block_size", _cffi_f_EVP_CIPHER_block_size, METH_O},
  {"EVP_CipherFinal_ex", _cffi_f_EVP_CipherFinal_ex, METH_VARARGS},
  {"EVP_CipherInit_ex", _cffi_f_EVP_CipherInit_ex, METH_VARARGS},
  {"EVP_CipherUpdate", _cffi_f_EVP_CipherUpdate, METH_VARARGS},
  {"EVP_DecryptFinal_ex", _cffi_f_EVP_DecryptFinal_ex, METH_VARARGS},
  {"EVP_DecryptInit_ex", _cffi_f_EVP_DecryptInit_ex, METH_VARARGS},
  {"EVP_DecryptUpdate", _cffi_f_EVP_DecryptUpdate, METH_VARARGS},
  {"EVP_DigestFinal_ex", _cffi_f_EVP_DigestFinal_ex, METH_VARARGS},
  {"EVP_DigestInit_ex", _cffi_f_EVP_DigestInit_ex, METH_VARARGS},
  {"EVP_DigestUpdate", _cffi_f_EVP_DigestUpdate, METH_VARARGS},
  {"EVP_EncryptFinal_ex", _cffi_f_EVP_EncryptFinal_ex, METH_VARARGS},
  {"EVP_EncryptInit_ex", _cffi_f_EVP_EncryptInit_ex, METH_VARARGS},
  {"EVP_EncryptUpdate", _cffi_f_EVP_EncryptUpdate, METH_VARARGS},
  {"EVP_MD_CTX_cleanup", _cffi_f_EVP_MD_CTX_cleanup, METH_O},
  {"EVP_MD_CTX_copy_ex", _cffi_f_EVP_MD_CTX_copy_ex, METH_VARARGS},
  {"EVP_MD_CTX_create", _cffi_f_EVP_MD_CTX_create, METH_NOARGS},
  {"EVP_MD_CTX_destroy", _cffi_f_EVP_MD_CTX_destroy, METH_O},
  {"EVP_MD_CTX_md", _cffi_f_EVP_MD_CTX_md, METH_O},
  {"EVP_MD_size", _cffi_f_EVP_MD_size, METH_O},
  {"EVP_PKEY_CTX_dup", _cffi_f_EVP_PKEY_CTX_dup, METH_O},
  {"EVP_PKEY_CTX_free", _cffi_f_EVP_PKEY_CTX_free, METH_O},
  {"EVP_PKEY_CTX_new", _cffi_f_EVP_PKEY_CTX_new, METH_VARARGS},
  {"EVP_PKEY_CTX_new_id", _cffi_f_EVP_PKEY_CTX_new_id, METH_VARARGS},
  {"EVP_PKEY_CTX_set_rsa_padding", _cffi_f_EVP_PKEY_CTX_set_rsa_padding, METH_VARARGS},
  {"EVP_PKEY_CTX_set_rsa_pss_saltlen", _cffi_f_EVP_PKEY_CTX_set_rsa_pss_saltlen, METH_VARARGS},
  {"EVP_PKEY_CTX_set_signature_md", _cffi_f_EVP_PKEY_CTX_set_signature_md, METH_VARARGS},
  {"EVP_PKEY_assign_DSA", _cffi_f_EVP_PKEY_assign_DSA, METH_VARARGS},
  {"EVP_PKEY_assign_RSA", _cffi_f_EVP_PKEY_assign_RSA, METH_VARARGS},
  {"EVP_PKEY_bits", _cffi_f_EVP_PKEY_bits, METH_O},
  {"EVP_PKEY_free", _cffi_f_EVP_PKEY_free, METH_O},
  {"EVP_PKEY_get1_RSA", _cffi_f_EVP_PKEY_get1_RSA, METH_O},
  {"EVP_PKEY_new", _cffi_f_EVP_PKEY_new, METH_NOARGS},
  {"EVP_PKEY_set1_DSA", _cffi_f_EVP_PKEY_set1_DSA, METH_VARARGS},
  {"EVP_PKEY_set1_RSA", _cffi_f_EVP_PKEY_set1_RSA, METH_VARARGS},
  {"EVP_PKEY_sign", _cffi_f_EVP_PKEY_sign, METH_VARARGS},
  {"EVP_PKEY_sign_init", _cffi_f_EVP_PKEY_sign_init, METH_O},
  {"EVP_PKEY_size", _cffi_f_EVP_PKEY_size, METH_O},
  {"EVP_PKEY_type", _cffi_f_EVP_PKEY_type, METH_O},
  {"EVP_PKEY_verify", _cffi_f_EVP_PKEY_verify, METH_VARARGS},
  {"EVP_PKEY_verify_init", _cffi_f_EVP_PKEY_verify_init, METH_O},
  {"EVP_SignFinal", _cffi_f_EVP_SignFinal, METH_VARARGS},
  {"EVP_SignInit", _cffi_f_EVP_SignInit, METH_VARARGS},
  {"EVP_SignUpdate", _cffi_f_EVP_SignUpdate, METH_VARARGS},
  {"EVP_VerifyFinal", _cffi_f_EVP_VerifyFinal, METH_VARARGS},
  {"EVP_VerifyInit", _cffi_f_EVP_VerifyInit, METH_VARARGS},
  {"EVP_VerifyUpdate", _cffi_f_EVP_VerifyUpdate, METH_VARARGS},
  {"EVP_get_cipherbyname", _cffi_f_EVP_get_cipherbyname, METH_O},
  {"EVP_get_digestbyname", _cffi_f_EVP_get_digestbyname, METH_O},
  {"EVP_md5", _cffi_f_EVP_md5, METH_NOARGS},
  {"GENERAL_NAME_print", _cffi_f_GENERAL_NAME_print, METH_VARARGS},
  {"HMAC_CTX_cleanup", _cffi_f_HMAC_CTX_cleanup, METH_O},
  {"HMAC_CTX_init", _cffi_f_HMAC_CTX_init, METH_O},
  {"M_ASN1_TIME_dup", _cffi_f_M_ASN1_TIME_dup, METH_O},
  {"NETSCAPE_SPKI_b64_encode", _cffi_f_NETSCAPE_SPKI_b64_encode, METH_O},
  {"NETSCAPE_SPKI_free", _cffi_f_NETSCAPE_SPKI_free, METH_O},
  {"NETSCAPE_SPKI_get_pubkey", _cffi_f_NETSCAPE_SPKI_get_pubkey, METH_O},
  {"NETSCAPE_SPKI_new", _cffi_f_NETSCAPE_SPKI_new, METH_NOARGS},
  {"NETSCAPE_SPKI_set_pubkey", _cffi_f_NETSCAPE_SPKI_set_pubkey, METH_VARARGS},
  {"NETSCAPE_SPKI_sign", _cffi_f_NETSCAPE_SPKI_sign, METH_VARARGS},
  {"NETSCAPE_SPKI_verify", _cffi_f_NETSCAPE_SPKI_verify, METH_VARARGS},
  {"OBJ_cleanup", _cffi_f_OBJ_cleanup, METH_NOARGS},
  {"OBJ_cmp", _cffi_f_OBJ_cmp, METH_VARARGS},
  {"OBJ_create", _cffi_f_OBJ_create, METH_VARARGS},
  {"OBJ_dup", _cffi_f_OBJ_dup, METH_O},
  {"OBJ_ln2nid", _cffi_f_OBJ_ln2nid, METH_O},
  {"OBJ_nid2ln", _cffi_f_OBJ_nid2ln, METH_O},
  {"OBJ_nid2obj", _cffi_f_OBJ_nid2obj, METH_O},
  {"OBJ_nid2sn", _cffi_f_OBJ_nid2sn, METH_O},
  {"OBJ_obj2nid", _cffi_f_OBJ_obj2nid, METH_O},
  {"OBJ_obj2txt", _cffi_f_OBJ_obj2txt, METH_VARARGS},
  {"OBJ_sn2nid", _cffi_f_OBJ_sn2nid, METH_O},
  {"OBJ_txt2nid", _cffi_f_OBJ_txt2nid, METH_O},
  {"OBJ_txt2obj", _cffi_f_OBJ_txt2obj, METH_VARARGS},
  {"OPENSSL_free", _cffi_f_OPENSSL_free, METH_O},
  {"OpenSSL_add_all_algorithms", _cffi_f_OpenSSL_add_all_algorithms, METH_NOARGS},
  {"PEM_read_bio_DHparams", _cffi_f_PEM_read_bio_DHparams, METH_VARARGS},
  {"PEM_read_bio_DSAPrivateKey", _cffi_f_PEM_read_bio_DSAPrivateKey, METH_VARARGS},
  {"PEM_read_bio_DSA_PUBKEY", _cffi_f_PEM_read_bio_DSA_PUBKEY, METH_VARARGS},
  {"PEM_read_bio_PKCS7", _cffi_f_PEM_read_bio_PKCS7, METH_VARARGS},
  {"PEM_read_bio_PUBKEY", _cffi_f_PEM_read_bio_PUBKEY, METH_VARARGS},
  {"PEM_read_bio_PrivateKey", _cffi_f_PEM_read_bio_PrivateKey, METH_VARARGS},
  {"PEM_read_bio_RSAPrivateKey", _cffi_f_PEM_read_bio_RSAPrivateKey, METH_VARARGS},
  {"PEM_read_bio_RSAPublicKey", _cffi_f_PEM_read_bio_RSAPublicKey, METH_VARARGS},
  {"PEM_read_bio_X509", _cffi_f_PEM_read_bio_X509, METH_VARARGS},
  {"PEM_read_bio_X509_CRL", _cffi_f_PEM_read_bio_X509_CRL, METH_VARARGS},
  {"PEM_read_bio_X509_REQ", _cffi_f_PEM_read_bio_X509_REQ, METH_VARARGS},
  {"PEM_write_bio_DSAPrivateKey", _cffi_f_PEM_write_bio_DSAPrivateKey, METH_VARARGS},
  {"PEM_write_bio_DSA_PUBKEY", _cffi_f_PEM_write_bio_DSA_PUBKEY, METH_VARARGS},
  {"PEM_write_bio_PKCS8PrivateKey", _cffi_f_PEM_write_bio_PKCS8PrivateKey, METH_VARARGS},
  {"PEM_write_bio_PUBKEY", _cffi_f_PEM_write_bio_PUBKEY, METH_VARARGS},
  {"PEM_write_bio_PrivateKey", _cffi_f_PEM_write_bio_PrivateKey, METH_VARARGS},
  {"PEM_write_bio_RSAPrivateKey", _cffi_f_PEM_write_bio_RSAPrivateKey, METH_VARARGS},
  {"PEM_write_bio_RSAPublicKey", _cffi_f_PEM_write_bio_RSAPublicKey, METH_VARARGS},
  {"PEM_write_bio_X509", _cffi_f_PEM_write_bio_X509, METH_VARARGS},
  {"PEM_write_bio_X509_CRL", _cffi_f_PEM_write_bio_X509_CRL, METH_VARARGS},
  {"PEM_write_bio_X509_REQ", _cffi_f_PEM_write_bio_X509_REQ, METH_VARARGS},
  {"PKCS12_create", _cffi_f_PKCS12_create, METH_VARARGS},
  {"PKCS12_free", _cffi_f_PKCS12_free, METH_O},
  {"PKCS12_parse", _cffi_f_PKCS12_parse, METH_VARARGS},
  {"PKCS5_PBKDF2_HMAC", _cffi_f_PKCS5_PBKDF2_HMAC, METH_VARARGS},
  {"PKCS5_PBKDF2_HMAC_SHA1", _cffi_f_PKCS5_PBKDF2_HMAC_SHA1, METH_VARARGS},
  {"PKCS7_free", _cffi_f_PKCS7_free, METH_O},
  {"PKCS7_type_is_data", _cffi_f_PKCS7_type_is_data, METH_O},
  {"PKCS7_type_is_enveloped", _cffi_f_PKCS7_type_is_enveloped, METH_O},
  {"PKCS7_type_is_signed", _cffi_f_PKCS7_type_is_signed, METH_O},
  {"PKCS7_type_is_signedAndEnveloped", _cffi_f_PKCS7_type_is_signedAndEnveloped, METH_O},
  {"RAND_add", _cffi_f_RAND_add, METH_VARARGS},
  {"RAND_bytes", _cffi_f_RAND_bytes, METH_VARARGS},
  {"RAND_cleanup", _cffi_f_RAND_cleanup, METH_NOARGS},
  {"RAND_egd", _cffi_f_RAND_egd, METH_O},
  {"RAND_egd_bytes", _cffi_f_RAND_egd_bytes, METH_VARARGS},
  {"RAND_file_name", _cffi_f_RAND_file_name, METH_VARARGS},
  {"RAND_load_file", _cffi_f_RAND_load_file, METH_VARARGS},
  {"RAND_pseudo_bytes", _cffi_f_RAND_pseudo_bytes, METH_VARARGS},
  {"RAND_query_egd_bytes", _cffi_f_RAND_query_egd_bytes, METH_VARARGS},
  {"RAND_seed", _cffi_f_RAND_seed, METH_VARARGS},
  {"RAND_status", _cffi_f_RAND_status, METH_NOARGS},
  {"RAND_write_file", _cffi_f_RAND_write_file, METH_O},
  {"RSAPublicKey_dup", _cffi_f_RSAPublicKey_dup, METH_O},
  {"RSA_blinding_off", _cffi_f_RSA_blinding_off, METH_O},
  {"RSA_blinding_on", _cffi_f_RSA_blinding_on, METH_VARARGS},
  {"RSA_check_key", _cffi_f_RSA_check_key, METH_O},
  {"RSA_free", _cffi_f_RSA_free, METH_O},
  {"RSA_generate_key_ex", _cffi_f_RSA_generate_key_ex, METH_VARARGS},
  {"RSA_new", _cffi_f_RSA_new, METH_NOARGS},
  {"RSA_padding_add_PKCS1_OAEP", _cffi_f_RSA_padding_add_PKCS1_OAEP, METH_VARARGS},
  {"RSA_padding_add_PKCS1_PSS", _cffi_f_RSA_padding_add_PKCS1_PSS, METH_VARARGS},
  {"RSA_padding_check_PKCS1_OAEP", _cffi_f_RSA_padding_check_PKCS1_OAEP, METH_VARARGS},
  {"RSA_print", _cffi_f_RSA_print, METH_VARARGS},
  {"RSA_private_decrypt", _cffi_f_RSA_private_decrypt, METH_VARARGS},
  {"RSA_private_encrypt", _cffi_f_RSA_private_encrypt, METH_VARARGS},
  {"RSA_public_decrypt", _cffi_f_RSA_public_decrypt, METH_VARARGS},
  {"RSA_public_encrypt", _cffi_f_RSA_public_encrypt, METH_VARARGS},
  {"RSA_size", _cffi_f_RSA_size, METH_O},
  {"RSA_verify_PKCS1_PSS", _cffi_f_RSA_verify_PKCS1_PSS, METH_VARARGS},
  {"SSL_CIPHER_get_bits", _cffi_f_SSL_CIPHER_get_bits, METH_VARARGS},
  {"SSL_CIPHER_get_name", _cffi_f_SSL_CIPHER_get_name, METH_O},
  {"SSL_CIPHER_get_version", _cffi_f_SSL_CIPHER_get_version, METH_O},
  {"SSL_CTX_add_client_CA", _cffi_f_SSL_CTX_add_client_CA, METH_VARARGS},
  {"SSL_CTX_add_extra_chain_cert", _cffi_f_SSL_CTX_add_extra_chain_cert, METH_VARARGS},
  {"SSL_CTX_free", _cffi_f_SSL_CTX_free, METH_O},
  {"SSL_CTX_get_cert_store", _cffi_f_SSL_CTX_get_cert_store, METH_O},
  {"SSL_CTX_get_info_callback", _cffi_f_SSL_CTX_get_info_callback, METH_O},
  {"SSL_CTX_get_mode", _cffi_f_SSL_CTX_get_mode, METH_O},
  {"SSL_CTX_get_options", _cffi_f_SSL_CTX_get_options, METH_O},
  {"SSL_CTX_get_session_cache_mode", _cffi_f_SSL_CTX_get_session_cache_mode, METH_O},
  {"SSL_CTX_get_timeout", _cffi_f_SSL_CTX_get_timeout, METH_O},
  {"SSL_CTX_get_verify_callback", _cffi_f_SSL_CTX_get_verify_callback, METH_O},
  {"SSL_CTX_get_verify_depth", _cffi_f_SSL_CTX_get_verify_depth, METH_O},
  {"SSL_CTX_get_verify_mode", _cffi_f_SSL_CTX_get_verify_mode, METH_O},
  {"SSL_CTX_load_verify_locations", _cffi_f_SSL_CTX_load_verify_locations, METH_VARARGS},
  {"SSL_CTX_new", _cffi_f_SSL_CTX_new, METH_O},
  {"SSL_CTX_set_cert_store", _cffi_f_SSL_CTX_set_cert_store, METH_VARARGS},
  {"SSL_CTX_set_cipher_list", _cffi_f_SSL_CTX_set_cipher_list, METH_VARARGS},
  {"SSL_CTX_set_client_CA_list", _cffi_f_SSL_CTX_set_client_CA_list, METH_VARARGS},
  {"SSL_CTX_set_default_passwd_cb", _cffi_f_SSL_CTX_set_default_passwd_cb, METH_VARARGS},
  {"SSL_CTX_set_default_passwd_cb_userdata", _cffi_f_SSL_CTX_set_default_passwd_cb_userdata, METH_VARARGS},
  {"SSL_CTX_set_default_verify_paths", _cffi_f_SSL_CTX_set_default_verify_paths, METH_O},
  {"SSL_CTX_set_info_callback", _cffi_f_SSL_CTX_set_info_callback, METH_VARARGS},
  {"SSL_CTX_set_mode", _cffi_f_SSL_CTX_set_mode, METH_VARARGS},
  {"SSL_CTX_set_options", _cffi_f_SSL_CTX_set_options, METH_VARARGS},
  {"SSL_CTX_set_session_cache_mode", _cffi_f_SSL_CTX_set_session_cache_mode, METH_VARARGS},
  {"SSL_CTX_set_timeout", _cffi_f_SSL_CTX_set_timeout, METH_VARARGS},
  {"SSL_CTX_set_tlsext_servername_callback", _cffi_f_SSL_CTX_set_tlsext_servername_callback, METH_VARARGS},
  {"SSL_CTX_set_tmp_dh", _cffi_f_SSL_CTX_set_tmp_dh, METH_VARARGS},
  {"SSL_CTX_set_tmp_ecdh", _cffi_f_SSL_CTX_set_tmp_ecdh, METH_VARARGS},
  {"SSL_CTX_set_verify", _cffi_f_SSL_CTX_set_verify, METH_VARARGS},
  {"SSL_CTX_set_verify_depth", _cffi_f_SSL_CTX_set_verify_depth, METH_VARARGS},
  {"SSL_CTX_use_PrivateKey", _cffi_f_SSL_CTX_use_PrivateKey, METH_VARARGS},
  {"SSL_CTX_use_PrivateKey_file", _cffi_f_SSL_CTX_use_PrivateKey_file, METH_VARARGS},
  {"SSL_CTX_use_certificate", _cffi_f_SSL_CTX_use_certificate, METH_VARARGS},
  {"SSL_CTX_use_certificate_chain_file", _cffi_f_SSL_CTX_use_certificate_chain_file, METH_VARARGS},
  {"SSL_CTX_use_certificate_file", _cffi_f_SSL_CTX_use_certificate_file, METH_VARARGS},
  {"SSL_SESSION_free", _cffi_f_SSL_SESSION_free, METH_O},
  {"SSL_do_handshake", _cffi_f_SSL_do_handshake, METH_O},
  {"SSL_free", _cffi_f_SSL_free, METH_O},
  {"SSL_get1_session", _cffi_f_SSL_get1_session, METH_O},
  {"SSL_get_cipher_list", _cffi_f_SSL_get_cipher_list, METH_VARARGS},
  {"SSL_get_client_CA_list", _cffi_f_SSL_get_client_CA_list, METH_O},
  {"SSL_get_current_cipher", _cffi_f_SSL_get_current_cipher, METH_O},
  {"SSL_get_error", _cffi_f_SSL_get_error, METH_VARARGS},
  {"SSL_get_info_callback", _cffi_f_SSL_get_info_callback, METH_O},
  {"SSL_get_mode", _cffi_f_SSL_get_mode, METH_O},
  {"SSL_get_options", _cffi_f_SSL_get_options, METH_O},
  {"SSL_get_peer_cert_chain", _cffi_f_SSL_get_peer_cert_chain, METH_O},
  {"SSL_get_peer_certificate", _cffi_f_SSL_get_peer_certificate, METH_O},
  {"SSL_get_servername", _cffi_f_SSL_get_servername, METH_VARARGS},
  {"SSL_get_shutdown", _cffi_f_SSL_get_shutdown, METH_O},
  {"SSL_get_verify_callback", _cffi_f_SSL_get_verify_callback, METH_O},
  {"SSL_get_verify_depth", _cffi_f_SSL_get_verify_depth, METH_O},
  {"SSL_get_verify_mode", _cffi_f_SSL_get_verify_mode, METH_O},
  {"SSL_library_init", _cffi_f_SSL_library_init, METH_NOARGS},
  {"SSL_load_error_strings", _cffi_f_SSL_load_error_strings, METH_NOARGS},
  {"SSL_new", _cffi_f_SSL_new, METH_O},
  {"SSL_pending", _cffi_f_SSL_pending, METH_O},
  {"SSL_read", _cffi_f_SSL_read, METH_VARARGS},
  {"SSL_set_SSL_CTX", _cffi_f_SSL_set_SSL_CTX, METH_VARARGS},
  {"SSL_set_accept_state", _cffi_f_SSL_set_accept_state, METH_O},
  {"SSL_set_bio", _cffi_f_SSL_set_bio, METH_VARARGS},
  {"SSL_set_connect_state", _cffi_f_SSL_set_connect_state, METH_O},
  {"SSL_set_fd", _cffi_f_SSL_set_fd, METH_VARARGS},
  {"SSL_set_info_callback", _cffi_f_SSL_set_info_callback, METH_VARARGS},
  {"SSL_set_mode", _cffi_f_SSL_set_mode, METH_VARARGS},
  {"SSL_set_options", _cffi_f_SSL_set_options, METH_VARARGS},
  {"SSL_set_session", _cffi_f_SSL_set_session, METH_VARARGS},
  {"SSL_set_shutdown", _cffi_f_SSL_set_shutdown, METH_VARARGS},
  {"SSL_set_tlsext_host_name", _cffi_f_SSL_set_tlsext_host_name, METH_VARARGS},
  {"SSL_set_verify_depth", _cffi_f_SSL_set_verify_depth, METH_VARARGS},
  {"SSL_shutdown", _cffi_f_SSL_shutdown, METH_O},
  {"SSL_total_renegotiations", _cffi_f_SSL_total_renegotiations, METH_O},
  {"SSL_want_read", _cffi_f_SSL_want_read, METH_O},
  {"SSL_want_write", _cffi_f_SSL_want_write, METH_O},
  {"SSL_write", _cffi_f_SSL_write, METH_VARARGS},
  {"SSLeay", _cffi_f_SSLeay, METH_NOARGS},
  {"SSLeay_version", _cffi_f_SSLeay_version, METH_O},
  {"SSLv23_client_method", _cffi_f_SSLv23_client_method, METH_NOARGS},
  {"SSLv23_method", _cffi_f_SSLv23_method, METH_NOARGS},
  {"SSLv23_server_method", _cffi_f_SSLv23_server_method, METH_NOARGS},
  {"SSLv2_client_method", _cffi_f_SSLv2_client_method, METH_NOARGS},
  {"SSLv2_method", _cffi_f_SSLv2_method, METH_NOARGS},
  {"SSLv2_server_method", _cffi_f_SSLv2_server_method, METH_NOARGS},
  {"SSLv3_client_method", _cffi_f_SSLv3_client_method, METH_NOARGS},
  {"SSLv3_method", _cffi_f_SSLv3_method, METH_NOARGS},
  {"SSLv3_server_method", _cffi_f_SSLv3_server_method, METH_NOARGS},
  {"TLSv1_1_client_method", _cffi_f_TLSv1_1_client_method, METH_NOARGS},
  {"TLSv1_1_method", _cffi_f_TLSv1_1_method, METH_NOARGS},
  {"TLSv1_1_server_method", _cffi_f_TLSv1_1_server_method, METH_NOARGS},
  {"TLSv1_2_client_method", _cffi_f_TLSv1_2_client_method, METH_NOARGS},
  {"TLSv1_2_method", _cffi_f_TLSv1_2_method, METH_NOARGS},
  {"TLSv1_2_server_method", _cffi_f_TLSv1_2_server_method, METH_NOARGS},
  {"TLSv1_client_method", _cffi_f_TLSv1_client_method, METH_NOARGS},
  {"TLSv1_method", _cffi_f_TLSv1_method, METH_NOARGS},
  {"TLSv1_server_method", _cffi_f_TLSv1_server_method, METH_NOARGS},
  {"X509V3_EXT_get", _cffi_f_X509V3_EXT_get, METH_O},
  {"X509V3_EXT_get_nid", _cffi_f_X509V3_EXT_get_nid, METH_O},
  {"X509V3_EXT_nconf", _cffi_f_X509V3_EXT_nconf, METH_VARARGS},
  {"X509V3_EXT_print", _cffi_f_X509V3_EXT_print, METH_VARARGS},
  {"X509V3_set_ctx", _cffi_f_X509V3_set_ctx, METH_VARARGS},
  {"X509V3_set_ctx_nodb", _cffi_f_X509V3_set_ctx_nodb, METH_O},
  {"X509_CRL_add0_revoked", _cffi_f_X509_CRL_add0_revoked, METH_VARARGS},
  {"X509_CRL_free", _cffi_f_X509_CRL_free, METH_O},
  {"X509_CRL_new", _cffi_f_X509_CRL_new, METH_NOARGS},
  {"X509_CRL_print", _cffi_f_X509_CRL_print, METH_VARARGS},
  {"X509_CRL_set_issuer_name", _cffi_f_X509_CRL_set_issuer_name, METH_VARARGS},
  {"X509_CRL_set_lastUpdate", _cffi_f_X509_CRL_set_lastUpdate, METH_VARARGS},
  {"X509_CRL_set_nextUpdate", _cffi_f_X509_CRL_set_nextUpdate, METH_VARARGS},
  {"X509_CRL_sign", _cffi_f_X509_CRL_sign, METH_VARARGS},
  {"X509_EXTENSION_dup", _cffi_f_X509_EXTENSION_dup, METH_O},
  {"X509_EXTENSION_free", _cffi_f_X509_EXTENSION_free, METH_O},
  {"X509_EXTENSION_get_critical", _cffi_f_X509_EXTENSION_get_critical, METH_O},
  {"X509_EXTENSION_get_data", _cffi_f_X509_EXTENSION_get_data, METH_O},
  {"X509_EXTENSION_get_object", _cffi_f_X509_EXTENSION_get_object, METH_O},
  {"X509_NAME_ENTRY_free", _cffi_f_X509_NAME_ENTRY_free, METH_O},
  {"X509_NAME_ENTRY_get_data", _cffi_f_X509_NAME_ENTRY_get_data, METH_O},
  {"X509_NAME_ENTRY_get_object", _cffi_f_X509_NAME_ENTRY_get_object, METH_O},
  {"X509_NAME_add_entry_by_NID", _cffi_f_X509_NAME_add_entry_by_NID, METH_VARARGS},
  {"X509_NAME_cmp", _cffi_f_X509_NAME_cmp, METH_VARARGS},
  {"X509_NAME_delete_entry", _cffi_f_X509_NAME_delete_entry, METH_VARARGS},
  {"X509_NAME_dup", _cffi_f_X509_NAME_dup, METH_O},
  {"X509_NAME_entry_count", _cffi_f_X509_NAME_entry_count, METH_O},
  {"X509_NAME_free", _cffi_f_X509_NAME_free, METH_O},
  {"X509_NAME_get_entry", _cffi_f_X509_NAME_get_entry, METH_VARARGS},
  {"X509_NAME_get_index_by_NID", _cffi_f_X509_NAME_get_index_by_NID, METH_VARARGS},
  {"X509_NAME_hash", _cffi_f_X509_NAME_hash, METH_O},
  {"X509_NAME_oneline", _cffi_f_X509_NAME_oneline, METH_VARARGS},
  {"X509_REQ_add_extensions", _cffi_f_X509_REQ_add_extensions, METH_VARARGS},
  {"X509_REQ_free", _cffi_f_X509_REQ_free, METH_O},
  {"X509_REQ_get_extensions", _cffi_f_X509_REQ_get_extensions, METH_O},
  {"X509_REQ_get_pubkey", _cffi_f_X509_REQ_get_pubkey, METH_O},
  {"X509_REQ_get_subject_name", _cffi_f_X509_REQ_get_subject_name, METH_O},
  {"X509_REQ_get_version", _cffi_f_X509_REQ_get_version, METH_O},
  {"X509_REQ_new", _cffi_f_X509_REQ_new, METH_NOARGS},
  {"X509_REQ_print_ex", _cffi_f_X509_REQ_print_ex, METH_VARARGS},
  {"X509_REQ_set_pubkey", _cffi_f_X509_REQ_set_pubkey, METH_VARARGS},
  {"X509_REQ_set_version", _cffi_f_X509_REQ_set_version, METH_VARARGS},
  {"X509_REQ_sign", _cffi_f_X509_REQ_sign, METH_VARARGS},
  {"X509_REQ_verify", _cffi_f_X509_REQ_verify, METH_VARARGS},
  {"X509_REVOKED_add1_ext_i2d", _cffi_f_X509_REVOKED_add1_ext_i2d, METH_VARARGS},
  {"X509_REVOKED_free", _cffi_f_X509_REVOKED_free, METH_O},
  {"X509_REVOKED_new", _cffi_f_X509_REVOKED_new, METH_NOARGS},
  {"X509_REVOKED_set_serialNumber", _cffi_f_X509_REVOKED_set_serialNumber, METH_VARARGS},
  {"X509_STORE_CTX_get_current_cert", _cffi_f_X509_STORE_CTX_get_current_cert, METH_O},
  {"X509_STORE_CTX_get_error", _cffi_f_X509_STORE_CTX_get_error, METH_O},
  {"X509_STORE_CTX_get_error_depth", _cffi_f_X509_STORE_CTX_get_error_depth, METH_O},
  {"X509_STORE_CTX_set_error", _cffi_f_X509_STORE_CTX_set_error, METH_VARARGS},
  {"X509_STORE_add_cert", _cffi_f_X509_STORE_add_cert, METH_VARARGS},
  {"X509_STORE_free", _cffi_f_X509_STORE_free, METH_O},
  {"X509_STORE_new", _cffi_f_X509_STORE_new, METH_NOARGS},
  {"X509_add_ext", _cffi_f_X509_add_ext, METH_VARARGS},
  {"X509_alias_get0", _cffi_f_X509_alias_get0, METH_VARARGS},
  {"X509_digest", _cffi_f_X509_digest, METH_VARARGS},
  {"X509_dup", _cffi_f_X509_dup, METH_O},
  {"X509_free", _cffi_f_X509_free, METH_O},
  {"X509_get_default_cert_area", _cffi_f_X509_get_default_cert_area, METH_NOARGS},
  {"X509_get_default_cert_dir", _cffi_f_X509_get_default_cert_dir, METH_NOARGS},
  {"X509_get_default_cert_dir_env", _cffi_f_X509_get_default_cert_dir_env, METH_NOARGS},
  {"X509_get_default_cert_file", _cffi_f_X509_get_default_cert_file, METH_NOARGS},
  {"X509_get_default_cert_file_env", _cffi_f_X509_get_default_cert_file_env, METH_NOARGS},
  {"X509_get_default_private_dir", _cffi_f_X509_get_default_private_dir, METH_NOARGS},
  {"X509_get_ext", _cffi_f_X509_get_ext, METH_VARARGS},
  {"X509_get_ext_count", _cffi_f_X509_get_ext_count, METH_O},
  {"X509_get_issuer_name", _cffi_f_X509_get_issuer_name, METH_O},
  {"X509_get_notAfter", _cffi_f_X509_get_notAfter, METH_O},
  {"X509_get_notBefore", _cffi_f_X509_get_notBefore, METH_O},
  {"X509_get_pubkey", _cffi_f_X509_get_pubkey, METH_O},
  {"X509_get_serialNumber", _cffi_f_X509_get_serialNumber, METH_O},
  {"X509_get_subject_name", _cffi_f_X509_get_subject_name, METH_O},
  {"X509_get_version", _cffi_f_X509_get_version, METH_O},
  {"X509_gmtime_adj", _cffi_f_X509_gmtime_adj, METH_VARARGS},
  {"X509_new", _cffi_f_X509_new, METH_NOARGS},
  {"X509_print_ex", _cffi_f_X509_print_ex, METH_VARARGS},
  {"X509_set_issuer_name", _cffi_f_X509_set_issuer_name, METH_VARARGS},
  {"X509_set_pubkey", _cffi_f_X509_set_pubkey, METH_VARARGS},
  {"X509_set_serialNumber", _cffi_f_X509_set_serialNumber, METH_VARARGS},
  {"X509_set_subject_name", _cffi_f_X509_set_subject_name, METH_VARARGS},
  {"X509_set_version", _cffi_f_X509_set_version, METH_VARARGS},
  {"X509_sign", _cffi_f_X509_sign, METH_VARARGS},
  {"X509_subject_name_hash", _cffi_f_X509_subject_name_hash, METH_O},
  {"X509_verify_cert", _cffi_f_X509_verify_cert, METH_O},
  {"X509_verify_cert_error_string", _cffi_f_X509_verify_cert_error_string, METH_O},
  {"d2i_ASN1_OBJECT", _cffi_f_d2i_ASN1_OBJECT, METH_VARARGS},
  {"d2i_PKCS12_bio", _cffi_f_d2i_PKCS12_bio, METH_VARARGS},
  {"d2i_PKCS8PrivateKey_bio", _cffi_f_d2i_PKCS8PrivateKey_bio, METH_VARARGS},
  {"d2i_PrivateKey_bio", _cffi_f_d2i_PrivateKey_bio, METH_VARARGS},
  {"d2i_X509_CRL_bio", _cffi_f_d2i_X509_CRL_bio, METH_VARARGS},
  {"d2i_X509_REQ_bio", _cffi_f_d2i_X509_REQ_bio, METH_VARARGS},
  {"d2i_X509_bio", _cffi_f_d2i_X509_bio, METH_VARARGS},
  {"i2a_ASN1_INTEGER", _cffi_f_i2a_ASN1_INTEGER, METH_VARARGS},
  {"i2d_ASN1_OBJECT", _cffi_f_i2d_ASN1_OBJECT, METH_VARARGS},
  {"i2d_PKCS12_bio", _cffi_f_i2d_PKCS12_bio, METH_VARARGS},
  {"i2d_PKCS8PrivateKey_bio", _cffi_f_i2d_PKCS8PrivateKey_bio, METH_VARARGS},
  {"i2d_PrivateKey_bio", _cffi_f_i2d_PrivateKey_bio, METH_VARARGS},
  {"i2d_X509_CRL_bio", _cffi_f_i2d_X509_CRL_bio, METH_VARARGS},
  {"i2d_X509_NAME", _cffi_f_i2d_X509_NAME, METH_VARARGS},
  {"i2d_X509_REQ_bio", _cffi_f_i2d_X509_REQ_bio, METH_VARARGS},
  {"i2d_X509_bio", _cffi_f_i2d_X509_bio, METH_VARARGS},
  {"sk_GENERAL_NAME_num", _cffi_f_sk_GENERAL_NAME_num, METH_O},
  {"sk_GENERAL_NAME_push", _cffi_f_sk_GENERAL_NAME_push, METH_VARARGS},
  {"sk_GENERAL_NAME_value", _cffi_f_sk_GENERAL_NAME_value, METH_VARARGS},
  {"sk_X509_EXTENSION_delete", _cffi_f_sk_X509_EXTENSION_delete, METH_VARARGS},
  {"sk_X509_EXTENSION_free", _cffi_f_sk_X509_EXTENSION_free, METH_O},
  {"sk_X509_EXTENSION_new_null", _cffi_f_sk_X509_EXTENSION_new_null, METH_NOARGS},
  {"sk_X509_EXTENSION_num", _cffi_f_sk_X509_EXTENSION_num, METH_O},
  {"sk_X509_EXTENSION_push", _cffi_f_sk_X509_EXTENSION_push, METH_VARARGS},
  {"sk_X509_EXTENSION_value", _cffi_f_sk_X509_EXTENSION_value, METH_VARARGS},
  {"sk_X509_NAME_free", _cffi_f_sk_X509_NAME_free, METH_O},
  {"sk_X509_NAME_new_null", _cffi_f_sk_X509_NAME_new_null, METH_NOARGS},
  {"sk_X509_NAME_num", _cffi_f_sk_X509_NAME_num, METH_O},
  {"sk_X509_NAME_push", _cffi_f_sk_X509_NAME_push, METH_VARARGS},
  {"sk_X509_NAME_value", _cffi_f_sk_X509_NAME_value, METH_VARARGS},
  {"sk_X509_REVOKED_num", _cffi_f_sk_X509_REVOKED_num, METH_O},
  {"sk_X509_REVOKED_value", _cffi_f_sk_X509_REVOKED_value, METH_VARARGS},
  {"sk_X509_free", _cffi_f_sk_X509_free, METH_O},
  {"sk_X509_new_null", _cffi_f_sk_X509_new_null, METH_NOARGS},
  {"sk_X509_num", _cffi_f_sk_X509_num, METH_O},
  {"sk_X509_push", _cffi_f_sk_X509_push, METH_VARARGS},
  {"sk_X509_value", _cffi_f_sk_X509_value, METH_VARARGS},
  {"_cffi_layout_struct_ERR_string_data_st", _cffi_layout_struct_ERR_string_data_st, METH_NOARGS},
  {"_cffi_layout_struct_aes_key_st", _cffi_layout_struct_aes_key_st, METH_NOARGS},
  {"_cffi_layout_struct_asn1_string_st", _cffi_layout_struct_asn1_string_st, METH_NOARGS},
  {"_cffi_layout_struct_bio_method_st", _cffi_layout_struct_bio_method_st, METH_NOARGS},
  {"_cffi_layout_struct_bio_st", _cffi_layout_struct_bio_st, METH_NOARGS},
  {"_cffi_layout_struct_dh_st", _cffi_layout_struct_dh_st, METH_NOARGS},
  {"_cffi_layout_struct_dsa_st", _cffi_layout_struct_dsa_st, METH_NOARGS},
  {"_cffi_layout_struct_env_md_ctx_st", _cffi_layout_struct_env_md_ctx_st, METH_NOARGS},
  {"_cffi_layout_struct_evp_pkey_st", _cffi_layout_struct_evp_pkey_st, METH_NOARGS},
  {"_cffi_layout_struct_rsa_st", _cffi_layout_struct_rsa_st, METH_NOARGS},
  {"_cffi_setup", _cffi_setup, METH_VARARGS},
  {NULL, NULL}    /* Sentinel */
};

PyMODINIT_FUNC
init_cffi__x657b2f6exf0ae7e21(void)
{
  PyObject *lib;
  lib = Py_InitModule("_cffi__x657b2f6exf0ae7e21", _cffi_methods);
  if (lib == NULL || 0 < 0)
    return;
  _cffi_init();
  return;
}
