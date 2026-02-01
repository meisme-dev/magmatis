{
  inputs = {
    nixpkgs.url = "nixpkgs/nixos-25.11";
  };

  outputs = inputs@{ flake-parts, ... }: flake-parts.lib.mkFlake { inherit inputs; } {
    systems = [
      "x86_64-linux"
      "aarch64-linux"
    ];
    perSystem = { pkgs, ... }: {
      devShells.default = pkgs.mkShell {
        packages = with pkgs; [
          llvmPackages_20.stdenv
          pkg-config
          gdb
          meson
          ninja
          vulkan-tools
          vulkan-headers
          vulkan-validation-layers
          glfw
          vulkan-loader
          shaderc
        ];
      };
      packages.default = pkgs.callPackage ./default.nix { };
    };
  };
}
