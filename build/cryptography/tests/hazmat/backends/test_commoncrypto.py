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
from cryptography.hazmat.bindings.commoncrypto.binding import Binding
from cryptography.hazmat.primitives import interfaces
from cryptography.hazmat.primitives.ciphers.algorithms import AES
from cryptography.hazmat.primitives.ciphers.base import Cipher
from cryptography.hazmat.primitives.ciphers.modes import CBC, GCM


@utils.register_interface(interfaces.CipherAlgorithm)
class DummyCipher(object):
    name = "dummy-cipher"
    block_size = 128


@pytest.mark.skipif(not Binding.is_available(),
                    reason="CommonCrypto not available")
class TestCommonCrypto(object):
    def test_supports_cipher(self):
        from cryptography.hazmat.backends.commoncrypto.backend import backend
        assert backend.cipher_supported(None, None) is False

    def test_register_duplicate_cipher_adapter(self):
        from cryptography.hazmat.backends.commoncrypto.backend import backend
        with pytest.raises(ValueError):
            backend._register_cipher_adapter(
                AES, backend._lib.kCCAlgorithmAES128,
                CBC, backend._lib.kCCModeCBC
            )

    def test_handle_response(self):
        from cryptography.hazmat.backends.commoncrypto.backend import backend

        with pytest.raises(ValueError):
            backend._check_response(backend._lib.kCCAlignmentError)

        with pytest.raises(InternalError):
            backend._check_response(backend._lib.kCCMemoryFailure)

        with pytest.raises(InternalError):
            backend._check_response(backend._lib.kCCDecodeError)

    def test_nonexistent_aead_cipher(self):
        from cryptography.hazmat.backends.commoncrypto.backend import Backend
        b = Backend()
        cipher = Cipher(
            DummyCipher(), GCM(b"fake_iv_here"), backend=b,
        )
        with pytest.raises(UnsupportedAlgorithm):
            cipher.encryptor()
