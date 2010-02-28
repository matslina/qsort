/* Replace if statements with || and &&.
 * Use goto instead of while-loop in main().
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
 foo:
  p[mem] = ip[CODE] - '+' ? mem[p] : p[mem] + 1;
  mem[p] = CODE[ip] - '-' ? p[mem] : mem[p] - 1;
  p = ip[CODE] - '>' ? p : p + 1;
  p = CODE[ip] - '<' ? p : p - 1;
  ip[CODE] - '.' || putchar(p[mem]);
  mem[p] = CODE[ip] ^ ',' ? mem[p] : getchar();
  ip[CODE] ^ '[' || p[mem] || jump(1, 0);
  CODE[ip] - ']' || mem[p] && jump(-1, 0);
  ip += 1;
  if(ip - sizeof(CODE))
    goto foo;
}
