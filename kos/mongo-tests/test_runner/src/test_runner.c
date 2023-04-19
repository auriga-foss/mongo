#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <coresrv/entity/entity_api.h>
#include <coresrv/elf/elf_api.h>
#include <elf/elf_types.h>
#include <coresrv/fs/fs_api.h>
#include <kos/alloc.h>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define TEST_SUCCESS    ANSI_COLOR_GREEN"SUCCESS"ANSI_COLOR_RESET
#define TEST_FAIL       ANSI_COLOR_RED"FAIL"ANSI_COLOR_RESET
#define TEST_NAME       ANSI_COLOR_BLUE"%s"ANSI_COLOR_RESET


static const char* 
retcodeToStr(Retcode code)
{
    switch (code) {
        case rcOk:
        return "rcOk";
                        
        case rcInvalidArgument:
        return "rcInvalidArgument";

        case rcNotConnected:
        return "rcNotConnected";

        case rcOutOfMemory:
        return "rcOutOfMemory";        
        
        case rcBufferTooSmall:
        return "rcBufferTooSmall";

        case rcInternalError:
        return "rcInternalError";

        case rcTransferError:
        return "rcTransferError";

        case rcReceiveError:
        return "rcReceiveError";

        case rcSourceFault:
        return "rcSourceFault";

        case rcTargetFault:
        return "rcTargetFault";

        case rcIpcInterrupt:
        return "rcIpcInterrupt";
        
        case rcRestart:
        return "rcRestart";

        case rcFail:
        return "rcFail";

        case rcNoCapability:
        return "rcNoCapability";

        case rcNotReady:
        return "rcNotReady";

        case rcUnimplemented:
        return "rcUnimplemented";

        case rcBufferTooLarge:
        return "rcBufferTooLarge";

        case rcBusy:
        return "rcBusy";              
        
        case rcResourceNotFound:
        return "rcResourceNotFound";

        case rcTimeout:
        return "rcResourceNotFound";

        case rcSecurityDisallow:  
        return "rcResourceNotFound";

        case rcFutexWouldBlock:
        return "rcFutexWouldBlock";
        
        case rcAbort:             
        return "rcAbort";

        case rcInvalidThreadState:
        return "rcInvalidThreadState";

        case rcAlreadyExists:
        return "rcAlreadyExists";

        case rcInvalidOperation:  
        return "rcInvalidOperation";

        case rcHandleRevoked:     
        return "rcHandleRevoked";

        case rcQuotaExceeded:     
        return "rcQuotaExceeded";

        case rcDeviceNotFound: 
        return "rcDeviceNotFound";

        default:
        return "Uknown retcode";
    }
} 


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
        fprintf(stderr, "Cannot open %s file\n", path);
        return rcFail;
    }

	if ((fstat(fd, &fsb) != 0) || (!S_ISREG(fsb.st_mode))) {
        fprintf(stderr, "fstat error for %s file\n", path);
        close(fd);
        return rcFail;
	}
    
    file_size = (rtl_uint64_t) fsb.st_size;

    file_size_aligned = rtl_align_up(file_size, PAGE_SIZE);

    *img_ptr = KosMemAlloc(file_size_aligned);
    if (*img_ptr == RTL_NULL) {
        fprintf(stderr, "Cannot allocate %lu bytes for %s file\n", 
                file_size_aligned, path);
        close(fd);
        return rcOutOfMemory;
    }

    fp = fdopen(fd, "rb");
    if (fp == NULL) {
        fprintf(stderr, "fdopen error for %s file\n", path);
        KosMemFree(*img_ptr);
        close(fd);
        return rcFail;
    }

    r_cnt = fread((void*) *img_ptr, file_size, 1, fp);
    if (r_cnt != 1) {
        fprintf(stderr, "read %zu from total %lu bytes for %s file\n", 
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
    Entity *entity = RTL_NULL;
    Retcode rc = rcOk;
    rtl_uint8_t *img = RTL_NULL;
    rtl_uint64_t img_size = 0;

    fprintf(stderr, "Test " TEST_NAME " loading...\n", path);

    rc = testElfLoadFromDisk(path, &img, &img_size);
    if (rc == rcOk) {
        rtl_uintptr_t entry;
        VmSeg *segs = RTL_NULL;
        rtl_size_t segsCount = 0;
        unsigned symIndex = 0;
        rtl_size_t symSize = 0;
        unsigned strIndex = 0;
        rtl_size_t strSize = 0;
        void *hdrData = RTL_NULL;
        rtl_size_t hdrSize = 0;

        rc = KnElfCreateVmSegEx(img, img_size, &entry,
                                &segs, &segsCount,
                                &symIndex, &symSize,
                                &strIndex, &strSize,
                                &hdrData, &hdrSize);
        
        if (rc == rcOk) {
              fprintf(stderr, "Test " TEST_NAME " loaded. Size %lu bytes\n",
                path, img_size);

            entity = EntityInitFromSegEx(einfo,
                                        einfo->eiid, RTL_NULL,
                                        segsCount, entry, segs,
                                        symIndex, symSize,
                                        strIndex, strSize,
                                        hdrData, hdrSize);
            
            if (entity == RTL_NULL) {
                fprintf(stderr,"testRun: EntityInitFromSegEx error for %s\n", 
                        path);
            }

            KosMemFree(img);

            rc = EntityRun(entity);

            KnElfFreeVmSeg(segs, segsCount);

            if (rc == rcOk) {
                unsigned entity_rc;
                
                fprintf(stderr, 
                        "Test "ANSI_COLOR_BLUE"%s"ANSI_COLOR_RESET" started!\n",
                        path);
            
                rc = EntityWait(entity, INFINITE_TIMEOUT, &entity_rc);

                if (rc == rcOk) {
                    char* test_result = (entity_rc == 0) 
                                        ? TEST_SUCCESS : TEST_FAIL;    
                    fprintf(stderr,
                            "Test "ANSI_COLOR_BLUE"%s"ANSI_COLOR_RESET
                            " completed: %s\n", path, test_result);
                } else {
                    fprintf(stderr,
                            "testRun: EntityWait return code: %s for %s\n",
                            retcodeToStr(rc), path);
                }
            } else {
                fprintf(stderr,"testRun: EntityRun return code: %s for %s\n",
                        retcodeToStr(rc), path);
            }
            
            EntityFree(entity);
        }
    }
    return rc;
}

/* 
 *   Arguments:
 *      (1) Path to directory where test executables are located
 */

int main(int argc, const char *argv[])
{
    DIR *dirp;
    struct dirent *dp;
    static const EntityInfo test_einfo = { "test_runner.Test", 0, RTL_NULL };

    if (argc < 2) {
        fprintf(stderr, "Wrong %s usage. "
                "Path to folder with executables "
                "must be provided as an argument\n", argv[0]);
        return EXIT_FAILURE;
    }

    if (chdir(argv[1]) != 0) {
        fprintf(stderr, "Cannot change working directory to %s: "
                "Error \"%s\"\n", argv[1], strerror(errno));
        return EXIT_FAILURE;

    }

    dirp = opendir(".");
    if (dirp == NULL) {
        fprintf(stderr, "Cannot open test directory to %s: "
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
