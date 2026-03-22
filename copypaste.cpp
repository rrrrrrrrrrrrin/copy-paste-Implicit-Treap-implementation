#include <iostream>

#include "edit.h"

void pushLine(Vector<char*>& inBuffer, Vector<NewLine>& new_lines, char*& str,
              int& i, NewLine new_line) {
  str[i] = '\0';

  char* copy = new char[i + 1]{0};
  std::memcpy(copy, str, i + 1);
  inBuffer.push_back(copy);
  new_lines.push_back(new_line);

  i = 0;
}

int main(int argc, char* argv[]) {
  if (argc != 4) {
    std::cout << "Usage: input-file commands-file output-file\n";
    return 1;
  }

  // ===============================
  // Parse the input and commands
  // ===============================

  std::ifstream input(argv[1], std::ios::binary);
  if (!input) {
    std::cout << "Couldn't open input file\n";
    return 2;
  }

  std::ifstream commands(argv[2]);
  if (!commands) {
    std::cout << "Couldn't open commands file\n";
    return 3;
  }

  int c;
  char* str = new char[1003]{0};
  int i = 0;
  Vector<char*> inBuffer;
  Vector<NewLine> new_lines;
  while ((c = input.get()) != EOF) {
    char ch = static_cast<char>(c);

    if (ch == '\r') {
      if (input.peek() == '\n') {
        input.get();  // skip '\n'
        pushLine(inBuffer, new_lines, str, i, NewLine::CRLF);
      } else {
        pushLine(inBuffer, new_lines, str, i, NewLine::CR);
      }
    } else if (ch == '\n') {
      pushLine(inBuffer, new_lines, str, i, NewLine::LF);
    } else {
      str[i++] = char(ch);
    }
  }
  input.close();
  // Don't store last line (it's empty)

  // To accurately std::strcmp() commands, don't store '\r' or '\n'
  Vector<char*> cmdBuffer;
  while ((c = commands.get()) != EOF) {
    char ch = static_cast<char>(c);

    if (ch == '\r') {
      if (commands.peek() == '\n') {
        commands.get();
      }
      str[i] = '\0';

      char* copy = new char[i + 1]{0};
      std::memcpy(copy, str, i + 1);
      cmdBuffer.push_back(copy);

      i = 0;
    } else if (ch == '\n') {
      str[i] = '\0';

      char* copy = new char[i + 1]{0};
      std::memcpy(copy, str, i + 1);
      cmdBuffer.push_back(copy);

      i = 0;
    } else {
      str[i++] = ch;
    }
  }
  commands.close();

  delete[] str;
  // ~Edit will free char* copy

  // ====================================
  // Edit
  // ====================================
  Edit edit(inBuffer, new_lines);
  for (uint64_t i = 0; i < cmdBuffer.get_size(); i++) {
    auto* command = cmdBuffer[i];

    if (std::strcmp(command, "Up") == 0) {
      edit.Up();
    } else if (std::strcmp(command, "Down") == 0) {
      edit.Down();
    } else if (std::strcmp(command, "Ctrl+X") == 0) {
      edit.Cut();
    } else if (std::strcmp(command, "Ctrl+V") == 0) {
      edit.Paste();
    } else if (std::strcmp(command, "Shift") == 0) {
      edit.startShift();
    }
  }

  std::ofstream output(argv[3], std::ios::binary);
  if (!output) {
    std::cout << "Couldn't open output file\n";
    return 4;
  }

  Edit::editPrint(edit, output);

  for (uint64_t i = 0; i < cmdBuffer.get_size(); i++) {
    auto* ptr = cmdBuffer[i];
    delete[] ptr;
  }

  return 0;
}