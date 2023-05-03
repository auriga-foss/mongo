# MongoDB (v6.2) for KasperskyOS

This is a fork of [MongoDB project][1] adapted for usage with KasperskyOS. For
more information about the target OS, please refer to [KasperskyOS Community
Edition][2].

For general information on using MongoDB, its features and so on, please see
the [MongoDB website][3].

Please refer to the original MongoDB (parent) [README.md][4] (also available in
the current repository as [README.mongo.md](./README.mongo.md)) for more
details not related to this fork.

## How to build (Linux host or using Docker)

### Brief

There are 2 ways to build and run MongoDB under KOS:

- using host environment;
- using docker.

Docker is much easier to use because it takes responsibility for preparing the
build environment: KOS SDK, python, pip, etc.  
It will also allow you to keep your host environment clean.

### Prerequisites

Before you begin, ensure that you have met the following requirements:

#### Build in host environment

- You have Debian GNU/Linux "Buster" 10 x64 or above;
- You have installed the latest version of [KasperskyOS Community Edition][5];
- You met the [MongoDB build](./docs/building.md) requirements;

#### Build using docker

- You have installed docker;
- You have successfully built a docker image with KOS MongoDB build environment;

To build the Docker image, run
[kos/docker/build-image.sh](kos/docker/build-image.sh).  
To create and run a Docker container from the image created in the previous
step, run
[kos/docker/create-and-run-container.sh](kos/docker/create-and-run-container.sh).

### Build

Building in a host or docker environment is the same.

Run `make help` to get a list of all available targets.

#### QEMU image

1. `cd <repo_root>/kos/mongod/`;
2. `make compile-mongod`;
3. `make qemubuild`;
4. `make qemurun`;
5. **mongod** is running on KOS QEMU and listening on localhost:`MONGOD_PORT`
port (default 27018).

#### HW (RaspberryPi) image

1. `cd <repo_root>/kos/mongod/`;
2. `make compile-mongod`;
3. `make hwbuild`;  
   Resulting image path: *<target_arch>/einit/kos-image*;
4. [Prepare][6] SD Card for RaspberryPi;
5. Copy the image to the SD card;
6. Turn on your Raspberry Pi. Communication is implemented via UART;
7. Using a COM terminal (minicom or otherwise), run the following command after
starting u-boot (assuming that your `num` partition is formatted to ext4):

```
U-Boot> ext4load mmc 0:<partition_num> 0x200000 kos-image
...
U-Boot> bootelf 0x200000

```

## Directory structure

- *kos* - directory with KOS related stuff;
  - *docker* - Docker files and helper scripts;
    - *Dockerfile* - dockerfile to create a Docker image with KOS CE SDK
      installed and MongoDB build requirements;
    - *build-image.sh* - example command to create a Docker image;
    - *create-and-run-container.sh* - example command to create and run a
      Docker container from a previously created image;
  - *mongod* - project to run **mongod** under KOS OS;
    - *einit*, *mongod* - sources and CMake build rules for corresponding
      entities;
    - *resources* - description files for all the entities, additional files to
      place in the resulting image;
      - *edl* - entities *.edl description files;
      - *image* - additional files to place in the resulting image;
    - *CMakeLists.txt* - project-level CMake build rules;
    - *Makefile* - main Makefile to configure/build/run MongoDB project for KOS;
  - *mongo-tests* - project to run **mongod unit-tests** under KOS OS;
    - *einit*, *test_runner* - sources and CMake build rules for corresponding
      entities;
    - *resources* - description files for all the entities, additional files to
      place in the resulting image;
      - *edl* - entities *.edl description files;
      - *image* - additional files to place in the resulting image;
    - *CMakeLists.txt* - project-level CMake build rules;
    - *Makefile* - main Makefile to configure/build/run mongod unit-tests for KOS;
  - *patches* - optional patches for MongoDB KOS;

## Known issues and limitations

**Currently, only the mongod (server daemon) build is supported for KOS**.

1. **IPv6** is not yet supported at the moment, as it's not available in KOS for
now;
2. Subprocess creation isn't available in KOS for now;
3. Only static build of MongoDB is supported (dynamic libraries are not
supported by KOS);
4. Mongo **mozjs** JS engine is disabled;
5. Mongo HTTP client & Free Monitoring are disabled (no **libcurl** in KOS CE);

## How to run tests

To run mongod unit-tests, there is a special **mongod unit-tests** project,
located in *kos/mongo-tests* directory. In result of building process, all
unit-test executables will be placed on the SD Card image. To limit the number
of running unit-tests, `CHOSEN_TESTS` variable must be populated with the list
of desired unit-tests (empty variable will cause running of all unit-tests). 
To obtain the list of all available unit-tests, `make list-tests` must be
executed.

### QEMU image

1. `cd <repo_root>/kos/mongo-tests/`;
2. `make compile-mongo-tests`;
3. `CHOSEN_TESTS = "<list of desired tests>" make qemubuild`;
4. `make qemurun`.

### HW (RaspberryPi) image

1. `cd <repo_root>/kos/mongo-tests/`;
2. `make compile-mongo-tests`;
3. `CHOSEN_TESTS = "<list of desired tests>" make hwbuild`;
4. Prepare SD Card:
    1. Insert SD Сard and run the command below to format it with two partitions
(1st partition for u-boot loader and the 2nd for tests and supporting
files):  
`dd if=<repo_root>/kos/mongo-tests/build-aarch64/sdcard0.img of=/dev/mmcblk<num> status=progress`
    2. [Install u-boot to the 1st partition][6];
    3. Copy `kos-image` to the any partition of SD Card:  
    `cp <repo_root>/kos/mongo-tests/build-aarch64/einit/kos-image <path_to_sd_partition>`
5. Turn on your Raspberry Pi. Communication is implemented via UART;
6. Using a COM terminal (minicom or otherwise), run the following command after
starting u-boot (assuming that your `num` partition is formatted to fat32):
```
U-Boot> fatload mmc 0:<partition_num> 0x200000 kos-image
...
U-Boot> bootelf 0x200000
```

## Current tests status

All unit-tests have been built with KasperskyOS CE 1.1.1.13 SDK and
run with the following results (on both QEMU and RaspberryPi):

<details>
  <summary>Result: SUCCESS</summary>

- algebra_test
- bson_test
- bson_util_test
- database_name_util_test
- db_bson_test
- db_catalog_util_test
- db_fts_unicode_test
- db_geo_test
- db_query_collation_test
- executor_stats_test
- idl_test
- namespace_string_util_test
- platform_test
- query_datetime_test
- set_terminate_dispatch_test
- set_terminate_from_main_die_in_thread_test
- set_terminate_from_thread_die_in_main_test
- set_terminate_from_thread_die_in_thread_test
- sigaltstack_location_test
- stdx_test
- util_net_test
</details>

<details>
  <summary>Result: FAIL. Reason: unimplemented mlock()</summary>

- async_command_execution_test
- base_test
- ce_array_data_test
- ce_dataflow_nodes_test
- ce_edge_cases_test
- ce_generated_histograms_test
- ce_heuristic_test
- ce_histogram_test
- ce_interpolation_test
- client_rs_test
- client_test
- cluster_server_parameter_test
- command_mirroring_test
- crypto_test
- cst_pipeline_translation_test
- cst_test
- db_auth_test
- db_catalog_test
- db_commands_test
- db_concurrency_test
- db_exec_test
- db_free_mon_test
- db_ftdc_test
- db_fts_test
- db_index_test
- db_matcher_test
- db_op_observer_test
- db_ops_test
- db_pipeline_test
- db_query_test
- db_repl_cloners_test
- db_repl_coordinator_test
- db_repl_idempotency_test
- db_repl_set_aware_service_test
- db_repl_test
- db_s_config_server_test
- db_s_shard_server_test
- db_sbe_test
- db_serverless_test
- db_stats_test
- db_storage_test
- db_timeseries_test
- db_transaction_test
- db_unittest_test
- db_unittest_with_config_server_test_fixture_test
- db_update_test
- db_views_test
- executor_test
- fault_base_classes_test
- interval_intersection_test
- jwt_test
- map_reduce_agg_test
- maxdiff_histogram_test
- mongo_embedded_test
- op_msg_fuzzer_fixture_test
- optimizer_failure_test
- optimizer_test
- process_interface_test
- rpc_test
- s_commands_test
- s_query_test
- s_test
- sbe_abt_test
- scoped_db_connection_pool_test
- scripting_test
- server_description_test
- server_selector_test
- shell_test
- stats_cache_loader_test
- stats_cache_test
- stats_path_test
- stitch_support_test
- storage_wiredtiger_record_store_and_index_test
- storage_wiredtiger_test
- tenant_migration_donor_service_test
- topology_description_test
- topology_listener_test
- topology_manager_test
- topology_state_machine_test
- topology_version_observer_test
- transport_test
- util_concurrency_test
- util_net_ssl_test
- util_test
- watchdog_test
</details>

<details>
  <summary>Result: FAIL. Reason: unimplemented fork()</summary>

 - bson_mutable_test
 - client_out_of_line_executor_test
 - db_sorter_test
 - logv2_test
 - options_parser_test
 - thread_safety_context_test
 - tracing_support_test  
</details>

<details>
  <summary>Result: FAIL. Reason: unimplemented backtrace()</summary>

 - stacktrace_test
 - unittest_test 
</details>

## Changed third-party components

Following list of built-in third-party subprojects which were changed with
respect to enabling KasperskyOS support:

- *src/third_party/asio-master*;
- *src/third_party/boost*;
- *src/third_party/s2*;
- *src/third_party/wiredtiger*;

## Contributing

Please see the [Contributing](./CONTRIBUTING.rst) page for generic info.

We'll follow the parent project contributing rules but would consider to accept
only KasperskyOS-specific changes, so for that it is advised to use
pull-requests.

## Licensing

See [LICENSE-Community.txt](./LICENSE-Community.txt) for the full license text.

[1]: https://github.com/mongodb/mongo
[2]: https://support.kaspersky.com/help/KCE/1.1/en-US/community_edition.htm
[3]: https://www.mongodb.com/
[4]: https://github.com/mongodb/mongo/blob/master/README.md
[5]: https://os.kaspersky.com/development/download/
[6]: https://support.kaspersky.com/help/KCE/1.1/ru-RU/preparing_sd_card_rpi.htm
