{ autoconf
, autoconf-archive
, automake
, boost
, binutils
, binutils-unwrapped
, bison
, flex
, libtool
, libxml2
, pkg-config
, stdenv
, yacc
, zlib
}:
let
  unisim_src = ../unisim;
  unisim_version = "dev";
  postPatch = ''
    find . -name "*.sh" -execdir chmod +x '{}' \;
    patchShebangs .
  '';
  systemc = stdenv.mkDerivation rec {
    pname = "systemc";
    version = "2.3.3";
    src = unisim_src + "/extra/systemc/systemc-${version}.tar.gz";
    nativeBuildInputs = [ pkg-config ];
    enableParallelBuilding = true;
    postInstall = "mv $out/lib-linux64 $out/lib";
  };
  genisslib = stdenv.mkDerivation {
    pname = "genisslib";
    version = unisim_version;
    src = unisim_src;
    inherit postPatch;
    nativeBuildInputs = [ autoconf automake libtool flex bison ];
    enableParallelBuilding = true;
    preConfigure = ''
      DISTCOPY='ln -s' package/dist_genisslib.sh genisslib
      cd genisslib
    '';
  };
  # unisim fails to link libstdc++.a if not using pkgsStatic
  unisim = stdenv.mkDerivation
    {
      pname = "unisim";
      version = unisim_version;
      src = unisim_src;
      enableParallelBuilding = true;
      patches = [ ./unisim_static.patch ];
      inherit postPatch;
      nativeBuildInputs = [
        genisslib
        bison
        yacc
        flex
        libtool
        autoconf-archive
        autoconf
        automake
        pkg-config
      ];
      buildInputs = [
        systemc
        boost
        zlib
        libxml2
      ];
      preConfigure = ''
        DISTCOPY='ln -s' package/dist_armsec.sh armsec
        cd armsec
      '';
      # by default, unisim is versionned
      postInstall = ''
        mv $out/bin/unisim-armsec-* $out/bin/unisim-armsec
      '';
    };
in
unisim
