#include <stdio.h>
#include <string.h>

#define MEMORY_SIZE 65535

char code[] = "++[->+++++<]++++++++[->>++++++++<<]>>....<....>..<.[-]>[-]";

int jump(int ip, int direction) {
  int depth = 0;

  do {
    if (code[ip] == '[')
      depth += 1;
    else if (code[ip] == ']')
      depth -= 1;

    ip += direction;
  } while( depth );
  return ip;
}

int main() {
  char mem[MEMORY_SIZE];
  int ip, p;

  ip = p = 0;
  memset(mem, 0, MEMORY_SIZE);

  while( ip < sizeof(code) ) {
    switch( code[ip] ) {
    case '+':
      mem[p] += 1;
      break;
    case '-':
      mem[p] -= 1;
      break;
    case '>':
      p += 1;
      break;
    case '<':
      p -= 1;
      break;
    case '.':
      putchar(mem[p]);
      break;
    case ',':
      mem[p] = getchar();
      break;
    case '[':
      if(mem[p] == 0)
	ip = jump(ip, 1);
      break;
    case ']':
      if(mem[p] != 0)
	ip = jump(ip, -1);
      break;
    }
    ip += 1;
  }

  return 0;
}
