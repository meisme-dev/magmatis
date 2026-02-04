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
, moltenvk
, cglm
, cmake
, vulkan-memory-allocator
, stdenv
, makeWrapper
}:

llvmPackages_20.stdenv.mkDerivation {
  pname = "magmatis";
  version = "0.1.0";

  src = ./.;

  nativeBuildInputs = [ pkg-config meson ninja cmake shaderc makeWrapper ];
  buildInputs = [ vulkan-headers vulkan-loader vulkan-validation-layers glfw cglm vulkan-memory-allocator ] ++ lib.optionals stdenv.isDarwin [ moltenvk moltenvk.dev ];

  postBuild = ''
    mkdir -p $out/
    cp compile_commands.json $out/compile_commands.json
  '';

  postInstall = lib.optionalString stdenv.isDarwin ''
    wrapProgram $out/bin/magmatis \
      --prefix DYLD_LIBRARY_PATH : "${vulkan-loader}/lib" \
      --set VK_LAYER_PATH ${vulkan-validation-layers}/share/vulkan/explicit_layer.d \
      --set VK_ICD_FILENAMES ${moltenvk}/share/vulkan/icd.d/MoltenVK_icd.json
  '';

  meta = with lib; {
    homepage = "";
    description = ''
    '';
    platforms = with platforms; all;
  };
}
