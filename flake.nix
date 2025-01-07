{
  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixpkgs-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs =
    inputs:
    inputs.flake-utils.lib.eachDefaultSystem (
      system:
      let
        pkgs = inputs.nixpkgs.legacyPackages.${system};
        fs = pkgs.lib.fileset;
        packages = {
          qalias = pkgs.stdenv.mkDerivation {
            name = "qalias";
            meta.mainProgram = "qalias";
            src = fs.toSource {
              root = ./.;
              fileset = fs.unions [
                ./images
                ./codeeditor.cpp
                ./codeeditor.h
                ./highlighter.cpp
                ./highlighter.h
                ./main.cpp
                ./mainwindow.cpp
                ./mainwindow.h
                ./mdichild.h
                ./mdichild.cpp
                ./qalias.pro
                ./qalias.qrc
              ];
            };
            buildInputs = [
              pkgs.libsForQt5.full
              # https://nixos.wiki/wiki/Qt#qt.qpa.plugin:_Could_not_find_the_Qt_platform_plugin_.22xcb.22_in_.22.22
              pkgs.libsForQt5.qt5.wrapQtAppsHook
            ];
            buildPhase = ''
              qmake
              make
            '';
            installPhase = ''
              mkdir -p $out/bin
              cp qalias $out/bin
            '';
          };
        };
      in
      {
        inherit packages;
        devShells.default = pkgs.mkShell { buildInputs = [ packages.qalias ]; };
      }
    );
}
