// test_sp.c
#include <stdio.h>
#include <string.h>
#include "pf.h"    // Paged File layer
#include "sp.h"    // Your new Slotted Page header

int main() {
    int fd;
    int pageNum;
    char *pagePtr;
    int err;
    int slotID;
    char *record;
    int recLen;

    printf("--- Slotted Page Test Program ---\n");

    // 1. Initialize the PF Layer
    PF_Init();

    // 2. Create a test file
    printf("Creating test file 'test_sp_file.db'...\n");
    err = PF_CreateFile("test_sp_file.db");
    if (err != PFE_OK) { 
        PF_PrintError("Error: PF_CreateFile"); 
        // If it says "File exists", just run 'rm test_sp_file.db' and try again
        return 1; 
    }

    // 3. Open the test file (Corrected call)
    fd = PF_OpenFile("test_sp_file.db");
    if (fd < 0) { 
        PF_PrintError("Error: PF_OpenFile"); 
        return 1; 
    }
    printf("Test file opened (fd: %d).\n", fd);

    // 4. Allocate a new page
    err = PF_AllocPage(fd, &pageNum, &pagePtr);
    if (err != PFE_OK) { 
        PF_PrintError("Error: PF_AllocPage"); 
        return 1; 
    }
    printf("Allocated new page (Page #%d).\n", pageNum);

    // 5. Initialize it as a slotted page
    SP_InitPage(pagePtr);
    printf("Page initialized as slotted page.\n");
    printf("Initial free space: %d bytes\n", SP_GetFreeSpace(pagePtr));
    printf("------------------------------------\n");

    // 6. Test Insertion
    printf("Testing record insertion...\n");
    
    char *s1 = "100;Joe;ComputerScience"; // Length 23
    char *s2 = "200;Chandrasekhar;ElectricalEngineering"; // Length 37
    char *s3 = "300;Smith;Mechanical"; // Length 18

    int slot1 = SP_InsertRecord(pagePtr, s1, strlen(s1));
    printf("Inserted '%s' into Slot %d. Free space: %d\n", s1, slot1, SP_GetFreeSpace(pagePtr));

    int slot2 = SP_InsertRecord(pagePtr, s2, strlen(s2));
    printf("Inserted '%s' into Slot %d. Free space: %d\n", s2, slot2, SP_GetFreeSpace(pagePtr));

    int slot3 = SP_InsertRecord(pagePtr, s3, strlen(s3));
    printf("Inserted '%s' into Slot %d. Free space: %d\n", s3, slot3, SP_GetFreeSpace(pagePtr));
    printf("------------------------------------\n");

    // 7. Test Scanning (Full Page)
    printf("Scanning all valid records on page (before deletion):\n");
    slotID = -1; // Start scan from the beginning
    while (SP_GetNextRecord(pagePtr, &slotID, &record, &recLen) == SPE_OK) {
        printf("  Found in Slot %d: '%.*s' (Length: %d)\n", slotID, recLen, record, recLen);
    }
    printf("--- End of scan ---\n");
    printf("------------------------------------\n");

    // 8. TEST DELETION
    printf("Testing deletion...\n");
    printf("Deleting record from Slot %d ('%s')...\n", slot1, s1);
    err = SP_DeleteRecord(pagePtr, slot1);
    if (err != SPE_OK) {
        printf("Error during deletion!\n");
    } else {
        printf("Deletion successful.\n");
    }
    printf("------------------------------------\n");

    // 9. TEST SCANNING (AFTER DELETION)
    printf("Scanning all valid records (after deleting Slot %d):\n", slot1);
    slotID = -1; // Restart scan
    while (SP_GetNextRecord(pagePtr, &slotID, &record, &recLen) == SPE_OK) {
        // This loop should now only find Slot 2 and Slot 3
        printf("  Found in Slot %d: '%.*s' (Length: %d)\n", slotID, recLen, record, recLen);
    }
    printf("--- End of scan (Slot %d should be missing) ---\n", slot1);
    printf("------------------------------------\n");

    // 10. TEST GET SPECIFIC RECORD (PROVES DELETION)
    printf("Fetching record from Slot %d directly...\n", slot2);
    if (SP_GetRecord(pagePtr, slot2, &record, &recLen) == SPE_OK) {
        printf("  Got: '%.*s'\n", recLen, record);
    } else {
        printf("  ERROR: Failed to get record from Slot %d\n", slot2);
    }

    printf("Fetching record from (deleted) Slot %d directly...\n", slot1);
    if (SP_GetRecord(pagePtr, slot1, &record, &recLen) == SPE_OK) {
        printf("  ERROR: Should not have found this record! '%.*s'\n", recLen, record);
    } else {
        printf("  Correctly failed to get deleted record.\n");
    }
    printf("------------------------------------\n");
    
    // 11. Test Page Full
    printf("Testing page-full condition...\n");
    char *tinyRec = "a";
    int tinyLen = 1;
    int count = 0;
    while (SP_InsertRecord(pagePtr, tinyRec, tinyLen) >= 0) {
        count++;
    }
    printf("Inserted %d more tiny 'a' records until page was full.\n", count);
    printf("Final free space: %d bytes\n", SP_GetFreeSpace(pagePtr));
    printf("------------------------------------\n");

    // 12. Unfix the page (mark it "dirty" since we changed it)
    printf("Unfixing page %d (marked as dirty)...\n", pageNum);
    err = PF_UnfixPage(fd, pageNum, TRUE);
    if (err != PFE_OK) { 
        PF_PrintError("Error: PF_UnfixPage"); 
        return 1; 
    }

    // 13. Clean up (Corrected calls)
    printf("Closing and destroying test file...\n");
    
    err = PF_CloseFile(fd);
    if (err != PFE_OK) { 
        PF_PrintError("Error: PF_CloseFile"); 
        return 1; 
    }
    
    err = PF_DestroyFile("test_sp_file.db");
    if (err != PFE_OK) { 
        PF_PrintError("Error: PF_DestroyFile"); 
        return 1; 
    }
    
    printf("\n--- Slotted Page Test Complete ---\n");
    return 0;
}