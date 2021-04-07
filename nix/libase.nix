{ ocaml
, nix-gitignore
, stdenv
, lib
, gmp
, findlib
, framac
, ocamlgraph
, zarith
, cudd
}:
stdenv.mkDerivation {
  pname = "libase";
  version = "1.0.0";
  src = nix-gitignore.gitignoreSourcePure
    [
      ./../libase/.gitignore
      ''*.nix
        result''
    ] ./../libase;
  buildInputs = [
    gmp # for zarith
    ocaml
    findlib
    ocamlgraph
    zarith
    cudd
  ];
}
