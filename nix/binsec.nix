{ stdenv
, nix-gitignore
, zlib
, ncurses
, zeromq
, gmp
, ocaml
, findlib
, ocamlgraph
, piqi
, piqi-ocaml
, zarith
, zmq
, menhir
, llvm
, qtest
, ounit
, benchmark
, qcheck
, seq
, autoreconfHook
, protobuf
, libase
, cudd
, lib
}:
stdenv.mkDerivation {
  pname = "binsec-codex";
  version = "0.4";
  src = nix-gitignore.gitignoreSourcePure
    [
      ./../binsec/.gitignore
      ''*.nix
        result''
    ] ./../binsec;

  strictDeps = true;

  buildInputs = [
    zlib
    ncurses
    zeromq
    gmp # for zarith
    ocamlgraph
    piqi
    piqi-ocaml
    zarith
    zmq
    menhir
    llvm
    qtest
    ounit
    benchmark
    libase
    cudd
  ];
  nativeBuildInputs = [
    autoreconfHook
    protobuf
    ocaml
    findlib
    menhir
    piqi
    piqi-ocaml
  ];

  enableParallelBuilding = true;

  makeFlags = [ "VERBOSEMAKE=1" "WARN_ERROR=no" ];
}
