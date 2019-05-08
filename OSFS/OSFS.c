/**
* Over Simplified File System (OSFS) ==================================
*
* Provides an extremely basic, low footprint file system for EEPROM access in
* an AVR microprocessor or otherwise.
*
*/

#include "OSFS.h"

const int DELBIT = 7;

uint16_t startOfEEPROM = 1;
uint16_t endOfEEPROM = 4096;

void readNBytes(uint16_t address, unsigned int num, byte* output) {
    eeprom_read_block((void*) output, (const void*) address, num);
}

void writeNBytes(uint16_t address, unsigned int num, const byte* input) {
    eeprom_update_block((const void*) input, (void*) address, num);
}

result getFileInfo(const char* filename, uint16_t* filePointer, uint16_t* fileSize) {

   // Confirm that the EEPROM is managed by this version of OSFS
   result r = checkLibVersion();

   if (r != NO_ERROR)
       return r;

   // It is! Now get the first file header
   fileHeader workingHeader;
   uint16_t workingAddress = startOfEEPROM + sizeof(FSInfo);

   char paddedFilename[11];
   padFilename(filename, paddedFilename);

   // Loop through checking the file header until
   // 	a) we reach a NULL pointer,
   // 	b) we find a deleted file that can be overwritten
   // 	c) we get an OOL pointer somehow
   while (1) {

       // Load the next header
       result r = readNBytesChk(workingAddress, sizeof(fileHeader), &workingHeader);

       // Quit if we're out of bounds
       if (r != NO_ERROR)
           return r;

       // Is this the right file?
       // Check the file ID
       if (0 == strncmp(workingHeader.fileID, paddedFilename, 11)) {
           // We found it!
           // Is it marked as deleted?
           if (isDeletedFile(workingHeader)) {
               // File is deleted. :( continue onwards...
           } else {
               // Load the data into the receiving variables
               *filePointer = workingAddress + sizeof(fileHeader);
               *fileSize = workingHeader.fileSize;

               return NO_ERROR;
           }
       }

       // If there's no next file
       if (workingHeader.nextFile == 0) {

           return FILE_NOT_FOUND;
       }

       // Keep going
       workingAddress = workingHeader.nextFile;
   }

   return UNDEFINED_ERROR;
}

result newFile(const char* filename, const char* data, unsigned int size, uint8_t overwrite) {

   // Header for new file
   fileHeader newHeader;

   // Store padded filename in newHeader
   padFilename(filename, newHeader.fileID);

   // Confirm that the EEPROM is managed by this version of OSFS
   result r = checkLibVersion();

   if (r != NO_ERROR)
       return r;

   // It is! Now get the first file header
   int sizeRequired = sizeof(fileHeader) + size;

   fileHeader workingHeader;
   uint16_t workingAddress = startOfEEPROM + sizeof(FSInfo);
   uint16_t writeAddress;

   // Loop through checking the file header until
   // 	a) we reach a NULL pointer,
   // 	b) we find a deleted file that can be overwritten
   // 	c) we find a file with the same name as this one (overwrite it if same size and overwrite == true)
   // 	d) we get an OOL pointer somehow
   while (1) {

       // Load the next header
       result r = readNBytesChk(workingAddress, sizeof(fileHeader), &workingHeader);

       // Quit if we're out of bounds
       if (r != NO_ERROR)
           return r;

       // Quit if it has the same name and isn't deleted
       if (!isDeletedFile(workingHeader) && 0 == strncmp(workingHeader.fileID, newHeader.fileID, 11)) {
           // Error if different sizes or overwrite == false
           if (size != workingHeader.fileSize || !overwrite)
               return FILE_ALREADY_EXISTS;

           // else overwrite it
           writeAddress = workingAddress;
           break;
       }

       // If there's no next file, calculate the start of the spare space and break the loop
       if (workingHeader.nextFile == 0) {

           if (workingHeader.fileSize != 0)
               writeAddress = workingAddress + sizeof(workingHeader) + workingHeader.fileSize;
           else
               writeAddress = workingAddress;

           break;
       }

       // If this is a deleted file, see if we can fit our file here
       if (isDeletedFile(workingHeader)) {
           // It is. Is the space large enough for us?
           int deletedSpace = workingHeader.nextFile - workingAddress;
           if (deletedSpace >= sizeRequired)
               writeAddress = workingAddress;
       }

       // Nope, next
       workingAddress = workingHeader.nextFile;
   }

   // See if there's enough space in the EEPROM to fit our file in
   if (writeAddress + sizeRequired - 1 > endOfEEPROM)
       return INSUFFICIENT_SPACE;

   // We have a pointer to an address that has sufficient space to store our
   // data, in writeAddress.
   //
   // We have a pointer to the previous header in workingAddress
   //
   // We have a copy of the previous header in workingHeader
   //
   // First, constuct a header for this file:

   newHeader.fileSize = size;
   if (workingHeader.nextFile == 0)
       newHeader.nextFile = 0;
   else
       newHeader.nextFile = workingHeader.nextFile;
   newHeader.flags = 0;

   // Now, alter the previous file's header to point to this new file
   workingHeader.nextFile = writeAddress;

   // Write the headers and the data
   writeNBytesChk(workingAddress, sizeof(fileHeader), &workingHeader);
   writeNBytesChk(writeAddress, sizeof(fileHeader), &newHeader);
   return writeNBytesChk(writeAddress + sizeof(fileHeader), size, data);
}

result deleteFile(const char * filename) {

   // Confirm that the EEPROM is managed by this version of OSFS
   result r = checkLibVersion();

   if (r != NO_ERROR)
       return r;

   // Store padded filename in filenamePadded
   char filenamePadded[11];
   padFilename(filename, filenamePadded);

   // Get the first header
   fileHeader workingHeader;
   uint16_t workingAddress = startOfEEPROM + sizeof(FSInfo);

   // Loop through checking the file header until
   // 	a) we reach a NULL pointer,
   // 	b) we find our file and it's not deleted
   // 	c) we get an OOL pointer somehow
   while (1) {

       // Load the next header
       result r = readNBytesChk(workingAddress, sizeof(fileHeader), &workingHeader);

       // Quit if we're out of bounds
       if (r != NO_ERROR)
           return r;

       // Delete the file if it has the same name and isn't already deleted
       if (!isDeletedFile(workingHeader) && 0 == strncmp(workingHeader.fileID, filenamePadded, 11)) {

           workingHeader.flags = workingHeader.flags | 1<<DELBIT;
           r = writeNBytesChk(workingAddress, sizeof(fileHeader), &workingHeader);

           if (r != NO_ERROR)
               return r;

           return NO_ERROR;
       }

       // Quit if we get a NULL pointer
       if (workingHeader.nextFile == 0)
           return FILE_NOT_FOUND;

       // Next file
       workingAddress = workingHeader.nextFile;
   }

   // We will never get here
   return UNDEFINED_ERROR;
}

result checkLibVersionInternal(uint16_t ver) {

   // Load the identifying info
   FSInfo theROMInfo;

   readNBytesChk(startOfEEPROM, sizeof(FSInfo), &theROMInfo);

   // Check for the ID string
   if (0 != strncmp(theROMInfo.idStr, OSFS_ID_STR, 4)) {
       ver = 0;
       return UNFORMATTED;
   }

   // Check the version
   ver = theROMInfo.version;

   if (ver != OSFS_VER) {
       return WRONG_VERSION;
   }

   return NO_ERROR;
}

result format() {

   // Create identifying info for this version
   FSInfo thisInfo;

   strncpy(thisInfo.idStr, OSFS_ID_STR, 4);
   thisInfo.version = OSFS_VER;

   // Write this to the FS
   result r = writeNBytesChk(startOfEEPROM, sizeof(FSInfo), &thisInfo);

   if (r != NO_ERROR)
       return r;

   // Create a dummy file header, marking where the next file will go
   fileHeader dummyHeader;
   padFilename("", dummyHeader.fileID);
   dummyHeader.fileSize = 0;
   dummyHeader.nextFile = 0;
   dummyHeader.flags = 0;

   // Store this after the FS identifying info
   return writeNBytesChk(startOfEEPROM + sizeof(FSInfo), sizeof(fileHeader), &dummyHeader);
}

result writeNBytesChk(uint16_t address, unsigned int num, const void* input) {
   if (address < startOfEEPROM || address > endOfEEPROM) return UNCAUGHT_OOR;
   if (address + num < startOfEEPROM || address + num > endOfEEPROM) return UNCAUGHT_OOR;

   writeNBytes(address, num, input);

   return NO_ERROR;
}

result readNBytesChk(uint16_t address, unsigned int num, void* output) {

   if (address < startOfEEPROM || address > endOfEEPROM) return UNCAUGHT_OOR;
   if (address + num < startOfEEPROM || address + num > endOfEEPROM) return UNCAUGHT_OOR;

   readNBytes(address, num, output);

   return NO_ERROR;
}


void padFilename(const char * filenameIn, char * filenameOut) {
   // Pad filename to 11 chars
   uint8_t ended = 0;
   for (int i = 0; i<11; i++) {

       char inChar = *(filenameIn+i);

       if (inChar == '\0' || ended)
       {
           *(filenameOut+i) = ' ';
           ended = 1;
       } else {
           *(filenameOut+i) = inChar;
       }
   }
}
