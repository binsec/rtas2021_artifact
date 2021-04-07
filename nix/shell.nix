let
  pkgs = import ./pkgs.nix {};
  ocamlCallPackage = pkgs.ocaml-ng.ocamlPackages_4_07.callPackage;
  cudd = ocamlCallPackage ./cudd.nix {};
  libase = ocamlCallPackage ./libase.nix { inherit cudd; };
  binsec = ocamlCallPackage ./binsec.nix { inherit cudd libase; };
in
pkgs.mkShell {
  buildInputs = [ binsec ]
  ++ (with pkgs; [ time unixtools.whereis nix ]);
  NIX_ARTIFACT_ENV = "true";
  ARTIFACT_ROOT = toString ./..;
}
