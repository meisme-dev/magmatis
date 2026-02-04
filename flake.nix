{
  inputs = {
    nixpkgs.url = "nixpkgs/nixos-25.11";
  };

  outputs = inputs@{ flake-parts, ... }: flake-parts.lib.mkFlake { inherit inputs; } {
    systems = [
      "x86_64-linux"
      "aarch64-linux"
      "x86_64-darwin"
      "aarch64-darwin"
    ];
    perSystem = { pkgs, ... }: {
      devShells.default = pkgs.mkShell {
        packages = with pkgs; [
          llvmPackages_20.stdenv
          pkg-config
          gdb
          cmake
          meson
          ninja
          vulkan-tools
          vulkan-headers
          vulkan-validation-layers
          glfw
          vulkan-loader
          shaderc
          vulkan-memory-allocator
          cglm
        ] ++ lib.optionals stdenv.isDarwin [ pkgs.moltenvk pkgs.moltenvk.dev ];
      };
      packages.default = pkgs.callPackage ./default.nix { };
    };
  };
}
