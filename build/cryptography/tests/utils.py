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

import collections
import os

import six
import pytest


HashVector = collections.namedtuple("HashVector", ["message", "digest"])
KeyedHashVector = collections.namedtuple(
    "KeyedHashVector", ["message", "digest", "key"]
)


def select_backends(names, backend_list):
    if names is None:
        return backend_list
    split_names = [x.strip() for x in names.split(',')]
    # this must be duplicated and then removed to preserve the metadata
    # pytest associates. Appending backends to a new list doesn't seem to work
    selected_backends = []
    for backend in backend_list:
        if backend.name in split_names:
            selected_backends.append(backend)

    if len(selected_backends) > 0:
        return selected_backends
    else:
        raise ValueError(
            "No backend selected. Tried to select: {0}".format(split_names)
        )


def check_for_iface(name, iface, item):
    if name in item.keywords and "backend" in item.funcargs:
        if not isinstance(item.funcargs["backend"], iface):
            pytest.skip("{0} backend does not support {1}".format(
                item.funcargs["backend"], name
            ))


def check_backend_support(item):
    supported = item.keywords.get("supported")
    if supported and "backend" in item.funcargs:
        if not supported.kwargs["only_if"](item.funcargs["backend"]):
            pytest.skip("{0} ({1})".format(
                supported.kwargs["skip_message"], item.funcargs["backend"]
            ))
    elif supported:
        raise ValueError("This mark is only available on methods that take a "
                         "backend")


def load_vectors_from_file(filename, loader):
    base = os.path.join(
        os.path.dirname(__file__), "hazmat", "primitives", "vectors",
    )
    with open(os.path.join(base, filename), "r") as vector_file:
        return loader(vector_file)


def load_nist_vectors(vector_data):
    test_data = None
    data = []

    for line in vector_data:
        line = line.strip()

        # Blank lines, comments, and section headers are ignored
        if not line or line.startswith("#") or (line.startswith("[")
                                                and line.endswith("]")):
            continue

        if line.strip() == "FAIL":
            test_data["fail"] = True
            continue

        # Build our data using a simple Key = Value format
        name, value = [c.strip() for c in line.split("=")]

        # Some tests (PBKDF2) contain \0, which should be interpreted as a
        # null character rather than literal.
        value = value.replace("\\0", "\0")

        # COUNT is a special token that indicates a new block of data
        if name.upper() == "COUNT":
            test_data = {}
            data.append(test_data)
            continue
        # For all other tokens we simply want the name, value stored in
        # the dictionary
        else:
            test_data[name.lower()] = value.encode("ascii")

    return data


def load_cryptrec_vectors(vector_data):
    cryptrec_list = []

    for line in vector_data:
        line = line.strip()

        # Blank lines and comments are ignored
        if not line or line.startswith("#"):
            continue

        if line.startswith("K"):
            key = line.split(" : ")[1].replace(" ", "").encode("ascii")
        elif line.startswith("P"):
            pt = line.split(" : ")[1].replace(" ", "").encode("ascii")
        elif line.startswith("C"):
            ct = line.split(" : ")[1].replace(" ", "").encode("ascii")
            # after a C is found the K+P+C tuple is complete
            # there are many P+C pairs for each K
            cryptrec_list.append({
                "key": key,
                "plaintext": pt,
                "ciphertext": ct
            })
        else:
            raise ValueError("Invalid line in file '{}'".format(line))
    return cryptrec_list


def load_hash_vectors(vector_data):
    vectors = []
    key = None
    msg = None
    md = None

    for line in vector_data:
        line = line.strip()

        if not line or line.startswith("#") or line.startswith("["):
            continue

        if line.startswith("Len"):
            length = int(line.split(" = ")[1])
        elif line.startswith("Key"):
            # HMAC vectors contain a key attribute. Hash vectors do not.
            key = line.split(" = ")[1].encode("ascii")
        elif line.startswith("Msg"):
            # In the NIST vectors they have chosen to represent an empty
            # string as hex 00, which is of course not actually an empty
            # string. So we parse the provided length and catch this edge case.
            msg = line.split(" = ")[1].encode("ascii") if length > 0 else b""
        elif line.startswith("MD"):
            md = line.split(" = ")[1]
            # after MD is found the Msg+MD (+ potential key) tuple is complete
            if key is not None:
                vectors.append(KeyedHashVector(msg, md, key))
                key = None
                msg = None
                md = None
            else:
                vectors.append(HashVector(msg, md))
                msg = None
                md = None
        else:
            raise ValueError("Unknown line in hash vector")
    return vectors


def load_pkcs1_vectors(vector_data):
    """
    Loads data out of RSA PKCS #1 vector files.
    """
    private_key_vector = None
    public_key_vector = None
    attr = None
    key = None
    example_vector = None
    examples = []
    vectors = []
    for line in vector_data:
        if (
            line.startswith("# PSS Example") or
            line.startswith("# PKCS#1 v1.5 Signature")
        ):
            if example_vector:
                for key, value in six.iteritems(example_vector):
                    hex_str = "".join(value).replace(" ", "").encode("ascii")
                    example_vector[key] = hex_str
                examples.append(example_vector)

            attr = None
            example_vector = collections.defaultdict(list)

        if line.startswith("# Message to be signed"):
            attr = "message"
            continue
        elif line.startswith("# Salt"):
            attr = "salt"
            continue
        elif line.startswith("# Signature"):
            attr = "signature"
            continue
        elif (
            example_vector and
            line.startswith("# =============================================")
        ):
            for key, value in six.iteritems(example_vector):
                hex_str = "".join(value).replace(" ", "").encode("ascii")
                example_vector[key] = hex_str
            examples.append(example_vector)
            example_vector = None
            attr = None
        elif example_vector and line.startswith("#"):
            continue
        else:
            if attr is not None and example_vector is not None:
                example_vector[attr].append(line.strip())
                continue

        if (
            line.startswith("# Example") or
            line.startswith("# =============================================")
        ):
            if key:
                assert private_key_vector
                assert public_key_vector

                for key, value in six.iteritems(public_key_vector):
                    hex_str = "".join(value).replace(" ", "")
                    public_key_vector[key] = int(hex_str, 16)

                for key, value in six.iteritems(private_key_vector):
                    hex_str = "".join(value).replace(" ", "")
                    private_key_vector[key] = int(hex_str, 16)

                private_key_vector["examples"] = examples
                examples = []

                assert (
                    private_key_vector['public_exponent'] ==
                    public_key_vector['public_exponent']
                )

                assert (
                    private_key_vector['modulus'] ==
                    public_key_vector['modulus']
                )

                vectors.append(
                    (private_key_vector, public_key_vector)
                )

            public_key_vector = collections.defaultdict(list)
            private_key_vector = collections.defaultdict(list)
            key = None
            attr = None

        if private_key_vector is None or public_key_vector is None:
            continue

        if line.startswith("# Private key"):
            key = private_key_vector
        elif line.startswith("# Public key"):
            key = public_key_vector
        elif line.startswith("# Modulus:"):
            attr = "modulus"
        elif line.startswith("# Public exponent:"):
            attr = "public_exponent"
        elif line.startswith("# Exponent:"):
            if key is public_key_vector:
                attr = "public_exponent"
            else:
                assert key is private_key_vector
                attr = "private_exponent"
        elif line.startswith("# Prime 1:"):
            attr = "p"
        elif line.startswith("# Prime 2:"):
            attr = "q"
        elif line.startswith("# Prime exponent 1:"):
            attr = "dmp1"
        elif line.startswith("# Prime exponent 2:"):
            attr = "dmq1"
        elif line.startswith("# Coefficient:"):
            attr = "iqmp"
        elif line.startswith("#"):
            attr = None
        else:
            if key is not None and attr is not None:
                key[attr].append(line.strip())
    return vectors
