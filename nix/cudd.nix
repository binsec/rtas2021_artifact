{ stdenv, fetchzip, ocaml, findlib, camlidl, m4 }:

stdenv.mkDerivation rec {
  pname = "ocaml${ocaml.version}-mlcuddidl";
  version = "3.0.4";
  src = fetchzip {
    url = "http://nberth.space/pool/mlcuddidl/mlcuddidl-${version}.tar.gz";
    sha256 = "05s09cfw6v64nn8hyydlglhwgdzl5v28il60d0m55sd6bxx1g0w3";
  };

  nativeBuildInputs = [ ocaml findlib m4 ];
  buildInputs = [ camlidl ];

  createFindlibDestdir = true;

  meta = {
    description = "OCaml interface to the CUDD BDD library";
    homepage = https://www.inrialpes.fr/pop-art/people/bjeannet/mlxxxidl-forge/mlcuddidl/;
    license = stdenv.lib.licenses.lgpl21;
    inherit (ocaml.meta) platforms;
  };
}

