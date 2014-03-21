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

from cryptography.hazmat.bindings.commoncrypto.binding import Binding


@pytest.mark.skipif(not Binding.is_available(),
                    reason="CommonCrypto not available")
class TestCommonCrypto(object):
    def test_binding_loads(self):
        binding = Binding()
        assert binding
        assert binding.lib
        assert binding.ffi

    def test_binding_returns_same_lib(self):
        binding = Binding()
        binding2 = Binding()
        assert binding.lib == binding2.lib
        assert binding.ffi == binding2.ffi
