#include <iostream>
#include <cstring>
#include "edit.h"

int main(int argc, char* argv[]) {
  if (argc != 4) {
    std::cout << "Usage: input-file output-file\n";
    return 1;
  }

  std::ifstream input(argv[1]);
  if (!input) {
    std::cout << "Couldn't open input file\n";
    return 2;
  }

  std::ifstream commands(argv[2]);
  if (!commands) {
    std::cout << "Couldn't open input file\n";
    return 3;
  }

  std::ofstream output(argv[3]);
  if (!output) {
    std::cout << "Couldn't open input file\n";
    return 4;
  }

  // =============================== Parse the input and commands ===============================
  char ch;
  char* str =  new char[1001]{0};
  int i = 0;
  Vector<char*> inBuffer;
  while ((ch = input.get()) != EOF) {
    if (ch == '\n') {
      str[i] = '\0';  // cut remnants of previous string

      // inBuffer is storing pointers: store distinct allocation each line
      char* copy = new char[i + 1]{0};
      std::memcpy(copy, str, i + 1);
      inBuffer.push_back(copy);

      i = 0;
      continue;
    }
    str[i++] = ch;
  }
  input.close();

  // Don't save last line: '\n'

  Vector<char*> cmdBuffer;
  while ((ch = commands.get()) != EOF) {
    if (ch == '\n') {
      str[i] = '\0';

      char* copy = new char[i + 1]{0};
      std::memcpy(copy, str, i + 1);
      cmdBuffer.push_back(copy);

      i = 0;
      continue;
    }
    str[i++] = ch;
  }
  commands.close();

  str[i] = '\0';
  char* copy = new char[i + 1]{0};
  std::memcpy(copy, str, i + 1);
  cmdBuffer.push_back(copy);
  
  delete[] str;
  // ~Edit will free char* copy

  // ==================================== Edit ====================================
  Edit edit(inBuffer);
  for (uint64_t i = 0; i < cmdBuffer.get_size(); i++) {
      auto command = cmdBuffer[i];

      if (std::strcmp(command, "Up") == 0) { edit.Up(); }
      else if (std::strcmp(command, "Down") == 0) { edit.Down(); }
      else if (std::strcmp(command, "Ctrl+X") == 0) { edit.Cut(); }
      else if (std::strcmp(command, "Ctrl+V") == 0) { edit.Paste(); }
      else if (std::strcmp(command, "Shift") == 0) { edit.startShift(); }
  }

  Edit::editPrint(edit, output);

  return 0;
}