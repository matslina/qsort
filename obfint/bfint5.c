/* Flip x[y] to y[x].
 * Replace comparison operators: a==b ? x : b  <==>  a-b ? b : x  <==>
 *                               a^b ? b : x
                                 if(a==b) x  <==>  if(!(a-b)) x  <==>  if(!(a^b)) x
 */

#include <stdio.h>
#include <string.h>

#define MEMORY_SIZE 65535

char CODE[] = "++[->+++++<]++++++++[->>++++++++<<]>>....<....>..<.[-]>[-]";

int ip, p;
char mem[MEMORY_SIZE];

/* finds matching loop instruction in either direction */
int jump(int direction, int depth) {
  depth += ip[CODE] == ']' ? -1 : !(CODE[ip] - '[');
  ip += direction;
  return depth ? jump(direction, depth) : ip;
}

int main() {
  while( ip < sizeof(CODE) ) {
    /* increase current cell */
    p[mem] = ip[CODE] - '+' ? mem[p] : p[mem] + 1;

    /* decrease current cell */
    mem[p] = CODE[ip] - '-' ? p[mem] : mem[p] - 1;

    /* move pointer up */
    p = ip[CODE] - '>' ? p : p + 1;

    /* move pointer down */
    p = CODE[ip] - '<' ? p : p - 1;

    if (!(ip[CODE] - '.'))
      /* output current cell to stdout */
      putchar(p[mem]);

    if (! (CODE[ip] ^ ','))
      /* read one byte into current cell from stdin*/
      mem[p] = getchar();

    if (!(ip[CODE] ^ '['))
      /* jump forward to matching ']' if current cell is zero */
      if (!p[mem])
	jump(1, 0);

    if (!(CODE[ip] - ']'))
      /* jump back to matching '[' if current cell is non-zero */
      if (mem[p])
	ip = jump(-1, 0);

    ip += 1;
  }
  return 0;
}
