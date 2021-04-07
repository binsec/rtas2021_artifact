{ clang }:
  if clang then
    let pkgs = import ../nix/pkgs.nix {}; in pkgs.pkgsi686Linux.clangStdenv.mkDerivation {
      name = "clang-nix-shell";
      hardeningDisable = [ "all" ];
      buildInputs = with pkgs; [ qemu ];
    }
  else
    let pkgs = import ../nix/pkgs.nix {}; in pkgs.pkgsi686Linux.mkShell {
      name = "gcc-nix-shell";
      hardeningDisable = [ "all" ];
      buildInputs = with pkgs; [ gcc qemu ];
    }
