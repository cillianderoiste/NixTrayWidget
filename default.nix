with import <nixpkgs> {}; {
  qtNix = stdenv.mkDerivation {
    name = "qtNix";

    buildInputs = [
      qtcreator
      qt5.full
    ];
  };
}
