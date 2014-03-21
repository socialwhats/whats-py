# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
# implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import pytest

from cryptography import utils
from cryptography.exceptions import UnsupportedAlgorithm, InternalError
from cryptography.hazmat.backends.openssl.backend import backend, Backend
from cryptography.hazmat.primitives import interfaces, hashes
from cryptography.hazmat.primitives.ciphers import Cipher
from cryptography.hazmat.primitives.ciphers.algorithms import AES
from cryptography.hazmat.primitives.ciphers.modes import CBC


@utils.register_interface(interfaces.Mode)
class DummyMode(object):
    name = "dummy-mode"

    def validate_for_algorithm(self, algorithm):
        pass


@utils.register_interface(interfaces.CipherAlgorithm)
class DummyCipher(object):
    name = "dummy-cipher"


class TestOpenSSL(object):
    def test_backend_exists(self):
        assert backend

    def test_openssl_version_text(self):
        """
        This test checks the value of OPENSSL_VERSION_TEXT.

        Unfortunately, this define does not appear to have a
        formal content definition, so for now we'll test to see
        if it starts with OpenSSL as that appears to be true
        for every OpenSSL.
        """
        assert backend.openssl_version_text().startswith("OpenSSL")

    def test_supports_cipher(self):
        assert backend.cipher_supported(None, None) is False

    def test_register_duplicate_cipher_adapter(self):
        with pytest.raises(ValueError):
            backend.register_cipher_adapter(AES, CBC, None)

    @pytest.mark.parametrize("mode", [DummyMode(), None])
    def test_nonexistent_cipher(self, mode):
        b = Backend()
        b.register_cipher_adapter(
            DummyCipher,
            type(mode),
            lambda backend, cipher, mode: backend._ffi.NULL
        )
        cipher = Cipher(
            DummyCipher(), mode, backend=b,
        )
        with pytest.raises(UnsupportedAlgorithm):
            cipher.encryptor()

    def test_handle_unknown_error(self):
        with pytest.raises(InternalError):
            backend._handle_error_code(0)

        backend._lib.ERR_put_error(backend._lib.ERR_LIB_EVP, 0, 0,
                                   b"test_openssl.py", -1)
        with pytest.raises(InternalError):
            backend._handle_error(None)

        backend._lib.ERR_put_error(
            backend._lib.ERR_LIB_EVP,
            backend._lib.EVP_F_EVP_ENCRYPTFINAL_EX,
            0,
            b"test_openssl.py",
            -1
        )
        with pytest.raises(InternalError):
            backend._handle_error(None)

        backend._lib.ERR_put_error(
            backend._lib.ERR_LIB_EVP,
            backend._lib.EVP_F_EVP_DECRYPTFINAL_EX,
            0,
            b"test_openssl.py",
            -1
        )
        with pytest.raises(InternalError):
            backend._handle_error(None)

    def test_handle_multiple_errors(self):
        for i in range(10):
            backend._lib.ERR_put_error(backend._lib.ERR_LIB_EVP, 0, 0,
                                       b"test_openssl.py", -1)

        assert backend._lib.ERR_peek_error() != 0

        with pytest.raises(InternalError):
            backend._handle_error(None)

        assert backend._lib.ERR_peek_error() == 0

    def test_openssl_error_string(self):
        backend._lib.ERR_put_error(
            backend._lib.ERR_LIB_EVP,
            backend._lib.EVP_F_EVP_DECRYPTFINAL_EX,
            0,
            b"test_openssl.py",
            -1
        )

        with pytest.raises(InternalError) as exc:
            backend._handle_error(None)

        assert (
            "digital envelope routines:"
            "EVP_DecryptFinal_ex:digital envelope routines" in str(exc)
        )

    def test_ssl_ciphers_registered(self):
        meth = backend._lib.TLSv1_method()
        ctx = backend._lib.SSL_CTX_new(meth)
        assert ctx != backend._ffi.NULL
        backend._lib.SSL_CTX_free(ctx)

    def test_evp_ciphers_registered(self):
        cipher = backend._lib.EVP_get_cipherbyname(b"aes-256-cbc")
        assert cipher != backend._ffi.NULL

    def test_error_strings_loaded(self):
        # returns a value in a static buffer
        err = backend._lib.ERR_error_string(101183626, backend._ffi.NULL)
        assert backend._ffi.string(err) == (
            b"error:0607F08A:digital envelope routines:EVP_EncryptFinal_ex:"
            b"data not multiple of block length"
        )

    def test_derive_pbkdf2_raises_unsupported_on_old_openssl(self):
        if backend.pbkdf2_hmac_supported(hashes.SHA256()):
            pytest.skip("Requires an older OpenSSL")
        with pytest.raises(UnsupportedAlgorithm):
            backend.derive_pbkdf2_hmac(hashes.SHA256(), 10, b"", 1000, b"")

    # This test is not in the next class because to check if it's really
    # default we don't want to run the setup_method before it
    def test_osrandom_engine_is_default(self):
        e = backend._lib.ENGINE_get_default_RAND()
        name = backend._lib.ENGINE_get_name(e)
        assert name == backend._lib.Cryptography_osrandom_engine_name
        res = backend._lib.ENGINE_free(e)
        assert res == 1


class TestOpenSSLRandomEngine(object):
    def teardown_method(self, method):
        # we need to reset state to being default. backend is a shared global
        # for all these tests.
        backend.activate_osrandom_engine()
        current_default = backend._lib.ENGINE_get_default_RAND()
        name = backend._lib.ENGINE_get_name(current_default)
        assert name == backend._lib.Cryptography_osrandom_engine_name

    def test_osrandom_sanity_check(self):
        # This test serves as a check against catastrophic failure.
        buf = backend._ffi.new("char[]", 500)
        res = backend._lib.RAND_bytes(buf, 500)
        assert res == 1
        assert backend._ffi.buffer(buf)[:] != "\x00" * 500

    def test_activate_osrandom_already_default(self):
        e = backend._lib.ENGINE_get_default_RAND()
        name = backend._lib.ENGINE_get_name(e)
        assert name == backend._lib.Cryptography_osrandom_engine_name
        res = backend._lib.ENGINE_free(e)
        assert res == 1
        backend.activate_osrandom_engine()
        e = backend._lib.ENGINE_get_default_RAND()
        name = backend._lib.ENGINE_get_name(e)
        assert name == backend._lib.Cryptography_osrandom_engine_name
        res = backend._lib.ENGINE_free(e)
        assert res == 1

    def test_activate_osrandom_no_default(self):
        backend.activate_builtin_random()
        e = backend._lib.ENGINE_get_default_RAND()
        assert e == backend._ffi.NULL
        backend.activate_osrandom_engine()
        e = backend._lib.ENGINE_get_default_RAND()
        name = backend._lib.ENGINE_get_name(e)
        assert name == backend._lib.Cryptography_osrandom_engine_name
        res = backend._lib.ENGINE_free(e)
        assert res == 1

    def test_activate_builtin_random(self):
        e = backend._lib.ENGINE_get_default_RAND()
        assert e != backend._ffi.NULL
        name = backend._lib.ENGINE_get_name(e)
        assert name == backend._lib.Cryptography_osrandom_engine_name
        res = backend._lib.ENGINE_free(e)
        assert res == 1
        backend.activate_builtin_random()
        e = backend._lib.ENGINE_get_default_RAND()
        assert e == backend._ffi.NULL

    def test_activate_builtin_random_already_active(self):
        backend.activate_builtin_random()
        e = backend._lib.ENGINE_get_default_RAND()
        assert e == backend._ffi.NULL
        backend.activate_builtin_random()
        e = backend._lib.ENGINE_get_default_RAND()
        assert e == backend._ffi.NULL
