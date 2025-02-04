{
  swift,
  swiftpm,
  swiftpm2nix,
  swiftPackages,
  fetchFromGitHub,
  ...
}: swift.stdenv.mkDerivation rec {
  pname = "fuzzilli";
  version = "v0.9.3";

  src = fetchFromGitHub {
    owner = "googleprojectzero";
    repo = pname;
    rev = version;
    hash = "sha256-DHvx/G3Jt2wMcIy8lr9iYq2v2OunXE5dB59Yaw4/oBA=";
  };

  nativeBuildInputs = [
    swift
    swiftpm
  ];

  patches = [
    ./001-add-libjs-profile.patch
  ];

  env.LD_LIBRARY_PATH = "${swiftPackages.Dispatch}/lib"; # ??

  configurePhase = (swiftpm2nix.helpers ./nix).configure;

  installPhase = ''
    mkdir -p $out/bin
    cp -r $(swiftpmBinPath)/* $out/bin/
  '';
}
