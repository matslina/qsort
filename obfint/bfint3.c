/* Conditional expressions instead of if statements in jump().
 */

#include <stdio.h>
#include <string.h>

#define MEMORY_SIZE 65535

char CODE[] = "++[->+++++<]++++++++[->>++++++++<<]>>....<....>..<.[-]>[-]";


/* finds matching loop instruction in either direction */
int jump(int ip, int direction, int depth) {
  depth += CODE[ip] == ']' ? -1 : CODE[ip] == '[';
  ip += direction;
  return depth ? jump(ip, direction, depth) : ip;
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
	ip = jump(ip, 1, 0);
      break;
    case ']': /* jump back to matching '[' if current cell is non-zero */
      if (mem[p] != 0)
	ip = jump(ip, -1, 0);
      break;
    }
    ip += 1;
  }
  return 0;
}
