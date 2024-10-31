{
  inputs = {
    nixpkgs.url = "nixpkgs/nixos-unstable";
    utils.url = "github:numtide/flake-utils";
  };

  outputs =
    { ... }@inputs:
    inputs.utils.lib.eachDefaultSystem (
      system: let
        pkgs = import inputs.nixpkgs { inherit system; };
      in {
        formatter = pkgs.nixfmt-rfc-style;

        devShells.default = pkgs.mkShell {
          inputsFrom = [ pkgs.raylib ];
          packages = with pkgs; [
            gcc14
            vcpkg
            curl
            zip
            unzip
            gnutar

            udev
            pkg-config
            xorg.libX11
            xorg.libXinerama
            xorg.libXcursor
            libGLU
            wayland-scanner
            wayland
            libxkbcommon

            pkg-config
            freetype
            ncurses
            sfml
            openal
            libvorbis
            flac
            SDL2
            SDL2_image
            libtiff
            libwebp
            SDL2_ttf
            pcre2.dev
            xdot
          ];

          env = {
            MAKEFLAGS = "-j8";
            X11_X11_INCLUDE_PATH = "${pkgs.xorg.libX11.dev}/include";
          };
        };
      }
    );
}
