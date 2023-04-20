#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <rtl/stdio.h>
#include <rtl/retcode_hr.h>
#include <coresrv/entity/entity_api.h>
#include <coresrv/elf/elf_api.h>
#include <kos/alloc.h>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define TEST_SUCCESS    ANSI_COLOR_GREEN"SUCCESS"ANSI_COLOR_RESET
#define TEST_FAIL       ANSI_COLOR_RED"FAIL"ANSI_COLOR_RESET
#define TEST_NAME       ANSI_COLOR_BLUE"%s"ANSI_COLOR_RESET


static Retcode 
testElfLoadFromDisk(const char* path, 
                    rtl_uint8_t** img_ptr, rtl_uint64_t* img_size)
{
    rtl_uint64_t file_size = 0, file_size_aligned = 0;
    struct stat fsb;
    int fd = open(path, O_RDONLY);
    FILE *fp;
    size_t r_cnt;

    if (fd < 0) {
        rtl_printf("Cannot open %s file\n", path);
        return rcFail;
    }

    if ((fstat(fd, &fsb) != 0) || (!S_ISREG(fsb.st_mode))) {
        rtl_printf("fstat error for %s file\n", path);
        close(fd);
        return rcFail;
    }
    
    file_size = (rtl_uint64_t) fsb.st_size;
    file_size_aligned = rtl_align_up(file_size, PAGE_SIZE);

    *img_ptr = KosMemAlloc(file_size_aligned);
    if (*img_ptr == RTL_NULL) {
        rtl_printf("Cannot allocate %lu bytes for %s file\n", 
                    file_size_aligned, path);
        close(fd);
        return rcOutOfMemory;
    }

    fp = fdopen(fd, "rb");
    if (fp == NULL) {
        rtl_printf("fdopen error for %s file\n", path);
        KosMemFree(*img_ptr);
        close(fd);
        return rcFail;
    }

    r_cnt = fread((void*) *img_ptr, file_size, 1, fp);
    if (r_cnt != 1) {
        rtl_printf("Error: read %zu from total %lu bytes for %s file\n",
                    r_cnt, file_size, path);
        KosMemFree(*img_ptr);
        fclose(fp);
        return rcFail;
    }

    fclose(fp);
    *img_size = file_size;
    return rcOk;
}


static Retcode 
testRun(const EntityInfo *einfo, const char *path)
{
    Retcode rc;
    Entity *entity = RTL_NULL;
    rtl_uint8_t *img = RTL_NULL;
    rtl_uint64_t img_size = 0;
    rtl_uintptr_t entry;
    VmSeg *segs = RTL_NULL;
    rtl_size_t segsCount = 0;
    unsigned symIndex = 0;
    rtl_size_t symSize = 0;
    unsigned strIndex = 0;
    rtl_size_t strSize = 0;
    void *hdrData = RTL_NULL;
    rtl_size_t hdrSize = 0;
    unsigned entity_rc;
    char* test_result;

    rtl_printf("Test " TEST_NAME " loading...\n", path);

    rc = testElfLoadFromDisk(path, &img, &img_size);
    if (rc != rcOk) {
        rtl_printf("testElfLoadFromDisk(%s) failure, rc = "RETCODE_HR_FMT"\n",
                    path, RETCODE_HR_PARAMS(rc));
        return rc;
    }

    rc = KnElfCreateVmSegEx(img, img_size, &entry,
                            &segs, &segsCount,
                            &symIndex, &symSize,
                            &strIndex, &strSize,
                            &hdrData, &hdrSize);
    if (rc != rcOk) {
        KosMemFree(img);
        rtl_printf("KnElfCreateVmSegEx(%s) failure, rc = "RETCODE_HR_FMT"\n",
                    path, RETCODE_HR_PARAMS(rc));
        return rc;
    }
    
    rtl_printf("Test " TEST_NAME " loaded. Size %lu bytes\n", path, img_size);

    entity = EntityInitFromSegEx(einfo,
                                 einfo->eiid, RTL_NULL,
                                 segsCount, entry, segs,
                                 symIndex, symSize,
                                 strIndex, strSize,
                                 hdrData, hdrSize);           
    KosMemFree(img);
            
    if (entity == RTL_NULL) {
        rtl_printf("EntityInitFromSegEx(%s) failure\n", path);
        return rcFail;
    }

    rc = EntityRun(entity);

    KnElfFreeVmSeg(segs, segsCount);

    if (rc != rcOk) {
        rtl_printf("EnityRun(%s) failure, rc = "RETCODE_HR_FMT"\n", path,
                    RETCODE_HR_PARAMS(rc));
        EntityFree(entity);
        return rc;
    }
                
    rtl_printf("Test "ANSI_COLOR_BLUE"%s"ANSI_COLOR_RESET" started!\n", path);
            
    rc = EntityWait(entity, INFINITE_TIMEOUT, &entity_rc);

    if (rc != rcOk) {
        rtl_printf("EnityWait(%s) failure, rc = "RETCODE_HR_FMT"\n", path,
                    RETCODE_HR_PARAMS(rc));
        EntityFree(entity);
        return rc;
    }

    test_result = (entity_rc == 0) ? TEST_SUCCESS : TEST_FAIL;    

    rtl_printf("Test "ANSI_COLOR_BLUE"%s"ANSI_COLOR_RESET" completed: %s\n",
                path, test_result);
      
    EntityFree(entity);

    return rc;
}

/* 
 *   Arguments:
 *      (1) Path to directory where test executables are located
 */

int
main(int argc, const char *argv[])
{
    DIR *dirp;
    struct dirent *dp;
    static const EntityInfo test_einfo = { "test_runner.Test", 0, RTL_NULL };

    if (argc < 2) {
        rtl_printf("Wrong %s usage. "
                    "Path to folder with executables "
                    "must be provided as an argument\n", argv[0]);
        return EXIT_FAILURE;
    }

    if (chdir(argv[1]) != 0) {
        rtl_printf("Cannot change working directory to %s: "
                    "Error \"%s\"\n", argv[1], strerror(errno));
        return EXIT_FAILURE;
    }

    dirp = opendir(".");
    if (dirp == NULL) {
        rtl_printf("Cannot open test directory to %s: "
                    "Error \"%s\"\n", argv[1], strerror(errno));
        return EXIT_FAILURE;
    }
    
    while ((dp = readdir(dirp)) != NULL) {
        if (dp->d_type == DT_REG) {
            testRun(&test_einfo, dp->d_name);
        }
    }

    closedir(dirp);
    return EXIT_SUCCESS;
}
