{ pkgs, ... }: pkgs.mkShell {
    packages = [
      (pkgs.callPackage ./fuzzilli.nix {})
    ];
}