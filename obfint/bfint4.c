/* Store variables on heap.
 * Conditional expressions instead of switch in main().
 */

#include <stdio.h>
#include <string.h>

#define MEMORY_SIZE 65535

char CODE[] = "++[->+++++<]++++++++[->>++++++++<<]>>....<....>..<.[-]>[-]";

int ip, p;
char mem[MEMORY_SIZE];

/* finds matching loop instruction in either direction */
int jump(int direction, int depth) {
  depth += CODE[ip] == ']' ? -1 : CODE[ip] == '[';
  ip += direction;
  return depth ? jump(direction, depth) : ip;
}

int main() {
  while( ip < sizeof(CODE) ) {
    /* increase current cell */
    mem[p] = CODE[ip] == '+' ? mem[p] + 1 : mem[p];

    /* decrease current cell */
    mem[p] = CODE[ip] == '-' ? mem[p] - 1 : mem[p];

    /* move pointer up */
    p = CODE[ip] == '>' ? p + 1 : p;

    /* move pointer down */
    p = CODE[ip] == '<' ? p - 1 : p;

    if (CODE[ip] == '.')
      /* output current cell to stdout */
      putchar(mem[p]);

    if (CODE[ip] == ',')
      /* read one byte into current cell from stdin*/
      mem[p] = getchar();

    if (CODE[ip] == '[')
      /* jump forward to matching ']' if current cell is zero */
      if (mem[p] == 0)
	jump(1, 0);

    if (CODE[ip] == ']')
      /* jump back to matching '[' if current cell is non-zero */
      if (mem[p] != 0)
	ip = jump(-1, 0);

    ip += 1;
  }
  return 0;
}
