{ pkgs ? import <nixpkgs> {} }:

pkgs.mkShell {
  buildInputs = with pkgs; [
    gcc
    gnumake
    ncurses.dev
  ];

  shellHook = ''
    echo "Corewar development environment loaded"
    echo "ncurses headers available at: ${pkgs.ncurses.dev}/include"
  '';
}
