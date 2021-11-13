// Erick Tran ericktran@csu.fullerton.edu
// CPSC351 Kenytt Avery
// Project 3
// Due November 12, 2021
// Purpose: Create functions to pack files into one "box file",
// read the contents of the box, and unpack the box.

#include <fcntl.h>
#include <iostream>
#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

// Default size of read buffer
const int NUM_BYTES_TO_READ = 100;

// function headers
bool listFiles(char *filename);
bool unboxFiles(char *boxname, char *outputFiles[], int numFiles);
bool boxFiles(char *boxname, char *inputFiles[], int numFiles);

struct boxdata {
  char filenames[10][10];
  mode_t fileModes[10];
  int fileOffsets[10];
  int numFiles;
};

// main funct
int main(int argc, char *argv[]) {
  // Display command args
  if (argc == 1) {
    std::cerr << "Usage: ./box COMMAND BOXFILE [FILE]\n";
    exit(EXIT_FAILURE);
  }
  else if (argc >= 3) {
    std::string command = argv[1];
    if(command == "list") {
      if(!listFiles(argv[2])){
        exit(EXIT_FAILURE);
      }
    }
    else if(argc > 3 && command == "unpack") {
      // Create new char *array of files to be unpacked
      char **outputFiles;
      outputFiles = new char*[argc];
      for (int i = 0; i < argc - 3; i++) {
        outputFiles[i] = argv[i + 3];
      }

      if(!unboxFiles(argv[2], outputFiles, argc - 3)){
        delete[] outputFiles;
        exit(EXIT_FAILURE);
      }
      delete[] outputFiles;
    }
    else if (argc > 3 && command == "create") {
      if (argc > 13) {
        std::cerr << "Max of 10 files can be added into a box\n" << std::endl;
        exit(EXIT_FAILURE);
      }
      // Create new char *array of files to be read
      char **inputFiles;
      inputFiles = new char*[argc];
      for (int i = 0; i < argc - 3; i++) {
        inputFiles[i] = argv[i + 3];
      }
      // Call function to pack files together
      // Pass in boxfile's name, char* array of filenames, and num of files to be packed
      if(!boxFiles(argv[2], inputFiles, argc - 3)){
        delete[] inputFiles;
        exit(EXIT_FAILURE);
      }
      delete[] inputFiles;
    }
    else {
      std::cerr << "Usage: ./box (create/list/unpack) BOXFILE [FILE]\n";
      exit(EXIT_FAILURE);
    }
  }
  else {
    std::cerr << "Usage: ./box COMMAND BOXFILE [FILE]\n";
    exit(EXIT_FAILURE);
  }

  return EXIT_SUCCESS;
}

// Lists the names of files contained within a boxfile
bool listFiles(char *boxname) {
  boxdata box1data;
  int box_fd = open(boxname, O_RDONLY);
  if(box_fd < 0) {
    std::cerr << "Error reading boxfile\n" << std::endl;
    return 0;
  }
  char metadataBuffer[sizeof(boxdata)];
  read(box_fd, metadataBuffer, sizeof(metadataBuffer));
  box1data = *reinterpret_cast<boxdata*>(metadataBuffer);
  std::cout << box1data.numFiles << std::endl;
  for(int i = 0; i < box1data.numFiles; i++) {
    std::cout << box1data.fileOffsets[i] << "\t" << box1data.filenames[i] << std::endl;
  }
  close(box_fd);
  return 1;
}

// Unpacks specified files from a boxfile, without changing the boxfile
bool unboxFiles(char *boxname, char *outputFiles[], int numFiles) {
  boxdata box1data;
  int box_fd = open(boxname, O_RDONLY);
  if(box_fd < 0) {
    std::cerr << "Error reading boxfile\n" << std::endl;
    return 0;
  }

  // Reads metadata and gets contents of box
  char metadataBuffer[sizeof(boxdata)];
  read(box_fd, metadataBuffer, sizeof(metadataBuffer));
  box1data = *reinterpret_cast<boxdata*>(metadataBuffer);
  // Create map that stores index location of each filename
  std::map<std::string, int> fileIndices;
  for (int i = 0; i < box1data.numFiles; i++) {
    fileIndices.insert(std::pair<std::string, int>(box1data.filenames[i], i));
  }
  // increment through given files to unpack
  for (int i = 0; i < numFiles; i++) {
    char readBuffer[NUM_BYTES_TO_READ];
    int bytesRead = 1;
    int bytesToRead = NUM_BYTES_TO_READ;
    int boxIndex = fileIndices.find(outputFiles[i])->second;
    lseek(box_fd, box1data.fileOffsets[boxIndex], SEEK_SET);
    int output_fd = open(outputFiles[i], O_WRONLY | O_CREAT, box1data.fileModes[boxIndex]);

    if(boxIndex < box1data.numFiles - 1) {
      int bytesLeft = box1data.fileOffsets[boxIndex + 1] - box1data.fileOffsets[boxIndex];
      while (bytesRead > 0) {
        if (bytesToRead > bytesLeft) {
          bytesToRead = bytesLeft;
        }
        bytesRead = read(box_fd, readBuffer, bytesToRead);
        bytesLeft -= bytesRead;
        write(output_fd, readBuffer, bytesRead);
      }
    }
    else {
      while (bytesRead > 0) {
        bytesRead = read(box_fd, readBuffer, bytesToRead);
        write(output_fd, readBuffer, bytesRead);
      }
    }
    close(box_fd);
    close(output_fd);
  }
  return 1;
}

// Reads files and writes them to a new box
bool boxFiles(char *boxname, char *inputFiles[], int numFiles) {
  boxdata box1data;
  box1data.numFiles = numFiles;

  int box_fd = open(boxname, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
  if(box_fd < 0) {
    std::cerr << "Error creating boxfile" << std::endl;
    return 0;
  }
  int offset = sizeof(boxdata);
  lseek(box_fd, offset, SEEK_SET);

  // increment through all files in inputFiles
  for(int i = 0; i < numFiles; i++){
    struct stat sb;
    char readBuffer[NUM_BYTES_TO_READ];
    int bytesRead = 1;
    int file_fd = open(inputFiles[i], O_RDONLY);
    fstat(file_fd, &sb);
    strcpy(box1data.filenames[i],inputFiles[i]);
    box1data.fileModes[i] = sb.st_mode;
    box1data.fileOffsets[i] = offset;

    // error
    if(file_fd < 0) {
      std::cerr << "fd: " << file_fd << "\nError reading input file: " << inputFiles[i] << std::endl;
      return 0;
    }

    // read entire file and write its contents to box
    while(bytesRead > 0) {
      bytesRead = read(file_fd, readBuffer, NUM_BYTES_TO_READ);
      offset += bytesRead;
      write(file_fd, readBuffer, bytesRead);
    }

    close(file_fd);

    // deletes file after writing
    if(remove(inputFiles[i]) != 0) {
      std::cerr << "Error deleting file" << std::endl;
      return 0;
    }
  }
  //write metadata to box after all files are read
  char metadataBuffer[sizeof(boxdata)];
  memcpy(metadataBuffer, &box1data, sizeof(boxdata));
  lseek(box_fd, 0, SEEK_SET);
  write(box_fd, metadataBuffer, sizeof(boxdata));
  close(box_fd);

  return 1;
}
