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
            buildInputs = [ pkgs.libsForQt5.full ];
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
        devShells.default = pkgs.mkShell {
          buildInputs = [ packages.qalias ];
          shellHook = ''
            export QT_DEBUG_PLUGINS=1
          '';
        };
      }
    );
}
