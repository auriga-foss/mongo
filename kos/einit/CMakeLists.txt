project (einit)

# Toolchain for work with NK-parser.
include (platform/image)

# Set compilation flags.
project_header_default ("STANDARD_GNU_11:YES" "STRICT_WARNINGS:NO")

# Define an ENTITIES variable with a list of entities.
set (ENTITIES Mongod
              ${precompiled_vfsVfsNet})

set (SECURITY_PSL_FILE "src/security.psl.in")

# File VFS
set (ROOTFS_ENTITY "kl.VfsRamFs")
set (FS_BACKEND ${precompiled_vfsVfsRamFs})
set (FS_IMAGE ramdisk0.img)
set (FS_IMG_FORMAT ext4)
set (FS_IMG_SIZE 100)
set (FILES ${CMAKE_BINARY_DIR}/einit/${FS_IMAGE})

# Ramdisk
add_custom_target (${FS_IMAGE}
    COMMAND  ${KL_SDK_ROOT_PATH}/common/prepare_hdd_img.sh
        -d   ${CMAKE_SOURCE_DIR}/resources/image
        -s   ${FS_IMG_SIZE}
        -f   ${FS_IMG_FORMAT}
        -img ${FS_IMAGE})

list (APPEND ENTITIES ${FS_BACKEND})
set_target_properties(${FS_BACKEND} PROPERTIES
    EXTRA_ARGS "\
    - -l
    - nodev /tmp ramfs 0
    - -l
    - romfs /romfs romfs 0
    - -l
    - nodev /var ramfs 0
    - -l
    - devfs /dev devfs 0")

# Net VFS
set_target_properties (${precompiled_vfsVfsNet} PROPERTIES
    EXTRA_ARGS "\
    - -l
    - devfs /dev devfs 0
    - -l
    - romfs /etc romfs 0")

# Building Kaspersky OS solution image for a hardware platform.
build_kos_hw_image (kos-image
                    EINIT_ENTITY EinitHw
                    CONNECTIONS_CFG "src/init.yaml.in"
                    SECURITY_PSL ${SECURITY_PSL_FILE}
                    IMAGE_FILES ${ENTITIES}
                                ${FILES})

# Building Kaspersky OS solution image for a QEMU platform.
build_kos_qemu_image (kos-qemu-image
                      EINIT_ENTITY EinitQemu
                      CONNECTIONS_CFG "src/init.yaml.in"
                      SECURITY_PSL ${SECURITY_PSL_FILE}
                      IMAGE_FILES ${ENTITIES}
                                  ${FILES})

add_dependencies (kos-qemu-image ${FS_IMAGE})
