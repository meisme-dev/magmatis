project(
  'magmatis',
  'c',
  version: '0.0.1',
  default_options: [
    'warning_level=2',
  ],
)

glfw_dep = dependency('glfw3', required: true)
vulkan_dep = dependency('vulkan', required: true)

sources = files(
  'src/common/list.c',
  'src/core/device.c',
  'src/core/image.c',
  'src/core/instance.c',
  'src/core/magmatis.c',
  'src/core/pipeline.c',
  'src/core/queue.c',
  'src/core/swapchain.c',
  'src/core/window.c',
  'src/main.c',
)

include_directories = include_directories('src')

glslc = find_program('glslc')

shaders = ['shader.frag', 'shader.vert']

compiled_shaders = []
foreach shader : shaders
  spv_shader = shader + '.spv'
  compiled_shaders += custom_target(
    spv_shader,
    input: 'res/' + shader,
    output: spv_shader,
    command: [glslc, '@INPUT@', '-o', '@OUTPUT@'],
    install: true,
    install_dir: get_option('bindir'),
  )
endforeach

dependencies = [glfw_dep, vulkan_dep]

executable(
  'magmatis',
  sources: [sources, compiled_shaders],
  dependencies: dependencies,
  include_directories: include_directories,
  install: true,
)