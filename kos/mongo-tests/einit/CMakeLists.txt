project (einit)

# Toolchain for work with NK-parser.
include (platform/image)

# Set compilation flags.
project_header_default ("STANDARD_GNU_11:YES" "STRICT_WARNINGS:NO")

# Define an ENTITIES variable with a list of entities.
set (ENTITIES Testrunner
              ${ns_ENTITY}
              ${precompiled_vfsVfsNet}
              ${rump_DHCPCD_ENTITY})

set (SECURITY_PSL_FILE "src/security.psl.in")

# File VFS
set (ROOTFS_ENTITY "kl.VfsSdCardFs")
set (ROOTFS_DEVICE "sdcard")
set (FS_BACKEND ${precompiled_vfsVfsSdCardFs})

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

# add network related files to romfs image
# FIXME: these files are duplicated in sdcard.
list (APPEND FILES ${CMAKE_SOURCE_DIR}/resources/image/etc/hosts)
list (APPEND FILES ${CMAKE_SOURCE_DIR}/resources/image/etc/services)
list (APPEND FILES ${CMAKE_SOURCE_DIR}/resources/image/etc/hostname)

# DHCP client
set (EXTRA_CONNECTIONS "\
    - target: kl.VfsNet
      id: kl.VfsNet
    - target: ${ROOTFS_ENTITY}
      id: ${ROOTFS_ENTITY}")

set_target_properties (${rump_DHCPCD_ENTITY} PROPERTIES
    DEPENDS_ON_ENTITY ""
    EXTRA_CONNECTIONS ${EXTRA_CONNECTIONS}
    EXTRA_ENV "\
    VFS_FILESYSTEM_BACKEND: client{fs->net}:${ROOTFS_ENTITY}
    VFS_NETWORK_BACKEND: client:kl.VfsNet"
    EXTRA_ARGS "\
    - '-4'
    - -f
    - /etc/dhcpcd.conf")

# Building Kaspersky OS solution image for a hardware platform.
#TODO: need to prepare sd-card image and check it on RPi  
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
                      QEMU_FLAGS "${QEMU_FLAGS}"
                      IMAGE_FILES ${ENTITIES}
                                  ${FILES})
