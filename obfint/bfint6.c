/* Remove comments, inline constants, strip some whitespace.
 * Don't specifiy data types.
 */

#include <stdio.h>
#include <string.h>

char CODE[] = "++[->+++++<]++++++++[->>++++++++<<]>>....<....>..<.[-]>[-]";

ip, p;
mem[0xffff];

jump(direction, depth){
  depth += ip[CODE] - 93 ? !(CODE[ip] - 91) : -1;
  ip += direction;
  return depth ? jump(direction, depth) : ip;
}

main() {
  while( ip < sizeof(CODE) ) {
    p[mem] = ip[CODE] - '+' ? mem[p] : p[mem] + 1;
    mem[p] = CODE[ip] - '-' ? p[mem] : mem[p] - 1;
    p = ip[CODE] - '>' ? p : p + 1;
    p = CODE[ip] - '<' ? p : p - 1;
    if (!(ip[CODE] - '.'))
      putchar(p[mem]);
    if (! (CODE[ip] ^ ','))
      mem[p] = getchar();
    if (!(ip[CODE] ^ '['))
      if (!p[mem])
	jump(1, 0);
    if (!(CODE[ip] - ']'))
      if (mem[p])
	ip = jump(-1, 0);
    ip += 1;
  }
  return 0;
}
