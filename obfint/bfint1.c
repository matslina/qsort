/* Non-obfuscated brainfuck interpreter.
 * Runs the brainfuck source code in CODE.
 */

#include <stdio.h>
#include <string.h>

#define MEMORY_SIZE 65535
#define DIR_LEFT (-1)
#define DIR_RIGHT 1

char CODE[] = "++[->+++++<]++++++++[->>++++++++<<]>>....<....>..<.[-]>[-]";


/* finds matching loop instruction in either direction */
int jump(int ip, int direction) {
  int depth = 0;

  do {
    if (CODE[ip] == '[')
      depth += 1;
    else if (CODE[ip] == ']')
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

  while( ip < sizeof(CODE) ) {
    switch( CODE[ip] ) {
    case '+': /* increase current cell */
      mem[p] += 1;
      break;
    case '-': /* decrease current cell */
      mem[p] -= 1;
      break;
    case '>': /* move pointer up */
      p += 1;
      break;
    case '<': /* move pointer down */
      p -= 1;
      break;
    case '.': /* output current cell to stdout */
      putchar(mem[p]);
      break;
    case ',': /* read one byte into current cell from stdin*/
      mem[p] = getchar();
      break;
    case '[': /* jump forward to matching ']' if current cell is zero */
      if (mem[p] == 0)
	ip = jump(ip, DIR_RIGHT);
      break;
    case ']': /* jump back to matching '[' if current cell is non-zero */
      if (mem[p] != 0)
	ip = jump(ip, DIR_LEFT);
      break;
    }
    ip += 1;
  }
  return 0;
}
