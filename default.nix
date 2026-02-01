{ lib
, llvmPackages_20
, pkg-config
, vulkan-headers
, vulkan-loader
, vulkan-validation-layers
, meson
, ninja
, shaderc
, glfw
}:

llvmPackages_20.stdenv.mkDerivation {
  pname = "magmatis";
  version = "0.1.0";

  src = ./.;

  nativeBuildInputs = [ pkg-config meson ninja shaderc ];
  buildInputs = [ vulkan-headers vulkan-loader vulkan-validation-layers glfw ];

  postBuild = ''
    mkdir -p $out/
    cp compile_commands.json $out/compile_commands.json
  '';

  meta = with lib; {
    homepage = "";
    description = ''
    '';
    platforms = with platforms; linux;
  };
}
